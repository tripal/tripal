<?php
$feature = $variables['node']->feature;
$references = array();

// First, get the dbxref record from feature recrod itself if one exists
if ($feature->dbxref_id) {
  $feature->dbxref_id->is_primary = 1;  // add this new property so we know it's the primary reference
  $references[] = $feature->dbxref_id;
}

// Second, expand the feature object to include the records from the feature_dbxref table
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_dbxref', $options);
$feature_dbxrefs = $feature->feature_dbxref;
if (count($feature_dbxrefs) > 0 ) {
  foreach ($feature_dbxrefs as $feature_dbxref) {    
    if($feature_dbxref->dbxref_id->db_id->name == 'GFF_source'){
      // check to see if the reference 'GFF_source' is there.  This reference is
      // used to if the Chado Perl GFF loader was used to load the features   
    }
    else {
      $references[] = $feature_dbxref->dbxref_id;
    }
  }
}


if(count($references) > 0){ ?>
  <div id="tripal_feature-references-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Cross References</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">External references for this <?php print $feature->type_id->name ?></div>
    <table id="tripal_feature-references-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Dababase</th>
        <th>Accession</th>
      </tr> <?php
      $i = 0; 
      foreach ($references as $dbxref){ 
        if($dbxref_id->db_id->name == 'GFF_source'){
           continue;  // skip the GFF_source entry as this is just needed for the GBrowse chado adapter
        }
        $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_feature-table-even-row tripal-table-even-row';
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
}?>

