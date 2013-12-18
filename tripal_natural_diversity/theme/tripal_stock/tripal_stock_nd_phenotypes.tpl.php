<?php
$stock = $variables['node']->stock;

// expand the stock object to include the nd_experiment_stock table
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'nd_experiment_stock', $options);
$nd_experiment_stocks = $stock->nd_experiment_stock;

// Get the experiments to which this stock belongs that have a phenotype
// associated.  Store those in the phenotypes array indexed by the experiment_id
$all_phenotypes = array();
if (count($nd_experiment_stocks) > 0) {
  foreach ($nd_experiment_stocks as $nd_experiment_stock){      
    $values = array('nd_experiment_id' => $nd_experiment_stock->nd_experiment_id->nd_experiment_id);
    $nd_experiment_phenotypes = tripal_core_generate_chado_var('nd_experiment_phenotype', $values, $options);
    if ($nd_experiment_phenotypes) {
    	foreach ($nd_experiment_phenotypes as $nd_exp_phenotype){
        $phenotype = $nd_exp_phenotype->phenotype_id;
        $all_phenotypes[$nd_experiment_stock->nd_experiment_id->nd_experiment_id][] = $phenotype;
    	}
    }
  }
}
if (count($all_phenotypes) > 0) {?>
  <div id="tripal_stock-phenotypes-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Phenotypes</div>
    <div class="tripal_stock-info-box-desc tripal-info-box-desc">This following phenotypes have been recorded for this stock.</div>
 
    <table id="tripal_stock-table-phenotypes_exp" class="tripal_stock-table tripal-table tripal-table-horz">     
      <tr class="tripal_stock-table-odd-row tripal-table-even-row">
        <th>Phenotypes</th>
        <th>Project</th>
      </tr> <?php
      
      // iterate through the nd_experiment_stock records and get 
      // each experiment and the associated phenotypes
      foreach ($nd_experiment_stocks as $nd_experiment_stock){      
        // Get the project for this experiment. For each nd_experiment_id there can only be one project
        $values = array('nd_experiment_id' => $nd_experiment_stock->nd_experiment_id->nd_experiment_id);
        $nd_experiment_project = tripal_core_generate_chado_var('nd_experiment_project', $values, $options);
        $project = $nd_experiment_project[0]->project_id;

        // get the phenotypes
        $phenotypes = $all_phenotypes[$nd_experiment_stock->nd_experiment_id->nd_experiment_id];
        if(!$phenotypes){
          $phenotypes = array();
        }
                
        $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
          $class = 'tripal_stock-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td>           
            <table class="tripal-subtable"> <?php
              foreach ($phenotypes as $phenotype){
                if ($phenotype->name) { ?>
	                <tr> 
	                  <td>Name</td>
	                  <td>:</td>
	                  <td><?php print $phenotype->name ?></td>
	                </tr> <?php 
                }?>
                <tr> 
                  <td><?php print ucwords(preg_replace('/_/', ' ', $phenotype->attr_id->name)) ?></td>
                  <td>:</td>                  
                  <td><?php 
	                  // pheonotypes values are stored qualitatively or quantitatively.  If
	                  // qualitatively the cvalue_id will link to a type. If quantitative we
	                  // us ethe value column                  
	                  if ($phenotype->cvalue_id) { 
	                  	print ucwords(preg_replace('/_/', ' ', $phenotype->cvalue_id->name));
	                  }
	                  else { 
	                    print $phenotype->value;
	                  } ?>
                  </td>
                </tr> <?php 
                if ($phenotype->observable_id) { ?>
	                <tr>
	                  <td>Observable Unit</td>
	                  <td>:</td>
	                  <td><?php print ucwords(preg_replace('/_/', ' ', $phenotype->observable_id->name)) ?></td>
	                </tr> <?php 
                }
                if ($phenotype->assay_id) { ?>
	                <tr>
	                  <td>Evidence</td>
	                  <td>:</td>
	                  <td><?php print ucwords(preg_replace('/_/', ' ', $phenotype->assay_id->name)) ?></td>
	                </tr><?php
                }
              } ?>
            </table>
          </td>
          <td><?php 
            if($project->nid){    
              $link =  url("node/$project->name");        
              print "<a href=\"$link\">$project->name</a>";
            } 
            else {
              print $project->name;
            } ?>
          </td>
        </tr> <?php
        $i++; 
      }?>  
    </table> 
  </div><?php
}