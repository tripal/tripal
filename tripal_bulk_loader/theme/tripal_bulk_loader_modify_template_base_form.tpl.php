
<style>
tr.odd .form-item, tr.even .form-item {
  white-space: normal;
  word-wrap: break-word;
}
fieldset {
  padding: 20px;
}
td.active{
  width: 10px;
}
td.tbl-action-record-links {
  width: 150px;
}
td.tbl-action-field-links {
  width: 100px;
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
    drupal_add_tabledrag('records-table', 'order', 'sibling', 'records-reorder');
    $header = array(' ', ' ', 'Record Name', 'Chado Table', 'Mode', 'Order',);
    $rows = array();
    foreach (element_children($form['records']['records-data']) as $key) {
      $element = &$form['records']['records-data'][$key];
      $element['new_priority']['#attributes']['class'] = array('records-reorder');

      $row = array();
      $row[] = '';
      $row[] = array(
        'class' => array('tbl-action-record-links'),
        'data' => drupal_render($element['submit-edit_record']) . ' | '
          . drupal_render($element['submit-delete_record']) . ' | '
          . drupal_render($element['submit-duplicate_record']) . '<br>'
          . drupal_render($element['view-fields-link']) . ' | '
          . drupal_render($element['submit-add_field'])
        );
      $row[] = drupal_render($element['title']);
      $row[] = drupal_render($element['chado_table']);
      $row[] = drupal_render($element['mode']);
      $row[] = drupal_render($element['new_priority'])
        . drupal_render($element['id']);

      $rows[] = array('data' => $row, 'class' => array('draggable'));
    }

    print theme(
      'table',
      array(
        'header' => $header,
        'rows' => $rows,
        'attributes' => array('id' => 'records-table')
      )
    );

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
      $row[] = array(
        'class' => array('tbl-action-field-links', 'active'),
        'data' => drupal_render($element['edit_submit']) . ' | '
          . drupal_render($element['delete_submit']) . '<br />'
          . drupal_render($element['view-record-link'])
        );
      $row[] = drupal_render($element['record_id']);
      $row[] = drupal_render($element['field_name']);
      $row[] = drupal_render($element['chado_table_name']);
      $row[] = drupal_render($element['chado_field_name']);
      $row[] = drupal_render($element['column_num']);
      $row[] = drupal_render($element['constant_value']);
      $row[] = drupal_render($element['foreign_record_id']);

      $rows[] = $row;
    }
    print theme(
      'table',
      array(
        'header' => $header,
        'rows' => $rows,
        //'attributes' => array('style'=>'table-layout: fixed; width: 100%')
      )
    );

    // Render other elements
    print drupal_render($form['fields']['add_field']);
    unset($form['fields']);
  ?>
  </fieldset>
<?php } ?>

<!-- Display Rest of form -->
<?php print drupal_render_children($form); ?>
</div>