<?php
$feature = $variables['node']->feature;
?>
<div id="tripal_feature-sequence-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title"><?php print $feature->type_id->name ?> Sequence</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The sequence for this <?php print $feature->type_id->name; ?> </div>
  <?php 
  if($feature->residues){ ?>   
    <pre id="tripal_feature-sequence-residues"><?php 
    // format the sequence to break ever 100 residues
    print ereg_replace("(.{60})","\\1<br>",$feature->residues); ?>  
    </pre> <?php
  } else {
      print '<div class="tripal-no-results">The sequence is currently not available</div>';
  } ?>
</div>

