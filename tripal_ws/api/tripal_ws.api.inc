<?php
/**
 * @file
 *
 * This file provides the Tripal Web Services API: a set of functions for
 * interacting with the Tripal Web Services.
 */

/**
 * @defgroup tripal_ws_api Tripal Web Services
 *
 * @ingroup tripal_api
 * The Tripal Web Services API provides a set of functions for interacting
 * with the Tripal Web Services.
 *
 */

/**
 * Adjust the values of a field for display in web services.
 *
 * This hook should be used sparingly. It is meant primarily to adjust 3rd
 * Party (non Tripal) fields so that they work with web
 * services.  The caller should adjust the $items array as needed.
 * This change only affects the value displayed in web services.  Web services
 * expect that every field have a 'value' element for each of the items. If a
 * field for some reason does not have a 'value' element then this hook will
 * allow setting of that element.
 *
 * @param $items
 *   The list of items for the field.
 * @param $field
 *   The field array.
 * @param $instance
 *   The field instance array.
 *
 * @ingroup tripal_ws_api
 */

function hook_tripal_ws_value(&$items, $field, $instance) {
  // The image module doesn't properly set the 'value' field, so we'll do it
  // here.
  if($field['type'] == 'image' and $field['module'] == 'image') {
    foreach ($items as $delta => $details) {
      if ($items[$delta] and array_key_exists('uri', $items[$delta])) {
        $items[$delta]['value']['schema:url'] = file_create_url($items[$delta]['uri']);
      }
    }
  }
}