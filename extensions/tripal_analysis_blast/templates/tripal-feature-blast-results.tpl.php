<?php
$feature  = $variables['node']->feature;
$blast_results_list = $feature->tripal_analysis_blast->blast_results_list;

?>
<div id="tripal_ajaxLoading" style="display:none">
	<div id="loadingText">Loading...</div>
	<img src="sites/all/themes/theme_tripal/images/ajax-loader.gif">
</div>

<?php 
if(count($blast_results_list) > 0){
   foreach ($blast_results_list as $blast_result) {
	  $hits_array = $blast_result->hits_array;
	  $db = $blast_result->db;
     $analysis = $blast_result->analysis;
?>
<div id="blast_db_<?php print $db->db_id ?>">
<div id="tripal_analysis_blast-results-<?php print "$db->db_id" ?>-box" class="tripal_analysis_blast-box tripal-info-box">
	<div class="tripal-info-box-title tripal_analysis_blast-info-box-title"><?php if ($db->displayname) {print $db->displayname;} else {print $db->name . "Homologs";} ?></div>
	<div class="tripal-info-box-desc tripal_analysis_blast-info-box-desc">
	<strong>Analysis Date: </strong><?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted) . " (<a href=".url("node/$analysis->nid").">$analysis->name</a>)"?><br>
	<!--Query: <?php print "$blast_result->xml_tag"?><br>-->
	
	<?php 
   if($blast_result->max != 10){    
      $url = url("tripal_top_blast/$feature->feature_id/$db->db_id/10");
		?><span><a onclick="return tripal_update_blast(this,<?php print $db->db_id?>)" href="<?php print $url ?>">Show Best 10 Hits</a></span><?php
	} else { 
		?><span>Best 10 Hits Shown</span><?php
	} 
	
	if($blast_result->number_hits <= 10){ 
	} 
   else if ($blast_result->max != 25) { 
  	   $url = url("tripal_top_blast/$feature->feature_id/$db->db_id/25"); 	
		?><span> | <a onclick="return tripal_update_blast(this,<?php print"$db->db_id"?>)" href="<?php print $url ?>">Show Best 25 Hits</a></span><?php
	}else {
		?><span> | Best 25 Hits Shown</span><?php
	} 
	
	if($blast_result->number_hits <= 25){
	} 
   else if ($blast_result->max != 0) {
	   $url = url("tripal_top_blast/$feature->feature_id/$db->db_id/all"); 		
		?><span> | <a onclick="return tripal_update_blast(this, <?php print $db->db_id ?>)" href="<?php print $url ?>">Show All Hits</a> </span><?php
	} else {
		?><span> | All Hits Shown</span><?php
	} 
   ?>
	<br><br><span>Click a description for more details</span>
   </div>
	<table id="tripal_analysis_blast-table" class="tripal-table tripal-table-horz tripal_analysis_blast-table">
		<tr>
         <th>&nbsp;</th>
			<th nowrap>Match Name</th>
			<th nowrap>E value</th>
			<th nowrap>Identity</th>
			<th nowrap>Description</th>
		</tr>
		
		<?php 
      $i = 0; 
      if(sizeof($hits_array)==0){?>
        <tr>
          <td colspan="5">    
            <div class="tripal-no-results">There are no matches against <?php print $db->name?> for this <?php print $feature->type_id->name?>.</div> 
          </td>
        </tr><?php
      }
		foreach($hits_array AS $hit) { 
         $class = 'tripal-table-odd-row tripal_analysis_blast-table-odd-row';
         if($i % 2 == 0 ){
            $class = 'tripal-table-even-row tripal_analysis_blast-table-odd-row';
         }?>
         <tr class="<?php print $class ?> tripal_analysis_blast-result-first-row">
            <td><?php print $i+1 ?>.</td>
		      <?php if ($hit['hit_url']) { ?>
			      <td><a href="<?php print $hit['hit_url']?>" target="_blank"><?php print $hit['hit_name']?></a></td>
		      <?php } else {?>
			      <td><?php print $hit['hit_name'] ?></td>
		      <?php } ?>
			   <td nowrap><?php print $hit['best_evalue']?></td>
			   <td nowrap><?php  print $hit['percent_identity']?></td>
			   <td><?php print $hit['description']?></td>
		   </tr>
	      <tr class="<?php print $class ?>">
		      <td colspan=5>
			      <a class="blast-hit-arrow-icon" onclick="return tripal_blast_toggle_alignment(<?php print $analysis->analysis_id ?>,<?php print $i?>)"><img id="tripal_analysis_blast-info-toggle-image-<?php print $analysis->analysis_id ?>-<?php print $i?>" src=<?php print $hit['arrowr_url']?> align="top"> View Alignment</a>
			      <div class="tripal_analysis_blast-info-hsp-title"></div>
		      </td>
	      </tr>
	      <tr class="<?php print $class ?> tripal_analysis_blast-result-last-row">
		      <td colspan=5>
		      <?php 
		      $hsps_array = $hit['hsp'];
		      foreach ($hsps_array AS $hsp) { ?>
			      <div class="tripal_analysis_blast-info-hsp-desc" id="tripal_analysis_blast-info-hsp-desc-<?php print $analysis->analysis_id ?>-<?php print $i?>">
				      &nbsp;HSP <?php  print $hsp['hsp_num'] ?>
				      <pre>Score: <?php print $hsp['bit_score'] ?> bits (<?php print $hsp['score'] ?>), Expect = <?php print $hsp['evalue'] ?><br>Identity = <?php print sprintf("%d/%d (%.2f%%)", $hsp['identity'], $hsp['align_len'], $hsp['identity']/$hsp['align_len']*100) ?>, Postives = <?php print sprintf("%d/%d (%.2f%%)", $hsp['positive'], $hsp['align_len'], $hsp['positive']/$hsp['align_len']*100)?>, Query Frame = <?php print $hsp['query_frame']?></a><br><br></a>Query: <?php print sprintf("%4d", $hsp['query_from'])?> <?php print $hsp['qseq'] ?> <?php print sprintf("%d", $hsp['query_to']); ?><br>            <?php print $hsp['midline'] ?><br>Sbjct: <?php print sprintf("%4d", $hsp['hit_from']) ?> <?php print $hsp['hseq']?> <?php print sprintf("%d",$hsp['hit_to']) ?></pre><br>
			      </div>
		      <?php } ?>
		      </td>
	      </tr>		
         <?php $i++;
	   } ?>
	</table>
</div>
</div>
  <?php } ?>
<?php } ?>
