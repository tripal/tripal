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
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for confirming deletion of a chado dbxref.
 */
class ChadoDbxrefDeleteForm extends FormBase {

  /**
   * Provide the dbxref buddy instance.
   */
  protected ChadoDbxrefBuddy $dbxref_buddy;

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
    $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_dbxref_delete_form';
  }

  /**
   * A simple form for confirming deletion of a chado dbxref.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int $dbxref_id
   *   The cvterm primary key.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $dbxref_id = NULL) {

    // Get values of the CV term.
    $dbxref_records = $this->dbxref_buddy->getDbxref(['dbxref.dbxref_id' => $dbxref_id]);
    if (!$dbxref_records) {
      throw new \Exception("Invalid dbxref_id \"$dbxref_id\" passed to " . $this->getFormId());
    }
    $dbxref_record = reset($dbxref_records);

    $table_def = $this->chado_connection->schema()->getTableDef('dbxref',
      ['source' => 'database', 'format' => 'default']);
    // Format is [referencing_table =>
    // [dbxref column (dbxref_id) => referencing_table column], ].
    $foreign_keys = $table_def['referenced_by'] ?? [];
    $total_references = 0;
    foreach ($foreign_keys as $referencing_table => $keydef) {
      foreach ($keydef as $refkey) {
        $query = $this->chado_connection->select('1:' . $referencing_table);
        $query->condition($refkey, $dbxref_id, '=');
        $n = $query->countQuery()->execute()->fetchField();
        $total_references += $n;
      }
    }

    $form = [];
    $form['dbxref_id'] = [
      '#type' => 'value',
      '#value' => $dbxref_id,
    ];
    $form['db_name'] = [
      '#type' => 'value',
      '#value' => $dbxref_record->getValue('db.name'),
    ];
    $form['dbxref_accession'] = [
      '#type' => 'value',
      '#value' => $dbxref_record->getValue('dbxref.accession'),
    ];
    $form['description'] = [
      '#type' => 'table',
      '#rows' => [
        [$this->t('Database:'), $dbxref_record->getValue('db.name')],
        [$this->t('Accession:'), $dbxref_record->getValue('dbxref.accession')],
        [$this->t('Dbxref Internal ID:'), $dbxref_record->getValue('dbxref.dbxref_id')],
      ],
    ];
    if ($total_references) {
      $this->messenger()->addError($this->t('There are @n foreign key references to this cross reference, it cannot be deleted.',
        ['@n' => $total_references]));
    }
    else {
      $form['sure'] = [
        '#markup' => '<p><strong>'
        . $this->t('Be VERY CAREFUL when deleting cross references, they may be referenced by other records. We recommend you make a backup before deleting references.')
        . '<br>'
        . $this->t('Are you sure you want to delete this cross reference?')
        . '</strong></p>',
      ];
      $form['submit'] = [
        '#type' => 'submit',
        '#name' => 'delete',
        '#value' => $this->t('Delete'),
      ];
    }
    $form['cancel'] = [
      '#type' => 'submit',
      '#name' => 'cancel',
      '#value' => $this->t('Cancel'),
    ];
    return $form;
  }

  /**
   * Form submit hook for the chado_dbxref_delete_form form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $triggering_element = $form_state->getTriggeringElement();

    if ($triggering_element['#name'] == 'delete') {
      $dbxref_id = $form_state->getValue('dbxref_id');
      $db_name = $form_state->getValue('db_name');
      $dbxref_accession = $form_state->getValue('dbxref_accession');
      try {
        $this->dbxref_buddy->deleteDbxref(['dbxref.dbxref_id' => $dbxref_id], []);
        $this->messenger()->addStatus($this->t('The database cross reference "@db:@acc" has been deleted',
          ['@db' => $db_name, '@acc' => $dbxref_accession]));
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to delete the cross reference "@db:@acc": @error',
          ['@db' => $db_name, '@acc' => $dbxref_accession, '@error' => $e->getMessage()]));
      }

      // Views caching can cause deleted term to persist in the view.
      $view = Views::getView('chado_database_cross_references');
      $view->storage->invalidateCaches();
    }

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_dbxref')->toString());
    $response->send();
  }

}
