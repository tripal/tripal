<?php
/*
* Typically in a Tripal template, the data needed is retrieved using a call to
* tripal_core_expand_chado_vars function.  For example, to retrieve all 
* of the feature alignments for this node, the following function call would be made:
* 
*   $feature = tripal_core_expand_chado_vars($feature,'table','featureloc');
*   
* However, this will return all records from the featureloc table without any context.
* To help provide context, a special variable is provided to this template named
* 'all_featurelocs'.   
* 
*   $feature->all_featurelocs
*   
* The records in the 'all_featurelocs' array are all properly arranged for easy iteration.
* 
* However, access to the original alignment records is possible through the 
* $feature->featureloc object.  In the following ways:  
*
* Alignment context #1:
* --------------------
* If the feature for this node is the parent in the alignment relationships,
* then those alignments are available in this variable:
* 
*    $feature->featureloc->srcfeature_id;
*    
*
* Alignment context #2:
* ---------------------
* If the feature for this node is the child in the alignment relationsips,
* then those alignments are available in this variable:
* 
*   $feature->featureloc->feature_id;
*   
*
* Alignment context #3:
* --------------------
* If the feature is aligned to another through an intermediary feature (e.g.
* a feature of type 'match', 'EST_match', 'primer_match', etc) then those
* alignments are stored in this variable:
*   feature->matched_featurelocs
*
* Below is an example of a feature that may be aligned to another through
* an intermediary:
*
*    Feature 1: Contig      ---------------   (left feature)
*    Feature 2: EST_match           -------
*    Feature 3: EST                 --------- (right feature)
*
* The feature for this node is always "Feature 1".  The purpose of this type 
* alignment is to indicate cases where there is the potential for overhang
* in the alignments, or, the ends of the features are not part of the alignment
* prehaps due to poor quality of the ends.  Blast results and ESTs mapped to
* contigs in Unigenes would fall under this category.
*
*/
$feature = $variables['node']->feature;
$alignments = $feature->all_featurelocs;

if(count($alignments) > 0){ ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">The following features are aligned</div><?php
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Aligned Feature' ,'Feature Type', 'Alignment Location');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($alignments as $alignment){
    $feature_name = $alignment->name;
    if (property_exists($alignment, 'nid')) {
      $feature_name = l($feature_name, "node/" . $alignment->nid);
    }
    $feature_loc = '';
    $strand = '.';
    if ($alignment->strand == -1) {
      $strand = '-';
    } 
    elseif ($alignment->strand == 1) {
       $strand = '+';
    } 
    // if this is a match then make the other location 
    if(property_exists($alignment, 'right_feature')){
      $rstrand = '.';
      if ($alignment->right_strand == -1) {
        $rstrand = '-';
      } 
      elseif ($alignment->right_strand == 1) {
        $rstrand = '+';
      }
      $feature_loc = $feature->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand; 
      $feature_loc .= "<br>" . $alignment->name .":". ($alignment->right_fmin + 1) . ".." . $alignment->right_fmax . " " . $rstrand; 
    }
    else {
      $feature_loc = $alignment->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand; 
    }
    
    $rows[] = array(
      $feature_name,
      $alignment->type,
      $feature_loc
    );
  } 
  
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_feature-table-alignments',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table); 
}

