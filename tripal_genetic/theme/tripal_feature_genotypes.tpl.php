
<?php
$feature = $variables['node']->feature;
$genotypes = $variables['tripal_feature']['genotypes'];
?>

<?php if(count($genotypes) > 0){ ?>
<div id="tripal_feature-genotypes-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Genotypes</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">
    Genotypes of this <?php print $feature->type_id->name; ?> in various germplasm
  </div>
  <ul>
    <?php foreach ($genotypes as $g) { 
        $genotype = $g->description;
        if (preg_match('/insufficient sequence/',$genotype)) { $genotype = "<font colour='grey'>".$genotype.'</font>'; }
    ?>
      <li><?php print $genotype; ?></li>
    <?php } ?>
  </ul>
</div>
<?php } ?>