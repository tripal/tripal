<?php
/*
 * NOTE: if the tripal_natural_diversity module is enabled this template will be
 * used and the tripal_feature_genotypes.tpl.php template will be ignored
 *
 * There are two ways that feature genotypes can be housed in Chado.  The first, more simple
 * method, is via the feature_genotype table.  This is simply a linker table between the
 * feature and genotype tables of Chado.  A more complex method is via the Natural Diversity
 * tables.  In these tables, the genotypes are in the nd_experiment_genotype table
 * and there may be an associated project, contact info, etc. This template is for Natural 
 * Diversity tables.
 */
$feature = $variables['node']->feature;

// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$feature_pager_id = 6;

// get the genotypes from the feature_genotype table
$options = array(
  'return_array' => 1,
  'pager' => array('limit' => $num_results_per_page, 'element' => $feature_pager_id),
);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_genotype', $options);

// because this table has two FK constraints for the feature table, the expand function call
// above doesn't know which one we're interested in, so it expands both the feature_id and the
// chromosome_id and makes both available to us.  This feature can only have a genotype if
// it matches the feature_genotype through the 'feature_id' FK (not the chromosome_id) so, we
// retrieve our results from the 'feature_id' key.
$feature_genotypes = $feature->feature_genotype->feature_id;

// create the pager.
$feature_pager = theme('pager', array(), $num_results_per_page, $feature_pager_id, array('block' => 'nd_genotypes'));


// now iterate through the feature genotypes and print a paged table.
if (count($feature_genotypes) > 0) {?>
  <div id="tripal_feature-nd_genotypes-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Genotypes</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This following genotypes have been recorded for this feature.</div>
    <table id="tripal_feature-table-feature_genotypes_exp" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr class="tripal_feature-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <th>Type</th>
        <th>Genotype</th>
        <th>Details</th>
        <th>Germplasm</th>
        <th>Project</th>
      </tr> <?php
      $i = 0;
      foreach($feature_genotypes as $feature_genotype) {
        $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
          $class = 'tripal_feature-table-odd-row tripal-table-even-row';
        }
        $genotype = $feature_genotype->genotype_id;

        // get the genotype properties
        $options = array('return_array' => 1);
        $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'genotypeprop', $options);
        $properties = $genotype->genotypeprop;

        // get the experiment via the genotype:
        $options = array(
          'return_array' => 1,
          'inlude_fk' => array(
            'nd_experiment_id' => 1
          ),
        );
        $values = array('genotype_id' => $genotype->genotype_id);
        $nd_experiment_genotype = tripal_core_generate_chado_var('nd_experiment_genotype', $values);
        $nd_experiment_genotype = tripal_core_expand_chado_vars($nd_experiment_genotype, 'table', 'nd_experiment_stock', $options);
        $nd_experiment = $nd_experiment_genotype->nd_experiment_id;

        // get the project via the experiment
        $options = array(
          'return_array' => 1,
          'inlude_fk' => array(
            'project_id' => 1
          ),
        );
        $values = array('nd_experiment_id' => $nd_experiment->nd_experiment_id);
        $nd_experiment_project = tripal_core_generate_chado_var('nd_experiment_project', $values, $options);

        $nd_experiment_stock = $nd_experiment->nd_experiment_stock;
        $nd_experiment_stock = tripal_core_expand_chado_vars($nd_experiment_stock, 'node', 'stock', $options);
        ?>
        <tr class="<?php print $class ?>">
          <td><?php
          if($genotype->name){
             print $genotype->name;
          }
          else {
             print $genotype->uniquename;
          } ?>
          </td>
          <td><?php
            // Chado versions < v1.2 did not have a type_id field
            if ($genotype->type_id) {
              print ucwords(preg_replace('/_/', ' ', $genotype->type_id->name));
            }
            else {
              print 'N/A';
            } ?>
          </td>
          <td><?php print $genotype->description ?></td>
          <td><?php
            if(count($properties) > 0) { ?>
              <table class="tripal-subtable"> <?php
                foreach ($properties as $property){ ?>
                  <tr>
                    <td><?php print ucwords(preg_replace('/_/', ' ', $property->type_id->name)) ?></td>
                    <td>:</td>
                    <td><?php print $property->value ?>
                    </td>
                  </tr> <?php
                } ?>
              </table><?php
            } ?>
          </td>
          <td><?php
            if(count($nd_experiment_stock) > 0) {
              $name = $nd_experiment_stock->stock_id->stock->name . ' (' . $nd_experiment_stock->stock_id->stock->uniquename . ')';
              print l($name, 'node/' . $nd_experiment_stock->stock_id->nid);
            } ?>
          </td>
          <td><?php
            if(count($nd_experiment_project) == 0) {
              print $project->name;
            }
            elseif(count($nd_experiment_project) == 1) {
              $project = $nd_experiment_project[0]->project_id;
              if($project->nid){
                print  l($project->name, "node/$project->nid");
              }
              else {
                print $project->name;
              }
            }
            elseif(count($nd_experiment_project) > 1) {
              drupal_set_message("A feature genotype has multiple projects for the same experiment. Data warning.", 'warning');
            } ?>
          </td>
        </tr> <?php
        $i++;
      } ?>
    </table> <?php
    print $feature_pager ?>
  </div><?php
}