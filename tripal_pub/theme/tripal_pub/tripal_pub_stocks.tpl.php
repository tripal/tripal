<?php
$pub = $variables['node']->pub;
$stocks = array();

// expand the pub object to include the records from the pub_dbxref table
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$stock_pager_id = 5;

// get the genotypes from the stock_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $stock_pager_id),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'stock_pub', $options);
$stock_pubs = $pub->stock_pub;
if (count($stock_pubs) > 0 ) {
  foreach ($stock_pubs as $stock_pub) {    
    $stocks[] = $stock_pub->stock_id;
  }
}

// create the pager.  
global $pager_total_items;
$stock_pager = theme('pager', array(), $num_results_per_page, $stock_pager_id, array('block' => 'stocks'));
$total_stocks = $pager_total_items[$stock_pager_id];


if(count($stocks) > 0){ ?>
  <div id="tripal_pub-stocks-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Stocks</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication contains information about <?php print number_format($total_stocks) ?> stocks:</div>
    <table id="tripal_pub-stock-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Stock Name</th>
        <th>Type</th>
      </tr> <?php
      $i = 0; 
      foreach ($stocks as $stock){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($stock->nid) { 
              print l($stock->name, 'node/' . $stock->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $stock->name;
            } ?>
          </td>
          <td><?php print $stock->type_id->name ?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $stock_pager ?>
  </div><?php 
}?>

