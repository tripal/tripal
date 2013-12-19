<?php

$featuremap  = $variables['node']->featuremap;

// expand the featuremap to include the properties.
$featuremap = tripal_core_expand_chado_vars($featuremap,'table','featuremapprop');
$featuremap = tripal_core_expand_chado_vars($featuremap,'field','featuremapprop.value');

?>
<div id="tripal_featuremap-base-box" class="tripal_featuremap-info-box tripal-info-box">
  <div class="tripal_featuremap-info-box-title tripal-info-box-title">featuremap Details</div>
  <div class="tripal_featuremap-info-box-desc tripal-info-box-desc"></div>

   <?php if(strcmp($featuremap->is_obsolete,'t')==0){ ?>
      <div class="tripal_featuremap-obsolete">This featuremap is obsolete</div>
   <?php }?>
   <table id="tripal_featuremap-base-table" class="tripal_featuremap-table tripal-table tripal-table-vert">
      <tr class="tripal_featuremap-table-even-row tripal-table-even-row">
        <th nowrap>Unique Name</th>
        <td><?php print $featuremap->uniquename; ?></td>
      </tr>
      <tr class="tripal_featuremap-table-odd-row tripal-table-odd-row">
        <th>Internal ID</th>
        <td><?php print $featuremap->featuremap_id; ?></td>
      </tr>
      <tr class="tripal_featuremap-table-even-row tripal-table-even-row">
        <th>Organism</th>
        <td>
          <?php if ($featuremap->organism_id->nid) { 
      	   print "<a href=\"".url("node/".$featuremap->organism_id->nid)."\">".$featuremap->organism_id->genus ." " . $featuremap->organism_id->species ." (" .$featuremap->organism_id->common_name .")</a>";      	 
          } else { 
            print $featuremap->organism_id->genus ." " . $featuremap->organism_id->species ." (" .$featuremap->organism_id->common_name .")";
          } ?>
        </td>
      </tr>      
      <tr class="tripal_featuremap-table-odd-row tripal-table-odd-row">
        <th>Type</th>
        <td><?php 
            if ($featuremap->type_id->name == 'cdna_featuremap') {
               print 'cDNA';
            } else if ($featuremap->type_id->name == 'bac_featuremap') {
               print 'BAC';
            } else {
               print $featuremap->type_id->name;
            }
          ?>
        </td>
      </tr>
      <tr class="tripal_featuremap-table-even-row tripal-table-even-row">
        <th>Description</th>
        <td><?php
           // right now we only have one property for libraries. So we can just
           // refernece it directly.  If we had more than one property
           // we would need to convert this to an if statment and loop
           // until we found the right one.
           print $featuremap->featuremapprop->value?>
        </td>
     	</tr>           	                                
   </table>
</div>
