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
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for adding or editing a chado DB.
 */
class ChadoDbForm extends FormBase {

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
    // Create the buddy instances we will need.
    $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_db_form';
  }

  /**
   * A form to Create or Edit an external database.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int|null $db_id
   *   If editing, the cv primary key.
   *   If adding a new CV, this will be null.
   *
   * @return array
   *   Render array containing elements for the form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, ?int $db_id = NULL): array {

    // If editing, get existing values.
    $action = 'add';
    $db_record = NULL;
    if ($db_id) {
      $action = 'edit';
      $db_records = $this->dbxref_buddy->getDb(['db.db_id' => $db_id]);
      if (!$db_records) {
        throw new \Exception("Invalid db_id \"$db_id\" passed to chado_db_form");
      }
      $db_record = reset($db_records);
    }

    // Determine form defaults.
    $default_db_name = '';
    $default_db_description = '';
    $default_db_url = '';
    $default_db_urlprefix = '';
    if ($db_record) {
      $default_db_name = $db_record->getValue('db.name');
      $default_db_description = $db_record->getValue('db.description');
      $default_db_url = $db_record->getValue('db.url');
      $default_db_urlprefix = $db_record->getValue('db.urlprefix');
    }

    // Build the form.
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['db_id'] = [
      '#type' => 'value',
      '#value' => $db_id,
    ];

    $form['db_name'] = [
      '#disabled' => FALSE,
      '#type' => 'textfield',
      '#maxlength' => 255,
      '#title' => $this->t('External Database Name'),
      '#description' => $this->t('Please enter the name of an external database'),
      '#required' => TRUE,
      '#default_value' => $default_db_name,
    ];

    $form['db_description'] = [
      '#disabled' => FALSE,
      '#type' => 'textarea',
      '#maxlength' => 255,
      '#title' => $this->t('Database Description'),
      '#description' => $this->t('Enter a description of the external database, 255 characters or less'),
      '#required' => FALSE,
      '#default_value' => $default_db_description,
    ];

    $form['db_url'] = [
      '#disabled' => FALSE,
      '#type' => 'textarea',
      '#maxlength' => 255,
      '#title' => $this->t('Database URL'),
      '#description' => $this->t('The main page address describing the database'),
      '#required' => FALSE,
      '#default_value' => $default_db_url,
    ];

    $form['db_urlprefix'] = [
      '#disabled' => FALSE,
      '#type' => 'textarea',
      '#maxlength' => 255,
      '#title' => $this->t('Database URL Prefix'),
      '#description' => $this->t('A URL with placeholders to link to a particular database accession. Use the placeholder "{db}" for the database or "{accession}" for the specific accession.'),
      '#required' => FALSE,
      '#default_value' => $default_db_urlprefix,
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
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/terms/chado_db'))->toString(),
    ];

    return $form;
  }

  /**
   * Validate the db form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();

    // The action will be either 'edit' or 'add'.
    $action = $values['action'];

    // Validate that an existing cv does not match the new cv.
    if ($action == 'add') {
      $records = $this->dbxref_buddy->getDb([
        'db.name' => $values['db_name'],
      ]);
      if ($records) {
        $form_state->setErrorByName('db_name',
          'The specified database already exists.');
      }
    }
  }

  /**
   * Submit the DB form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();

    // The action will be either 'edit' or 'add'.
    $action = $values['action'];

    $buddy_values = [
      'db.name' => $values['db_name'],
      'db.description' => $values['db_description'],
      'db.url' => $values['db_url'],
      'db.urlprefix' => $values['db_urlprefix'],
    ];
    if ($action == 'add') {
      try {
        $this->dbxref_buddy->insertDb($buddy_values, []);
        $this->messenger()->addStatus($this->t('The database "@name" has been added.', ['@name' => $values['db_name']]));
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to insert database "@name": @error',
          ['@name' => $values['db_name'], '@error' => $e->getMessage()]));
      }
    }
    else {
      try {
        $buddy_conditions = ['db.db_id' => $values['db_id']];
        $this->dbxref_buddy->updateDb($buddy_values, $buddy_conditions, []);
        $this->messenger()->addStatus($this->t('The database "@name" has been updated.', ['@name' => $values['db_name']]));
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to update database "@name": @error',
          ['@name' => $values['db_name'], '@error' => $e->getMessage()]));
      }
    }

    // Views caching can prevent seeing edits we just made.
    $view = Views::getView('chado_databases');
    $view->storage->invalidateCaches();

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_db')->toString());
    $response->send();
  }

}
