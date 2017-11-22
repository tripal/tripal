<?php

/**
 * Implements hook_preprocess_hook()
 *
 * @param $variables
 */
function tripal_phylogeny_preprocess_tripal_phylogeny_phylogram(&$variables) {
  $phylotree = $variables['node']->phylotree;
  module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.phylotree');
  tripal_phylogeny_prepare_tree_viewer($phylotree);
}

/**
 * Implements hook_preprocess_hook()
 *
 * @param $variables
 */
function tripal_phylogeny_preprocess_tripal_phylogeny_taxonomic_tree(&$variables) {
  $phylotree = $variables['node']->phylotree;
  module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.phylotree');
  tripal_phylogeny_prepare_tree_viewer($phylotree);
}
