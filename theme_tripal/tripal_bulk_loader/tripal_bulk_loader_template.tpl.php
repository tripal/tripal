<?php
	//dpm($template->template_array, 'Template Array (tpl)');
	$fields = array();
	$constants = array();
	foreach ($template->template_array as $table => $table_array) {
		if (!is_array($table_array)) {
			continue;
		}
		
		foreach ($table_array['field'] as $field) {
			if (preg_match('/table field/', $field['type'])) {
				$field['table'] = $table;
				$sheet = $field['spreadsheet sheet'];
				$column = $field['spreadsheet column'];
				$fields[$sheet.'-'.$column][] = $field;
			} elseif ($field['type'] == 'constant') {
				$field['table'] = $table;
				$constants[] = $field;
			}
		}
	}
?>

<div id="tripal_bulk_loader-base-box" class="tripal_bulk_loader-info-box tripal-info-box">
  <div class="tripal_bulk_loader-info-box-title tripal-info-box-title">Template Description</div>
  <div class="tripal_bulk_loader-info-box-desc tripal-info-box-desc"></div>
  
  <table id="tripal_bulk_loader-template_constant-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
  <caption><b>Constants</b> -These values are applied to all records in the Spreadsheet</caption>
    <tr><th rowspan="2">Field Name</th><th rowspan="2">Value</th><th colspan="2">Chado Database</th></tr>
    <tr><th>Table</th><th>Field</th></tr>
    <?php $row = 'even' ?>
    <?php foreach ($constants as $field) {?>
      <tr class="tripal_bulk_loader-table-<?php print $row; ?>-row tripal-table-<?php print $row; ?>-row">
        <td><?php print $field['title'];?></td>
        <td><?php print $field['constant value']; ?></td>
        <td><?php print $field['table'];?></td>
        <td><?php print $field['field'];?></td>	
      </tr>
      <?php $row = ($row == 'odd') ? 'even':'odd' ; ?>
    <?php } ?>
  </table>
  
  <table id="tripal_bulk_loader-template_fields-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
  <caption><b>Fields</b> -Below is a mapping between Spreadsheet columns and the Chado Database</caption>
  <tr><th rowspan="2">Field Name</th><th colspan="2">Spreadsheet</th><th colspan="2">Chado Datbase</th></tr>
  <tr><th>Worksheet</th><th>Column</th><th>Table</th><th>Field</th></tr>
  <?php $row = 'even' ?>
  <?php foreach ($fields as $column) {?>
    <?php foreach ($column as $field) {?>
    <tr class="tripal_bulk_loader-table-<?php print $row; ?>-row tripal-table-<?php print $row; ?>-row">
      <td><?php print $field['title'];?></td>
      <td><?php print $field['spreadsheet sheet']; ?></td>
      <td><?php print $field['spreadsheet column'];?></td>
      <td><?php print $field['table'];?></td>
      <td><?php print $field['field'];?></td>
    <tr>
    <?php $row = ($row == 'odd') ? 'even':'odd' ; ?>
  <?php }} ?>
  </table>
</div>