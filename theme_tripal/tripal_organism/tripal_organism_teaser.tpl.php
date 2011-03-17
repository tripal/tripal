<?php
$organism = $variables['node']->organism;
?>
<div id="tripal_organism-base-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_organism-info-box-title tripal-info-box-title">
    <?php print l($organism->common_name . ' ('.$organism->genus.' '.$organism->species.')', 'node/'.$node->nid); ?>
  </div>
  <div class="tripal_organism-info-box-desc tripal-info-box-desc"></div>
   <table id="tripal_organism-table-base" class="tripal_organism-table tripal-table tripal-table-vert">
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Common Name</th>
        <td><?php print $organism->common_name; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-odd-row">
        <th>Genus</th>
        <td><?php print $organism->genus; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Species</th>
        <td><?php print $organism->species; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-odd-row">
        <th>Abbreviation</th>
        <td><?php print $organism->abbreviation; ?></td>
      </tr>         	                                
   </table>
</div>
