<?php
$library = $variables['node']->library;
$features = $library->feature_browser['features'];
$pager    = $library->feature_browser['pager'];
$enabled  = $library->feature_browser['enabled'];

if($enabled){ ?>
   <div id="tripal_library-feature_browser-box" class="tripal_library-info-box tripal-info-box">
     <div class="tripal_library-info-box-title tripal-info-box-title">Feature Browser</div>
     <div class="tripal_library-info-box-desc tripal-info-box-desc">The following browser provides a quick view for new visitors.  Use the searching mechanism to find specific features.</div>
     <?php 
     if(count($features) > 0){ ?>
       <table id="tripal_library-table-feature_browser" class="tripal_library-table tripal-table tripal-table-horz">     
         <tr class="tripal_library-table-odd-row tripal-table-even-row">
           <th>Feature Name</th>
           <th>Unique Name</th>
           <th>Type</th>
         </tr>
         <?php
         foreach ($features as $feature){ 
           $class = 'tripal_library-table-odd-row tripal-table-odd-row';
           if($i % 2 == 0 ){
             $class = 'tripal_library-table-odd-row tripal-table-even-row';
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



