<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado biomaterial widget.
 *
 * @FieldWidget(
 *   id = "chado_biomaterial_widget_default",
 *   label = @Translation("Chado Biomaterial Widget"),
 *   description = @Translation("The default biomaterial widget."),
 *   field_types = {
 *     "chado_biomaterial_type_default"
 *   }
 * )
 */
class ChadoBiomaterialWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'biomaterial_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();

    // Get the list of biomaterials.
    $biomaterials = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('biomaterial', 'b');
    $query->fields('b', ['biomaterial_id', 'name']);
    $query->orderBy('name');
    $results = $query->execute();
    while ($biomaterial = $results->fetchObject()) {
      $biomaterials[$biomaterial->biomaterial_id] = $biomaterial->name;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $biomaterial_id = $item_vals['biomaterial_id'] ?? 0;

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
    $elements[$linker_fkey_column] = $element + [
      '#type' => 'select',
      '#options' => $biomaterials,
      '#default_value' => $biomaterial_id,
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
    $this->saveInitialValues($delta, $biomaterial_id, $linker_id, $linker_fkey_column, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    return $this->massageLinkingFormValues('biomaterial_id', $values, $form, $form_state);
  }
}
