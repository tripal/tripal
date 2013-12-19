<?php
$featuremap = $variables['node']->featuremap;
$references = array();

// expand the featuremap object to include the records from the featuremap_dbxref table
$options = array('return_array' => 1);
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremap_dbxref', $options);
$featuremap_dbxrefs = $featuremap->featuremap_dbxref;
if (count($featuremap_dbxrefs) > 0 ) {
  foreach ($featuremap_dbxrefs as $featuremap_dbxref) {    
    $references[] = $featuremap_dbxref->dbxref_id;
  }
}

if(count($references) > 0){ ?>
  <div id="tripal_featuremap-references-box" class="tripal_featuremap-info-box tripal-info-box">
    <div class="tripal_featuremap-info-box-title tripal-info-box-title">Cross References</div>
    <div class="tripal_featuremap-info-box-desc tripal-info-box-desc">This Map is also available in the following databases:</div>
    <table id="tripal_featuremap-references-table" class="tripal_featuremap-table tripal-table tripal-table-horz">
      <tr>
        <th>Dababase</th>
        <th>Accession</th>
      </tr> <?php
      $i = 0; 
      foreach ($references as $dbxref){         
        $class = 'tripal_featuremap-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_featuremap-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($dbxref->db_id->url) { 
              print l($dbxref->db_id->name, $dbxref->db_id->url, array('attributes' => array('target' => '_blank'))) . '<br> ' . $dbxref->db_id->description;             
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

