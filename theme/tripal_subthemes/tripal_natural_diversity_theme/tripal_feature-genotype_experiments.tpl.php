
<?php
  $num_results_per_page = 25;
  $feature = $variables['node']->feature;
  
  // get all genotypes associatated with the current feature
  $query = "SELECT * FROM genotype WHERE genotype_id IN (SELECT genotype_id FROM feature_genotype WHERE feature_id=%d)";
  $resource = db_query($query, $feature->feature_id);
  $genotypes = array();
  while( $r = db_fetch_array($resource)) {
    $genotypes[$r['genotype_id']] = $r;
  }
  
  if (!empty($genotypes)) {
    // SELECT all nd_experiments where type=genotype and experiment is connected to the current feature
    $query = "SELECT nd_experiment_id, genotype_id FROM nd_experiment_genotype "
      ."WHERE genotype_id IN (%s) "
      ."ORDER BY nd_experiment_id";
    $resource = pager_query($query, $num_results_per_page, 0, NULL, implode(',',array_keys($genotypes)));
    $results = array();
    while ($r = db_fetch_object($resource)) {
    
      // Get the stock associated with each experiment
      $query2 = "SELECT s.* FROM stock s "
        ."WHERE s.stock_id IN (SELECT stock_id FROM nd_experiment_stock WHERE nd_experiment_id=%d)";
      $stock = db_fetch_array(db_query($query2, $r->nd_experiment_id));
      
      $item = array(
        'nd_experiment' => array(
          'nd_experiment_id' => $r->nd_experiment_id
        ),
        'genotype' =>  $genotypes[$r->genotype_id],
        'stock' => $stock,
      );
      
      // Get the nid associated with the feature (used for linking)
      $query3 = "SELECT nid FROM chado_stock WHERE stock_id=%d";
      $nid = db_fetch_object(db_query($query3,$stock['stock_id']));
      $item['stock']['nid'] = $nid->nid;
      
      $results[$r->nd_experiment_id] = $item;
    }
  }
?>

<?php if(count($results) > 0){ ?>
<div id="tripal_feature-genotype_experiments-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Genotype Experiments</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">
    Genotypes of this <?php print $feature->type_id->name; ?> in various germplasm
  </div>
  <table>
    <tr><th>Germplasm Assayed</th><th>Genotype Observed</th></tr>
    <?php foreach ($results as $r) { 
        $genotype = $r['genotype']['description'];
        if (preg_match('/insufficient/',$genotype)) { $genotype = "<font color='grey'>".$genotype.'</font>'; }
        $stock_name = $r['stock']['name'];
        if ($r['stock']['nid']) {
          $stock_link = 'node/'.$r['stock']['nid'];
          $stock = l($stock_name, $stock_link);
        } else {
          $stock = $stock_name;
        }
    ?>
      <tr><td><?php print $stock; ?></td><td><?php print $genotype; ?></td></tr>
    <?php } ?>
  </table>
  <?php 
   print theme('pager', array(), $num_results_per_page, 0, array('block'=>'genotype_experiments'), 5); 
  ?>
</div>
<?php } ?>

