<?php
$results = $variables['tripal_analysis_interpro']['results'];
?>

<div id="tripal_feature-interpro_results-box" class="tripal_analysis_interpro-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">InterPro Report</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>
  <?php if($results){?>
     <?php print $results ?>
  <?php } else { ?>
    <div class="tripal-no-results">There is no InterPro report for this feature</div> 
  <?php }?>
</div>
