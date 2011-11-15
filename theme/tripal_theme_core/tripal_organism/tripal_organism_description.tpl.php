<?php
$organism = $variables['node']->organism;
?>
<div id="tripal_organism-description-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_organism-info-box-title tripal-info-box-title">Organism Description</div>
   <table id="tripal_organism-table-description" class="tripal_organism-table tripal-table tripal-table-horz">
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <td><?php print $organism->comment; ?></td>
      </tr>        	                                
   </table>
</div>
