<?php
/*
 * There are two ways that sequences can be displayed.  They can come from the 
 * feature.residues column or they can come from an alignment with another feature.  
 * This template will show both or one or the other depending on the data available.
 * 
 * For retreiving the sequence from an alignment we would typically make a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all
 * of the featurelocs in order to get the sequences needed for this template, the
 * following function call would be made:
 *
 *   $feature = tripal_core_expand_chado_vars($feature,'table','featureloc');
 *
 * Then all of the sequences would need to be retreived from the alignments and
 * formatted for display below.  However, to simplify this template, this has already
 * been done by the tripal_feature module and the sequences are made available in
 * the variable:
 *
 *   $feature->featureloc_sequences
 *
 */

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

// get the sequence derived from alignments
$feature = $variables['node']->feature;
$featureloc_sequences = $feature->featureloc_sequences;

if ($residues or count($featureloc_sequences) > 0) { ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc"></div> <?php
  
  // show the alignment sequences first as they are colored with child features
  if(count($featureloc_sequences) > 0){
    foreach($featureloc_sequences as $src => $attrs){ 
      print $attrs['formatted_seq'];
    } 
  }
  
  // add in the residues if they are present
  if ($residues) { ?>
    <pre id="tripal_feature-sequence-residues"><?php 
      // format the sequence to break every 100 residues
      print preg_replace("/(.{50})/","\\1<br>",$feature->residues); ?>  
    </pre> <?php 
  } 
}