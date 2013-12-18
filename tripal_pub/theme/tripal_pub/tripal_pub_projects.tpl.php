<?php
$pub = $variables['node']->pub;
$projects = array();

// expand the pub object to include the records from the pub_dbxref table
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$project_pager_id = 5;

// get the genotypes from the project_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $project_pager_id),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'project_pub', $options);
$project_pubs = $pub->project_pub;
if (count($project_pubs) > 0 ) {
  foreach ($project_pubs as $project_pub) {    
    $projects[] = $project_pub->project_id;
  }
}

// create the pager.  
global $pager_total_items;
$project_pager = theme('pager', array(), $num_results_per_page, $project_pager_id, array('block' => 'projects'));
$total_projects = $pager_total_items[$project_pager_id];


if(count($projects) > 0){ ?>
  <div id="tripal_pub-projects-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Projects</div>
    <div class="tripal_pub-info-box-desc tripal-info-box-desc">This publication contains information about <?php print number_format($total_projects) ?> projects:</div>
    <table id="tripal_pub-project-table" class="tripal_pub-table tripal-table tripal-table-horz">
      <tr>
        <th>Project Name</th>
      </tr> <?php
      $i = 0; 
      foreach ($projects as $project){         
        $class = 'tripal_pub-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pub-table-even-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td> <?php 
            if ($project->nid) { 
              print l($project->name, 'node/' . $project->nid, array('attributes' => array('target' => '_blank')));
            } 
            else { 
              print $project->name;
            } ?>
          </td>
        </tr> <?php
        $i++;  
      } ?>
    </table> <?php 
    print $project_pager ?>
  </div><?php 
}?>

