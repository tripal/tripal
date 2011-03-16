<?php
$blast_object_array = $variables['blast_object'];
?>
<div id="tripal_ajaxLoading" style="display:none">
	<div id="loadingText">Loading...</div>
	<img src="sites/all/themes/theme_tripal/images/ajax-loader.gif">
</div>
<?php 
if($blast_object_array){
   foreach ($blast_object_array as $blast_object) {
	  $hits_array = $blast_object->hits_array;
	  $db = $blast_object->db;
?>
<div id="tripal_analysis_blast-results-box" class="tripal_analysis_blast-box tripal-info-box">
	<div class="tripal_analysis_blast-info-box-title tripal-info-box-title"><h3><?php print $blast_object->title ?></h3></div>
	<div class="tripal_analysis_blast-info-box-desc tripal-info-box-desc" id="blast_db_<?php print "$db->db_id" ?>">
	<strong>Analysis Date: </strong><?php print "$blast_object->ana_time (<a href=node/$blast_object->ana_nid>$blast_object->ana_name</a>)"?><br>
	Query: <?php print "$blast_object->xml_tag"?><br>
	
	<?php 	if($blast_object->max != 10){ ?>
		<span><a onclick="return tripal_update_blast(this,<?php print $db->db_id?>)" href="tripal_top_blast/<?php print "$blast_object->feature_id/$db->db_id/10" ?>>Show Best 10 Hits</a></span>";
	<?php } else { ?>
		<span>Best 10 Hits Shown</span>
	<?php } ?>
	
	<?php if($blast_object->number_hits <= 10){ ?>
	<?php } else if ($blast_object->max != 25) { ?>
		<span> | <a onclick="return tripal_update_blast(this,<?php print"$db->db_id"?>)" href="tripal_top_blast/<?php print "$blast_object->feature_id/$db->db_id/25"?>">Show Best 25 Hits</a></span>
	<?php }else { ?>
		<span> | Best 25 Hits Shown</span>
	<?php } ?>
	
	<?php if($blast_object->number_hits <= 25){ ?>
	<?php } else if ($blast_object->max != 0) {
						$url = url("tripal_top_blast/$feature_id/$db->db_id/0");
		?>		
		<span> | <a onclick="return tripal_update_blast(this, <?php print $db->db_id ?>)" href="<?php print "tripal_top_blast/$blast_object->feature_id/$db->db_id/0"?>">Show All Hits</a> </span>
	<?php } else { ?>
		<span> | All Hits Shown</span>
	<?php } ?>
	
	<br><span><i>Note:</i> Click a description for more details</span>
		<span>
		<table class="tripal_analysis_blast-table">
			<tr>
				<th nowrap>Match Name</th>
				<th nowrap>E value</th>
				<th nowrap>Identity</th>
				<th nowrap>Description</th>
			</tr>
			
			<?php 
			foreach($hits_array AS $hit) {
			?>
			<tr>
			<?php if ($hit['hit_url']) { ?>
				<td><a href="<?php print $hit['hit_url']?>" target="_blank"><?php print $hit['hit_name']?></a></td>
			<?php } else {?>
				<td><?php print $hit['hit_name'] ?></td>
			<?php } ?>
				<td nowrap><?php print $hit['best_evalue']?></td>
				<td nowrap><?php  print $hit['percent_identity']?></td>
				<td nowrap><?php print $hit['description']?></td>
			</tr>
		<tr>
			<td colspan=4>
				<a class="blast-hit-arrow-icon"><img src=<?php print $hit['arrowr_url']?> align="top"> View Alignment</a>
				<div class="tripal_analysis_blast-info-hsp-title"></div>
			</td>
		</tr>
		<tr>
			<td colspan=4>
			<?php 
			$hsps_array = $hit['hsp'];
			foreach ($hsps_array AS $hsp) { ?>
				<div class="tripal_analysis_blast-info-hsp-desc">
					<b>HSP <?php  print $hsp['hsp_num'] ?></b>
					<pre>Score: <?php print $hsp['bit_score'] ?> bits (<?php print $hsp['score'] ?>), Expect = <?php print $hsp['evalue'] ?><br>Identity = <?php print sprintf("%d/%d (%.2f%%)", $hsp['identity'], $hsp['align_len'], $hsp['identity']/$hsp['align_len']*100) ?>, Postives = <?php print sprintf("%d/%d (%.2f%%)", $hsp['positive'], $hsp['align_len'], $hsp['positive']/$hsp['align_len']*100)?>, Query Frame = <?php print $hsp['query_frame']?></a><br><br></a>Query: <?php print sprintf("%4d", $hsp['query_from'])?> <?php print $hsp['qseq'] ?> <?php print sprintf("%d", $hsp['query_to']); ?><br>            <?php print $hsp['midline'] ?><br>Sbjct: <?php print sprintf("%4d", $hsp['hit_from']) ?> <?php print $hsp['hseq']?> <?php print sprintf("%d",$hsp['hit_to']) ?></pre><br>
				</div>
			<?php } ?>
			</td>
		</tr>		
		<?php	} ?>
		</table>
	</span>
	</div>
</div>
  <?php } ?>
<?php } ?>
