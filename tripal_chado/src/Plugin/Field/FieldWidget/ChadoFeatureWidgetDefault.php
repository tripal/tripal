<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado feature widget.
 *
 * @FieldWidget(
 *   id = "chado_feature_widget_default",
 *   label = @Translation("Chado Feature Widget"),
 *   description = @Translation("The default feature widget."),
 *   field_types = {
 *     "chado_feature_type_default"
 *   }
 * )
 */
class ChadoFeatureWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'feature_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_name = $items->getFieldDefinition()->get('field_name');

    // Get the list of features.
    $features = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('feature', 'f');
    $query->leftJoin('cvterm', 'cvt', 'f.type_id = cvt.cvterm_id');
    $query->fields('f', ['feature_id', 'name']);
    $query->addField('cvt', 'name', 'feature_type');
    $query->orderBy('name', 'feature_type');
    $results = $query->execute();
    while ($feature = $results->fetchObject()) {
      $feature_name = $feature->name;
      if ($feature->feature_type) {
        $feature_name .= ' (' . $feature->feature_type . ')';
      }
      $features[$feature->feature_id] = $feature_name;
    }
    natcasesort($features);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $feature_id = $item_vals['feature_id'] ?? 0;

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
    $elements[$linker_fkey_column] = $element + [
      '#type' => 'select',
      '#options' => $features,
      '#default_value' => $feature_id,
      '#empty_option' => '-- Select --',
    ];

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
    return $this->massageLinkingFormValues('feature_id', $values, $form_state);
  }

}
