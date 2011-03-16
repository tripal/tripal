<?php
$organism = $variables['node']->organism;
$features = $variables['tripal_feature']['browser']['features'];
$pager = $variables['tripal_feature']['browser']['pager'];
?>
<div id="tripal_organism-feature_browser-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_organism-info-box-title tripal-info-box-title">Feature Browser</div>
   <?php if(count($features) > 0){ ?>
   <table id="tripal_organism-table-feature_browser" class="tripal_organism-table tripal-table tripal-table-horz">     
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Feature Name</th>
        <th>Unique Name</th>
        <th>Type</th>
      </tr>
      <?php
      foreach ($features as $feature){ 
      $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_organism-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php 
           if($feature->nid){    
              $link =   url("node/$feature->nid");        
              print "<a href=\"$link\">$feature->name</a>";
           } else {
              print $feature->name;
           }
           ?>
        </td>
        <td><?php print $feature->uniquename?></td>
        <td><?php print $feature->cvname?></td>
      </tr>
      <?php
      $i++;  
    } ?>
   </table>
  <?php } ?>
  <?php print $pager ?>

</div>



