<?php
$report  = $variables['report_object'];
$hits = $report->hits;
?>

<style type="text/css">
#tripal_blast_report_current_page {
	float: right;
	margin-bottom: 5px;
	margin-right: 20px;
}
#tripal_blast_report_pager {
	float: right;
	margin-bottom: 10px;
	margin-right: 20px;
}
#tripal_blast_report_per_page {
	float: left;
	margin-bottom: 10px;
}
</style>

<div id="tripal_ajaxLoading" style="display:none">
	<div id="loadingText">Loading...</div>
	<img src="<?php print url("sites/all/themes/theme_tripal/images/ajax-loader.gif") ?>">
</div>

<div id="blast-hits-report">
	<strong>Analysis Date: </strong><?php print $report->time?> (<a href=<?php print $report->url ?>><?php print $report->name ?></a>)<br>
	There are <strong><?php print $report->counter ?></strong> records. 
	<div id=tripal_blast_report_current_page>page <strong><?php print $report->currentpage ?></strong> of <strong><?php print $report->no_page ?></strong>
	</div>
	
	<table id="tripal_analysi_blast-report-table" class="tripal_analysis_blast-table tripal-table tripal-table-horz">
		<tr class="tripal_analysis_blast-table-odd-row tripal-table-odd-row">
			<th nowrap><?php if ($report->sort == 0) {print $report->symbol;} ?>
				<a href=<?php print $report->byQuery ?>>Query</a>
			</th>
			<th nowrap><?php if ($report->sort == 1) {print $report->symbol;} ?>
				<a href=<?php print $report->byMatchName ?>>Match Name</a>
			</th>
			<th nowrap><?php if ($report->sort == 2) {print $report->symbol;} ?>
				<a href=<?php print $report->byDescription ?>>Description</a>
			</th>
			<th nowrap><?php if ($report->sort == 3) {print $report->symbol;} ?>
				<a href=<?php print $report->byEvalue ?>>E-value</a>
			</th>
			<th nowrap><?php if ($report->sort == 4) {print $report->symbol;} ?>
				<a href=<?php print $report->byIdentity ?>>%Identity</a>
			</th>
			<th nowrap><?php if ($report->sort == 5) {print $report->symbol;} ?>
				<a href=<?php print $report->byLength ?>>Length</a>
			</th>
		</tr>
		
		<?php foreach($hits AS $hit) {?>
		<tr class="<?php print $hit->class ?>">
		                           <td nowrap><a href=<?php print $hit->q_url ?>><?php print $hit->query ?></a></td>
		                           <td nowrap><a href=<?php print $hit->urlprefix.$hit->match ?> target=_blank><?php print $hit->match ?></td>
		                           <td><?php print $hit->desc ?></td>
		                           <td nowrap><?php print $hit->evalue ?></td>
		                           <td nowrap><?php print $hit->identity ?></td>
		                           <td nowrap><?php print $hit->length ?></td>
		                         </tr>
		<?php } ?>	
			
	</table>
			<div id="tripal_blast_report_per_page">Show	
				<?php 
				$per_page = $report->per_page;
				$path = $report->path;
				if ($per_page == 10) {?>
					<strong>10</strong> | 
				<?php } else { 
					$url_path = url($path."10");
				?>	
					<a href=<?php print $url_path ?>>10</a> | 
				<?php } 
				if ($per_page == 20) {
				?>
					<strong>20</strong> | 
				<?php } else {
					$url_path = url($path."20");
				?>
					<a href=<?php print $url_path ?>>20</a> | 
				<?php } 
				if ($per_page == 50) {
				?>
					<strong>50</strong> | 
				<?php } else {
					$url_path = url($path."50");
				?>
					<a href=<?php print $url_path ?>>50</a> | 
				<?php }
				if ($per_page == 100) {
				?>
					<strong>100</strong> | 
				<?php } else {
					$url_path = url($path."100");
				?>
					<a href=<?php print $url_path ?>>100</a> | 
				<?php }
				if ($per_page == 200) {
				?>
					<strong>200</strong> | 
				<?php } else {
					$url_path = url($path."200");
				?>
			   	<a href=<?php print $url_path ?>>200</a>
				<?php } ?>
				
				records per page
		</div>
		<div id=tripal_blast_report_pager>page
			<select id=tripal_blast_report_page_selector onChange="tripal_update_best_hit_report(this, <?php print $report->analysis_id ?>,<?php print $report->sort ?>, <?php print $report->descending ?>, <?php print $report->per_page ?>)">
			<?php  for ($i = 1; $i <= $report->no_page; $i ++) { ?>
				<option value=<?php print $i ?>><?php print $i ?></option>
			<?php } ?>	                           
			</select>
		</div>
</div>
			
	