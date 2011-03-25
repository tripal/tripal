<?php
// Copyright 2010 University of Saskatchewan (Lacey-Anne Sanderson)
//
// Purpose: Provides the layout and content for stock external database 
//   references (this doesn't include the main database reference from the
//   stock table). This includes all fields from the dbxref and db chado tables
//   where each dbxref is associated with the current stock through the
//   stock_dbxref table
//
// Note: This template controls the layout/content for the default stock node
//   template (node-chado_stock.tpl.php) and the Stock Database References Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->db_references: an array of stock database reference objects 
//       where each object has the following fields: dbxref_id, accession,
//       version, description, db_id, db_name, db_description, db_url, 
//       db_urlprefix 
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<?php
  $db_references = $node->stock->stock_dbxref;
  if (!$db_references) {
    $db_references = array();
  } elseif (!is_array($db_references)) {
    $db_references = array($db_references);
  }
?>

<div id="tripal_stock-references-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">References</div>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc">The stock '<?php print $node->stock->name ?>' is also available at these locations</div>
  <?php if(count($db_references) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
    <tr>
      <th>Dababase</th>
      <th>Accession</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($db_references as $result){ 
      $dbxref = $result->dbxref_id;
      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $dbxref->db_id->name?></td>
        <td><?php 
           if($dbxref->db_id->urlprefix){ 
           	 print l($dbxref->accession, $dbxref->db_id->urlprefix.$dbxref->accession);
           } else { 
             print $dbxref->accession; 
           } 
           ?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else {
    print '<b>There are no external database references for the current stock.</b>';
  }?>
</div>