<?php
$feature = $variables['node']->feature;

// expand the feature object to include the libraries from the library_feature
// table in chado.
$feature = tripal_core_expand_chado_vars($feature,'table','analysisfeature');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$analyses = $feature->analysisfeature;
if (!$analyses) {
   $analyses = array();
} 
elseif (!is_array($analyses)) { 
   $analyses = array($analyses); 
}

// don't show this page if there are no libraries
if (count($analyses) > 0) { ?>
  <div id="tripal_feature-analyses-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Analyses</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This <?php print $feature->type_id->name ?> is derived from or has results from the following analyses</div>
    <table id="tripal_feature-analyses-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Analysis Name</th>
        <th>Date Performed</th>
      </tr> <?php
      $i = 0; 
      foreach ($analyses as $analysis) {
        $class = 'tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
            $nid = chado_get_node_id('analysis', $analysis->analysis_id->analysis_id);
            if ($nid) {
               print "<a href=\"". url("node/".$nid) . "\">".$analysis->analysis_id->name."</a>";
            } else {
               print $analysis->analysis_id->name;
            } ?>
          </td>
          <td> <?php print preg_replace('/\d\d:\d\d:\d\d/', '',  $analysis->analysis_id->timeexecuted) ?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> 
  </div><?php 
} 

