
<div id="tripal_bulk_loader-base-box" class="tripal_bulk_loader-info-box tripal-info-box">
  <div class="tripal_bulk_loader-info-box-title tripal-info-box-title"></div>
  <div class="tripal_bulk_loader-info-box-desc tripal-info-box-desc"></div>
  
	<table id="tripal_bulk_loader-base-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
		<tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
			<th>Job Name</th>
			<td><?php print $node->loader_name;?></td>
		</tr>
		<tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
			<th>Submitted By</th>
			<td><span class="author"><?php print theme('username', $node); ?></span></td>
		</tr>
		<tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
			<th>Job Creation Date</th>
			<td><?php print format_date($node->created, 'custom', "F j, Y, g:i a"); ?></td>
		</tr>		
		<tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
			<th>Last Updated</th>
			<td><?php print format_date($node->changed, 'custom', "F j, Y, g:i a"); ?></td>
		</tr>	
		<tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
			<th>Template Name</th>
			<td><?php print $node->template->name; ?></td>
		</tr>
		<tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
			<th>Data File</th>
			<td><?php print $node->file;?></td>
		</tr>
		<tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
			<th>Job Status</th>
			<td><?php print $node->job_status;?></td>
		</tr>
	</table>  
</div>

<h3>Template</h3>
<?php print theme('tripal_bulk_loader_template', $node->template->template_id); ?>
