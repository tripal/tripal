<?php

drupal_add_js('misc/form.js');
drupal_add_js('misc/collapse.js');
drupal_add_js(drupal_get_path('module','tripal_fields_layout') . '/theme/js/tripal_fields_layout.js');
$panels = $variables['element']['#panels'];
$fields = $variables['element']['#fields'];

// TODO, the horz_table variable needs to be set in a variable and checked here.
$horz_table = TRUE;

// Group fields into panels
$content = '';
$toc = '';
$has_base_panel_only = TRUE;
foreach ($panels AS $panel_id => $panel) {
  if ($panel->name != 'te_base') {
    $has_base_panel_only = FALSE;
  }
  $panel_settings = unserialize($panel->settings);
  $hz_table_group = key_exists('hz_table', $panel_settings) ? $panel_settings['hz_table'] : array();
  $vt_table_group = key_exists('vt_table', $panel_settings) ? $panel_settings['vt_table'] : array();
  
  $panel_fields = $fields[$panel_id];
  // Rearrange fields into groups for each panel
  $hz_table = array();
  $vt_table = array();
  $no_group = array();
  // Keyed by field's '#weight' and '#field_name so we can ksort() by weight
  foreach ($panel_fields AS $field) {
    if (in_array($field['#field_name'], $hz_table_group)) {
      $hz_table [$field['#weight'] . $field['#field_name']] = $field;
    }
    else if (in_array($field['#field_name'], $vt_table_group)) {
      $vt_table [$field['#weight'] . $field['#field_name']] = $field;
    }
    else {
      $no_group [$field['#weight'] . $field['#field_name']] = $field;
    }
  }

  // Render horizontal table
  $horz_table = '';
  if (count($hz_table) != 0) {
    ksort($hz_table, SORT_NUMERIC);
    $rows = array();
    foreach ($hz_table as $field) {
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
    $horz_table = theme_table(array(
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
  
  // Render horizontal table
  $vert_table = '';  
  if (count($vt_table) != 0) {
    ksort($vt_table, SORT_NUMERIC);
    $value = array();
    $headers = array();
    foreach ($vt_table as $field) {
      $headers [] = $field['#title'];
      array_push($value, $field[0]['#markup']);
    }
    $vert_table = theme_table(array(
      'header' => $headers,
      'rows' => array($value),
      'attributes' => array(
        'id' => '',  // TODO: need to add an ID
        'class' => 'tripal-data-vert-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    ));
  }
  
  // Render field not in a group
  $ungrouped = '';
  if (count($no_group) != 0) {
    ksort($no_group, SORT_NUMERIC);
    foreach ($no_group as $field) {
      $ungrouped .= render($field);
    }    
  }
  
  $output = $horz_table . $vert_table . $ungrouped ;

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