<?php
$stock = $variables['node']->stock;
$references = array();

// First, get the dbxref record from stock recrod itself if one exists
if ($stock->dbxref_id) {
  $stock->dbxref_id->is_primary = 1;  // add this new property so we know it's the primary reference
  $references[] = $stock->dbxref_id;
}

// Second, expand the stock object to include the records from the stock_dbxref table
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stock_dbxref', $options);
$stock_dbxrefs = $stock->stock_dbxref;
if (count($stock_dbxrefs) > 0 ) {
  foreach ($stock_dbxrefs as $stock_dbxref) {
    $references[] = $stock_dbxref->dbxref_id;
  }
}

if(count($references) > 0){ ?>
	<div id="tripal_stock-references-box" class="tripal_stock-info-box tripal-info-box">
	  <div class="tripal_stock-info-box-title tripal-info-box-title">Cross References</div>
	  <div class="tripal_stock-info-box-desc tripal-info-box-desc">The stock '<?php print $node->stock->name ?>' is also available at these locations</div>
	  <table class="tripal_stock-table tripal-table tripal-table-horz">
	    <tr>
	      <th>Dababase</th>
	      <th>Accession</th>
	    </tr> <?php
	    $i = 0; 
	    foreach ($references as $dbxref){ 
	      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
	      if($i % 2 == 0 ){
	         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
	      } ?>
	      <tr class="<?php print $class ?>">
	        <td> <?php 
	          if ($dbxref->db_id->url) { 
              print l($dbxref->db_id->name, $dbxref->db_id->url);
            } 
            else { 
              print $dbxref->db_id->name; 
            } ?>
	        </td>
	        <td> <?php 
	          if ($dbxref->db_id->urlprefix) { 
	           	print l($dbxref->accession, $dbxref->db_id->urlprefix.$dbxref->accession);
	          } 
	          else { 
	            print $dbxref->accession; 
	          }
	          if ($dbxref->is_primary) {
	            print " <i>(primary cross-reference)</i>";
	          } ?>
	        </td>
	      </tr> <?php
	      $i++;  
	    } ?>
	  </table> 
	</div><?php 
}