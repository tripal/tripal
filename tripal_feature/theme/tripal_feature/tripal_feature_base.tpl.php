<?php

$feature  = $variables['node']->feature;

?>
<div id="tripal_feature-base-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Feature Details</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>

   <?php if(strcmp($feature->is_obsolete,'t')==0){ ?>
      <div class="tripal_feature-obsolete">This feature is obsolete</div>
   <?php }?>
   <table id="tripal_feature-base-table" class="tripal_feature-table tripal-table tripal-table-vert">
      <tr class="tripal_feature-table-even-row tripal-table-even-row">
        <th>Name</th>
        <td><?php print $feature->name; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
        <th nowrap>Unique Name</th>
        <td><?php print $feature->uniquename; ?></td>
      </tr>
      <tr class="tripal_feature-table-even-row tripal-table-even-row">
        <th>Internal ID</th>
        <td><?php print $feature->feature_id; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
        <th>Type</th>
        <td><?php print $feature->type_id->name; ?></td>
      </tr>
      <tr class="tripal_feature-table-even-row tripal-table-even-row">
        <th>Organism</th>
        <td>
          <?php if ($feature->organism_id->nid) { 
      	   print "<a href=\"".url("node/".$feature->organism_id->nid)."\">".$feature->organism_id->genus ." " . $feature->organism_id->species ." (" .$feature->organism_id->common_name .")</a>";      	 
          } else { 
            print $feature->organism_id->genus ." " . $feature->organism_id->species ." (" .$feature->organism_id->common_name .")";
          } ?>
        </td>
     	</tr> <?php   
     	if ($feature->seqlen) { ?>
        <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
          <th>Length</th>
          <td><?php print $feature->seqlen ?></td>
        </tr> <?php 
     	} ?>   	                                
   </table>
</div>
