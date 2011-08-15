<?php
  $organism = $node->organism;
  $form = $organism->tripal_analysis_go->select_form['form'];
  $has_results = $organism->tripal_analysis_go->select_form['has_results'];
?>
<div id="tripal_organism-go_summary-box" class="tripal_organism-info-box tripal-info-box">
  <div  class="tripal_organism-info-box-title tripal-info-box-title">GO Analysis Reports</div>
  <?php 
     if($has_results){
        print $form;
     } else {
       ?><div class="tripal-no-results">
           There are no GO reports available
           <?php if(user_access('access administration pages')){ ?>
              <p><br>Administrators, to view a GO report you must:
              <ul>
                 <li>load GO assignments and associate them to features and a corresponding analysis</li>
                 <li>Set the <a href="<?php print url('admin/tripal/tripal_cv/cvtermpath');?>">cvtermpath</a> for the 'biological process', 'molecular_function' and 'cellular_component' vocabularies.</li>
                 <li>Populate the <a href="<?php print url('admin/tripal/tripal_mviews');?>">go_count_analysis</a> materialized view</li>
              </ul> </p>
           <?php }?>
         </div><?php
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

