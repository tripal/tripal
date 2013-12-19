<?php
// get the featurelocs for this feature. If the variable is not already 
// expanded then do so
$feature = $variables['node']->feature;

$feature = tripal_core_expand_chado_vars($feature, 'table', 'featureloc');
$featurelocs = $feature->featureloc;

// get the featurelocs. if only one featureloc exists then we want to convert
// the object into an array, otherwise the value is an array
$ffeaturelocs = $feature->featureloc->feature_id;
if (!$ffeaturelocs) {
   $ffeaturelocs = array();
} elseif (!is_array($ffeaturelocs)) { 
   $ffeaturelocs = array($ffeaturelocs); 
}
$featureloc_sequences = tripal_feature_load_featureloc_sequences ($feature->feature_id,$ffeaturelocs);

if(count($featureloc_sequences) > 0){
   foreach($featureloc_sequences as $src => $attrs){ ?>
       <div id="tripal_feature-<?php print $attrs['type']?>-box" class="tripal_feature-info-box tripal-info-box">
         <div class="tripal_feature-info-box-title tripal-info-box-title">Annotated Sequence</div>
         <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>
            <?php print $attrs['formatted_seq'] ?>
       </div>
   <?php } 
}?>

