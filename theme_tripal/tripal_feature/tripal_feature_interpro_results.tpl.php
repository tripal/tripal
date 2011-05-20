<?php
$feature  = $variables['node']->feature;
$results = $feature->tripal_analysis_interpro->results->xml;
$resultsHTML = $feature->tripal_analysis_interpro->results->html;

if(count($results) > 0){ 
   $i = 0;
   foreach($results as $analysis_id => $analysisprops){ 
     $analysis = $analysisprops['analysis'];
     $protein_ORFs = $analysisprops['protein_ORFs']; 
     $terms = $analysisprops['allterms']; 
     ?>
     <div id="tripal_feature-interpro_results_<?php print $i?>-box" class="tripal_analysis_interpro-box tripal-info-box">
        <div class="tripal_feature-info-box-title tripal-info-box-title">InterPro Report <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?></div>
        <div class="tripal_feature-info-box-desc tripal-info-box-desc"><?php 
            if($analysis->nid){ ?>
               Analysis name: <a href="<?php print url('node/'.$analysis->nid) ?>"><?php print $analysis->name?></a><?php
            } else { ?>
               Analysis name: <?php print $analysis->name;
            } ?><br>
            Date Performed: <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?>
        </div>

     <div class="tripal_feature-interpro_results_subtitle">Summary of Annotated IPR terms</div>
     <table id="tripal_feature-interpro_summary-<?php $i ?>-table" class="tripal_analysis_interpro-summary-table tripal-table tripal-table-horz">
      <tr>
        <th>Term</td>
        <th>Name</td>
      </tr>
     <?php 
     $j=0;
     foreach($terms as $term){ 
       $ipr_id = $term[0];
       $ipr_name = $term[1];
       $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
       if($j % 2 == 0 ){
         $class = 'tripal_feature-table-even-row tripal-table-even-row';
       }?>
       <tr class="<?php print $class ?>">
         <td><?php print $ipr_id ?></td>
         <td><?php print $ipr_name ?></td>         
       </tr>
       <?php
       $j++;
     } ?>
     </table>
     <br><br>
     <div class="tripal_feature-interpro_results_subtitle">Analysis Details</div>
     <table id="tripal_feature-interpro_results-<?php $i ?>-table" class="tripal-table tripal_feature_interpro-results-table tripal-table-horz" style="border-top: 0px; border-bottom: 0px">
     <?php
     foreach($protein_ORFs as $orf){  
        $terms = $orf['terms'];
        $orf = $orf['orf'];  
        ?>
        <?php foreach($terms as $term){ 
          $matches = $term['matches'];
          $ipr_id = $term['ipr_id'];
          $ipr_name = $term['ipr_name'];
          $ipr_type = $term['ipr_type']; ?>          
            <tr>
              <td colspan="4" style="padding-left: 0px">ORF: <?php print $orf['orf_id'] ?>, Length: <?php print $orf['orf_length'] ?> <br>
                              IPR Term: <?php print "$ipr_id $ipr_name ($ipr_type)"; ?></th>
            </tr>
            <tr style="border-top: solid 1px;">
              <th>Method</th>
              <th>Identifier</th>
              <th>Description</th>
              <th>Matches<sup>*</sup></th>
            </tr>
            <?php $j = 0; 
            foreach ($matches as $match){
               $match_id = $match['match_id'];
               $match_name = $match['match_name'];
               $match_dbname = $match['match_dbname'];


               $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
               if($j % 2 == 0 ){
                  $class = 'tripal_feature-table-even-row tripal-table-even-row';
               }?>
               <tr class="<?php print $class ?>">
                 <td><?php print $match_dbname ?></td>
                 <td><?php print $match_id ?></td>
                 <td><?php print $match_name ?></td>
                 <td nowrap><?php
                    $locations = $match['locations'];
                    foreach($locations as $location){
                      print $location['match_score']." [".$location['match_start']."-".$location['match_end']."] " . $location['match_status'] ."<br>";
                      #$match_evidence =  $location['match_evidence'];
                    } ?>
                 </td>
               </tr>
               <?php
               $j++;  
            } // end foreach matches ?>
            <tr><td colspan="4"><sup>* score [start-end] status</sup></td></tr> <?php
        } // end foreach terms
        $i++;
     } // end foreach orfs ?>
     </table>
     </div> <?php
   } // end for each analysis 
} // end if
if($resultsHTML){  ?>
   <div id="tripal_feature-interpro_results_<?php print $i?>-box" class="tripal_analysis_interpro-box tripal-info-box">
     <div class="tripal_feature-info-box-title tripal-info-box-title">InterPro Report <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?></div>
     <div class="tripal_feature-info-box-desc tripal-info-box-desc"><?php 
         if($analysis->nid){ ?>
            Analysis name: <a href="<?php print url('node/'.$analysis->nid) ?>"><?php print $analysis->name?></a><?php
         } else { ?>
            Analysis name: <?php print $analysis->name;
         } ?><br>
         Date Performed: <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?>
     </div>

   <div class="tripal_feature-interpro_results_subtitle">Summary of Annotated IPR terms</div> <?php 
   print $resultsHTML;?>
   </div> <?php
}
?>

