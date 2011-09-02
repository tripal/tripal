
<?php
  $num_results_per_page = 25;
  
  // SELECT all nd_experiments where type=genotype and experiment is connected to the current stock
  $query = "SELECT nd_experiment_id FROM nd_experiment "
    ."WHERE nd_experiment_id IN (SELECT nd_experiment_id FROM nd_experiment_stock WHERE stock_id=%d) "
      ."AND type_id IN (SELECT cvterm_id FROM cvterm WHERE name='genotype') "
    ."ORDER BY nd_experiment_id";
  $resource = pager_query($query, $num_results_per_page, 0, NULL, $node->stock->stock_id);
  $results = array();
  while ($r = db_fetch_object($resource)) {

    // Get the genotype & feature associated with each experiment
    $query2 = "SELECT g.*, f.uniquename as feature_uniquename, f.name as feature_name, f.feature_id FROM genotype g "
      ."LEFT JOIN feature_genotype fg ON fg.genotype_id=g.genotype_id "
      ."LEFT JOIN feature f ON fg.feature_id=f.feature_id "
      ."WHERE g.genotype_id IN (SELECT genotype_id FROM nd_experiment_genotype WHERE nd_experiment_id=%d)";
    $genotype_feature = db_fetch_object(db_query($query2, $r->nd_experiment_id));

    $item = array(
      'nd_experiment' => array(
        'nd_experiment_id' => $r->nd_experiment_id
      ),
      'genotype' => array(
        'genotype_id' => $genotype_feature->genotype_id,
        'uniquename' => $genotype_feature->uniquename,
        'description' => $genotype_feature->description,
      ),
      'feature' => array(
        'feature_id' => $genotype_feature->feature_id,
        'uniquename' => $genotype_feature->feature_uniquename,
        'name' => $genotype_feature->feature_name,
      ),
    );
    
    // Get the nid associated with the feature (used for linking)
    $query3 = "SELECT nid FROM chado_feature WHERE feature_id=%d";
    $nid = db_fetch_object(db_query($query3,$genotype_feature->feature_id));
    $item['feature']['nid'] = $nid->nid;
    
    $results[$r->nd_experiment_id] = $item;
  }
?>

<?php if (!empty($results)) { ?>
  <div id="tripal_stock-genotype_experiments-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Genotype Experiments</div>
    <div class="tripal_stock-info-box-desc tripal-info-box-desc"></div>
    <table>
      <tr><th>Marker Assayed</th><th>Genotype Observed</th></tr>
      <?php
        foreach ($results as $r) {
          // genotype
          $genotype = $r['genotype']['description'];
          if (preg_match('/insufficient/',$genotype)) { 
            $genotype = "<font color='grey'>".$genotype.'</font>'; 
          }
          
          // feature name
          if ($r['feature']['name']) {
            $marker_name = $r['feature']['name'];
          } else {
            $marker_name = $r['feature']['uniquename'];
          }
          
          // add link if feature sync'd
          if ($r['feature']['nid']) {
            $marker_link = 'node/'.$r['feature']['nid'];
            $marker = l($marker_name, $marker_link);
          } else {
            $marker = $marker_name;
          }
      ?>
      <tr><td><?php print $marker; ?></td><td><?php print $genotype; ?></td></tr>
      <?php } ?>
    </table>
    <?php print theme('pager', array(), $num_results_per_page, 0, array('block'=>'genotype_experiments'), 5); ?>
  </div>
<?php } ?>