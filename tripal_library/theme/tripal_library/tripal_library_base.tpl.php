<?php

$library  = $variables['node']->library;

// expand the library to include the properties so that we can get the description
$library = tripal_core_expand_chado_vars($library,'table','libraryprop', array('return_array' => 1));  
$libraryprops = $library->libraryprop;
if (count($libraryprops) > 0){ 
  foreach ($libraryprops as $property) {
    if($property->type_id->name == 'library_description') {
      $property = tripal_core_expand_chado_vars($property,'field','libraryprop.value');
      $library_description = $property->value;      
    }
  }
}

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
        <td><?php print $library->type_id->definition ?></td>
      </tr>
      <tr class="tripal_library-table-even-row tripal-table-even-row">
        <th>Description</th>
        <td><?php print $library_description ?>
        </td>
     	</tr>           	                                
   </table>
</div>
