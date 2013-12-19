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
  <div id="tripal_feature-alignments-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Alignments</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The following features are aligned to this <b><?php print $feature->type_id->name;?></b></div>
    <table id="tripal_feature-featurelocs_as_child-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Aligned Feature</th>
        <th>Feature Type</th>
        <th>Alignment Location</th>
      </tr><?php
      $i = 0; 
      foreach ($alignments as $alignment){
        $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
          $class = 'tripal_feature-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
            if ($alignment->nid) {
              print "<a href=\"" . url("node/".$alignment->nid) . "\">".$alignment->name."</a>";
            } else {
              print $alignment->name;
            }?>
          </td>
          <td><?php print $alignment->type ?></td>
          <td><?php  
            $strand = '.';
            if ($alignment->strand == -1) {
              $strand = '-';
            } 
            elseif ($alignment->strand == 1) {
               $strand = '+';
            } 
              
            // if this is a match then make the other location 
            if($alignment->right_feature){
              $rstrand = '.';
              if ($alignment->right_strand == -1) {
                   $rstrand = '-';
              } 
              elseif ($alignment->right_strand == 1) {
                   $rstrand = '+';
              }
              print $feature->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand; 
              print "<br>" . $alignment->name .":". ($alignment->right_fmin + 1) . ".." . $alignment->right_fmax . " " . $rstrand; 
            }
            else {
              print $alignment->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand; 
            }?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
  </div><?php
}

