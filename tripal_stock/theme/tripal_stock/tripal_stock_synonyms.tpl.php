<?php
// there is no stock_synonym table, analogous to the feature_synonym table.
// Therefore, synonyms have been stored in the stockprop table with a type 
// of 'synonym' or 'alias'.
$stock = $node->stock;
$synonyms = array();

// expand the stock object to include the stockprop records
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockprop', $options);
$stockprops = $stock->stockprop;

// iterate through all of the properties and pull out only the synonyms
if ($stockprops) {
  foreach ($stockprops as $stockprop){    
    if($stockprop->type_id->name == 'synonym' or $stockprop->type_id->name == 'alias'){
      $synonyms[] = $stockprop;
    }
  }
}

if(count($synonyms) > 0){ ?>
	<div id="tripal_stock-synonyms-box" class="tripal_stock-info-box tripal-info-box">
	  <div class="tripal_stock-info-box-title tripal-info-box-title">Synonyms</div>
	  <div class="tripal_stock-info-box-desc tripal-info-box-desc">The feature '<?php print $stock->name ?>' has the following synonyms</div> 
	  		
	  <table id="tripal_stock-synonyms-table" class="tripal_stock-table tripal-table tripal-table-horz">
      <tr>
        <th>Name</th>
      </tr> <?php
      $i = 0; 
      foreach ($synonyms as $synonym){
        $class = 'tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php print $synonym->value?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
	</div><?php
}
