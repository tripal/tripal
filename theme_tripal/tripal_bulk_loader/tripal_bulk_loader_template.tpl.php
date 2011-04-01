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

<table>
<caption><b>Constants</b> -These values are applied to all records in the Spreadsheet</caption>
	<tr><th rowspan="2">Field Name</th><th rowspan="2">Value</th><th colspan="2">Chado Database</th></tr>
	<tr><th>Table</th><th>Field</th></tr>
	<?php foreach ($constants as $field) {?>
		<td><?php print $field['title'];?></td>
		<td><?php print $field['constant value']; ?></td>
		<td><?php print $field['table'];?></td>
		<td><?php print $field['field'];?></td>	
	<?php } ?>
</table>

<table>
<caption><b>Fields</b> -Below is a mapping between Spreadsheet columns and the Chado Database</caption>
<tr><th rowspan="2">Field Name</th><th colspan="2">Spreadsheet</th><th colspan="2">Chado Datbase</th></tr>
<tr><th>Worksheet</th><th>Column</th><th>Table</th><th>Field</th></tr>
<?php foreach ($fields as $column) {?>
	<?php foreach ($column as $field) {?>
	<tr>
		<td><?php print $field['title'];?></td>
		<td><?php print $field['spreadsheet sheet']; ?></td>
		<td><?php print $field['spreadsheet column'];?></td>
		<td><?php print $field['table'];?></td>
		<td><?php print $field['field'];?></td>
	<tr>
<?php }} ?>
</table>

