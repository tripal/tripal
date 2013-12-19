<?php
$pub = $variables['node']->pub;
$features = array();

// expand the pub object to include the records from the pub_dbxref table
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$feature_pager_id = 5;

// get the genotypes from the feature_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $feature_pager_id),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'feature_pub', $options);
$feature_pubs = $pub->feature_pub;
if (count($feature_pubs) > 0 ) {
  foreach ($feature_pubs as $feature_pub) {    
    $features[] = $feature_pub->feature_id;
  }
}

// create the pager.  
global $pager_total_items;
$feature_pager = theme('pager', array(), $num_results_per_page, $feature_pager_id, array('block' => 'features'));
$total_features = $pager_total_items[$feature_pager_id];


if(count($features) > 0){ ?>
  <div id="tripal_pub-features-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Features</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication contains information about <?php print number_format($total_features) ?> features:</div>
    <table id="tripal_pub-feature-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Feature Name</th>
        <th>Type</th>
      </tr> <?php
      $i = 0; 
      foreach ($features as $feature){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($feature->nid) { 
              print l($feature->name, 'node/' . $feature->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $feature->name;
            } ?>
          </td>
          <td><?php print $feature->type_id->name ?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $feature_pager ?>
  </div><?php 
}?>

