<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal linker property widget.
 *
 * @FieldWidget(
 *   id = "chado_property_widget_default",
 *   label = @Translation("Chado Property: Long Text"),
 *   description = @Translation("Provides a long text widget for Chado Properties using a formatted textarea."),
 *   field_types = {
 *     "chado_property_type_default"
 *   }
 * )
 */
class ChadoPropertyWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $field_name = $items->getFieldDefinition()->get('field_name');

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $prop_id = $item_vals['prop_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $default_value = $item_vals['value'] ?? '';
    $term_id = NULL;
    if ($field_settings['termIdSpace'] and $field_settings['termAccession']) {
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($field_settings['termIdSpace']);

      $term = $idSpace->getTerm($field_settings['termAccession']);
      $term_id = $term->getInternalId();
    }

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['prop_id'] = [
      '#type' => 'value',
      '#default_value' => $prop_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#value' => $linker_id,
    ];
    $elements['type_id'] = [
      '#type' => 'value',
      '#value' => $term_id,
    ];
    // pass the field machine name through the form for massageFormValues()
    $elements['field_name'] = [
      '#type' => 'value',
      '#default_value' => $field_name,
    ];
    $elements['value'] = $element + [
      '#type' => 'textarea',
      '#default_value' => $default_value,
      '#title' => '',
      '#description' => '',
      '#rows' => '',
      '#required' => FALSE,
    ];
    $elements['rank'] = [
      '#type' => 'value',
      '#value' => $delta,
    ];

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_name, $prop_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Look up the rank term
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $storage_settings = $this->getFieldSetting('storage_plugin_settings');
    $prop_table = $storage_settings['prop_table'];
    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'rank'));

    // Call parent massage helper function
    $values = $this->massagePropertyFormValues('value', $values, $form_state, $rank_term, 'prop_id');

    return $values;
  }
}
