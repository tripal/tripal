<?php
// Purpose: Provide layout and content for the basic stock details. This
//   includes all fields in the chado stock table supplemented with extra
//   details for each foreign key to provide human-readable output
//
// Note: This template controls the layout/content for the default stock node
//   template (node-chado_stock.tpl.php) and the Stock Details Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<?php 
  $stock = $node->stock;
  $organism = $node->stock->organism_id; 
  $main_db_reference = $stock->dbxref_id;
?>

<div id="tripal_stock-base-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">
    <?php print l($node->stock->name, 'node/'.$node->nid); ?>
  </div>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc"></div>
  
   <?php if($stock->is_obsolete == 't'){ ?>
      <div class="tripal_stock-obsolete">This stock is obsolete and no longer used in analysis, but is here for reference</div>
   <?php }?>
   <table class="tripal_stock-table tripal-table tripal-table-vert">
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <td><?php print $stock->name; ?></td>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-odd-row">
        <th nowrap>Unique Name</th>
        <td><?php print $stock->uniquename; ?></td>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Internal ID</th>
        <?php if (!empty($main_db_reference->dbxref_id)) { ?>
        	<?php 
        		if ($main_db_reference->db_id->urlprefix) {
        			$accession = l($main_db_reference->accession, $main_db_reference->db_id->urlprefix.$main_db_reference->accession);
        		} else {
        			$accession = $main_db_reference->accession;
        		}
        		if ($main_db_reference->db_id->url) {
        			$accession .= ' ('.l($main_db_reference->db_id->name, $main_db_reference->db_id->url).')';
        		} else {
        			$accession .= ' ('.$main_db_reference->db_id->name.')';
        		}
        	?>
        	<td><?php print $accession; ?></td>
        <?php } else { ?>
        	<td></td>
        <?php } ?>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-odd-row">
        <th>Type</th>
        <td><?php print $stock->type_id->name; ?></td>
      </tr>
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Organism</th>
        <td>
          <?php if ($organism->nid) { ?>
      	   <a href="<?php print url("node/$organism->nid") ?>"><?php print $organism->genus ." " . $organism->species ." (" .$organism->common_name ." )"?></a>
      	 <?php 
          } else { 
            print $organism->genus ." " . $organism->species ." (" .$organism->common_name ." )";
          } ?>
        </td>
     	</tr>           	                                
   </table>
</div>