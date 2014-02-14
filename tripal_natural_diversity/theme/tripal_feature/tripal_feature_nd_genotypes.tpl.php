<?php
/*
 * Deatils about genotypes associated features can be found by traversing the foreign key (FK)
 * relationships in this way
 * 
 * feature => feature_genotype => genotype
 *
 * There are two ways that features with genotypes can be associated with stocks.  The first, 
 * more simple method, is by traversion the FK relationships in this manner:
 * 
 *   Simple Method: feature => feature_genotype => genotype => stock_genotype => stock
 *   
 * The second method involves use of the natural diversity (ND) tables which allows for association
 * or more ancilliary information. Within the ND tables, If a feature has genotypes then 
 * you can find the corresponding stock by traversing the FK relationships 
 * in this manner: 
 * 
 *   ND MEthod: feature => feature_genotype => nd_experiment_genotype => nd_experiment => nd_experiment_stock => stock
 * 
 * You can find ancilliary information about data associated with a genotype in the ND tables such as
 * a contact, pub, protocol, project, genotype, dbxref by using the 
 * nd_experiment.nd_experiment_id value and traversing the other FK relationships
 * 
 * feature => feature_genotype => nd_experiment_genotype => nd_experiment => nd_experiment_stock => stock
 *                                                                        => nd_experiment_project => project
 *                                                                        => nd_experiment_pub => pub
 *                                                                        => nd_experiment_contact => contact
 *                                                                        => nd_experiment_dbxref => dbxref
 *                                                                        => nd_experiment_protocol => protocol
 *                                                                        => nd_experiment_stockprop
 *                                                                        => nd_experiment_stock_dbxref
 * 
 * In the FK relationships shown above, the nd_experiment_id value represents a single 
 * experimental value that may have all of the ancilliary data associated with it.  
 * If the genotype record shares an nd_experiment_id with a genotype, pub, contact,
 * protocol, etc then all of that data is associated with the genotype and vice-versa.
 * 
 * Techincally, we can skip including the 'nd_experiment' table when traversing the FK's 
 * because we have the nd_experiment_id value when we get the nd_experiment_genotype record. 
 * 
 * NOTE: if the tripal_natural_diversity module is enabled this template will supercede 
 * the tripal_feature_genotypes.tpl.php template (provided by the tripal_genetic module).
 * Therefore, this template must handle both cases for linking to stocks as described above
 */

// get the current feature
$feature = $variables['node']->feature;

// we can have any number of genotypes for a given feature, so we want a pager.
// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$feature_pager_id = 15;

// get the genotypes from the feature_genotype table
$options = array(
  'return_array' => 1,
  'pager' => array(
    'limit' => $num_results_per_page,
    'element' => $feature_pager_id
  ),
);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_genotype', $options);
$feature_genotypes = $feature->feature_genotype->feature_id;

// get the total number of records
$total_records = chado_pager_get_count($feature_pager_id);

// now iterate through the feature genotypes and print a paged table.
if (count($feature_genotypes) > 0) { ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">This following <?php print number_format($total_records) ?> genotype(s) have been recorded for this feature.</div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Name', 'Type', 'Genotype', 'Details', 'Germplasm', 'Project');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($feature_genotypes as $feature_genotype) {
    $project_names = 'N/A';
    $stock_names   = 'N/A';
    
    // get the genotype from the feature_genotype record
    $genotype = $feature_genotype->genotype_id;

    // build the name for displaying the genotype. Use the uniquename by default
    // unless a name exists
    $name = $genotype->uniquename;
    if ($genotype->name){
      $name = $genotype->name;
    }
    
    // build the genotype type for display
    $type = 'N/A';
    if ($genotype->type_id) {
      $type = ucwords(preg_replace('/_/', ' ', $genotype->type_id->name));
    }

    // build the genotype properties
    $options = array('return_array' => 1);
    $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'genotypeprop', $options);
    $properties = $genotype->genotypeprop;
    $details = '';
    if(count($properties) > 0) {
      foreach ($properties as $property){
        $details .=  ucwords(preg_replace('/_/', ' ', $property->type_id->name)) . ': ' . $property->value . '<br>';
      }
      $details = substr($details, 0, -4); // remove trailing <br>
    }

    // get the nd_experiment_genotype records and if any
    $values = array('genotype_id' => $genotype->genotype_id);
    $nd_experiment_genotype = tripal_core_generate_chado_var('nd_experiment_genotype', $values);
    if ($nd_experiment_genotype) {
      $nd_experiment    = $nd_experiment_genotype->nd_experiment_id;
      $nd_experiment_id = $nd_experiment_genotype->nd_experiment_id->nd_experiment_id;

      // expand the nd_experiment object to incldue the nd_experiment_stock table
      $values = array('nd_experiment_id' => $nd_experiment_id);
      $options = array(
        'return_array' => 1,
        'include_fk' => array(
          'stock_id' => array(
            'type_id' => 1
          )
        ),
      );
      $nd_experiment = tripal_core_expand_chado_vars($nd_experiment, 'table', 'nd_experiment_stock', $options);
      $nd_experiment_stocks = $nd_experiment->nd_experiment_stock;
      if (count($nd_experiment_stocks) > 0) {
        $stock_names = '';
        foreach ($nd_experiment_stocks as $nd_experiment_stock) {
          $stock = $nd_experiment_stock->stock_id;
          $stock_name = $stock->name . ' (' . $stock->uniquename . ')';
          if (property_exists($stock, 'nid')) {
            $stock_name = l($stock_name, 'node/' . $stock->nid);
          }
          $stock_names .= $stock_name . '<br>';
        }
        $stock_names = substr($stock_names, 0, -4); // remove trailing <br>
      }
      
      // expand the nd_experiment object to incldue the nd_experiment_project table
      $values = array('nd_experiment_id' => $nd_experiment_id);
      $options = array('return_array' => 1);
      $nd_experiment = tripal_core_expand_chado_vars($nd_experiment, 'table', 'nd_experiment_project', $options);
      $nd_experiment_projects = $nd_experiment->nd_experiment_project;
      if (count($nd_experiment_projects) > 0) {
        $project_names = '';
        foreach ($nd_experiment_projects as $nd_experiment_project) {
          $project = $nd_experiment_project->project_id;
          $project_name = $project->name;
          if (property_exists($project, 'nid')) {
            $project_name = l($project_name, "node/" . $project->nid, array('attributes' => array('target' => '_blank')));
          }
          $project_names .= $project_name . '<br>';
        }
        $project_names = substr($project_names, 0, -4); // remove trailing <br>
      }
    }

    $rows[] = array(
      $name,
      $type,
      $genotype->description,
      $details,
      $stock_names,
      $project_names,
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
      'id' => 'tripal_natural_diversity-table-genotypes',
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
    'element' => $feature_pager_id,
    'parameters' => array(
      'block' => 'genotypes'
    ),
    'quantity' => $num_results_per_page,
  );
  print theme_pager($pager); 
}