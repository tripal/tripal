<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for confirming deletion of a chado CV term.
 */
class ChadoCvtermDeleteForm extends FormBase {

  /**
   * Provide the cvterm buddy instance.
   */
  protected ChadoCvtermBuddy $cvterm_buddy;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('tripal_chado.database'),
      $container->get('tripal_chado.chado_buddy'),
    );
  }

  /**
   * Class constructor.
   */
  public function __construct(
    protected readonly ChadoConnection $chado_connection,
    protected readonly ChadoBuddyPluginManager $buddy_manager,
  ) {
    // Create the buddy instances we will need.
    $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_cvterm_delete_form';
  }

  /**
   * A simple form for confirming deletion of a chado CV term.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int $cvterm_id
   *   The cvterm primary key.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $cvterm_id = NULL) {

    // Get values of the CV term.
    $cvterm_records = $this->cvterm_buddy->getCvterm(['cvterm.cvterm_id' => $cvterm_id]);
    if (!$cvterm_records) {
      throw new \Exception("Invalid cvterm_id \"$cvterm_id\" passed to chado_cvterm_delete_form");
    }
    $cvterm_record = reset($cvterm_records);

    $table_def = $this->chado_connection->schema()->getTableDef('cvterm',
      ['source' => 'database', 'format' => 'default']);
    // Format is [referencing_table =>
    // [cvterm column (cvterm_id) => referencing_table column], ].
    $foreign_keys = $table_def['referenced_by'] ?? [];
    $total_references = 0;
    foreach ($foreign_keys as $referencing_table => $keydef) {
      foreach ($keydef as $refkey) {
        $query = $this->chado_connection->select('1:' . $referencing_table);
        $query->condition($refkey, $cvterm_id, '=');
        $n = $query->countQuery()->execute()->fetchField();
        $total_references += $n;
      }
    }

    $form = [];
    $form['cvterm_id'] = [
      '#type' => 'value',
      '#value' => $cvterm_id,
    ];
    $form['cvterm_name'] = [
      '#type' => 'value',
      '#value' => $cvterm_record->getValue('cvterm.name'),
    ];
    $form['description'] = [
      '#type' => 'table',
      '#rows' => [
        [$this->t('Vocabulary:'), $cvterm_record->getValue('cv.name')],
        [$this->t('Term name:'), $cvterm_record->getValue('cvterm.name')],
        [$this->t('Term definition:'), $cvterm_record->getValue('cvterm.definition')],
        [$this->t('Term Internal ID:'), $cvterm_record->getValue('cvterm.cvterm_id')],
        [$this->t('Database:'), $cvterm_record->getValue('db.name')],
        [$this->t('Accession:'), $cvterm_record->getValue('dbxref.accession')],
        [$this->t('Dbxref Internal ID:'), $cvterm_record->getValue('dbxref.dbxref_id')],
      ],
    ];
    $form['drop_dbxref'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Delete dbxref'),
      '#description' => $this->t('If checked, also delete the database cross reference record associated with this term.'),
      '#default_value' => 1,
    ];
    if ($total_references) {
      $this->messenger()->addError($this->t('There are @n foreign key references to this term, it cannot be deleted.',
        ['@n' => $total_references]));
    }
    else {
      $form['sure'] = [
        '#markup' => '<p><strong>'
        . $this->t('Be VERY CAREFUL when deleting terms, they may be referenced by other records. We recommend you make a backup before deleting terms.')
        . '<br>'
        . $this->t('Are you sure you want to delete this term?')
        . '</strong></p>',
      ];
      $form['submit'] = [
        '#type' => 'submit',
        '#value' => 'Delete',
      ];
    }
    $form['cancel'] = [
      '#type' => 'submit',
      '#value' => 'Cancel',
    ];
    return $form;
  }

  /**
   * Form submit hook for the tripal_chado_cvterm_delete_form form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $triggering_element = $form_state->getTriggeringElement();
    $user_input = $form_state->getUserInput();

    if ($triggering_element['#value'] == 'Delete') {
      $cvterm_id = $form_state->getValue('cvterm_id');
      $cvterm_name = $form_state->getValue('cvterm_name');
      $drop_dbxref = $user_input['drop_dbxref'];
      try {
        $this->cvterm_buddy->deleteCvterm(['cvterm.cvterm_id' => $cvterm_id], ['drop_dbxref' => $drop_dbxref]);
        if ($drop_dbxref) {
          $this->messenger()->addStatus($this->t('The term "@name" and dbxref have been deleted',
            ['@name' => $cvterm_name]));
        }
        else {
          $this->messenger()->addStatus($this->t('The term "@name" has been deleted',
            ['@name' => $cvterm_name]));
        }
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to delete the term "@name": @error',
          ['@name' => $cvterm_name, '@error' => $e->getMessage()]));
      }

      // Views caching can cause deleted term to persist in the view.
      $view = Views::getView('chado_controlled_vocabulary_terms');
      $view->storage->invalidateCaches();
    }

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_cvterm')->toString());
    $response->send();
  }

}
