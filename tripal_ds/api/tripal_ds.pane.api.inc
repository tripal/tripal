<?php
/**
 * @file
 * Tripal offers a display suite integration for additional fields.
 */

/**
 * @defgroup tripal_ds_pane_api Tripal DS API
 * @ingroup tripal_ds_api
 * @{
 * Tripal DS allows for the extension of the display suite module interface used
 * to display the tripal entity fields. The API allows for other modules to
 * integrate Tripal Pane creation, placement, and formatting.
 * @}
 */

/**
 * Adds a new Tripal Pane to a bundle.
 *
 * @param $field
 *    The machine name for the field.
 * @param $bundle_name
 *    The name of the bundle the pane is being added to.
 * @param $field_label
 *    The human readable name for the field.
 *
 * Example usage:
 *
 * @code
 *  field_create_field($field);
 *  field_create_instance($instance);
 *  tripal_ds_field_create_field($field_label, $field, $bundle);
 *
 * @ingroup tripal_ds_pane_api
 *
 */
function tripal_ds_create_field($field_label, $field_name, $bundle_name) {
  $field_name = str_replace(' ', '_', strtolower($field_name));
  //Build the rest of the passes parameters.
  $group_field_name = 'gp_' . $field_name;
  //Create the field groups.
  tripal_ds_add_generic_field_group($bundle_name, $field_label, $group_field_name, $field_name);
  //Place the field groups in the layout.
  tripal_ds_update_ds_layout($bundle_name, $field_name, $group_field_name);
}
