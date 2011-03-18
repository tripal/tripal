<?php
$organism = $variables['node']->organism;
$libraries = $variables['tripal_library']['libraries'];
?>
<div id="tripal_organism-library_list-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_organism-info-box-title tripal-info-box-title">Libraries</div>
  <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following libraries are associated with this organism.</div>
   <?php if(count($libraries) > 0){ ?>
   <table id="tripal_organism-table-library_list" class="tripal_organism-table tripal-table tripal-table-horz">     
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Library Name</th>
        <th>Description</th>
        <th>Type</th>
      </tr>
      <?php
      foreach ($libraries as $library){ 
      $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_organism-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php 
           if($library->nid){    
              $link =   url("node/$library->nid");        
              print "<a href=\"$link\">$library->name</a>";
           } else {
              print $library->name;
           }
           ?>
        </td>
        <td><?php print $library->description?></td>
        <td>
          <?php 
            if ($library->cvname == 'cdna_library') {
               print 'cDNA';
            } else if ($library->cvname == 'bac_library') {
               print 'BAC';
            } else {
               print $library->cvname;
            }
          ?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
   </table>
  <?php } else {?>
    <div class="tripal-no-results">There are no libraries available</div> 
  <?php }?>
  <?php print $pager ?>
</div>




