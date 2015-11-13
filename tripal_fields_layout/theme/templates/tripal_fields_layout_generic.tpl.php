<?php

drupal_add_js('misc/collapse.js');
$panels = $variables['element']['#panels'];
$fields = $variables['element']['#fields'];

// TODO, the horz_table variable needs to be set in a variable and checked here.
$horz_table = TRUE;

// Group fields into panels
$content = '';
$toc = '';
foreach ($panels AS $panel_id => $panel) {
  $panel_fields = $fields[$panel_id];
  $collapsible_item = array('element' => array());

  // If the format is horizontal table then format the fields in tabular format.
  if ($horz_table) {
    $rows = array();
    foreach ($panel_fields as $field) {
      $rows[] = array(
        array(
          'data' => $field['#title'],
          'header' => TRUE,
          'width' => '20%',
        ),
        $field[0]['#markup']
      );
    }
    $collapsible_item['element']['#description'] = theme_table(array(
      'header' => array(),
      'rows' => $rows,
      'attributes' => array(
        'id' => '',  // TODO: need to add an ID
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    ));
  }
  // If no format is provided then use the default Drupal render.
  else {
    $collapsible_item['element']['#description'] = render($panel_fields);
  }

  // If this is not the base content then the field should be collapsible.
  if ($panel->name != 'te_base') {
    $collapsible_item['element']['#title'] = $panel->label;
    $collapsible_item['element']['#children'] = '';
    $collapsible_item['element']['#attributes']['class'][] = 'collapsible';
    $collapsible_item['element']['#attributes']['class'][] = 'collapsed';
    $toc_item_id = $panel_id;
    $toc .= "<div class=\"tripal_toc_list_item\"><a id=\"" . $panel->name . "\" class=\"tripal_toc_list_item_link\" href=\"?pane=" . $panel->name . "\">" . $panel->label . "</a></div>";
    $content .= theme('fieldset', $collapsible_item);
  }
  // The base field should just be the fields
  else {
    $content .= render($panel_fields);
  }
}

$bundle_type = ''; // TODO: need to add the bundle type ?>
<table id ="tripal-<?php print $bundle_type?>-contents-table" class="tripal-contents-table">
  <tr class="tripal-contents-table-tr">
    <td nowrap class="tripal-contents-table-td tripal-contents-table-td-toc" align="left"><?php
      print $toc; ?>
    </td>
    <td class="tripal-contents-table-td-data" align="left" width="100%"> <?php

      // print the rendered content
      print $content; ?>
    </td>
  </tr>
</table>