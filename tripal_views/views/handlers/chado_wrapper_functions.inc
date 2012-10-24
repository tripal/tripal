<?php

/**
 * @file
 * TODO: add documentation
 */

/**
 * Splits an SQL array of results in a single field
 * into a php array
 *
 * @param $field
 *   An SQL array (ie: {"",^(.*)$,646,tripal_analysis_blast} )
 * @return
 *   A PHP version of the SQL array (ie: array('','^(.*)$','646','tripal_analysis_blast') )
 */
function chado_wrapper_split_array_agg_results($handler, &$values) {

  if ($handler->aggregated) {
    foreach ($values as $k => $v) {

      if (preg_match('/^{(.*)}$/', $v->{$handler->field_alias}, $matches)) {
        $values[$k]->{$handler->field_alias} = str_getcsv($matches[1]);
      }
      else {
        $values[$k]->{$handler->field_alias} = array();
      }
    }
  }

  return $values;
}

/**
 * Determines if the current field is generated via the aggregated join handler
 *
 * @param $handler
 *   pass in $this from handler::query()
 *
 * @return
 *   TRUE/FALSE if or if not the field has the aggregated join handler
 */
function chado_wrapper_is_aggregated_by_join($handler) {
  $aggregated = FALSE;

  $table = $handler->query->get_table_info($handler->table);
  if (preg_match('/aggregator/', $table['join']->definition['handler'])) {
    $aggregated = TRUE;
  }

  return $aggregated;
}

/**
 * Determines if the current field is generated via the result returned
 *
 * @param $handler
 *   pass in $this from handler::pre_render()
 * @param $values
 *   pass in $values from handler::pre_render()
 *
 * @return
 *   TRUE/FALSE if or if not the field is aggregated
 */
function chado_wrapper_is_aggregated_by_result($handler, $values) {
  $aggregated = FALSE;


  $i = 0;
  while (empty($values[$i]->{$handler->field_alias}) AND $i <= sizeof($values)) {
    $i++;
  }

  if (preg_match('/^{.*}$/', $values[$i]->{$handler->field_alias})) {
    $aggregated = TRUE;
  }

  return $aggregated;
}

function chado_wrapper_render_items($handler, $values) {

  // If it's aggregated (an array), then render each part
  // using the parent render functionality
  if ($handler->aggregated) {

    $items = array();

    $parts = $values->{$handler->field_alias};
    foreach ($parts as $p) {
      $v[ $handler->field_alias ] = $p;
      $val = (object) $v;
      $items[] = $handler->parent_render($val);
      unset($v, $val);
    }

    if ($handler->options['type'] == 'separator') {
      return implode(check_plain($handler->options['separator']), $items);
    }
    else {
      return theme('item_list', $items, NULL, $handler->options['type']);
    }

  // Otherwise it is not aggragated
  // Just render like the default handler would
  }
  else {
    return $handler->parent_render($values);
  }
}