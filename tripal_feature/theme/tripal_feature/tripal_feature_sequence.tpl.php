<?php
$feature = $variables['node']->feature;

// add the residues to the feature object.  Fields of type 'text' in Chado
// are not automatically added, so we must add them manually
$feature = tripal_core_expand_chado_vars($feature,'field','feature.residues');

if ($feature->residues) { ?>
  <div id="tripal_feature-sequence-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Sequence</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The sequence for this <?php print $feature->type_id->name; ?> </div>
    <pre id="tripal_feature-sequence-residues"><?php 
      // format the sequence to break ever 100 residues
      print ereg_replace("(.{60})","\\1<br>",$feature->residues); ?>  
    </pre>
  </div><?php
}

