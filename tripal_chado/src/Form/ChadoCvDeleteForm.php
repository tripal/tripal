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
 * This class provides a form for confirming deletion of a chado CV.
 */
class ChadoCvDeleteForm extends FormBase {

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
    return 'chado_cv_delete_form';
  }

  /**
   * A simple form for confirming deletion of a chado CV.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int $cv_id
   *   The cv primary key.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $cv_id = NULL) {

    // Get values of the CV.
    $cv_records = $this->cvterm_buddy->getCv(['cv.cv_id' => $cv_id]);
    if (!$cv_records) {
      throw new \Exception("Invalid cv_id \"$cv_id\" passed to chado_cv_delete_form");
    }
    $cv_record = reset($cv_records);

    $table_def = $this->chado_connection->schema()->getTableDef('cv', ['source' => 'database', 'format' => 'default']);
    // Format is [referencing_table =>
    // [cv column (cv_id) => referencing_table column], ].
    $foreign_keys = $table_def['referenced_by'] ?? [];
    $total_references = 0;
    foreach ($foreign_keys as $referencing_table => $keydef) {
      foreach ($keydef as $pkey => $refkey) {
        $query = $this->chado_connection->select('1:' . $referencing_table);
        $query->condition($refkey, $cv_id, '=');
        $n = $query->countQuery()->execute()->fetchField();
        $total_references += $n;
      }
    }

    $form = [];
    $form['cv_id'] = [
      '#type' => 'value',
      '#value' => $cv_id,
    ];
    $form['description'] = [
      '#type' => 'table',
      '#rows' => [
        [$this->t('Vocabulary:'), $cv_record->getValue('cv.name')],
        [$this->t('Definition:'), $cv_record->getValue('cv.definition')],
        [$this->t('Vocabulary Internal ID:'), $cv_record->getValue('cv.cv_id')],
      ],
    ];
    if ($total_references) {
      $this->messenger()->addError($this->t('There are @n foreign key references to this vocabulary, it cannot be deleted.',
        ['@n' => $total_references]));
    }
    else {
      $form['sure'] = [
        '#markup' => '<p><strong>'
          . $this->t('Be VERY CAREFUL when deleting controlled vocabularies, they may be referenced by other records. We recommend you make a backup before deleting vocabularies.')
          . '<br>'
          . $this->t('Are you sure you want to delete this vocabulary?')
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
   * Form submit hook for the tripal_chado_cv_delete_form form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $triggering_element = $form_state->getTriggeringElement();

    if ($triggering_element['#value'] == 'Delete') {
      $cv_id = $form_state->getValue('cv_id');
      try {
        $this->cvterm_buddy->deleteCv(['cv.cv_id' => $cv_id], []);
        $this->messenger()->addStatus($this->t('The vocabulary has been deleted'));
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to delete the vocabulary: @error', ['@error' => $e->getMessage()]));
      }

      // Views caching can cause deleted term to persist in the view.
      $view = Views::getView('chado_controlled_vocabularies');
      $view->storage->invalidateCaches();
    }

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_cv')->toString());
    $response->send();
  }

}
