<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Entity\TripalEntityType;

class TripalContentFieldsForm implements FormInterface {
  /**
   * {@inheritDoc}
   * @see \Drupal\Core\Form\FormInterface::getFormId()
   */
  public function getFormId() {
    return 'tripal_content_fields_form';
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\Core\Form\FormInterface::buildForm()
   */
  public function buildForm(array $form, FormStateInterface $form_state,
      TripalEntityType $tripal_entity_type = NULL) {

    /** @var \Drupal\tripal\Services\TripalFieldCollection $tripal_fields **/
    $tripal_field_collection = \Drupal::service('tripal.tripalfield_collection');
    $fields = $tripal_field_collection->discover($tripal_entity_type);

    // Save the tripal entity type for use in the form submit.
    $form['tripal_entity_type'] = [
      '#type' => 'value',
      '#value' => $tripal_entity_type,
    ];

    // Save the field list for use in the form submit.
    $form['discovered_fields'] = [
      '#type' => 'value',
      '#value' => $fields,
    ];

    // Add an instructions fieldset.
    $form['instructions'] = [
      '#type' => 'details',
      '#title' => 'Instructions',
    ];

    $form['instructions']['instr_text'] = [
      '#type' => 'markup',
      '#markup' => t('The following tabs provide the list of fields that were '
          . 'discovered. Fields that are new can be added to the content type '
          . 'by selecting them and clicking the "Add Fields" button. Fields '
          . 'that were discovered but already added are shown but cannot be '
          . 'added again. For module developers, fields that do not pass '
          . 'validation checks are also shown but cannot be added'),
    ];

    // Add the vertical tab element.
    $form['field_type_tabs'] = [
      '#type' => 'vertical_tabs',
      '#title' => 'Discovered Fields',
    ];

    // Add the tabs.
    $form['new_fields_details'] = [
      '#type' => 'details',
      '#title' => 'New Fields',
      '#group' => 'field_type_tabs',
    ];
    $form['existing_fields_details'] = [
      '#type' => 'details',
      '#title' => 'Existing Fields',
      '#group' => 'field_type_tabs',
    ];
    $form['invalid_fields_details'] = [
      '#type' => 'details',
      '#title' => 'Invalid Fields',
      '#group' => 'field_type_tabs',
    ];


    // Add elements for the new fields tab.
    $new_fields = [];
    $new_defaults = [];
    foreach ($fields['new'] as $field) {
      $new_fields[$field['name']] = $field['label'] . " (" . $field['name'] . ")";
      if ($field['description']) {
        $new_fields[$field['name']] .= ': ' . $field['description'];
      }
      $new_defaults[] = $field['name'];
    }
    $new_fields_desc = t('The following is a list of new fields that are '
      . 'compatible with this content type. Select those you want to add.');
    if (empty($new_fields)) {
      $new_fields_desc = t('No new fields were found for this content type');
    }
    $form['new_fields_details']['new_fields_choice'] = [
      '#type' => 'checkboxes',
      '#title' => 'New Fields',
      '#description' => $new_fields_desc,
      '#description_display' => 'before',
      '#options' => $new_fields,
      '#default_value' => $new_defaults,
      '#empty' => t('No new fields were discovered that could be added.'),
    ];

    // Add elements for the existing fields tab.
    $existing_fields = [];
    $existing_defaults = [];
    foreach ($fields['existing'] as $field) {
      $existing_fields[$field['name']] = $field['label'] . " (" . $field['name'] . ")";
      if ($field['description']) {
        $existing_fields[$field['name']] .= ": " . $field['description'];
      }
      $existing_defaults[] = $field['name'];
    }
    $existing_fields_desc = t('The following is a list of fields that were '
      . 'discovered but which are already attached to this content type. They '
      . 'are shown here but cannot be added again.');
    if (empty($existing_fields)) {
      $existing_fields_desc = t('No new fields were found for this content type');
    }
    $form['existing_fields_details']['existing_fields_list'] = [
      '#type' => 'checkboxes',
      '#title' => 'Existing Fields',
      '#description' => $existing_fields_desc,
      '#description_display' => 'before',
      '#options' => $existing_fields,
      '#default_value' => $existing_defaults,
      '#disabled' => TRUE,
    ];

    // Add elements for the invalid fields tab.
    $invalid_fields = [];
    $invalid_defaults = [];
    foreach ($fields['invalid'] as $field) {
      $invalid_fields[$field['name']] = $field['label'] . " (" . $field['name'] . ")";
      if ($field['description']) {
        $invalid_fields[$field['name']] .= ": " . $field['description'];
      }
      $invalid_fields[$field['name']] .= '. Invalid Reason: ' . $field['invalid_reason'];
    }
    $invalid_fields_desc = t('The following fields do not pass validation tests. '
      . 'They need correction by the module developer and cannot be added.');
    if (empty($invalid_fields_desc)) {
      $invalid_fields_desc = t('All fields passed validation tests!');
    }
    $form['invalid_fields_details']['invalid_fields_list'] = [
      '#type' => 'checkboxes',
      '#title' => 'Invalid Fields',
      '#description' => $invalid_fields_desc,
      '#description_display' => 'before',
      '#options' => $invalid_fields,
      '#default_value' => $invalid_defaults,
      '#disabled' => TRUE,
    ];

    $submit_disabled = FALSE;
    if (empty($new_fields)) {
      $submit_disabled = TRUE;
    }
    $form['sumbmit'] = [
      '#type' => 'submit',
      '#value' => 'Add Fields',
      '#disabled' => $submit_disabled,
    ];

    return $form;
  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\Core\Form\FormInterface::submitForm()
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $messenger = \Drupal::messenger();

    // Get submitted form elements.
    $tripal_entity_type = $form_state->getValue('tripal_entity_type');
    $fields = $form_state->getValue('discovered_fields');
    $add_fields = $form_state->getValue('new_fields_choice');

    /** @var \Drupal\tripal\Services\TripalFieldCollection $tripal_fields **/
    $tripal_field_collection = \Drupal::service('tripal.tripalfield_collection');
    foreach ($add_fields as $field_name => $is_checked) {
      $field = $fields['new'][$field_name];
      if($is_checked) {
        $is_added = $tripal_field_collection->addBundleField($field);
        if ($is_added) {
          $messenger->addStatus('Successfully added: ' . $field['label'] . ' (' . $field_name . ').');
        }
        else {
          $messenger->addError('Could not add field: ' . $field['label'] . ' (' . $field_name . '). Check the Drupal recent logs for error messages.');
        }
      }
    }
    $form_state->setRedirect('entity.tripal_entity.field_ui_fields',
        ['tripal_entity_type' => $tripal_entity_type->id()]);
  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\Core\Form\FormInterface::validateForm()
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // No validation is required. The form provides checkboxes that are either
    // on or off.
  }


}