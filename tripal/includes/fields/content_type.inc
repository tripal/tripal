<?php
/**
 *
 * @param $entity_type
 * @param $entity
 * @param $field
 * @param $instance
 * @param $langcode
 * @param $items
 * @param $display
 */
function tripal_content_type_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => $item['value'],
    );
  }
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 *
 * @param $field
 * @param $entity
 */
function tripal_content_type_field_load($field, $entity) {

  $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
  $field_name = $field['field_name'];
  $entity->{$field_name}['und'][0]['value'] = $bundle->label;
}