<?php
/**
 * This template file provides the layout for the Tripal Panes modules.  It
 * allows fields to be displayed inside of collapsible panes.
 *
 */

// Add some necessary JavaScript dependencies.
drupal_add_js('misc/form.js');
drupal_add_js('misc/collapse.js');
drupal_add_js(drupal_get_path('module','tripal_panes') . '/theme/js/tripal_panes.js');

// Get the variables passed into this template.
$bundle = $variables['element']['#bundle'];
$bundle_type = $bundle->name . '-' . $bundle->label;

$content = '';
$toc = '';
$has_base_pane_only = TRUE;

get_content($variables, $bundle_type, $content, $toc, $has_base_pane_only);

if ($has_base_pane_only) { ?>
  <div id ="tripal-<?php print $bundle_type?>-contents-box"> <?php
    // print the rendered content
    print $content; ?>
  </div> <?php
}
else { ?>
  <table id ="tripal-<?php print $bundle_type?>-contents-table" class="tripal-panes-table">
    <tr class="tripal-panes-table-tr">
      <td nowrap class="tripal-panes-table-td tripal-panes-table-td-toc" align="left"><?php
        print $toc; ?>
      </td>
      <td class="tripal-panes-table-td-data" align="left" width="100%"><?php
        // print the rendered content
        print $content; ?>
      </td>
    </tr>
  </table> <?php
}

/**
 * Iterates through the panes and generates a conent string in HTML format.
 *
 * @param $variables
 * @param $bundle_type
 * @param $content
 * @param $toc
 * @param $has_base_pane_only
 */
function get_content($variables, $bundle_type, &$content, &$toc, &$has_base_pane_only) {
  $panes = $variables['element']['#panes'];
  $fields = $variables['element']['#fields'];

  // Iterate through all of the panes.
  foreach ($panes AS $pane_id => $pane) {
    // If we have a pane other than the base pane then we have more than
    // just the base.
    if ($pane->name != 'te_base') {
      $has_base_pane_only = FALSE;
    }
    $pane_settings = unserialize($pane->settings);
    $table_layout_group = key_exists('table_layout', $pane_settings) ? $pane_settings['table_layout'] : array();

    // Rearrange fields into groups for each pane
    $pane_fields = $fields[$pane_id];

    // Order the fields by their assigned weights.
    $ordered_fields = array();
    foreach ($pane_fields as $field) {
      $ordered_fields[$field['#weight'] . $field['#field_name']] = $field;
    }
    ksort($ordered_fields, SORT_NUMERIC);

    // Render fields
    $table_layout = array();
    $no_group = array();
    $output = '';
    $current_layout = '';
    $counter = 0;
    foreach ($ordered_fields AS $field) {
      //Add CSS Class
      $css_class = $field['#css_class'] ? ' ' . $field['#css_class'] : '';
      $field['#prefix'] = '<div class="tripal_panes-field-wrapper' . $css_class . '">';
      $field['#suffix'] = '</div>';

      // The field is in a table
      if (in_array($field['#field_name'], $table_layout_group)) {

        if ($counter != 0 && $current_layout != 'Table') {
          $output .= tripal_panes_generic_render_fields($no_group);
          $no_group = array();
        }
        $table_layout [$field['#weight'] . $field['#field_name']] = $field;
        $current_layout = 'Table';
      }
      // The field is not in a table
      else {
        if ($counter != 0 && $current_layout != 'Default') {
          $output .= tripal_panes_generic_render_table($table_layout, $bundle_type);
          $table_layout = array();
        }
        $no_group [$field['#weight'] . $field['#field_name']] = $field;
        $current_layout = 'Default';
      }
      $counter ++;
    }
    if ($current_layout == 'Table') {
      $output .= tripal_panes_generic_render_table($table_layout, $bundle_type);
    }
    else if ($current_layout == 'Default') {
      $output .= tripal_panes_generic_render_fields($no_group);
    }

    $pane_label = $pane->name == 'te_base' ? 'Summary' : $pane->label;
    $collapsible_item = array('element' => array());
    $collapsible_item['element']['#description'] = $output;
    $collapsible_item['element']['#title'] = $pane_label;
    $collapsible_item['element']['#children'] = '';
    $collapsible_item['element']['#attributes']['id'] = 'tripal_pane-fieldset-' . $pane->name;
    $collapsible_item['element']['#attributes']['class'][] = 'tripal_pane-fieldset';
    $collapsible_item['element']['#attributes']['class'][] = 'collapsible';
    if ($pane->name != 'te_base') {
      $collapsible_item['element']['#attributes']['class'][] = 'collapsed';
    }
    $toc_item_id = $pane_id;
    $toc .= "<div class=\"tripal-panes-toc-list-item\"><a id=\"" . $pane->name . "\" class=\"tripal_panes-toc-list-item-link\" href=\"?pane=" . $pane->name . "\">" . $pane_label . "</a></div>";
    $content .= theme('fieldset', $collapsible_item);
    if ($pane->name == 'te_base') {
      $content .= '<div class="tripal-panes-content-top"></div>';
    }
  }
}


/**
 * A wrapper function for placing fields inside of a Drupal themed table.
 *
 * @param $fields
 *   The list of fields present in the pane.
 * @return
 *   A string containing the HTMLified table.
 */
function tripal_panes_generic_render_table($fields, $bundle_type) {
  // If we have no fields in table layout
  if (count($fields) == 0) {
    return '';
  }

  // Create the rows for the table.
  $header = array();
  $rows = array();
  foreach ($fields as $field) {
    // We may have multiple values for the field, so we need to iterate
    // through those values first and add each one.
    $value = '';
    foreach (element_children($field) as $index) {
      $eo = 'odd';
      if ($index % 2 == 0) {
        $eo = 'even';
      }
      $value .= "<div class=\"field-item $eo\">" . $field[$index]['#markup'] . '</div>';
    }

    // Add the new row.
    $rows[] = array(
      array(
        'data' => '<div class="field-label">' . $field['#title'] . '</div>',
        'header' => TRUE,
        'width' => '20%',
        'nowrap' => 'nowrap'
      ),
      $value,
    );
  }

  // Theme the table.
  return theme_table(array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_panes-' . $bundle_type . '-table',  // TODO: need to add an ID
      'class' => 'tripal-data-horz-table'
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  ));
}

/**
 * A wrapper function for default rending of fields.
 *
 * @param $fields
 *   An array of fields to be rendered
 * @return
 *   A string containing the HTML of the rendered fields.
 */
function tripal_panes_generic_render_fields($fields) {
  if (count($fields) == 0) {
    return '';
  }
  $content = '';
  foreach ($fields as $field) {
    $content .= render($field);
  }
  return $content;
}