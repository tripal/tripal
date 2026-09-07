<?php

namespace Drupal\tripal_image\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of the default Chado image widget.
 */
#[FieldWidget(
  id: 'chado_eimage_widget_default',
  label: new TranslatableMarkup('Chado Image Widget'),
  description: new TranslatableMarkup('Edits images store in the chado eimage table'),
  field_types: [
    'chado_eimage_type_default',
  ],
)]
class ChadoEimageWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'eimage_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $eimage_id = $item_vals[$linker_fkey_column] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['link'] = [
      '#type' => 'value',
      '#default_value' => $link,
    ];
    // Pass the foreign key name through the form for massageFormValues().
    $elements['linker_fkey_column'] = [
      '#type' => 'value',
      '#default_value' => $linker_fkey_column,
    ];
    // Pass the field machine name through the form for massageFormValues().
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];

    // Use the cvterm of the field as the default linker type.
    if (empty($item['linker_type_id'])) {
      $termIdSpace = $this->getFieldSetting('termIdSpace');
      $termAccession = $this->getFieldSetting('termAccession');
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($termIdSpace);
      $term = $idSpace->getTerm($termAccession);
      $item['linker_type_id'] = $term->getInternalId();
    }

    $elements['linker_type_id'] = [
      '#type' => 'value',
      '#default_value' => $item['linker_type_id'],
    ];

    // Use $delta as the default linker rank.
    $elements['linker_rank'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['linker_rank'] ?? $delta,
    ];

    $elements['eimage_id'] = [
      '#type' => 'value',
      '#default_value' => $eimage_id,
    ];

    $elements['image_uri'] = [
      '#type' => 'textarea',
      '#default_value' => $item_vals['image_uri'] ?? '',
      '#maxlength' => 255,
      '#rows' => 1,
      '#description' => $this->t('URI for a locally or remotely stored image file'),
      '#element_validate' => [[$this, 'validateUri']],
    ];

    // The widget should populate this behind the scenes if we implement upload.
    $elements['eimage_type'] = [
      '#type' => 'textarea',
      '#default_value' => $item_vals['eimage_type'] ?? '',
      '#maxlength' => 255,
      '#rows' => 1,
      '#description' => $this->t('MIME type, e.g. png, jpg for uuencoded images stored in the eimage table.'),
      '#element_validate' => [[$this, 'validateEimageType']],
    ];

    $elements['eimage_data'] = [
      '#type' => 'textarea',
      // Normalize is not removing \r for some reason.
      '#normalize_newlines' => TRUE,
      '#default_value' => $item_vals['eimage_data'] ?? '',
      '#rows' => 5,
      '#description' => $this->t('UUencoded image'),
      '#element_validate' => [[$this, 'validateEimageData']],
      '#attributes' => [
        'style' => 'font-family: monospace;',
      ],
    ];

    // Save some initial values to allow later handling of the "Remove" button.
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    if (!$values) {
      return $values;
    }

    $linker_key = 'linker_id';
    $first_delta = array_key_first($values);
    $field_name = $values[$first_delta]['field_name'];

    // Handle any empty values so that chado storage properly
    // deletes the linking record in chado. This happens when an
    // existing record is removed from the autocomplete field.
    $retained_records = [];
    foreach ($values as $delta => $value) {

      if ($value[$linker_key]) {
        $retained_records[$delta] = $value[$linker_key];
      }
      if ($value['eimage_data'] == '' && $value['image_uri'] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but neither image reference, this
          // means we need to pass in this record to chado storage
          // to have the linker record be deleted there.
          $values[$delta]['eimage_id'] = 0;
          $values[$delta]['eimage_type'] = '';
        }
        else {
          // If there is no record_id, then it is the empty
          // field at the end of the list, and can be ignored.
          unset($values[$delta]);
        }
      }
    }
    // Handle items that were removed with the "Remove" button.
    $this->handleRemove($values, $form, $form_state, $field_name, $retained_records);

    // Reset the weights
    $i = 0;
    foreach ($values as $delta => $value) {
      $values[$delta]['_weight'] = $i;
      $i++;
    }
    return $values;
  }

  /**
   * Handle items removed using the "Remove" button
   *
   * If there were any values in the initial values that are not
   * present in the current form state, then an existing record
   * was deleted by clicking the "Remove" button. We need to
   * include these in the values array so that chado storage is
   * informed to delete the linking record.
   *
   * @param array &$values
   *   The submitted form values produced by the widget.
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   * @param string $field_name
   *   The machine name of this field.
   * @param array $retained_records
   *   Records in the form state at the time of massaging
   *
   * @return void
   *   Changes are made to the $values array
   */
  protected function handleRemove(
    array &$values,
    array $form,
    FormStateInterface $form_state,
    string $field_name,
    array $retained_records,
  ): void {
    $next_delta = $values ? array_key_last($values) + 1 : 0;
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'][$field_name] ?? [];
    foreach ($initial_values as $initial_value) {
      // For initial values, the key is always 'linker_id', regardless of
      // the value of $linker_key.
      $linker_id = $initial_value['linker_id'] ?? 0;
      if ($linker_id and !in_array($linker_id, $retained_records)) {
        // This item was removed from the form. Add back a value
        // so that chado storage knows to remove the chado record.
        $values[$next_delta]['linker_id'] = $linker_id;
        $values[$next_delta]['eimage_id'] = 0;
        $next_delta++;
      }
    }
  }

  /**
   * Form element validation handler for eimage_data.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateEimageData($element, FormStateInterface $form_state) {
    $valid = TRUE;
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];
    if ($element_value != '') {
      try {
        $decoded = convert_uudecode($element_value);
      }
      catch (\Exception $e) {
        $valid = FALSE;
      }
      if (!$valid) {
        $form_state->setErrorByName(implode('][', $element_parents),
          $this->t('This is not valid UUencoded data'));
      }
    }
  }

  /**
   * Form element validation handler for eimage_data.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateEimageType($element, FormStateInterface $form_state) {
    $valid = TRUE;
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];
    $values = $form_state->getValues();
    $eimage_data = $values[$element_parents[0]][$element_parents[1]]['eimage_data'] ?? '';
    $image_uri = $values[$element_parents[0]][$element_parents[1]]['image_uri'] ?? '';
    if ($eimage_data != '' && $element_value == '') {
      $form_state->setErrorByName(implode('][', $element_parents),
        $this->t('An image type is required'));
    }
    // Type has a not null constraint, add a default if necessary.
    if ($image_uri != '' && $element_value == '') {
      $form_state->setValueForElement($element,  'uri');
    }
  }

  /**
   * Form element validation handler for image_uri.
   *
   * @param array $element
   *   The form element being validated.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateUri($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    $element_value = $element['#value'];
    if ($element_value != '') {
      if (preg_match('/^public:/i', $element_value)) {
        $path = \Drupal::service('file_system')->realpath($element_value);
        if (!is_file($path)) {
          $form_state->setErrorByName(implode('][', $element_parents),
            $this->t('URI does not point to an existing image file'));
        }
        else if (!is_readable($path)) {
          $form_state->setErrorByName(implode('][', $element_parents),
            $this->t('Image file is not readable'));
        }
      }
      else {
        $valid = FALSE;
        try {
          // Drupal's HTTP client service.
          $client = \Drupal::httpClient();
          // A HEAD request doesn't download the page body.
          $response = $client->head($url, ['timeout' => 5]);
          if ($response->getStatusCode() === 200) {
            $valid = TRUE;
          }
        }
        catch (RequestException $e) {
          // The URL returned 404, 500, or timed out.
        }
        if (!$valid) {
          $form_state->setErrorByName(implode('][', $element_parents),
            $this->t('URI is not accessible'));
        }
      }
    }
  }

}
