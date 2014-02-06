<?php

  // Retrieve Template
  $template = db_select('tripal_bulk_loader_template', 't')
    ->fields('t')
    ->condition('template_id', $variables['template_id'], '=')
    ->execute()
    ->fetchObject();

  $template->template_array = unserialize($template->template_array);

  // Summarize Template
  $fields = array();
  $constants = array();
  foreach ($template->template_array as $priority => $table_array) {
    if (!is_array($table_array)) {
      continue;
    }

    $table = $table_array['table'];
    $record = $table_array['record_id'];
    foreach ($table_array['fields'] as $field) {
      if (preg_match('/table field/', $field['type'])) {
        $field['table'] = $table;
        $field['record'] = $record;
        $sheet = 0;//$field['spreadsheet sheet'];
        $column = $field['spreadsheet column'];
        $fields[$sheet.'-'.$column][] = $field;
      } elseif ($field['type'] == 'constant') {
        $field['table'] = $table;
        $field['record'] = $record;
        $constants[] = $field;
      }
    }
  }
?>

<div id="tripal_bulk_loader-base-box" class="tripal_bulk_loader-info-box tripal-info-box">
  <div class="tripal_bulk_loader-info-box-title tripal-info-box-title">Template Description</div>
  <div class="tripal_bulk_loader-data-block-desc tripal-data-block-desc"></div>

<?php if (sizeof($constants)) { ?>
  <table id="tripal_bulk_loader-template_constant-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
  <caption><b>Constants</b> -These values are applied to all records in the Data File</caption>
    <tr><th rowspan="2">Record Name</th><th rowspan="2">Field Name</th><th rowspan="2">Value</th><th colspan="2">Chado Database</th></tr>
    <tr><th>Table</th><th>Field</th></tr>
    <?php $row = 'even' ?>
    <?php foreach ($constants as $field) {?>
      <tr class="tripal_bulk_loader-table-<?php print $row; ?>-row tripal-table-<?php print $row; ?>-row">
        <td><?php print $field['record'];?></td>
        <td><?php print $field['title'];?></td>
        <td><?php print $field['constant value']; ?></td>
        <td><?php print $field['table'];?></td>
        <td><?php print $field['field'];?></td>
      </tr>
      <?php $row = ($row == 'odd') ? 'even':'odd' ; ?>
    <?php } ?>
  </table>

<?php } if (sizeof($fields)) { ?>
  <table id="tripal_bulk_loader-template_fields-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
  <caption><b>Fields</b> -Below is a mapping between Data File columns and the Chado Database</caption>
  <tr><th rowspan="2">Record Name</th><th rowspan="2">Field Name</th><th rowspan="2">Data File Column</th><th colspan="2">Chado Datbase</th></tr>
  <tr><th>Table</th><th>Field</th></tr>
  <?php $row = 'even' ?>
  <?php foreach ($fields as $column) {?>
    <?php foreach ($column as $field) {?>
    <tr class="tripal_bulk_loader-table-<?php print $row; ?>-row tripal-table-<?php print $row; ?>-row">
      <td><?php print $field['record'];?></td>
      <td><?php print $field['title'];?></td>
      <td><?php print $field['spreadsheet column'];?></td>
      <td><?php print $field['table'];?></td>
      <td><?php print $field['field'];?></td>
    <tr>
    <?php $row = ($row == 'odd') ? 'even':'odd' ; ?>
  <?php }} ?>
  </table>
<?php } ?>
</div>