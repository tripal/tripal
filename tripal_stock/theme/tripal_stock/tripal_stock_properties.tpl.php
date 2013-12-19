<?php

$stock = $node->stock;
$properties = array();

// expand the stock object to include the stockprop records
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockprop', $options);
$stockprops = $stock->stockprop;

// iterate through all of the properties and pull out only the synonyms
if ($stockprops) {
  foreach ($stockprops as $stockprop){    
    if($stockprop->type_id->name == 'synonym' or $stockprop->type_id->name == 'alias'){
      // there is no stock_synonym table, analogous to the feature_synonym table.
      // Therefore, synonyms have been stored in the stockprop table with a type 
      // of 'synonym' or 'alias'. Synonyms are shown in the tripal_stock_synonyms.tpl.php
      // template, so we exclude those types of properties for this template    
    } 
    else {
      $properties[] = $stockprop;  
    }
  }
}

if(count($properties) > 0){ ?>
	<div id="tripal_stock-properties-box" class="tripal_stock-info-box tripal-info-box">
	  <div class="tripal_stock-info-box-title tripal-info-box-title">Properties</div>
	  <div class="tripal_stock-info-box-desc tripal-info-box-desc">Properties for the stock '<?php print $node->stock->name ?>' include:</div>
	  <table class="tripal_stock-table tripal-table tripal-table-horz">
	    <tr><th>Type</th><th>Value</th></tr> <?php	
			$i = 0;
			// iterate through each property
			foreach ($properties as $property){
			  $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
	      if($i % 2 == 0 ){
	         $class = 'tripal_stock-table-even-row tripal-table-even-row';
	      }?>   
        <tr class="<?php print $class ?>">
          <td><?php print ucwords(preg_replace('/_/', ' ', $property->type_id->name)) ?></td>
          <td><?php print $property->value?></td>
        </tr> <?php	      
				$i++;
			} ?>
	  </table>
	</div><?php  
}
