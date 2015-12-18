<?php

drupal_add_js('misc/form.js');
drupal_add_js('misc/collapse.js');
drupal_add_js(drupal_get_path('module','tripal_fields_layout') . '/theme/js/tripal_fields_layout.js');
$panels = $variables['element']['#panels'];
$fields = $variables['element']['#fields'];

// Render fields in a table
function render_table ($table_layout) {
  $table = '';
  if (count($table_layout) != 0) {
    $rows = array();
    foreach ($table_layout as $field) {
      $rows[] = array(
        array(
          'data' => $field['#title'],
          'header' => TRUE,
          'width' => '20%',
          'nowrap' => 'nowrap'
        ),
        $field[0]['#markup']
      );
    }
    $table = theme_table(array(
      'header' => array(),
      'rows' => $rows,
      'attributes' => array(
        'id' => '',  // TODO: need to add an ID
        'class' => 'tripal-data-horz-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    ));
  }
  return $table;
}

// Render fields not in a group
function render_fields($no_group) {
  $ungrouped = '';
  if (count($no_group) != 0) {
    foreach ($no_group as $field) {
      $ungrouped .= render($field);
    }
  }
  return $ungrouped;
}

// Process fields in panels
$content = '';
$toc = '';
$has_base_panel_only = TRUE;
foreach ($panels AS $panel_id => $panel) {
  if ($panel->name != 'te_base') {
    $has_base_panel_only = FALSE;
  }
  $panel_settings = unserialize($panel->settings);
  $table_layout_group = key_exists('table_layout', $panel_settings) ? $panel_settings['table_layout'] : array();

  // Rearrange fields into groups for each panel
  $panel_fields = $fields[$panel_id];

  // Keyed by field's '#weight' and '#field_name so we can ksort() by weight
  $weighed_fields = array();
  foreach ($panel_fields AS $field) {
      $weighed_fields [$field['#weight'] . $field['#field_name']] = $field;
  }
  ksort($weighed_fields, SORT_NUMERIC);
  
  // Render weighed fields
  $table_layout = array();
  $no_group = array();
  $output = '';
  $current_layout = '';
  $counter = 0;
  foreach ($weighed_fields AS $field) {
    // The field is in a table
    if (in_array($field['#field_name'], $table_layout_group)) {
  
      if ($counter != 0 && $current_layout != 'Table') {
        $output .= render_fields($no_group);
        $no_group = array();
      }
      $table_layout [$field['#weight'] . $field['#field_name']] = $field;
      $current_layout = 'Table';
    }
    // The field is not in a table
    else {
      if ($counter != 0 && $current_layout != 'Default') {
        $output .= render_table($table_layout);
        $table_layout = array();
      }
      $no_group [$field['#weight'] . $field['#field_name']] = $field;
      $current_layout = 'Default';
    }
    $counter ++;
  }
  if ($current_layout == 'Table') {
    $output .= render_table($table_layout);
  }
  else if ($current_layout == 'Default') {
    $output .= render_fields($no_group);
  }

  // If this is a base content, do not organize the content in a fieldset
  if ($panel->name == 'te_base') {
    $content .= '<div class="tripal_panel-base_panel">' . $output . '</div>';
  } else {
    $collapsible_item = array('element' => array());
    $collapsible_item['element']['#description'] = $output;
    $collapsible_item['element']['#title'] = $panel->label;
    $collapsible_item['element']['#children'] = '';
    $collapsible_item['element']['#attributes']['id'] = 'tripal_panel-fieldset-' . $panel->name;
    $collapsible_item['element']['#attributes']['class'][] = 'tripal_panel-fieldset';
    $collapsible_item['element']['#attributes']['class'][] = 'collapsible';
    $collapsible_item['element']['#attributes']['class'][] = 'collapsed';
    $toc_item_id = $panel_id;
    $toc .= "<div class=\"tripal_toc_list_item\"><a id=\"" . $panel->name . "\" class=\"tripal_toc_list_item_link\" href=\"?pane=" . $panel->name . "\">" . $panel->label . "</a></div>";
    $content .= theme('fieldset', $collapsible_item);
  }
}

$bundle_type = ''; // TODO: need to add the bundle type

if ($has_base_panel_only) { ?>
  <div id ="tripal-<?php print $bundle_type?>-contents-box"> <?php
    // print the rendered content
    print $content; ?>
  </div> <?php
} else { ?>
  <table id ="tripal-<?php print $bundle_type?>-contents-table" class="tripal-contents-table">
    <tr class="tripal-contents-table-tr"> <?php 
      ?>
      <td nowrap class="tripal-contents-table-td tripal-contents-table-td-toc" align="left"><?php
        print $toc; ?>
      </td>
      <td class="tripal-contents-table-td-data" align="left" width="100%"> <?php

        // print the rendered content
        print $content; ?>
      </td>
    </tr>
  </table> <?php 
} ?>