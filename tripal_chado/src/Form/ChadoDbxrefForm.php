<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for adding or editing a chado dbxref.
 */
class ChadoDbxrefForm extends FormBase {

  /**
   * Provide the dbxref buddy instance.
   */
  protected ChadoDbxrefBuddy $dbxref_buddy;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * Class constructor.
   */
  public function __construct(
    protected readonly ChadoBuddyPluginManager $buddy_manager,
  ) {
    // Create the buddy instance we will need.
    $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_dbxref_form';
  }

  /**
   * A form to Create or Edit a database crossreference.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int|null $dbxref_id
   *   If editing, the dbxref primary key.
   *   If adding a new dbxref, this will be null.
   *
   * @return array
   *   Render array containing elements for the form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, ?int $dbxref_id = NULL): array {

    // If editing, get existing values.
    $action = 'add';
    $dbxref_record = NULL;
    if ($dbxref_id) {
      $action = 'edit';
      $dbxref_records = $this->dbxref_buddy->getDbxref(['dbxref.dbxref_id' => $dbxref_id]);
      if (!$dbxref_records) {
        throw new \Exception("Invalid dbxref_id \"$dbxref_id\" passed to chado_dbxref_form");
      }
      $dbxref_record = reset($dbxref_records);
    }

    // Determine form defaults.
    $default_db_name = '';
    $default_dbxref_accession = '';
    $default_dbxref_version = '';
    $default_dbxref_description = '';
    if ($dbxref_record) {
      $default_db_name = $dbxref_record->getValue('db.name') . ' (' . $dbxref_record->getValue('db.db_id') . ')';
      $default_dbxref_accession = $dbxref_record->getValue('dbxref.accession');
      $default_dbxref_version = $dbxref_record->getValue('dbxref.version');
      $default_dbxref_description = $dbxref_record->getValue('dbxref.description');
    }

    // Build the form.
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['dbxref_id'] = [
      '#type' => 'value',
      '#value' => $dbxref_id,
    ];

    $form['db_name'] = [
      '#disabled' => ($action == 'edit'),
      '#type' => 'textfield',
      '#title' => $this->t('External Database Name'),
      '#description' => $this->t('This is the external database that is referenced by the accession in the cross reference. The database must already exist.'),
      '#required' => TRUE,
      '#default_value' => $default_db_name,
      '#maxlength' => 255,
      '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
      '#autocomplete_route_parameters' => [
        'base_table' => 'db',
        'column_name' => 'name',
        'type_column' => 'x',
        'property_table' => 'db',
        'match_limit' => 10,
        'type_id' => 0,
      ],
    ];

    $form['dbxref_accession'] = [
      '#type' => 'textfield',
      '#title' => $this->t('External Database Accession'),
      '#description' => $this->t('This is the accession (unique identifier) in the database. The accession format depends on the conventions each database uses, it may be numeric, or text, or a combination of both.'),
      '#required' => TRUE,
      '#default_value' => $default_dbxref_accession,
    ];

    $form['dbxref_version'] = [
      '#type' => 'textfield',
      '#title' => $this->t('External Database Accession Version'),
      '#description' => $this->t('Optional, this field is normally left blank'),
      '#required' => FALSE,
      '#default_value' => $default_dbxref_version,
    ];

    $form['dbxref_description'] = [
      '#type' => 'textfield',
      '#title' => $this->t('External Database Accession Description'),
      '#description' => $this->t('Optional, this field is normally left blank'),
      '#required' => FALSE,
      '#default_value' => $default_dbxref_description,
    ];

    if ($action == 'edit') {
      $value = $this->t('Save');
    }
    if ($action == 'add') {
      $value = $this->t('Add');
    }
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $value,
    ];

    $form['cancel'] = [
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/terms/chado_dbxref'))->toString(),
    ];

    return $form;
  }

  /**
   * Validate the dbxref form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $autocomplete = new ChadoGenericAutocompleteController();
    $values = $form_state->getValues();

    // Validate that the db exists.
    $db_id = $autocomplete->getPkeyId($values['db_name']);
    $records = $this->dbxref_buddy->getDb(['db.db_id' => $db_id]);
    if (!$records) {
      $form_state->setErrorByName('db_name',
        'The specified external database does not exist. Make sure it includes the internal ID value in parentheses.');
    }
  }

  /**
   * Submit the dbxref form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $autocomplete = new ChadoGenericAutocompleteController();
    $values = $form_state->getValues();

    // The action will be either 'edit' or 'add'.
    $action = $values['action'];
    $success = FALSE;

    $db_id = $autocomplete->getPkeyId($values['db_name']);

    $buddy_values = [
      'dbxref.db_id' => $db_id,
      'dbxref.accession' => $values['dbxref_accession'],
      'dbxref.version' => $values['dbxref_version'],
      'dbxref.description' => $values['dbxref_description'],
    ];
    if ($action == 'add') {
      try {
        $this->dbxref_buddy->insertDbxref($buddy_values, []);
        $this->messenger()->addStatus($this->t('The database crossreference "@db:@acc" has been added.',
          ['@db' => $values['db_name'], '@acc' => $values['dbxref_accession']]));
        $success = TRUE;
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to add the cross reference "@db:@acc": @error',
          ['@db' => $values['db_name'], '@acc' => $values['dbxref_accession'], '@error' => $e->getMessage()]));
      }
    }
    else {
      $buddy_conditions = [
        'dbxref.dbxref_id' => $values['dbxref_id'],
      ];
      try {
        $this->dbxref_buddy->updateDbxref($buddy_values, $buddy_conditions, []);
        $this->messenger()->addStatus($this->t('The database crossreference "@db:@acc" has been updated.',
          ['@db' => $values['db_name'], '@acc' => $values['dbxref_accession']]));
        $success = TRUE;
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to update the cross reference "@db:@acc": @error',
          ['@db' => $values['db_name'], '@acc' => $values['dbxref_accession'], '@error' => $e->getMessage()]));
      }
    }

    if ($success) {
      // Views caching can prevent seeing edits we just made.
      $view = Views::getView('chado_database_cross_references');
      $view->storage->invalidateCaches();

      // @todo This redirect loses any filters we may have applied.
      $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_dbxref')->toString());
      $response->send();
    }
  }

}
