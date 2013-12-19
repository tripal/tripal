<?php
$stock = $variables['node']->stock;

// expand the stock object to include the stockcollections associated with this stock
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockcollection_stock', $options);
$collections = $stock->stockcollection_stock;

if (count($collections) > 0) {?>
  <div id="tripal_stock-collections-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Stock Collections</div>
    <div class="tripal_stock-info-box-desc tripal-info-box-desc">This stock is found in the following collections.</div>
    <table id="tripal_stock-table-collection" class="tripal_stock-table tripal-table tripal-table-horz">     
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Collection Name</th>
        <th>Type</th>
        <th>Contact</th>
      </tr> <?php
      foreach ($collections as $collection_stock){ 
        // get the stock collection details
        $collection = $collection_stock->stockcollection_id;
        
        $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
          $class = 'tripal_stock-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
            if($collection->nid){    
              $link =  url("node/$collection->nid");        
              print "<a href=\"$link\">$collection->name</a>";
            } 
            else {
              print $collection->name;
            } ?>
          </td>
          <td><?php print ucwords(preg_replace('/_/', ' ', $collection->type_id->name)) ?> </td>
          <td><?php 
            $contact = $collection->contact_id; 
            print $contact->name . "<br>" .  $contact->description ?>              
          </td>
        </tr> <?php
        $i++; 
      }?>  
    </table> 
  </div><?php
}