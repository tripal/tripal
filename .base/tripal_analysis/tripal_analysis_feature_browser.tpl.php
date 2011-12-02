<?php
$analysis = $variables['node']->analysis;
$features = $analysis->feature_browser['features'];
$pager    = $analysis->feature_browser['pager'];
$enabled  = $analysis->feature_browser['enabled'];

if($enabled){ ?>
   <div id="tripal_analysis-feature_browser-box" class="tripal_analysis-info-box tripal-info-box">
     <div class="tripal_analysis-info-box-title tripal-info-box-title">Feature Browser</div>
     <div class="tripal_analysis-info-box-desc tripal-info-box-desc">The following browser provides a quick view for new visitors.  Use the searching mechanism to find specific features.</div>
     <?php 
     if(count($features) > 0){ ?>
       <table id="tripal_analysis-table-feature_browser" class="tripal_analysis-table tripal-table tripal-table-horz">     
         <tr class="tripal_analysis-table-odd-row tripal-table-even-row">
           <th>Feature Name</th>
           <th>Unique Name</th>
           <th>Type</th>
         </tr>
         <?php
         foreach ($features as $feature){ 
           $class = 'tripal_analysis-table-odd-row tripal-table-odd-row';
           if($i % 2 == 0 ){
             $class = 'tripal_analysis-table-odd-row tripal-table-even-row';
           } ?>
           <tr class="<?php print $class ?>">
             <td><?php 
               if($feature->nid){    
                 $link =   url("node/$feature->nid");        
                 print "<a href=\"$link\">$feature->name</a>";
               } else {
                 print $feature->name;
               }?>
             </td>
             <td><?php print $feature->uniquename?></td>
             <td><?php print $feature->cvname?></td>
           </tr><?php
           $i++;  
         } ?>
       </table><?php 
     } 
     else {?>
       <div class="tripal-no-results">There are no features available for browsing</div> <?php 
     }
     print $pager ?>
   </div> <?php 
} ?>



