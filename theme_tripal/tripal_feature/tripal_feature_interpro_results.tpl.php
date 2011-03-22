<?php
$results = $variables['tripal_analysis_interpro']['results'];
?>

<script type="text/javascript">
if (Drupal.jsEnabled) {
   $(document).ready(function() {
      var selectbox = document.getElementById('edit-analysis-select');
      if (selectbox) {
        default_option = selectbox.getElementsByTagName('option')[1];
        if (default_option) {
           selectbox.selectedIndex =default_option.index ;
           if (selectbox.onchange.toString().match('tripal_analysis_go_org_charts')) {   
              tripal_analysis_go_org_charts(default_option.value);
           } else if (selectbox.onchange.toString().match('tripal_analysis_kegg_org_report')) {            
              tripal_analysis_kegg_org_report(default_option.value);
           }
        }
      }
   });
}
</script>
<div id="tripal_feature-interpro_results-box" class="tripal_analysis_interpro-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">InterPro Report</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>
  <?php print $results ?>
</div>
