
<?php
$feature = $variables['node']->feature;
$genotypes = $variables['tripal_feature']['genotype_experiments'];
?>

<?php if(count($genotypes) > 0){ ?>
<div id="tripal_feature-genotypes-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Genotypes</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">
    Genotypes of this <?php print $feature->type_id->name; ?> in various germplasm
  </div>
  <table id="tripal_feature-genotypes-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr><th>Germplasm</th><th>Genotype</th></tr>
    <?php foreach ($genotypes as $g) { 
        $genotype = $g->nd_experiment_genotype->genotype_id->description;
        if (preg_match('/no sequence data/',$genotype)) { $genotype = "<font colour='grey'>".$genotype.'</font>'; }
        $stock_name = $g->nd_experiment_stock->stock_id->name;
        if ($g->nd_experiment_stock->stock_id->nid) {
          $stock_link = 'node/'.$g->nd_experiment_stock->stock_id->nid;
          $stock = l($stock_name, $stock_link);
        } else {
          $stock = $stock_name;
        }
    ?>
      <tr><td><?php print $stock; ?></td><td><?php print $genotype; ?></td></tr>
    <?php } ?>
  </table>
</div>
<?php } ?>