<?php
/*
 * There are several ways that sequences can be displayed.  They can come from the 
 * feature.residues column,  they can come from an alignment with another feature,
 * they can come from a protein sequence that has relationship with this sequence,
 * or they can come from sub children (e.g. CDS coding sequences).
 *   
 * This template will show all types depending on the data available.
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
  $feature = chado_expand_var($feature,'field','feature.residues');
  $residues = $feature->residues;
} 

// get the sequence derived from alignments
$feature = $variables['node']->feature;
$featureloc_sequences = $feature->featureloc_sequences;

if ($residues or count($featureloc_sequences) > 0) { 

  $sequences_html = '';  // a variable for holding all sequences HTML text
  $list_items = array(); // a list to be used for theming of content on this page ?>
  
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">The following sequences are available for this feature:</div> <?php
  
  // ADD IN RESIDUES FOR THIS FEATURE
  // add in the residues if they are present
  if ($residues) {
    $list_items[] = '<a href="#residues">Current ' . $feature->type_id->name . ' sequence</a>';
     
    // format the sequence to break every 50 residues
    $sequences_html .= '<a name="residues"></a>';
    $sequences_html .= '<div id="residues" class="tripal_feature-sequence-item">';
    $sequences_html .= '<p><b>Current ' . $feature->type_id->name . ' sequence</b></p>';
    $sequences_html .= '<pre class="tripal_feature-sequence">';
    $sequences_html .= '>' . tripal_get_fasta_defline($feature) . "\n";
    $sequences_html .= preg_replace("/(.{50})/","\\1<br>",$feature->residues);
    $sequences_html .= '</pre>';
    $sequences_html .= '<a href="#sequences-top">back to top</a>';
    $sequences_html .= '</div>';
    
  }
  
  // ADD IN RELATIONSHIP SEQUENCES (e.g. proteins)
  // see the explanation in the tripal_feature_relationships.tpl.php 
  // template for how the 'all_relationships' is provided. It is this
  // variable that we use to get the proteins.
  $all_relationships = $feature->all_relationships;
  $object_rels = $all_relationships['object'];
  $has_coding_seq = 0;
  foreach ($object_rels as $rel_type => $rels){
    foreach ($rels as $subject_type => $subjects){
      foreach ($subjects as $subject){
        
        // add in protein sequence if it has residues
        if ($rel_type == 'derives from' and $subject_type == 'polypeptide') {
          $protein = $subject->record->subject_id;
          $protein = chado_expand_var($protein, 'field', 'feature.residues');
          
          if ($protein->residues) {
            $list_items[] = '<a href="#residues">Protein sequence of ' . $protein->name . '</a>';
            $sequences_html .= '<a name="protein-' . $protein->feature_id . '"></a>';
            $sequences_html .= '<div id="protein-' . $protein->feature_id . '" class="tripal_feature-sequence-item">';
            $sequences_html .= '<p><b>Protein sequence of ' . $protein->name . '</b></p>';
            $sequences_html .= '<pre class="tripal_feature-sequence">';
            $sequences_html .= '>' . tripal_get_fasta_defline($protein) . "\n";
            $sequences_html .= preg_replace("/(.{50})/","\\1<br>", $protein->residues);
            $sequences_html .= '</pre>';
            $sequences_html .= '<a href="#sequences-top">back to top</a>';
            $sequences_html .= '</div>';
          }
        }
        
        // we want to know if there are any coding sequences associated with this feature
        // if so we will use some code a bit later on to get those sequences
        if ($rel_type == 'part of' and $subject_type == 'CDS') {
          $has_coding_seq = 1;
        }
        
        // add any other sequences that are related through a relationship
        // and that have values in the 'residues' column
      }
    }
  }
  
  
  /* ADD IN ALIGNMENT SEQUENCES FOR THIS FEATURE
   * For retreiving the sequence from an alignment we would typically make a call to
   * chado_expand_var function.  For example, to retrieve all
   * of the featurelocs in order to get the sequences needed for this template, the
   * following function call would be made:
   *
   *   $feature = chado_expand_var($feature,'table','featureloc');
   *
   * Then all of the sequences would need to be retreived from the alignments and
   * formatted for display below.  However, to simplify this template, this has already
   * been done by the tripal_feature module and the sequences are made available in
   * the variable: 
   *
   *   $feature->featureloc_sequences
   */
  if(count($featureloc_sequences) > 0){
    foreach($featureloc_sequences as $src => $attrs){
      // the $attrs array has the following keys
      //   * src:  a unique identifier combining the feature id with the cvterm id
      //   * type: the type of sequence (e.g. mRNA, etc)
      //   * location:  the alignment location
      //   * defline: the definition line
      //   * formatted_seq: the formatted sequences
      $list_items[] = '<a href="#' . $attrs['src'] . '">Alignment at  ' . $attrs['location'] . "</a>";
      $sequences_html .= '<a name="' . $attrs['src'] . '"></a>';
      $sequences_html .= '<div id="' . $attrs['src'] . '" class="tripal_feature-sequence-item">';
      $sequences_html .= '<p><b>Alignment at  ' . $attrs['location'] .'</b></p>';
      $sequences_html .= $attrs['formatted_seq'];
      $sequences_html .= '<a href="#sequences-top">back to top</a>';
      $sequences_html .= '</div>';
    }
  }
  
  // CODING SEQUENCES
  // add in any CDS sequences. 
  if ($has_coding_seq) {
    // use the tripal_get_sequence() API function to retreive the CDS sequences
    $cds_sequence = tripal_get_sequence(
      array(
        'feature_id' => $feature->feature_id,
        'name' => $feature->name, 
      ),
      array(
        'width' => 50,  // FASTA sequence should have 50 chars per line
        'derive_from_parent' => 1, // CDS are in parent-child relationships so we want to use the sequence from the parent
        'aggregate' => 1, // we want to combine all CDS for this feature into a single sequence
        'output_format' => 'fasta_txt', // we just want plain text, we'll format it here.
        'sub_feature_types' => array('CDS'), // we're looking for CDS features
        ''
      )
    );
    $list_items[] = '<a href="#coding_sequence">Coding sequence </a>';
    $sequences_html .= '<a name="coding_sequence"></a>';
    $sequences_html .= '<div id="coding_sequence" class="tripal_feature-sequence-item">';
    $sequences_html .= '<p><b>Coding sequence</b></p>';
    $sequences_html .= '<pre class="tripal_feature-sequence">';
    $sequences_html .= $cds_sequence;
    $sequences_html .= '</pre>';
    $sequences_html .= '<a href="#sequences-top">back to top</a>';
    $sequences_html .= '</div>';
  }
  
  // first add a list at the top of the page that can be formatted as the
  // user desires.  We use the theme_item_list function of Drupal to create 
  // the list rather than hard-code the HTML here.  Instructions for how
  // to create the list can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_item_list/7
  print '<a name="sequences-top"></a>';
  print theme_item_list(array(
    'items' => $list_items,
    'title' => '',
    'type' => 'ul',
    'attributes' => array(),
  ));
  
  // now print the sequences
  print $sequences_html;
}