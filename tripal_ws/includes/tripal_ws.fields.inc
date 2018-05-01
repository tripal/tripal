<?php
/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * Implements hook_bundle_fields_info().
 */
function tripal_ws_bundle_fields_info($entity_type, $bundle) {
  $fields = [];

  // No fields are added programmatically.
  return $fields;
}

/**
 * Implements hook_bundle_instances_info().
 */
function tripal_ws_bundle_instances_info($entity_type, $bundle) {
  $instances = [];

  // No field instances are added programmatically.
  return $instances;
}

/**
 * Implements hook_bundle_create_user_field().
 *
 * A priviledged user has the ability to add new fields to the bundle. The
 * remote__data field is allowed to be added dynamically by the user.
 * But, Drupal doesn't know how to deal with it, so this function is called
 * for any field attached to a TripalEntity bundle type. Any fields whose
 * TripalField::$module argument is set to 'tripal_ws' and that can be
 * added dynamically will result in a call to this function.
 */
function tripal_ws_bundle_create_user_field($new_field, $bundle) {

  // Get the table this bundle is mapped to.
  $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
  $vocab = $term->vocab;
  $params = [
    'vocabulary' => $vocab->vocabulary,
    'accession' => $term->accession,
  ];

  // We allow site admins to add new chado_linker__prop fields to an entity.
  // This function will allow us to properly add them.  But at this point we
  // don't know the controlled vocabulary term.  We'll have to use the
  // defaults and let the user set it using the interface.
  if ($new_field['type'] == 'remote__data') {
    $field_name = $new_field['field_name'];
    $field_type = 'remote__data';

    // First add the field.
    field_create_field([
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_tripal_ws_storage',
      ],
    ]);

    // Now add the instance
    field_create_instance([
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => $new_field['label'],
      'description' => '',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => FALSE,
        'term_vocabulary' => '',
        'term_accession' => '',
        'term_name' => '',
      ],
      'widget' => [
        'type' => 'remote__data_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'remote__data_formatter',
          'settings' => [],
        ],
      ],
    ]);
  }
}
