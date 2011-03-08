<?php
$feature = $variables['node']->feature;
?>
<div id="tripal_feature-sequence-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title"><?php print $feature->cvname ?> Sequence</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The nucleotide or peptide sequence for this feature</div>
  <?php print $feature->cvname; ?> sequence        
  <pre id="tripal_feature-sequence-residues"><?php 
     if($feature->residues){
        // format the sequence to break ever 100 residues
        print ereg_replace("(.{60})","\\1<br>",$feature->residues); 
     } else {
        print "sequence currently not available";
     } ?>
  </pre>
</div>

