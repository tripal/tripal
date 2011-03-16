<?php

$feature  = $variables['node']->feature;
$accession = $variables['node']->accession;
$organism = $variables['node']->organism;
$org_nid = $variables['node']->org_nid;

?>
<div id="tripal_feature-base-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Feature Details</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>

   <?php if($feature->is_obsolete == 't'){ ?>
      <div class="tripal_feature-obsolete">This feature is obsolete and no longer used in analysis, but is here for reference</div>
   <?php }?>
   <table id="tripal_feature-base-table" class="tripal_feature-table tripal-table tripal-table-vert">
      <tr class="tripal_feature-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <td><?php print $feature->featurename; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
        <th nowrap>Unique Name</th>
        <td><?php print $feature->uniquename; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-even-row">
        <th>Internal ID</th>
        <td><?php print $accession; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
        <th>Length</th>
        <td><?php print $feature->seqlen ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-even-row">
        <th>Type</th>
        <td><?php print $feature->cvname; ?></td>
      </tr>
      <tr class="tripal_feature-table-odd-row tripal-table-odd-row">
        <th>Organism</th>
        <td>
          <?php if ($node->org_nid) { ?>
      	   <a href="<?php print url("node/$org_nid") ?>"><?php print $organism->genus ." " . $organism->species ." (" .$organism->common_name ." )"?></a>
      	 <?php 
          } else { 
            print $organism->genus ." " . $organism->species ." (" .$organism->common_name ." )";
          } ?>
        </td>
     	</tr>           	                                
   </table>
</div>
