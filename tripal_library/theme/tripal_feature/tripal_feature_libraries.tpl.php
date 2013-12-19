<?php
$feature = $variables['node']->feature;

// expand the feature object to include the libraries from the library_feature
// table in chado.
$feature = tripal_core_expand_chado_vars($feature,'table','library_feature');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$library_features = $feature->library_feature;
if (!$library_features) {
   $library_features = array();
} 
elseif (!is_array($library_features)) { 
   $library_features = array($library_features); 
}

// don't show this page if there are no libraries
if (count($library_features) > 0) { ?>
  <div id="tripal_feature-libraries-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Libraries</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This <?php print $feature->type_id->name ?> is derived, or can be located in the following libraries</div>
    <table id="tripal_feature-libraries-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Library Name</th>
        <th>Type</th>
      </tr> <?php
      $i = 0; 
      foreach ($library_features as $library_feature) {
        $class = 'tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
            if ($library_feature->library_id->nid) {
               print "<a href=\"". url("node/".$library_feature->library_id->nid) . "\">".$library_feature->library_id->name."</a>";
            } else {
               print $library_feature->library_id->name;
            } ?>
          </td>
          <td> <?php 
              if ($library_feature->library_id->type_id->name == 'cdna_library') {
                 print 'cDNA';
              } else if ($library_feature->library_id->type_id->name == 'bac_library') {
                 print 'BAC';
              } else {
                 print $library_feature->library_id->type_id->name;
              } ?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> 
  </div><?php 
} 

