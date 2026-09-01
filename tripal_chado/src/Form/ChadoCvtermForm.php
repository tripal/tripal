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
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for adding or editing a chado CV term.
 */
class ChadoCvtermForm extends FormBase {

  /**
   * Provide the dbxref buddy instance.
   */
  protected ChadoDbxrefBuddy $dbxref_buddy;

  /**
   * Provide the cvterm buddy instance.
   */
  protected ChadoCvtermBuddy $cvterm_buddy;

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
    $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_cvterm_form';
  }

  /**
   * A form to Create or Edit a controlled vocabulary term.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int|null $cvterm_id
   *   If editing, the cvterm primary key.
   *   If adding a new term, this will be null.
   *
   * @return array
   *   Render array containing elements for the form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, ?int $cvterm_id = NULL): array {

    // If editing, get existing values.
    $action = 'add';
    $cvterm_record = NULL;
    if ($cvterm_id) {
      $action = 'edit';
      $cvterm_records = $this->cvterm_buddy->getCvterm(['cvterm.cvterm_id' => $cvterm_id]);
      if (!$cvterm_records) {
        throw new \Exception("Invalid cvterm_id \"$cvterm_id\" passed to " . $this->getFormId());
      }
      $cvterm_record = reset($cvterm_records);
    }

    // Determine form defaults.
    $default_cv_name = '';
    $default_cvterm_name = '';
    $default_cvterm_definition = '';
    $default_db_name = '';
    $default_dbxref_accession = '';
    $default_dbxref_version = '';
    $default_dbxref_description = '';
    $default_cvterm_is_relationshiptype = 0;
    $default_cvterm_is_obsolete = 0;
    if ($cvterm_record) {
      $default_cv_name = $cvterm_record->getValue('cv.name') . ' (' . $cvterm_record->getValue('cv.cv_id') . ')';
      $default_cvterm_name = $cvterm_record->getValue('cvterm.name');
      $default_cvterm_definition = $cvterm_record->getValue('cvterm.definition');
      $default_db_name = $cvterm_record->getValue('db.name') . ' (' . $cvterm_record->getValue('db.db_id') . ')';
      $default_dbxref_accession = $cvterm_record->getValue('dbxref.accession');
      $default_dbxref_version = $cvterm_record->getValue('dbxref.version');
      $default_dbxref_description = $cvterm_record->getValue('dbxref.description');
      $default_cvterm_is_relationshiptype = $cvterm_record->getValue('cvterm.is_relationshiptype');
      $default_cvterm_is_obsolete = $cvterm_record->getValue('cvterm.is_obsolete');
    }

    // Build the form.
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['cvterm_id'] = [
      '#type' => 'value',
      '#value' => $cvterm_id,
    ];

    $is_disabled = ($action == 'edit');

    $form['cv_name'] = [
      '#disabled' => $is_disabled,
      '#type' => 'textfield',
      '#title' => $this->t('Controlled Vocabulary (Ontology) Name'),
      '#description' => $this->t('Please select an existing controlled vocabulary'),
      '#required' => TRUE,
      '#default_value' => $default_cv_name,
      '#maxlength' => 255,
      '#autocomplete_route_name' => 'tripal_chado.generic_autocomplete',
      '#autocomplete_route_parameters' => [
        'base_table' => 'cv',
        'column_name' => 'name',
        'type_column' => 'x',
        'property_table' => 'cv',
        'match_limit' => 10,
        'type_id' => 0,
      ],
    ];

    $form['cvterm_name'] = [
      '#disabled' => FALSE,
      '#type' => 'textfield',
      '#maxlength' => 1024,
      '#title' => $this->t('Controlled Vocabulary Term'),
      '#description' => $this->t('Enter the name of the controlled vocabulary term'),
      '#required' => TRUE,
      '#default_value' => $default_cvterm_name,
    ];

    $form['cvterm_definition'] = [
      '#disabled' => FALSE,
      '#type' => 'textarea',
      '#title' => $this->t('Term Definition'),
      '#description' => $this->t('Enter the definition of the controlled vocabulary term'),
      '#required' => FALSE,
      '#default_value' => $default_cvterm_definition,
    ];

    // These are not checkboxes, on the off chance that someone stores
    // values other than 0 or 1 here, because the chado table would allow it.
    $form['cvterm_is_relationshiptype'] = [
      '#disabled' => FALSE,
      '#type' => 'number',
      '#min' => 0,
      '#max' => 1,
      '#title' => $this->t('Is Relationshiptype'),
      '#description' => $this->t('This condition is entered as a number, 1 = defines a relationship type, 0 = normal term (default)'),
      '#required' => FALSE,
      '#default_value' => $default_cvterm_is_relationshiptype,
    ];

    $form['cvterm_is_obsolete'] = [
      '#disabled' => FALSE,
      '#type' => 'number',
      '#min' => 0,
      '#max' => 1,
      '#title' => $this->t('Is Obsolete'),
      '#description' => $this->t('This condition is entered as a number, 1 = obsolete, 0 = not obsolete (default)'),
      '#required' => FALSE,
      '#default_value' => $default_cvterm_is_obsolete,
    ];

    $form['db_name'] = [
      '#disabled' => ($action == 'edit'),
      '#type' => 'textfield',
      '#title' => $this->t('External Database Name'),
      '#description' => $this->t('All terms must be associated with a database. If there is no database for this term (e.g. it is a custom term specific to this site) then select the database "null" or consider creating a database specific for your site and use that anytime you would like to add terms.'),
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
      '#disabled' => ($action == 'edit'),
      '#type' => 'textfield',
      '#title' => $this->t('External Database Accession'),
      '#description' => $this->t('If this term has an existing accession (unique identifier) in the database please enter that here. If the accession is numeric with a database prefix (e.g. GO:003023), please enter just the numeric value. The database prefix will be appended whenever the term is displayed. If you do not have a numeric value consider entering the term name as the accession.'),
      '#required' => TRUE,
      '#default_value' => $default_dbxref_accession,
    ];

    $form['dbxref_version'] = [
      '#disabled' => ($action == 'edit'),
      '#type' => 'textfield',
      '#title' => $this->t('External Database Accession Version'),
      '#description' => $this->t('Optional, this field is normally left blank'),
      '#required' => FALSE,
      '#default_value' => $default_dbxref_version,
    ];

    $form['dbxref_description'] = [
      '#disabled' => ($action == 'edit'),
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
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/terms/chado_cvterm'))->toString(),
    ];

    return $form;
  }

  /**
   * Validate the cvterm form.
   *
   * @param array &$form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $autocomplete = new ChadoGenericAutocompleteController();
    $values = $form_state->getValues();

    // The action will be either 'edit' or 'add'.
    $action = $values['action'];

    // Validate that the cv exists.
    $cv_id = $autocomplete->getPkeyId($values['cv_name']);
    $records = $this->cvterm_buddy->getCv(['cv.cv_id' => $cv_id]);
    if (!$records) {
      $form_state->setErrorByName('cv_name',
        'The specified controlled vocabulary does not exist. Make sure it includes the internal ID value in parentheses.');
    }

    // Validate that the db exists.
    $db_id = $autocomplete->getPkeyId($values['db_name']);
    $records = $this->dbxref_buddy->getDb(['db.db_id' => $db_id]);
    if (!$records) {
      $form_state->setErrorByName('db_name',
        'The specified external database does not exist. Make sure it includes the internal ID value in parentheses.');
    }

    // The dbxref accession can already exist only when editing.
    if ($action == 'add') {
      $records = $this->dbxref_buddy->getDbxref([
        'dbxref.db_id' => $db_id,
        'dbxref.accession' => $values['dbxref_accession'],
        'dbxref.version' => $values['dbxref_version'],
      ]);
      if ($records) {
        $form_state->setErrorByName('dbxref_accession',
          'The specified accession and version already exist for this database. It cannot be used for more than one term.');
      }
    }

    // Validate that an existing cvterm does not match the new term.
    if ($action == 'add') {
      $records = $this->cvterm_buddy->getCvterm([
        'cvterm.cv_id' => $cv_id,
        'cvterm.name' => $values['cvterm_name'],
        'cvterm.is_obsolete' => $values['cvterm_is_obsolete'],
      ]);
      if ($records) {
        $form_state->setErrorByName('cvterm_name',
          'The specified vocabulary term and value of "Is Obsolete" already exist for this vocabulary.');
      }
    }
  }

  /**
   * Submit the Cvterm form.
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

    $cv_id = $autocomplete->getPkeyId($values['cv_name']);
    $db_id = $autocomplete->getPkeyId($values['db_name']);
    $cvterm_id = $values['cvterm_id'];

    $buddy_values = [
      'cvterm.cv_id' => $cv_id,
      'cvterm.name' => $values['cvterm_name'],
      'cvterm.definition' => $values['cvterm_definition'],
      'cvterm.is_obsolete' => $values['cvterm_is_obsolete'],
      'cvterm.is_relationshiptype' => $values['cvterm_is_relationshiptype'],
    ];
    // For the 'add' action, we will have to also create the dbxref.
    if ($action == 'add') {
      $buddy_values += [
        'dbxref.db_id' => $db_id,
        'dbxref.accession' => $values['dbxref_accession'],
        'dbxref.version' => $values['dbxref_version'],
        'dbxref.description' => $values['dbxref_description'],
      ];
      try {
        $this->cvterm_buddy->insertCvterm($buddy_values, []);
        $this->messenger()->addStatus($this->t('The vocabulary term "@name" has been added.',
          ['@name' => $values['cvterm_name']]));
        $success = TRUE;
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to insert cv term "@name": @error',
          ['@name' => $values['cvterm_name'], '@error' => $e->getMessage()]));
      }
    }
    else {
      $buddy_conditions = [
        'cvterm.cvterm_id' => $cvterm_id,
      ];
      try {
        $this->cvterm_buddy->updateCvterm($buddy_values, $buddy_conditions, []);
        $this->messenger()->addStatus($this->t('The vocabulary term "@name" has been updated.', ['@name' => $values['cvterm_name']]));
        $success = TRUE;
      }
      catch (ChadoBuddyException $e) {
        $this->messenger()->addError($this->t('Unable to update cv term "@name": @error',
          ['@name' => $values['cvterm_name'], '@error' => $e->getMessage()]));
      }
    }

    if ($success) {
      // Views caching can prevent seeing edits we just made.
      $view = Views::getView('chado_controlled_vocabulary_terms');
      $view->storage->invalidateCaches();

      // @todo This redirect loses any filters we may have applied.
      $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_cvterm')->toString());
      $response->send();
    }
  }

}
