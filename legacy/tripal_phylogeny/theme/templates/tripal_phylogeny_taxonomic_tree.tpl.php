<?php
$phylotree = $variables['node']->phylotree;
$phylotree = chado_expand_var($phylotree, 'field', 'phylotree.comment');

if ($phylotree->type_id->name == "taxonomy" and $phylotree->has_nodes) {
  print $phylotree->comment ?>
    <div id="phylogram">
        <!-- d3js will add svg to this div, and remove the loader gif prefix with / for absolute url --><?php
      $ajax_loader = url(drupal_get_path('module', 'tripal_phylogeny') . '/theme/images/ajax-loader.gif'); ?>
        <img src="<?php print $ajax_loader ?>" class="phylogram-ajax-loader"/>
    </div> <?php
}