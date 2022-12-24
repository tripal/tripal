<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type widget.
 *
 * @FieldWidget(
 *   id = "chado_linker__prop_widget",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_linker__prop"
 *   }
 * )
 */
class chado_linker__prop_widget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $storage_settings = $field_settings['storage_plugin_settings'];

    // Get the primary key and FK columns.
    $base_table = $storage_settings['base_table'];
    $prop_table = $storage_settings['prop_table'];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($prop_table, ['format' => 'Drupal']);
    $fk_col = array_keys($schema_def['foreign keys'][$base_table]['columns'])[0];

    // Allow the user to set the property value.
    $default_value = $items[$delta]->getValue('value')['value'] ?? '';

    $element['value'] = [
      '#type' => 'textarea',
      '#default_value' => $default_value,
      '#title' => '',
      '#description' => '',
      '#rows' => '',
      '#required' => FALSE,
    ];

    // Set the property type_id value.  The user shouldn't edit this. It's
    // built into the field.
    if ($field_settings['termIdSpace'] and $field_settings['termAccession']) {
      $type_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'type_id'));
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $idSpace = $idSpace_manager->loadCollection($field_settings['termIdSpace']);

      $term = $idSpace->getTerm($field_settings['termAccession']);
      $element[$type_term] = [
        '#type' => 'value',
        '#value' => $term->getInternalId(),
      ];
    }

    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($prop_table, 'rank'));
    $element[$rank_term] = [
      '#type' => 'value',
      '#value' => $delta,
    ];

    // Set the foreign key value. This is the record ID to the base table and
    // should not be set by the end user.
    $fk_term = $mapping->getColumnTermId($prop_table, $fk_col);
    $element[$fk_term] = [
      '#type' => 'value',
      '#value' => $items[$delta]->getValue($fk_term),
    ];

    return $element + parent::formElement($items, $delta, $element, $form, $form_state);
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