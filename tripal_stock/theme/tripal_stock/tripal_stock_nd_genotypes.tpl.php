<?php
/*
 * There are two ways that stock genotypes can be housed in Chado.  The first, more simple
 * method, is via the stock_genotype table.  This is simply a linker table between the
 * stock and genotype tables of Chado.  A more complex method is via the Natural Diversity
 * tables.  In these tables, the genotypes are in the nd_experiment_genotype table
 * and there may be an associated project, contact info, etc.  This template handles
 * stocks that are stored via the ND method.
 */
$stock = $variables['node']->stock;

// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$pager_id = 500;

// expand the stock object to include the nd_experiment_stock table
$options = array(
  'return_array' => 1,
  'pager' => array (
    'limit' => $num_results_per_page,
    'element' => $pager_id,
  ),
);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'nd_experiment_stock', $options);
$nd_experiment_stocks = $stock->nd_experiment_stock;

// create the pager.  
global $pager_total_items;
$pager = theme('pager', array(), $num_results_per_page, $pager_id, array('block' => 'nd_genotypes'));
$total_features = $pager_total_items[$pager_id];


// now iterate through the experiments and create a paged table for each experiment
if (count($nd_experiment_stocks) > 0) {?>
  <div id="tripal_stock-nd_genotypes-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Genotypes</div>
    <div class="tripal_stock-info-box-desc tripal-info-box-desc">This <?php print $stock->type_id->name ?> has <?php print number_format($total_features) ?> associated genotypes: </div>

    <table id="tripal_stock-table-nd_stock_genotypes" class="tripal_stock-table tripal-table tripal-table-horz">
        <tr class="tripal_stock-table-odd-row tripal-table-even-row">
          <th>Project</th>
          <th>Marker (Type)</th>
          <th>Details</th>
        </tr>

    <?php
    foreach ($nd_experiment_stocks as $nd_experiment_stock) {

      // get the nd experiment
      $values = array('nd_experiment_id' => $nd_experiment_stock->nd_experiment_id->nd_experiment_id);
      $options = array('include_fk' => array('nd_geolocation_id' => 1)); 
      $nd_experiment = tripal_core_generate_chado_var('nd_experiment', $values, $options);
 
      // add in the project
      $options = array('include_fk' => array('project_id' => 1)); 
      $nd_experiment = tripal_core_expand_chado_vars($nd_experiment, 'table', 'nd_experiment_project', $options);
      $project = $nd_experiment->nd_experiment_project->project_id;

      // add in the nd_experiment_genotype table
      $options = array('include_fk' => array('genotype_id' => 1)); 
      $nd_experiment = tripal_core_expand_chado_vars($nd_experiment, 'table', 'nd_experiment_genotype', $options);

      // add in the feature_genotype table
      $genotype = $nd_experiment->nd_experiment_genotype->genotype_id;
      $options = array('include_fk' => array('feature_id' => array('type_id' => 1))); 
      $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'feature_genotype', $options);

      // add in the node for the feature
      $marker = $genotype->feature_genotype->feature_id;
      $marker = tripal_core_expand_chado_vars($marker, 'node', 'feature'); ?>
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <td><?php 
          if ($project->nid) {
            print l($project->name, 'node/' . $project->nid);
          }
          else {
            print $project->name; 
          } ?>
        </td>
        <td><?php
          if ($marker->name != $marker->uniquename) {
            $name = $marker->name . ', ' . $marker->uniquename . ' (' . $marker->type_id->name . ')';
          } 
          else {
            $name = $marker->name . ' (' . $marker->type_id->name . ')';
          }
          if ($marker->nid) {
            print l($name, 'node/' . $marker->nid);
          }
          else {         
            print $name;
          } ?>
        </td>
        <td><?php print $genotype->description; ?></td>
      </tr> <?php
    }?>
  </table> <?php
  print $pager; ?>
  </div><?php
}
