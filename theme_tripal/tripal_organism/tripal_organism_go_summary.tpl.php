<?php
  $form = $variables['tripal_analysis_go']['form']['value'];
  $form_status = $variables['tripal_analysis_go']['form']['status'];
?>
<div id="tripal_organism-go_summary-box" class="tripal_organism-info-box tripal-info-box">
  <div  class="tripal_organism-info-box-title tripal-info-box-title">GO Analysis Reports</div>
  <?php 
     if($form_status){
        print $form;
     } else {
       ?><div class="tripal-no-results"><?php print $form?></div><?php
     }
  ?>
  <div id="tripal_analysis_go_org_charts"></div>    
  <div id="tripal_cv_cvterm_info_box">
      <a href="#" onclick="$('#tripal_cv_cvterm_info_box').hide()" style="float: right">Close [X]</a>
      <div>Term Information</div>
      <div id="tripal_cv_cvterm_info"></div>
   </div>
   <div id="tripal_ajaxLoading" style="display:none">
     <div id="loadingText">Loading...</div>
   </div>   
</div>


