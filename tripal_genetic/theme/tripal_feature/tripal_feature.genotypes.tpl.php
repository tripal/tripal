<?php
/*
 * NOTE: if the tripal_natural_diversity module is enabled this template will be
 * ignored and the tripal_feature_nd_genotypes.tpl.php template will be used instead
 *
 * There are two ways that feature genotypes can be housed in Chado.  The first, more simple
 * method, is via the feature_genotype table.  This is simply a linker table between the
 * feature and genotype tables of Chado.  A more complex method is via the Natural Diversity
 * tables.  In these tables, the genotypes are in the nd_experiment_genotype table 
 * and there may be an associated project, contact info, etc. This template is for the simple
 * feature_genotype linker table.
 */
$feature = $variables['node']->feature;

// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$feature_pager_id = 7;

// get the genotypes from the feature_genotype table
$options = array(  
  'return_array' => 1,
  'pager' => array(
    'limit' => $num_results_per_page, 
    'element' => $feature_pager_id
  ),
);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_genotype', $options); 

// because this table has two FK constraints for the feature table, the expand function call
// above doesn't know which one we're interested in, so it expands both the feature_id and the
// chromosome_id and makes both available to us.  This feature can only have a genotype if
// it matches the feature_genotype through the 'feature_id' FK (not the chromosome_id) so, we
// retrieve our results from the 'feature_id' key.
$feature_genotypes = $feature->feature_genotype->feature_id;

// the total number of records for the paged query is stored in a session variable
$total_records = $_SESSION['chado_pager'][$feature_pager_id]['total_records'];

// now iterate through the feature genotypes and print a paged table.
if (count($feature_genotypes) > 0) {?>
  <div id="tripal_feature-genotypes-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Genotypes</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This following <?php print number_format($total_records) ?> genotype(s) have been recorded for this feature.</div><?php 
    
    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = array('Name', 'Type', 'Genotype', 'Details', 'Germplasm');

    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $rows = array();
    
    foreach($feature_genotypes as $feature_genotype) {
      $genotype = $feature_genotype->genotype_id;
      
      // get the genotype properties
      $options = array('return_array' => 1);
      $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'genotypeprop', $options);
      $properties = $genotype->genotypeprop; 
      
      // add in stocks associated with this genotype if any
      $options = array(
        'return_array' => 1,
        'inlude_fk' => array(
          'stock_id' => array(
            'type_id' => 1
          )
        ),
      );
      $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'stock_genotype', $options);
      $stock_genotypes = $genotype->stock_genotype; 
      
      // show the uniquename for the genotype unless a name exists
      $name = $genotype->uniquename;
      if ($genotype->name){
        $name = $genotype->name;
      }
      
      // get the genotype type 
      $type = 'N/A';
      if ($genotype->type_id) {
        $type = ucwords(preg_replace('/_/', ' ', $genotype->type_id->name));
      }
      
      // get the genotype properties
      $details = '';
      if(count($properties) > 0) { 
        foreach ($properties as $property){ 
          $details .=  ucwords(preg_replace('/_/', ' ', $property->type_id->name)) . ': ' . $property->value . '<br>';
        } 
        $details = substr($details, 0, -4); // remove trailing <br>
      }
      // build the list of germplasm.
      $germplasm = '';
      if(count($stock_genotypes) > 0) { 
        foreach ($stock_genotypes as $stock_genotype){ 
          $stock = $stock_genotype->stock_id; 
          $gname = $stock->name; 
          if(property_exists($stock, 'nid')) {
            $gname = l($gname, 'node/' . $stock->nid, array('attributes' => array('target' => '_blank')));
            dpm($gname);
          }
          $germplasm .= ucwords(preg_replace('/_/', ' ', $stock->type_id->name)) . ': ' . $gname . '<br>';
        }
        $germplasm = substr($germplasm, 0, -4); // remove trailing <br>
      } 
      // add the fields to the table row
      $rows[] = array(
        $name,
        $type,
        $genotype->description,
        $details,
        $germplasm
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
        'id' => 'tripal_genetic-table-genotypes',
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
        'block' => 'features'
      ),
      'quantity' => $num_results_per_page,
    );
    print theme_pager($pager); ?>
  </div><?php
} 