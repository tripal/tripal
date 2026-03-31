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
 * This class provides a form for confirming deletion of a chado DB.
 */
class ChadoDbDeleteForm extends FormBase {

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
    return 'chado_db_delete_form';
  }

  /**
   * A simple form for confirming deletion of a chado DB.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int $db_id
   *   The db primary key.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $db_id = NULL) {

    // Get values of the DB.
    $db_records = $this->dbxref_buddy->getDb(['db.db_id' => $db_id]);
    if (!$db_records) {
      throw new \Exception("Invalid db_id \"$db_id\" passed to chado_db_delete_form");
    }
    $db_record = reset($db_records);

    $table_def = $this->chado_connection->schema()->getTableDef('db', ['source' => 'database', 'format' => 'default']);
    // Format is [referencing_table =>
    // [db column (db_id) => referencing_table column], ].
    $foreign_keys = $table_def['referenced_by'] ?? [];
    $total_references = 0;
    foreach ($foreign_keys as $referencing_table => $keydef) {
      foreach ($keydef as $pkey => $refkey) {
        $query = $this->chado_connection->select('1:' . $referencing_table);
        $query->condition($refkey, $db_id, '=');
        $n = $query->countQuery()->execute()->fetchField();
        $total_references += $n;
      }
    }

    $form = [];
    $form['db_id'] = [
      '#type' => 'value',
      '#value' => $db_id,
    ];
    $form['warning'] = [
      '#markup' => '<p><strong>'
      . $this->t('Be VERY CAREFUL when deleting databases, they may be referenced by other records. We recommend you make a backup before deleting vocabularies.')
      . '</strong></p>',
    ];
    $form['description'] = [
      '#type' => 'table',
      '#rows' => [
        [$this->t('Database Name:'), $db_record->getValue('db.name')],
        [$this->t('Description:'), $db_record->getValue('db.description')],
        [$this->t('URL:'), $db_record->getValue('db.url')],
        [$this->t('URL Prefix:'), $db_record->getValue('db.urlprefix')],
        [$this->t('Database Internal ID:'), $db_record->getValue('db.db_id')],
      ],
    ];
    if ($total_references) {
      $form['sure'] = [
        '#markup' => $this->t('<p><strong>There are @n foreign key references to this database, it cannot be deleted.</strong></p>',
          ['@n' => $total_references]),
      ];
    }
    else {
      $form['sure'] = [
        '#markup' => $this->t('<p><strong>Are you sure you want to delete this database?</strong></p>'),
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
   * Form submit hook for the tripal_chado_db_delete_form form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $db_id = $form_state->getValue('db_id');
    try {
      $this->dbxref_buddy->deleteDb(['db.db_id' => $db_id], []);
      $this->messenger()->addStatus($this->t('The database has been deleted'));
    }
    catch (ChadoBuddyException $e) {
      $this->messenger()->addError($this->t('Unable to delete the database: @error', ['@error' => $e->getMessage()]));
    }

    // Views caching can cause deleted database to persist in the view.
    $view = Views::getView('chado_databases');
    $view->storage->invalidateCaches();

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_db')->toString());
    $response->send();
  }

}
