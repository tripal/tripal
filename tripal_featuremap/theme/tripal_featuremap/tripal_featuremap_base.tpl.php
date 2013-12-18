<?php

$featuremap  = $variables['node']->featuremap;

// expand the description field
$featuremap = tripal_core_expand_chado_vars($featuremap, 'field', 'featuremap.description');

?>
<div id="tripal_featuremap-base-box" class="tripal_featuremap-info-box tripal-info-box">
  <div class="tripal_featuremap-info-box-title tripal-info-box-title">Map Details</div>
  <div class="tripal_featuremap-info-box-desc tripal-info-box-desc"></div>

   <table id="tripal_featuremap-base-table" class="tripal_featuremap-table tripal-table tripal-table-vert">
      <tr class="tripal_featuremap-table-even-row tripal-table-even-row">
        <th nowrap>Name</th>
        <td><?php print $featuremap->name; ?></td>
      </tr>
      <tr class="tripal_featuremap-table-odd-row tripal-table-odd-row">
        <th>Map Units</th>
        <td><?php print $featuremap->unittype_id->name; ?></td>
      </tr>
      <tr class="tripal_featuremap-table-even-row tripal-table-even-row">
        <th>Description</th>
        <td><?php
           // right now we only have one property for featuremaps. So we can just
           // refernece it directly.  If we had more than one property
           // we would need to convert this to an if statment and loop
           // until we found the right one.
           print $featuremap->description?>
        </td>
     	</tr>
     	<!--     
     	<tr class="tripal_featuremap-table-odd-row tripal-table-odd-row">
        <th>Internal ID</th>
        <td><?php print $featuremap->featuremap_id; ?></td>
      </tr>  
       -->     	                                
   </table>
</div>
