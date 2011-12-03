<?php
$organism = $variables['node']->organism;
$types    = $organism->feature_counts['types'];
$enabled  = $organism->feature_counts['enabled'];

if($enabled){ ?>
   <div id="tripal_organism-feature_counts-box" class="tripal_organism-info-box tripal-info-box">
     <div class="tripal_organism-info-box-title tripal-info-box-title">Data Type Summary</div>
     <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following data types are currently present for this organism</div>
     <?php 
     if(count($types) > 0){ ?>
       <table id="tripal_organism-table-feature_counts" class="tripal_organism-table tripal-table tripal-table-horz">     
         <tr class="tripal_organism-table-odd-row tripal-table-even-row">
           <th>Feature Type</th>
           <th>Count</th>
         </tr> <?php
         foreach ($types as $type){ 
           $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
           if($i % 2 == 0 ){
             $class = 'tripal_organism-table-even-row tripal-table-even-row';
           }?>
           <tr class="<?php print $class ?>">
             <td><span title="<?php print $type->definition ?>"><?php print $type->feature_type?></span></td>
             <td><?php print number_format($type->num_features) ?></td>
           </tr> <?php
           $i++;  
         } ?>
       </table>
       <img class="tripal_cv_chart" id="tripal_feature_cv_chart_<?php print $organism->organism_id?>" src="" border="0"><?php 
     } 
     else { ?>
       <div class="tripal-no-results">There are no features available.</div> <?php 
     }?>
   </div> <?php 
} ?>



