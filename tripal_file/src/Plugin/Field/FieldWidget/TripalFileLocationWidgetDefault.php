<?php

namespace Drupal\tripal_file\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

#[FieldWidget(
  id: 'tripal_file_location_widget_default',
  label: new TranslatableMarkup('Tripal File Location Widget'),
  description: new TranslatableMarkup('The default file location widget.'),
  field_types: [
    'tripal_file_location_type_default',
  ],
)]
class TripalFileLocationWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
#    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
#    $linker_fkey_column = $storage_settings['linker_fkey_column']
#      ?? $storage_settings['base_column'] ?? 'file_id';
#    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $fileloc_id = $item_vals['fileloc_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $uri = $item_vals['fileloc_uri'] ?? '';
    $rank = $item_vals['fileloc_rank'] ?? $delta;
    $md5checksum = $item_vals['fileloc_md5checksum'] ?? '';
    $size = $item_vals['fileloc_size'] ?? '';
    $filename = $item_vals['fileloc_filename'] ?? '';

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    // pass the field machine name through the form for massageFormValues()
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];

    $elements['fileloc_id'] = [
      '#type' => 'value',
      '#default_value' => $fileloc_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['fileloc_uri'] = [
      '#title' => $this->t('URI'),
      '#type' => 'textfield',
      '#default_value' => $uri,
      '#description' => $this->t('Enter a web URL or a local URI to a file on this site. This value is required.'),
      // It is required, but check this in validation.
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'validateFilelocUri']],
    ];
    $elements['fileloc_filename'] = [
      '#title' => $this->t('File Name'),
      '#type' => 'textfield',
      '#default_value' => $filename,
      '#description' => $this->t('Enter an optional alternative name to display if there is no file name included in the URL.'),
      '#required' => FALSE,
    ];
    $elements['fileloc_md5checksum'] = [
      '#title' => $this->t('MD5 Checksum'),
      '#type' => 'textfield',
      '#default_value' => trim($md5checksum),
      '#description' => $this->t('If the file is local, this will be determined automatically.'),
      '#required' => FALSE,
      '#element_validate' => [[static::class, 'validateMd5checksum']],
    ];
    $elements['fileloc_size'] = [
      '#title' => $this->t('File Size'),
      '#type' => 'number',
      '#default_value' => $size,
      '#min' => 0,
      '#description' => $this->t('If the file is local, this will be determined automatically.'),
      '#required' => FALSE,
    ];
    // Mirror the delta as the rank value.
    $elements['fileloc_rank'] = [
      '#type' => 'value',
      '#default_value' => $delta,
    ];

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
# @todo wondering about the need for this...
#    foreach ($values as $delta => $properties) {
#      $values[$delta]['fileloc_md5checksum'] = trim($values[$delta]['fileloc_md5checksum']);
#    }
    return $values;
  }

  /**
   * Form element validation handler for the uri field.
   *
   * This field is required in the database table, but we do not set
   * it as required in the form because it affects empty records.
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateFilelocUri($element, FormStateInterface $form_state) {
    // element_parents e.g. 0 => "project_relationship", 1 => 0, 2 => "related_record".
    $element_parents = $element['#parents'];
    $delta = $element_parents[1];
    $element_value = $element['#value'];
    $form_state_values = $form_state->getValues();
    $other_values = $form_state_values['file_location'][$delta];
    if ($element_value == '') {
      // If all other fields of the same delta are empty, then we allow
      // the empty value, as it is probably the last delta.
      $other_values_empty = (!$other_values['fileloc_filename']
                          && !$other_values['fileloc_md5checksum']
                          && !$other_values['fileloc_size']);
      if (!$other_values_empty) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('The URI field is a required value'));
      }
    }
  }

  /**
   * Form element validation handler for the MD5 Checksum field.
   *
   * An MD5 checksum must have a length of exactly 32 and be valid
   * hexadecimal characters. An empty value is also allowed.
   *
   * @param array $element
   *   The form element being validated
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) configuration form
   */
  public static function validateMd5checksum($element, FormStateInterface $form_state) {
    $element_parents = $element['#parents'];
    // element_parents e.g. 0 => "project_relationship", 1 => 0, 2 => "related_record"
    $element_value = $element['#value'];
    if ($element_value != '' && $element_value != '                                ') {
      if (!preg_match('/^[0-9A-Fa-f]{32}$/', $element_value)) {
        $form_state->setErrorByName(implode('][', $element_parents),
          t('An MD5 checksum must be exactly 32 characters long and only contain valid hexadecimal characters, or be left blank.'));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return self::defaultSelectSettings() + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    return parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    return parent::settingsSummary();
  }
}
