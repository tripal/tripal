<?php
$organism = $variables['node']->organism;

// expand the organism object to include the libraries from the library
// table in chado.
$organism = tripal_core_expand_chado_vars($organism,'table','library');


// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$libraries = $organism->library;
if (!$libraries) {
   $libraries = array();
} 
elseif (!is_array($libraries)) { 
   $libraries = array($libraries); 
}

if (count($libraries) > 0) {?>
  <div id="tripal_organism-library_list-box" class="tripal_organism-info-box tripal-info-box">
    <div class="tripal_organism-info-box-title tripal-info-box-title">Libraries</div>
    <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following libraries are associated with this organism.</div>
    <table id="tripal_organism-table-library_list" class="tripal_organism-table tripal-table tripal-table-horz">     
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Library Name</th>
        <th>Description</th>
        <th>Type</th>
      </tr> <?php
      foreach ($libraries as $library){ 
        // expand the library to include the properties.
        $library = tripal_core_expand_chado_vars($library,'table','libraryprop');
        $library = tripal_core_expand_chado_vars($library,'field','libraryprop.value');
        $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
          $class = 'tripal_organism-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
            if($library->nid){    
              $link =  url("node/$library->nid");        
              print "<a href=\"$link\">$library->name</a>";
            } 
            else {
              print $library->name;
            } ?>
          </td>
          <td><?php 
            // right now we only have one property for libraries. So we can just
            // refernece it directly.  If we had more than one property
            // we would need to convert this to an if statment and loop
            // until we found the right one.
            print $library->libraryprop->value?>
          </td>
          <td> <?php 
            if ($library->type_id->name == 'cdna_library') {
              print 'cDNA';
            } 
            else if ($library->type_id->name == 'bac_library') {
              print 'BAC';
            } 
            else {
              print $library->type_id->name;
            }?>
          </td>
        </tr> <?php
        $i++; 
      }?>  
    </table> 
  </div><?php
}




