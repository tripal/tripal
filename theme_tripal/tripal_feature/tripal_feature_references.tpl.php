<?php
$feature = $variables['node']->feature;

// expand the feature object to include the external references stored
// in the feature_dbxref table
$feature = tripal_core_expand_chado_vars($feature,'table','feature_dbxref');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$references = $feature->feature_dbxref;
if (!$references) {
   $references = array();
} elseif (!is_array($references)) { 
   $references = array($references); 
}
// check to see if the reference 'GFF_source' is there.  This reference is
// used to help the GBrowse chado adapter find features.  We don't need to show
// it
if($references[0]->dbxref_id->db_id->name == 'GFF_source' and count($references)==1){
   $references = array();
}
?>
<div id="tripal_feature-references-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">References</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">External references for this <?php print $feature->type_id->name ?></div>
  <?php if(count($references) > 0){ ?>
  <table id="tripal_feature-references-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Dababase</th>
      <th>Accession</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($references as $feature_dbxref){ 
      if($feature_dbxref->dbxref_id->db_id->name == 'GFF_source'){
         continue;  // skip the GFF_source entry as this is just needed for the GBrowse chado adapter
      }
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $feature_dbxref->dbxref_id->db_id->name?></td>
        <td><?php 
           if($feature_dbxref->db_id->urlprefix){ 
              ?><a href="<?php print $feature_dbxref->db_id->urlprefix.$feature_dbxref->dbxref_id->accession?>" target="_blank"><?php print $feature_dbxref->dbxref_id->accession?></a><?php 
           } else { 
             print $feature_dbxref->dbxref_id->accession; 
           } 
           ?>
        </td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no external references</div> 
  <?php }?>
</div>
