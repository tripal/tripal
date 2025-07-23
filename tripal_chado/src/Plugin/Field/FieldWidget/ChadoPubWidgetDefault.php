<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

#[FieldWidget(
  id: 'chado_pub_widget_default',
  label: new TranslatableMarkup('Chado Publication Widget'),
  description: new TranslatableMarkup('The default publication widget.'),
  field_types: [
    'chado_pub_type_default',
  ],
)]
/**
 * Plugin implementation of default Chado publication widget.
 *
 * @FieldWidget(
 *   id = "chado_pub_widget_default",
 *   label = @Translation("Chado Publication Widget"),
 *   description = @Translation("The default publication widget."),
 *   field_types = {
 *     "chado_pub_type_default"
 *   }
 * )
 */
class ChadoPubWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'pub_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $pub_id = $item_vals['pub_id'] ?? 0;

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
      'base_table' => 'pub',
      'column_name' => 'title',
      'type_column' => 'type_id',
      'property_table' => 'pub',
    ];
    $select_element = $this->genericSelectElement('pub_id', $pub_id, $options);
    $elements[$linker_fkey_column] = $element + $select_element;

    // Special processing for the null publication which is defined by chado
    if (array_key_exists('#options', $select_element)) {
      $null_pub = array_search('', $select_element['#options']);
      if ($null_pub) {
        $select_element['#options'][$null_pub] = '- Unknown -';  // This will sort to the top
      }
      natcasesort($select_element['#options']);
    }

    // Insert the select element, either a select or an autocomplete depending
    // on the number of options.
    $elements[$linker_fkey_column] = $element + $select_element;

    // If there are any additional columns present in the linker table,
    // use a default of 1 which will work for type_id or rank.
    // or pub_id. Any existing value will pass through as the default.
    foreach ($property_definitions as $property => $definition) {
      if (($property != 'linker_id') and preg_match('/^linker_/', $property)) {
        $default_value = $item_vals[$property] ?? 1;
        $elements[$property] = [
          '#type' => 'value',
          '#default_value' => $default_value,
        ];
      }
    }

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $values = $this->genericSelectMassageFormValues('pub_id', $values);
    return $this->massageLinkingFormValues('pub_id', $values, $form_state);
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
