<?php
$library = $variables['node']->library;

// expand the library object to include the external references stored
// in the library_dbxref table
$options = array('return_array' => 1);
$library = tripal_core_expand_chado_vars($library,'table','library_dbxref', $options);
$references = $library->library_dbxref;


if(count($references) > 0){ ?>
  <div id="tripal_library-references-box" class="tripal_library-info-box tripal-info-box">
    <div class="tripal_library-info-box-title tripal-info-box-title">Cross References</div>
    <div class="tripal_library-info-box-desc tripal-info-box-desc">External references for this <?php print $library->type_id->name ?> library</div>
    <table id="tripal_library-references-table" class="tripal_library-table tripal-table tripal-table-horz">
      <tr>
        <th>Dababase</th>
        <th>Accession</th>
      </tr> <?php
      $i = 0; 
      foreach ($references as $library_dbxref) { 
        $dbxref = $library_dbxref->dbxref_id;
        $class = 'tripal_library-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_library-table-odd-row tripal-table-even-row';
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
            } ?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
  </div><?
}
