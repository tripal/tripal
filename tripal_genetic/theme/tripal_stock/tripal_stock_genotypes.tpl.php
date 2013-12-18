<?php
/*
 * There are two ways that stock genotypes can be housed in Chado.  The first, more simple
 * method, is via the stock_genotype table.  This is simply a linker table between the
 * stock and genotype tables of Chado.  A more complex method is via the Natural Diversity
 * tables.  In these tables, the genotypes are in the nd_experiment_genotype table 
 * and there may be an associated project, contact info, etc. This template is for the simple
 * stock_genotype linker table.
 */
$stock = $variables['node']->stock;

// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25; 
$stock_pager_id = 0;

// get the genotypes from the stock_genotype table
$options = array(
  'return_array' => 1, 
  'pager' => array('limit' => $num_results_per_page, 'element' => $stock_pager_id),
);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stock_genotype', $options); 
$stock_genotypes = $stock->stock_genotype;

// create the pager.  
$stock_pager = theme('pager', array(), $num_results_per_page, $stock_pager_id, array('block' => 'genotypes'));


// now iterate through the stock genotypes and print a paged table.
if (count($stock_genotypes) > 0) {?>
  <div id="tripal_stock-genotypes-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Genotypes</div>
    <div class="tripal_stock-info-box-desc tripal-info-box-desc">This following genotypes have been recorded for this stock.</div>
    <table id="tripal_stock-table-stock_genotypes_exp" class="tripal_stock-table tripal-table tripal-table-horz">     
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Name</th>
        <th>Type</th>
        <th>Genotype</th>
        <th>Details</th>
        <th>Markers</th>
      </tr> <?php       
      $i = 0;   
      foreach($stock_genotypes as $stock_genotype) {
        $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
          $class = 'tripal_stock-table-odd-row tripal-table-even-row';
        }         
        $genotype = $stock_genotype->genotype_id;
        
        // get the genotype properties
        $options = array('return_array' => 1);
        $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'genotypeprop', $options);
        $properties = $genotype->genotypeprop; 
        
        // add in markers associated with this genotype if any
        $options = array(
          'return_array' => 1,
          'inlude_fk' => array(
            'feature_id' => array(
              'type_id' => 1
            )
          ),
        );
        $genotype = tripal_core_expand_chado_vars($genotype, 'table', 'feature_genotype', $options);
        $feature_genotypes = $genotype->feature_genotype; ?>
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
            if(count($feature_genotypes) > 0) { ?>                      
              <table class="tripal-subtable"> <?php
                foreach ($feature_genotypes as $feature_genotype){ 
                  $feature = $feature_genotype->feature_id; ?>
                  <tr> 
                    <td><?php print ucwords(preg_replace('/_/', ' ', $feature->type_id->name)) ?></td>
                    <td>:</td>                  
                    <td><?php 
                      if($feature->nid) {
                        print l($feature->name, 'node/' . $feature->nid);
                      }
                      else {
                        print $feature->name;
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
    print $stock_pager ?>
  </div><?php
} 