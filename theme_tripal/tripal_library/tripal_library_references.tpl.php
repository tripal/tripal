<?php
$library = $variables['node']->library;

// expand the library object to include the external references stored
// in the library_dbxref table
$library = tripal_core_expand_chado_vars($library,'table','library_dbxref');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$references = $library->library_dbxref;
if (!$references) {
   $references = array();
} elseif (!is_array($references)) { 
   $references = array($references); 
}
// check to see if the reference 'GFF_source' is there.  This reference is
// used to help the GBrowse chado adapter find librarys.  We don't need to show
// it
if($references[0]->dbxref_id->db_id->name == 'GFF_source' and count($references)==1){
   $references = array();
}
?>
<div id="tripal_library-references-box" class="tripal_library-info-box tripal-info-box">
  <div class="tripal_library-info-box-title tripal-info-box-title">References</div>
  <div class="tripal_library-info-box-desc tripal-info-box-desc">External references for this <?php print $library->type_id->name ?></div>
  <?php if(count($references) > 0){ ?>
  <table id="tripal_library-references-table" class="tripal_library-table tripal-table tripal-table-horz">
    <tr>
      <th>Dababase</th>
      <th>Accession</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($references as $library_dbxref){ 
      if($library_dbxref->dbxref_id->db_id->name == 'GFF_source'){
         continue;  // skip the GFF_source entry as this is just needed for the GBrowse chado adapter
      }
      $class = 'tripal_library-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_library-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $library_dbxref->dbxref_id->db_id->name?></td>
        <td><?php 
           if($library_dbxref->db_id->urlprefix){ 
              ?><a href="<?php print $library_dbxref->db_id->urlprefix.$library_dbxref->dbxref_id->accession?>" target="_blank"><?php print $library_dbxref->dbxref_id->accession?></a><?php 
           } else { 
             print $library_dbxref->dbxref_id->accession; 
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
