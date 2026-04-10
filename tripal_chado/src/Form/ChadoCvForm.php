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
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy;
use Drupal\views\Views;

/**
 * This class provides a form for adding or editing a chado CV.
 */
class ChadoCvForm extends FormBase {

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
    return 'chado_cv_form';
  }

  /**
   * A form to Create or Edit a controlled vocabulary.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @param int|null $cv_id
   *   If editing, the cv primary key.
   *   If adding a new CV, this will be null.
   *
   * @return array
   *   Render array containing elements for the form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, ?int $cv_id = NULL): array {

    // If editing, get existing values.
    $action = 'add';
    $cv_record = NULL;
    if ($cv_id) {
      $action = 'edit';
      $cv_records = $this->cvterm_buddy->getCv(['cv.cv_id' => $cv_id]);
      if (!$cv_records) {
        throw new \Exception("Invalid cv_id \"$cv_id\" passed to chado_cv_form");
      }
      $cv_record = reset($cv_records);
    }

    // Determine form defaults.
    $default_cv_name = '';
    $default_cv_definition = '';
    if ($cv_record) {
      $default_cv_name = $cv_record->getValue('cv.name');
      $default_cv_definition = $cv_record->getValue('cv.definition');
    }

    // Build the form.
    $form['action'] = [
      '#type' => 'value',
      '#value' => $action,
    ];

    $form['cv_id'] = [
      '#type' => 'value',
      '#value' => $cv_id,
    ];

    $form['cv_name'] = [
      '#disabled' => FALSE,
      '#type' => 'textfield',
      '#maxlength' => 255,
      '#title' => $this->t('Controlled Vocabulary (Ontology) Name'),
      '#description' => $this->t('Please enter the name of a controlled vocabulary'),
      '#required' => TRUE,
      '#default_value' => $default_cv_name,
    ];

    $form['cv_definition'] = [
      '#disabled' => FALSE,
      '#type' => 'textarea',
      '#title' => $this->t('Controlled Vocabulary Definition'),
      '#description' => $this->t('Enter the definition of the controlled vocabulary'),
      '#required' => FALSE,
      '#default_value' => $default_cv_definition,
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
      '#markup' => Link::fromTextAndUrl('Cancel', Url::fromUserInput('/admin/tripal/terms/chado_cv'))->toString(),
    ];

    return $form;
  }

  /**
   * Validate the cv form.
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
      $records = $this->cvterm_buddy->getCv([
        'cv.name' => $values['cv_name'],
      ]);
      if ($records) {
        $form_state->setErrorByName('cv_name',
          'The specified controlled vocabulary already exists.');
      }
    }
  }

  /**
   * Submit the CV form.
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
      'cv.name' => $values['cv_name'],
      'cv.definition' => $values['cv_definition'],
    ];
    try {
      if ($action == 'add') {
        $this->cvterm_buddy->insertCv($buddy_values, []);
        $this->messenger()->addStatus($this->t('The vocabulary "@name" has been added.', ['@name' => $values['cv_name']]));
      }
      else {
        $buddy_conditions = ['cv.cv_id' => $values['cv_id']];
        $this->cvterm_buddy->updateCv($buddy_values, $buddy_conditions, []);
        $this->messenger()->addStatus($this->t('The vocabulary "@name" has been updated.', ['@name' => $values['cv_name']]));
      }
    }
    catch (ChadoBuddyException $e) {
      $this->messenger()->addError($this->t('An unexpected error occurred: @error', ['@error' => $e->getMessage()]));
    }

    // Views caching can prevent seeing edits we just made.
    $view = Views::getView('chado_controlled_vocabularies');
    $view->storage->invalidateCaches();

    // @todo This redirect loses any filters we may have applied.
    $response = new RedirectResponse(Url::fromUserInput('/admin/tripal/terms/chado_cv')->toString());
    $response->send();
  }

}
