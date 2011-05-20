<?php

$feature  = $variables['node']->feature;
$results = $feature->tripal_analysis_kegg->results;

if($feature->cvname != 'gene' and count($results) > 0){ 
   $i = 0;
   foreach($results as $analysis_id => $analysisprops){ 
     $analysis = $analysisprops['analysis'];
     $terms = $analysisprops['terms']; 
     ?>
     <div id="tripal_feature-kegg_results_<?php print $i?>-box" class="tripal_analysis_kegg-box tripal-info-box">
        <div class="tripal_feature-info-box-title tripal-info-box-title">KEGG Report <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?></div>
        <div class="tripal_feature-info-box-desc tripal-info-box-desc"><?php 
            if($analysis->nid){ ?>
               Analysis name: <a href="<?php print url('node/'.$analysis->nid) ?>"><?php print $analysis->name?></a><?php
            } else { ?>
               Analysis name: <?php print $analysis->name;
            } ?><br>
            Date Performed: <?php print preg_replace("/^(\d+-\d+-\d+) .*/","$1",$analysis->timeexecuted); ?>
        </div>

     <div class="tripal_feature-kegg_results_subtitle">Annotated Terms</div>
     <table id="tripal_feature-kegg_summary-<?php $i ?>-table" class="tripal_analysis_kegg-summary-table tripal-table tripal-table-horz">
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
         <td><?php print $term ?></td>
       </tr>
       <?php
       $j++;
     } ?>
     </table>     
     </div> <?php
     $i++;
   } // end for each analysis 
} // end if
?>

