<?php
$phylotree = $variables['node']->phylotree;

if ($phylotree->type_id->name != "taxonomy" and $phylotree->has_nodes) {

  if ($phylotree->type_id and $phylotree->type_id->name == 'polypeptide') { ?>
      <p>Phylogenies are essential to any analysis of evolutionary gene
          sequences collected across a group of organisms. A <b>phylogram</b>
          is a phylogenetic tree that has branch spans proportional to the
          amount of character change.
      </p> <?php
  } ?>

    <div id="phylogram">
        <!-- d3js will add svg to this div, and remove the loader gif prefix with / for absolute url --><?php
      $ajax_loader = url(drupal_get_path('module', 'tripal_phylogeny') . '/theme/images/ajax-loader.gif'); ?>
        <img src="<?php print $ajax_loader ?>" class="phylogram-ajax-loader"/>
    </div> <?php
}