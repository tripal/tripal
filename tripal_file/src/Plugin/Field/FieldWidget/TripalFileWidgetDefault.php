<?php

namespace Drupal\tripal_file\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

#[FieldWidget(
  id: 'tripal_file_widget_default',
  label: new TranslatableMarkup('Tripal File Widget'),
  description: new TranslatableMarkup('The default file widget.'),
  field_types: [
    'tripal_file_type_default',
  ],
)]
class TripalFileWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'file_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $file_id = $item_vals[$linker_fkey_column] ?? 0;

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
    // pass the foreign key name through the form for massageFormValues()
    $elements['linker_fkey_column'] = [
      '#type' => 'value',
      '#default_value' => $linker_fkey_column,
    ];
    // pass the field machine name through the form for massageFormValues()
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];

    // Create a select element specific to this content type
    $options = [
      'base_table' => 'file',
      'column_name' => 'name',
      'type_column' => 'type_id',
      'property_table' => 'file',
    ];
    $select_element = $this->genericSelectElement('file_id', $file_id, $options);
    $elements[$linker_fkey_column] = $element + $select_element;

    // Special processing for the null file which is defined by chado
    if (array_key_exists('#options', $select_element)) {
      $null_file = array_search('null', $select_element['#options']);
      if ($null_file) {
        $select_element['#options'][$null_file] = '- Unknown -';  // This will sort to the top
      }
      natcasesort($select_element['#options']);
    }

    // Insert the select element, either a select or an autocomplete depending
    // on the number of options.
    $elements[$linker_fkey_column] = $element + $select_element;

    // If there is a type_id and the value is not already set, then we want to
    // use the cvterm of the field as the default.
    if (array_key_exists('linker_type_id', $property_definitions)) {

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
    }

    // If there is a rank and it is not already set,
    // then we want to use 0 as the default.
    if (array_key_exists('linker_rank', $property_definitions)) {
      $default_value = $item_vals['linker_rank'] ?? 0;
      $elements['linker_rank'] = [
        '#type' => 'value',
        '#default_value' => $default_value,
      ];
    }

    // If there is a pub_id and it is not already set, then we want to use
    // the null pub which has an id of 1.
    if (array_key_exists('linker_pub_id', $property_definitions)) {
      $default_value = $item_vals['linker_pub_id'] ?? 1;
      $elements['linker_pub_id'] = [
        '#type' => 'value',
        '#default_value' => $default_value,
      ];
    }

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $values = $this->genericSelectMassageFormValues('file_id', $values);
    return $this->massageLinkingFormValues('file_id', $values, $form_state);
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
    return $this->selectSettingsForm($form, $form_state) + parent::settingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    return $this->selectSettingsSummary() + parent::settingsSummary();
  }
}
