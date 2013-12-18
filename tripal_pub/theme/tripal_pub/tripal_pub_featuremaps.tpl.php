<?php
$pub = $variables['node']->pub;
$featuremaps = array();

// expand the pub object to include the records from the pub_dbxref table
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$featuremap_pager_id = 5;

// get the genotypes from the featuremap_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $featuremap_pager_id),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'featuremap_pub', $options);
$featuremap_pubs = $pub->featuremap_pub;
if (count($featuremap_pubs) > 0 ) {
  foreach ($featuremap_pubs as $featuremap_pub) {    
    $featuremaps[] = $featuremap_pub->featuremap_id;
  }
}

// create the pager.  
global $pager_total_items;
$featuremap_pager = theme('pager', array(), $num_results_per_page, $featuremap_pager_id, array('block' => 'featuremaps'));
$total_featuremaps = $pager_total_items[$featuremap_pager_id];


if(count($featuremaps) > 0){ ?>
  <div id="tripal_pub-featuremaps-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Maps</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication contains information about <?php print number_format($total_featuremaps) ?> map(s):</div>
    <table id="tripal_pub-featuremap-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Map Name</th>
      </tr> <?php
      $i = 0; 
      foreach ($featuremaps as $featuremap){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($featuremap->nid) { 
              print l($featuremap->name, 'node/' . $featuremap->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $featuremap->name;
            } ?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $featuremap_pager ?>
  </div><?php 
}?>

