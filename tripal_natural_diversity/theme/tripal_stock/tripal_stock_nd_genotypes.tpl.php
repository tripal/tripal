<?php
/*
 * Details about genotypes associated with stocks can be found in two ways by 
 * traversing the the foreign key (FK) relationships in these ways:
 * 
 *   Simple Method: stock => stock_genotype => genotype
 *   ND Method:     stock => nd_experiment_stock => nd_experiment => nd_experiment_genotype => genotype
 *
 * The tripal_genetic module handles display of genotypes when stored using the 
 * simple method.  This template handles display of genotype when stored using
 * the ND Method.  The ND method uses the natural diversity tables and allows for 
 * association or more ancilliary information. 
 * 
 * 
 * Within the ND tables, If a stock has genotypes then you can find the corresponding 
 * features by traversing the FK relationships in this manner: 
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_genotype => genotype => feature_genotype => feature
 * 
 * You can find ancilliary information about data associated with a genotype in the ND tables such as
 * a contact, pub, protocol, project, genotype, dbxref by using the 
 * nd_experiment.nd_experiment_id value and traversing the other FK relationships
 * 
 * stock => nd_experiment_stock => nd_experiment => nd_experiment_project => project
 *                                               => nd_experiment_pub => pub
 *                                               => nd_experiment_contact => contact
 *                                               => nd_experiment_dbxref => dbxref
 *                                               => nd_experiment_protocol => protocol
 *                                               => nd_experiment_stockprop
 *                                               => nd_experiment_stock_dbxref
 * 
 * In the FK relationships shown above, the nd_experiment_id value represents a single 
 * experimental unit that may have all of the ancilliary data associated with it or none.  
 * If the genotype record shares an nd_experiment_id with a genotype, pub, contact,
 * protocol, etc then all of that data is associated with the genotype and vice-versa.
 * 
 * Techincally, we can skip including the 'nd_experiment' table when traversing the FK's 
 * because we have the nd_experiment_id value when we get the nd_experiment_stock record.
 * 
 * When lots of genotypes are associated with a stock (e.g. thousands) then traversing
 * the FK relationships as described above can be very slow. Ideally, we only need to
 * show a small subset with a pager. Therefore, a list of nd_experiment_genotype_id's 
 * are provided to this template automatically within the stock object.
 * 
 * NOTE: if the tripal_natural_diversity module is enabled this template will supercede 
 * the tripal_stock_genotypes.tpl.php template (provided by the tripal_genetic module).
 * Therefore, this template must handle both cases for linking to features as described above
 */

// get the current stock
$stock = $variables['node']->stock;


// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$stock_pager_id = 15;

// the nd_experiment_genotype IDs get passed into this template, so we use
// those to iterate and show a subset via a pager.  This is faster than trying
// to traverse all of the FK relationship, especially when thousands of 
// associations may be present.  Because the nd_experiment_id in Chado
// can be associated with other data types it becomes slow to use the
// chado_expand_var functions that we would normal use.
$nd_experiment_genotype_ids = $stock->nd_experiment_genotype_ids;
$total_records = count($nd_experiment_genotype_ids);

// initialize the Drupal pager
$current_page_num = pager_default_initialize($total_records, $num_results_per_page, $stock_pager_id);
$offset = $num_results_per_page * $current_page_num;

$genotypes = array();
if ($total_records > 0) {
  
  // iterate through the nd_experiment_genotype_ids and get the genotype record
  for ($i = $offset ; $i < $offset + $num_results_per_page; $i++) {
    $nd_experiment_genotype_id = $nd_experiment_genotype_ids[$i];
      
    // expand the nd_experiment record to include the nd_experiment_genotype table
    // there many be many genotypes for a stock so we want to use a pager to limit 
    // the results returned
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'genotype_id' => array(
          'type_id' => 1,
        )
      ),
    );
    $values = array('nd_experiment_genotype_id' => $nd_experiment_genotype_id);
    $nd_experiment_genotype = chado_generate_var('nd_experiment_genotype', $values);
    $genotype = $nd_experiment_genotype->genotype_id;
    $genotypes[$genotype->genotype_id]['genotype'] = $genotype;
    $genotypes[$genotype->genotype_id]['nd_experiment_id'] = $nd_experiment_genotype->nd_experiment_id->nd_experiment_id;
  }
}

// now iterate through the feature genotypes and print a paged table.
if (count($genotypes) > 0) { ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">
    The following <?php print number_format($total_records) ?> genotype(s) have been recorded.
  </div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Name', 'Type', 'Genotype', 'Details', 'Markers', 'Project');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  // iterate through the genotypes and build each row of the resulting table
  foreach ($genotypes as $info) {
    $genotype         = $info['genotype'];
    $nd_experiment_id = $info['nd_experiment_id'];
    
    // get the nd_experiment record
    $nd_experiment = chado_generate_var('nd_experiment', array('nd_experiment_id' => $nd_experiment_id));
    
    // set some defaults for project and feature names
    $project_names = 'N/A';
    $feature_names = 'N/A';
    
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
    
    // get the features as found in the feature_genotype table and if any, add them to the $features array
    // we will later add in the features list for display
    $features = array();
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'feature_id' => array(
          'type_id' => 1
        )
      ),
    );
    $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'feature_genotype', $options);
    $feature_genotypes = $genotype->feature_genotype;
    if (count($feature_genotypes) > 0) {
      $feature_names = '';
      foreach ($feature_genotypes as $feature_genotype) {
        $feature = $feature_genotype->feature_id;
        $feature_name = $feature->name . ' (' . $feature->uniquename . ')';
        if (property_exists($feature, 'nid')) {
          $feature_name = l($feature_name, 'node/' . $feature->nid);
        }
        $feature_names .= $feature_name . '<br>';
      }
      $feature_names = substr($feature_names, 0, -4); // remove trailing <br>
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

    $rows[] = array(
      $name,
      $type,
      $genotype->description,
      $details,
      $feature_names,
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
    'element' => $stock_pager_id,
    'parameters' => array(
      'block' => 'genotypes'
    ),
    'quantity' => $num_results_per_page,
  );
  print theme_pager($pager);
}