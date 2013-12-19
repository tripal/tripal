<?php
$pub = $variables['node']->pub;
$references = array();

// expand the pub object to include the records from the pub_dbxref table
$options = array('return_array' => 1);
$pub = tripal_core_expand_chado_vars($pub, 'table', 'pub_dbxref', $options);
$pub_dbxrefs = $pub->pub_dbxref;
if (count($pub_dbxrefs) > 0 ) {
  foreach ($pub_dbxrefs as $pub_dbxref) {    
    $references[] = $pub_dbxref->dbxref_id;
  }
}

if(count($references) > 0){ ?>
  <div id="tripal_pub-references-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Cross References</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication is also available in the following databases:</div>
    <table id="tripal_pub-references-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Dababase</th>
        <th>Accession</th>
      </tr> <?php
      $i = 0; 
      foreach ($references as $dbxref){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($dbxref->db_id->url) { 
              print l($dbxref->db_id->name, $dbxref->db_id->url, array('attributes' => array('target' => '_blank'))) . '<br>' . $dbxref->db_id->description;             
            } 
            else { 
              print $dbxref->db_id->name . '<br>' . $dbxref->db_id->description;
            } ?>
          </td>
          <td> <?php 
            if ($dbxref->db_id->urlprefix) { 
              print l($dbxref->db_id->name . ':' . $dbxref->accession, $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $dbxref->db_id->name . ':' . $dbxref->accession; 
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
}?>

