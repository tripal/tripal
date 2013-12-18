
<style>
tr.odd .form-item, tr.even .form-item {
  white-space: normal;
  word-wrap: break-word;
}
</style>

<div id="tripal-bulk-loader-fields">
<?php print drupal_render($form['template_name']); ?>

<!-- For each table display details in a draggable table -->
<?php if (!$form['records']['no_records']['#value']) { ?>
  <fieldset><legend><?php print $form['records']['#title']; ?></legend>
  <?php
    print drupal_render($form['records']['description']);

    // generate table
    drupal_add_tabledrag('draggable-table', 'order', 'sibling', 'records-reorder');
    $header = array('Record Name', 'Chado Table', 'Action', 'Order', '');
    $rows = array();
    foreach (element_children($form['records']['records-data']) as $key) {
      $element = &$form['records']['records-data'][$key];
      $element['new_priority']['#attributes']['class'] = 'records-reorder';

      $row = array();
      $row[] = drupal_render($element['title']);
      $row[] = drupal_render($element['chado_table']);
      $row[] = drupal_render($element['mode']);
      $row[] = drupal_render($element['new_priority']) . drupal_render($element['id']);
      $row[] = drupal_render($element['submit-edit_record']) . '<br>' . drupal_render($element['submit-duplicate_record']) . '<br>' . drupal_render($element['submit-add_field']);
      $rows[] = array('data' => $row, 'class' => 'draggable');
    }

print theme('table', $header, $rows, array('id' => 'draggable-table', 'width' => '100%'));

    // Render submit
    print drupal_render($form['records']['submit-new_record']);
    print drupal_render($form['records']['submit-reorder']);
    unset($form['records']);
  ?>
  </fieldset>
<?php } ?>

<!-- For each field display details plus edit/delete buttons-->
<?php if ($form['fields']['total_fields']['#value'] > 0) { ?>
  <fieldset><legend><?php print $form['fields']['#title']; ?></legend>

  <?php
    // generate table
  $header = array('','Record Name', 'Field Name', 'Chado Table', 'Chado Field', 'Data File Column', 'Constant Value', 'Foreign Record');
    $rows = array();
    foreach ($form['fields']['fields-data'] as $key => $element) {
      if (preg_match('/^#/', $key)) { continue; }

      $row = array();
      $row[] = drupal_render($element['edit_submit']) . '<br>' . drupal_render($element['delete_submit']);
      $row[] = drupal_render($element['record_id']);
      $row[] = drupal_render($element['field_name']);
      $row[] = drupal_render($element['chado_table_name']);
      $row[] = drupal_render($element['chado_field_name']);
      $row[] = drupal_render($element['column_num']);
      $row[] = drupal_render($element['constant_value']);
      $row[] = drupal_render($element['foreign_record_id']);

      $rows[] = $row;
    }
print theme('table', $header, $rows, array('style'=>'table-layout: fixed; width: 100%'));

    // Render other elements
    print drupal_render($form['fields']['add_field']);
    unset($form['fields']);
  ?>
  </fieldset>
<?php } ?>

<!-- Display Rest of form -->
<?php print drupal_render($form); ?>
</div>