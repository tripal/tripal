<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal linker property widget.
 *
 * @FieldWidget(
 *   id = "chado_property_widget_default",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_property_type_default"
 *   }
 * )
 */
class ChadoLinkerPropertyWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();

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
    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $storage_settings = $this->getFieldSetting('storage_plugin_settings');
    $prop_table = $storage_settings['prop_table'];
    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'rank'));

    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      if ($value['value'] == '' and $value['record_id'] == 0) {
        unset($values[$val_key]);
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      if ($value['value'] == '') {
        continue;
      }
      $values[$val_key]['_weight'] = $i;
      $values[$val_key][$rank_term] = $i;
      $i++;
    }

    return $values;
  }
}
