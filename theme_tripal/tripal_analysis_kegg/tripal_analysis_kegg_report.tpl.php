<?php
$analysis = $node->analysis;
$report = $analysis->tripal_analysis_kegg->kegg_report;
//dpm($analysis);
?>

<div id="tripal_analysis_kegg-report-box" class="tripal_analysis_kegg-box tripal-info-box">
  <div class="tripal_analysis_kegg-info-box-title tripal-info-box-title">KEGG Report</div>
  <div class="tripal_analysis_kegg-info-box-desc tripal-info-box-desc"><?php print $analysis->name ?></div>
  <?php print $report ?>
</div>
