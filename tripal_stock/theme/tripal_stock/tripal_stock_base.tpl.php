<?php
$stock = $node->stock;
$organism = $node->stock->organism_id; 
$main_db_reference = $stock->dbxref_id;

// expand the text fields
$stock = tripal_core_expand_chado_vars($stock, 'field', 'stock.description');
$stock = tripal_core_expand_chado_vars($stock, 'field', 'stock.uniquename');

?>
<div id="tripal_stock-base-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">Details</div>
  <!-- <div class="tripal_stock-info-box-desc tripal-info-box-desc"></div> -->

   <?php if($stock->is_obsolete == 't'){ ?>
      <div class="tripal_stock-obsolete">This stock is obsolete and no longer used in analysis, but is here for reference</div>
   <?php }?>
   <table class="tripal_stock-table tripal-table tripal-table-vert">
      <tr class="tripal_stock-table-even-row tripal-table-even-row">
        <th>Name</th>
        <td><?php print $stock->name; ?></td>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-odd-row">
        <th nowrap>Unique Name</th>
        <td><?php print $stock->uniquename ?></td>
      </tr>
      <tr class="tripal_stock-table-even-row tripal-table-even-row">
        <th>Type</th>
        <td><?php print ucwords(preg_replace('/_/', ' ', $stock->type_id->name)) ?></td>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-odd-row">
        <th>Organism</th>
        <td><?php 
          if ($organism->nid) { ?>
      	   <a href="<?php print url("node/$organism->nid") ?>"><?php print "<i>" . $organism->genus . 
            " " . $organism->species . "</i> (" . $organism->common_name . " )"?></a><?php 
          } else { 
            print "<i>" . $organism->genus . " " . $organism->species . "</i> (" . $organism->common_name . ")";
          } ?>
        </td>
     	</tr>   
      <tr class="tripal_stock-table-even-row tripal-table-even-row">
        <th>Description</th>
        <td><?php print $stock->description ?></td>      
      </tr>   
      <!--  
      <tr class="tripal_stock-table-odd-row tripal-table-odd-row">
        <th>Internal ID</th>
        <td><?php print $stock->stock_id ?></td>
      </tr>     -->	                                
   </table>
</div>