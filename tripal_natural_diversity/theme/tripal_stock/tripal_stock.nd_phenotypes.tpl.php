<?php
/*
 * Phenotype relationships with stocks are stored in the natural diversity tables.
 * If a stock has phenotypes associated then you can find the data by traversing
 * the foreign key (FK) relationships in this manner: 
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_phenotype => phenotype.
 * 
 * You can find ancillary information about data associated with a phenotype such as
 * a contact, pub, protocol, project, genotype, dbxref by using the 
 * nd_experiment.nd_experiment_id value and traversing the other FK relationships
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_phenotype => phenotype
 *                                               => nd_experiment_genotype => genotype
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
 * If the phenotype record shares an nd_experiment_id with a genotype, pub, contact,
 * protocol, etc then all of that data is associated with the phenotype and vice-versa.
 * 
 * Techincally, we can skip including the 'nd_experiment' table when traversing the FK's 
 * because we have the nd_experiment_id value when we get the nd_experiment_stock record.
 * 
 */

// get the current stock
$stock = $variables['node']->stock;

// expand the stock object to include the nd_experiment_stock table
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'nd_experiment_stock', $options);
$nd_experiment_stocks = $stock->nd_experiment_stock;

// Get the experiments to which this stock belongs that have a phenotype
// associated.  Store those in the $phenotypes array indexed by the nd_experiment_id
$phenotypes = array();
if (count($nd_experiment_stocks) > 0) {
  
  // iterate through the nd_experiment_stock records.  there could be multiple experimetnal
  // units (e.g. nd_experiment_id's) for this stock and we want to use only those that have
  // phenotypes associated with them.
  foreach ($nd_experiment_stocks as $nd_experiment_stock){
    
    // get the nd_experiment_id
    $nd_experiment_id = $nd_experiment_stock->nd_experiment_id->nd_experiment_id;
    
    // get the nd_experiment_phenotype records for this nd_experiment_id
    $values = array('nd_experiment_id' => $nd_experiment_id);
    $nd_experiment_phenotypes = tripal_core_generate_chado_var('nd_experiment_phenotype', $values, $options);
    
    // iterate through any nd_experiment_phenotype records and add them to our array
    if ($nd_experiment_phenotypes) {
      foreach ($nd_experiment_phenotypes as $nd_experiment_phenotype){
        $phenotype = $nd_experiment_phenotype->phenotype_id;
        $phenotypes[$nd_experiment_id]['phenotype'] = $phenotype;
      }
    }
  }
}

if (count($phenotypes) > 0) {?>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc">This following phenotypes have been recorded for this stock.</div><?php 

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
  foreach ($phenotypes as $nd_experiment_id => $phenotype){

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
}