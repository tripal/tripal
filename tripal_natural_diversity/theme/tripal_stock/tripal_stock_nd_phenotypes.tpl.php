<?php
/*
 * Phenotype relationships with stocks are stored in the natural diversity tables.
 * If a stock has phenotypes associated then you can find the data by traversing
 * the foreign key (FK) relationships in this manner: 
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_phenotype => phenotype.
 * 
 * You can find ancillary information about data associated with a phenotype such as
 * a contact, pub, protocol, project, phenotype, dbxref by using the 
 * nd_experiment.nd_experiment_id value and traversing the other FK relationships
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_phenotype => phenotype
 *                                               => nd_experiment_phenotype => phenotype
 *                                               => nd_experiment_project => project
 *                                               => nd_experiment_pub => pub
 *                                               => nd_experiment_contact => contact
 *                                               => nd_experiment_dbxref => dbxref
 *                                               => nd_experiment_protocol => protocol
 *                                               => nd_experiment_stockprop
 *                                               => nd_experiment_stock_dbxref
 * 
 * In the FK relationships shown above, the nd_experiment_id value represents a single 
 * experimental value that may have all of the ancilliary data associated with it.  
 * If the phenotype record shares an nd_experiment_id with a phenotype, pub, contact,
 * protocol, etc then all of that data is associated with the phenotype and vice-versa.
 * 
 * Techincally, we can skip including the 'nd_experiment' table when traversing the FK's 
 * because we have the nd_experiment_id value when we get the nd_experiment_stock record.
 * 
 * When lots of phenotypes are associated with a stock (e.g. thousands) then traversing
 * the FK relationships as described above can be very slow. Ideally, we only need to
 * show a small subset with a pager. Therefore, a list of nd_experiment_phenotype_id's 
 * are provided to this template automatically within the stock object.
 * 
 */

// get the current stock
$stock = $variables['node']->stock;

// specify the number of phenotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$stock_pager_id = 10;


// the nd_experiment_phenotype IDs get passed into this template, so we use
// those to iterate and show a subset via a pager.  This is faster than trying
// to traverse all of the FK relationship, especially when thousands of 
// associations may be present.  Because the nd_experiment_id in Chado
// can be associated with other data types it becomes slow to use the
// chado_expand_var functions that we would normal use.
$nd_experiment_phenotype_ids = $stock->nd_experiment_phenotype_ids;
$total_records = count($nd_experiment_phenotype_ids);

// initialize the Drupal pager
$current_page_num = pager_default_initialize($total_records, $num_results_per_page, $stock_pager_id);
$offset = $num_results_per_page * $current_page_num;


$phenotypes = array();
if ($total_records > 0) {
  
  // iterate through the nd_experiment_phenotype_ids and get the phenotype record
  for ($i = $offset ; $i < $offset + $num_results_per_page; $i++) {
    // expand the nd_experiment record to include the nd_experiment_phenotype table
    // there many be many phenotypes for a stock so we want to use a pager to limit 
    // the results returned
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'phenotype_id' => array(
          'type_id' => 1,
        )
      ),
    );
    $values = array('nd_experiment_phenotype_id' => $nd_experiment_phenotype_id);
    $nd_experiment_phenotype = chado_generate_var('nd_experiment_phenotype', $values);
    $phenotype = $nd_experiment_phenotype->phenotype_id;
    $phenotypes[$phenotype->phenotype_id]['phenotype'] = $phenotype;
    $phenotypes[$phenotype->phenotype_id]['nd_experiment_id'] = $nd_experiment_phenotype->nd_experiment_id->nd_experiment_id;
  }
}

if (count($phenotypes) > 0) {?>
  <div class="tripal_stock-data-block-desc tripal-data-block-desc">
    The following <?php print number_format($total_records) ?> phenotypes(s) have been recorded.
  </div><?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Phenotypes', 'Project');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  // iterate through the nd_experiment_stock records and get 
  // each experiment and the associated phenotypes
  foreach ($phenotypes as $info){
    $phenotype         = $info['phenotype'];
    $nd_experiment_id  = $info['nd_experiment_id'];
    
    // get the nd_experiment record
    $nd_experiment = chado_generate_var('nd_experiment', array('nd_experiment_id' => $nd_experiment_id));
    
    $details = '';

    if ($phenotype->name) { 
      $details .= "Name: $phenotype->name<br>";
    }
    
    // add in the attribute type pheonotypes values are stored qualitatively or quantitatively. 
    // If qualitatively the cvalue_id will link to a type. If quantitative we
    // use the value column
    $details .= ucwords(preg_replace('/_/', ' ', $phenotype->attr_id->name)) . ': ';
    if ($phenotype->cvalue_id) { 
      $details .= ucwords(preg_replace('/_/', ' ', $phenotype->cvalue_id->name)) . '<br>';
    }
    else { 
      $details .= $phenotype->value . '<br>';
    }  
    
    // get the observable unit and add it to the details
    if ($phenotype->observable_id) { 
      $details .= "Observable Unit: " . ucwords(preg_replace('/_/', ' ', $phenotype->observable_id->name)) . '<br>';
    }
    
    // get the evidence unit and add it to the details
    if ($phenotype->assay_id) { 
      $details .= "Evidence: " .  ucwords(preg_replace('/_/', ' ', $phenotype->assay_id->name)) . '<br>';
    }
    
    // Get the project for this experiment. For each nd_experiment_id there should only be one project
    // but the database does not constrain that there only be one project so just in case we get them all
    $projects = array();
    $values = array('nd_experiment_id' => $nd_experiment_stock->nd_experiment_id->nd_experiment_id);
    $nd_experiment_project = tripal_core_generate_chado_var('nd_experiment_project', $values, $options);
    $nd_experiment_projects = $nd_experiment_project;
    foreach ($nd_experiment_projects as $nd_experiment_project) {
      // we do have a project record, so add it to our $phenotypes array for display below
      $projects = $nd_experiment_project->project_id;
    }
    $pnames = 'N/A';
    foreach ($projects as $project) {
      $project = $project->project_id;
      $name = $project->name;
      if (property_exists($project, 'nid')) {
        $name = l($name, "node/" . $project->nid, array('attributes' => array('target' => '_blank')));
      }
      $pnames .= $name . '<br>';
    }
    $pnames = substr($pnames, 0, -4); // remove trailing <br>
    
    $rows[] = array(
       $details,
       $pnames,
    );
  }
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_natural_diversity-table-phenotypes',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table); 
  
  // the $pager array values that control the behavior of the pager.  For
  // documentation on the values allows in this array see:
  // https://api.drupal.org/api/drupal/includes!pager.inc/function/theme_pager/7
  // here we add the paramter 'block' => 'features'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  $pager = array(
    'tags' => array(),
    'element' => $stock_pager_id,
    'parameters' => array(
      'block' => 'genotypes'
    ),
    'quantity' => $num_results_per_page,
  );
  print theme_pager($pager);
}