<?php
  $organism = $node->organism;
  $form = $organism->tripal_analysis_kegg->select_form['form'];
  $has_results = $organism->tripal_analysis_kegg->select_form['has_results'];
?>
<div id="tripal_organism-kegg_summary-box" class="tripal_organism-info-box tripal-info-box">
  <div  class="tripal_organism-info-box-title tripal-info-box-title">KEGG Analysis Reports</div>
  <?php 
     if($has_results){
        print $form;
     } else {
       ?><div class="tripal-no-results">There are no KEGG reports available</div><?php
     }
  ?>
   <div id="tripal_analysis_kegg_org_report"></div>
   <div id="tripal_ajaxLoading" style="display:none">
     <div id="loadingText">Loading...</div>
   </div>   
</div>



