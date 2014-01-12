<?php
$feature = $variables['node']->feature;

// we don't want to get the sequence for traditionally large types. They are
// too big,  bog down the web browser, take longer to load and it's not
// reasonable to print them on a page.
$residues ='';
if(strcmp($feature->type_id->name,'scaffold') !=0 and
   strcmp($feature->type_id->name,'chromosome') !=0 and
   strcmp($feature->type_id->name,'supercontig') !=0 and
   strcmp($feature->type_id->name,'pseudomolecule') !=0) {
  $feature = tripal_core_expand_chado_vars($feature,'field','feature.residues');
  $residues = $feature->residues;
} 

if ($residues) { ?>
  <div id="tripal_feature-sequence-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Sequence</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The sequence for this <?php print $feature->type_id->name; ?> </div>
    <pre id="tripal_feature-sequence-residues"><?php 
      // format the sequence to break every 100 residues
      print preg_replace("/(.{50})/","\\1<br>",$feature->residues); ?>  
    </pre>
  </div> <?php
}

