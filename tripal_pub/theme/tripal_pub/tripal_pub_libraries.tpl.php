<?php
$pub = $variables['node']->pub;
$libraries = array();

// expand the pub object to include the records from the pub_dbxref table
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$library_pager_id = 5;

// get the genotypes from the library_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $library_pager_id),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'library_pub', $options);
$library_pubs = $pub->library_pub;
if (count($library_pubs) > 0 ) {
  foreach ($library_pubs as $library_pub) {    
    $libraries[] = $library_pub->library_id;
  }
}

// create the pager.  
global $pager_total_items;
$library_pager = theme('pager', array(), $num_results_per_page, $library_pager_id, array('block' => 'libraries'));
$total_libraries = $pager_total_items[$library_pager_id];


if(count($libraries) > 0){ ?>
  <div id="tripal_pub-libraries-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Libraries</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication contains information about <?php print number_format($total_libraries) ?> libraries:</div>
    <table id="tripal_pub-library-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Library Name</th>
        <th>Species</th>
        <th>Type</th>
      </tr> <?php
      $i = 0; 
      foreach ($libraries as $library){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($library->nid) { 
              print l($library->name, 'node/' . $library->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $library->name;
            } ?>
          </td>
          <td><?php print $library->type_id->name ?></td>
          <td><?php print $library->organism_id->genus ?> <?php print $library->organism_id->species ?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $library_pager ?>
  </div><?php 
}?>

