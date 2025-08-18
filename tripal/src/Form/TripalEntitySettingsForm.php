<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class TripalEntitySettingsForm.
 *
 * @package Drupal\tripal\Form
 *
 * @ingroup tripal
 */
class TripalEntitySettingsForm extends FormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'tripal_entity_settings_form';
  }

  /**
   * Defines the settings form for Tripal Content entities.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   *
   * @return array
   *   Form definition array.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $settings = \Drupal::config('tripal.settings');

    // Define the HTML tags that tripal supports in Tripal Entity titles.
    $allowed_title_tags = $form_state->getValue(
      'allowed_title_tags',
      $settings->get('tripal_entity_type.allowed_title_tags')
    );
    $drupal_entity_field_store = $form_state->getValue(
      'default_cache_backend_field_values',
      $settings->get('tripal_entity_type.default_cache_backend_field_values')
    );

    // Defines the limit of records for a select. Above this value,
    // the form element will change to an autocomplete.
    // Supplying zero or less means always use autocomplete.
    $widget_global_select_limit = $form_state->getValue('widget_global_select_limit',
      $settings->get('tripal_entity_type.widget_global_select_limit'));
    // If nothing is set, create a default value that matches what the code
    // defines as default.
    if (is_null($widget_global_select_limit) or (trim($widget_global_select_limit) === '')) {
      $widget_global_select_limit = 50;
    }

    // The maximum number of linked records of a given type to publish on a
    // single entity.
    $publish_global_max_delta = $form_state->getValue('publish_global_max_delta',
      $settings->get('tripal_entity_type.publish_global_max_delta'));
    // If nothing is set, create a default value that matches what the code
    // defines as default.
    if (is_null($publish_global_max_delta) or (trim($publish_global_max_delta) === '')) {
      $publish_global_max_delta = 100;
    }
    $publish_global_max_delta_inhibit = $form_state->getValue('publish_global_max_delta_inhibit',
      $settings->get('tripal_entity_type.publish_global_max_delta_inhibit'));

    $form['tripal_entity_settings']['#markup'] = 'Settings form for Tripal Content entities.';

    $form['default_cache_backend_field_values'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Cache Backend Storage field values in Drupal'),
      '#description' => $this->t('When enabled, a copy of data from the backend storage will be stored in the Drupal field tables. This is needed for Drupal views filtering and sorting and is recommended for most sites. Changing this setting does not affect already published content. When this option is changed from OFF to ON on an already populated site, all Tripal Content needs to be re-published to take effect. Note: values that have already been cached will not be removed when turning switching option OFF.'),
      '#default_value' => $drupal_entity_field_store,
      '#required' => FALSE,
    ];

    $form['allowed_title_tags'] = [
      '#type' => 'textfield',
      '#title' => $this->t('HTML tags allowed in page titles'),
      '#description' => $this->t('A list of HTML tags that can be used in page titles. Enter one or more tags separated by spaces, for example "em strong u". Leave blank to disable HTML tag rendering. Any tag not in this list will be filtered out if present in a page title. You may need to rebuild the cache for changes to take effect.'),
      '#default_value' => $allowed_title_tags,
      '#required' => FALSE,
    ];

    $form['widget_global_select_limit'] = [
      '#type' => 'number',
      '#min' => 0,
      '#step' => 1,
      '#title' => $this->t('Maximum records for a select'),
      '#description' => $this->t("The value here controls whether a widget select element uses a dropdown select list, or an autocomplete. A dropdown can be difficult to use and is a performance problem if the number of records is large. When the number of records is larger than the value entered here, use an autocomplete if the field supports one. Enter <em>0</em> to indicate that an autocomplete should always be used. This value can be overridden by an individual widget's settings."),
      '#default_value' => $widget_global_select_limit,
      '#required' => FALSE,
    ];

    $form['publish'] = [
      '#type' => 'details',
      '#title' => $this->t('Publishing Options'),
      '#description' => $this->t('Options that control how much tripal content is published'),
      '#open' => TRUE,
    ];

    $form['publish']['publish_global_max_delta'] = [
      '#type' => 'number',
      '#min' => 0,
      '#step' => 1,
      '#title' => $this->t('Maximum number of linked records to publish'),
      '#description' => $this->t("The value here controls how many linked records of a given type are published on a single entity. For example, there may be tens of thousands of genes linked to a single genome annotation, which is too many to display on the entity page. Enter <em>0</em> to indicate that there is no limit (not recommended) This value can be overridden for any specific field by setting the field's cardinality to a value greater than 1."),
      '#default_value' => $publish_global_max_delta,
      '#required' => FALSE,
    ];
    $form['publish']['publish_global_max_delta_inhibit'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Inhibit publish when many records are present'),
      '#description' => $this->t('This setting applies to each field individually on an entity. For a given field with more than the specified maximum number of records: if this setting is enabled then no records will be published for this field and if not, then only the specified maximum number of records will be published for this field.'),
      '#default_value' => $publish_global_max_delta_inhibit,
      '#required' => FALSE,
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save'),
    ];

    return $form;
  }

  /**
   * Validate the list of tags, only letters and spaces allowed.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $allowed_title_tags = $form_state->getValue('allowed_title_tags');

    if (!preg_match("/^[A-Za-z ]*$/", $allowed_title_tags)) {
      $form_state->setErrorByName('allowed_title_tags',
        $this->t('Only letters and spaces can be used.'));
    }
  }

  /**
   * Form submission handler. Saves the form values to tripal settings.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $allowed_title_tags = $form_state->getValue('allowed_title_tags');
    $drupal_entity_field_store = $form_state->getValue('default_cache_backend_field_values');
    // Trim, convert to lower case, and collapse multiple spaces
    // for consistency.
    $allowed_title_tags = strtolower(trim($allowed_title_tags));
    $allowed_title_tags = preg_replace('/ +/', ' ', $allowed_title_tags);

    $widget_global_select_limit = trim($form_state->getValue('widget_global_select_limit'));
    $publish_global_max_delta = trim($form_state->getValue('publish_global_max_delta'));
    $publish_global_max_delta_inhibit = $form_state->getValue('publish_global_max_delta_inhibit');

    // Update configuration.
    $this->configFactory()->getEditable('tripal.settings')
      ->set('tripal_entity_type.default_cache_backend_field_values', $drupal_entity_field_store)
      ->set('tripal_entity_type.allowed_title_tags', $allowed_title_tags)
      ->set('tripal_entity_type.widget_global_select_limit', $widget_global_select_limit)
      ->set('tripal_entity_type.publish_global_max_delta', $publish_global_max_delta)
      ->set('tripal_entity_type.publish_global_max_delta_inhibit', $publish_global_max_delta_inhibit)
      ->save();

    $this->messenger()->addStatus('Settings have been saved.');
  }

}
