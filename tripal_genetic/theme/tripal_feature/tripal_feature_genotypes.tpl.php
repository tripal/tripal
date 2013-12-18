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
$feature_pager_id = 5;

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
$feature_pager = theme('pager', array(), $num_results_per_page, $feature_pager_id, array('block' => 'genotypes'));


// now iterate through the feature genotypes and print a paged table.
if (count($feature_genotypes) > 0) {?>
  <div id="tripal_feature-genotypes-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Genotypes</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">This following genotypes have been recorded for this feature.</div>
    <table id="tripal_feature-table-feature_genotypes_exp" class="tripal_feature-table tripal-table tripal-table-horz">     
      <tr class="tripal_feature-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <th>Type</th>
        <th>Genotype</th>
        <th>Details</th>
        <th>Germplasm</th>
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
        $stock_genotypes = $genotype->stock_genotype; ?>
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
            if(count($stock_genotypes) > 0) { ?>                      
              <table class="tripal-subtable"> <?php
                foreach ($stock_genotypes as $stock_genotype){ 
                  $stock = $stock_genotype->stock_id; ?>
                  <tr> 
                    <td><?php print ucwords(preg_replace('/_/', ' ', $stock->type_id->name)) ?></td>
                    <td>:</td>                  
                    <td><?php 
                      if($stock->nid) {
                        print l($stock->name, 'node/' . $stock->nid);
                      }
                      else {
                        print $stock->name;
                      }?>
                    </td>
                  </tr> <?php                 
                } ?>
              </table><?php 
            } ?>
          </td>
        </tr> <?php
        $i++;
      } ?>  
    </table> <?php 
    print $feature_pager ?>
  </div><?php
} 