<?php

$library  = $variables['node']->library;

// expand the library to include the properties.
$library = tripal_core_expand_chado_vars($library,'table','libraryprop');
$library = tripal_core_expand_chado_vars($library,'field','libraryprop.value');

?>
<div id="tripal_library-base-box" class="tripal_library-info-box tripal-info-box">
  <div class="tripal_library-info-box-title tripal-info-box-title">Library Details</div>
  <div class="tripal_library-info-box-desc tripal-info-box-desc"></div>

   <?php if(strcmp($library->is_obsolete,'t')==0){ ?>
      <div class="tripal_library-obsolete">This library is obsolete</div>
   <?php }?>
   <table id="tripal_library-base-table" class="tripal_library-table tripal-table tripal-table-vert">
      <tr class="tripal_library-table-even-row tripal-table-even-row">
        <th nowrap>Unique Name</th>
        <td><?php print $library->uniquename; ?></td>
      </tr>
      <tr class="tripal_library-table-odd-row tripal-table-odd-row">
        <th>Internal ID</th>
        <td><?php print $library->library_id; ?></td>
      </tr>
      <tr class="tripal_library-table-even-row tripal-table-even-row">
        <th>Organism</th>
        <td>
          <?php if ($library->organism_id->nid) { 
      	   print "<a href=\"".url("node/".$library->organism_id->nid)."\">".$library->organism_id->genus ." " . $library->organism_id->species ." (" .$library->organism_id->common_name .")</a>";      	 
          } else { 
            print $library->organism_id->genus ." " . $library->organism_id->species ." (" .$library->organism_id->common_name .")";
          } ?>
        </td>
      </tr>      
      <tr class="tripal_library-table-odd-row tripal-table-odd-row">
        <th>Type</th>
        <td><?php 
            if ($library->type_id->name == 'cdna_library') {
               print 'cDNA';
            } else if ($library->type_id->name == 'bac_library') {
               print 'BAC';
            } else {
               print $library->type_id->name;
            }
          ?>
        </td>
      </tr>
      <tr class="tripal_library-table-even-row tripal-table-even-row">
        <th>Description</th>
        <td><?php
           // right now we only have one property for libraries. So we can just
           // refernece it directly.  If we had more than one property
           // we would need to convert this to an if statment and loop
           // until we found the right one.
           print $library->libraryprop->value?>
        </td>
     	</tr>           	                                
   </table>
</div>
