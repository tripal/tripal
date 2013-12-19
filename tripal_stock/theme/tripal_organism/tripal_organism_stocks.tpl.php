<?php
$organism = $variables['node']->organism;

// expand the featuremap object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 25; 
$pager_id = 3;

// get the features aligned on this map
$options = array(  
  'return_array' => 1,
  'order_by' => array('name' => 'ASC'),
  'pager' => array('limit' => $num_results_per_page, 'element' => $pager_id),
  'include_fk' => array(
    'type_id' => 1    
  ),
);

$organism = tripal_core_expand_chado_vars($organism, 'table', 'stock', $options);
$stocks = $organism->stock;

// create the pager.  
global $pager_total_items;
$pager = theme('pager', array(), $num_results_per_page, $pager_id, array('block' => 'stocks'));
$total_features = $pager_total_items[$pager_id];

 
if (count($stocks) > 0) { ?>
  <div id="tripal_organism-stocks-box" class="tripal_organism-info-box tripal-info-box">
    <div class="tripal_organism-info-box-title tripal-info-box-title">Stocks</div>
    <div class="tripal_organism-info-box-desc tripal-info-box-desc">This organism is associated with <?php print number_format($total_features) ?> stock(s):</div>
    
    <table id="tripal_organism-table-stocks" class="tripal_organism-table tripal-table tripal-table-horz">     
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <th>Type</th>
      </tr> <?php
      foreach ($stocks as $stock){ 
        $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
          $class = 'tripal_organism-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php
            $name = $stock->name;
            if (!$stock->name) {
            	$name = $stock->uniquename;
            }
            if ($stock->nid) {    
              print l($name, "node/$stock->nid", array('attributes' => array('target' => '_blank')));        
            } else {
              print $name;
            }?>
          </td>
          <td><?php print $stock->type_id->name?></td>
        </tr><?php
        $i++;  
      } ?>
    </table>
    <?php print $pager ?>
  </div> <?php
} 




