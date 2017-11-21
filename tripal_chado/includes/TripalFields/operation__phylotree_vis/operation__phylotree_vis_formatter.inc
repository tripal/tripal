<?php

class operation__phylotree_vis_formatter extends ChadoFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Phylogenetic tree visualisation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('operation__phylotree_vis');

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) > 0) {
      $phylotree = $entity->chado_record;

      $node = new stdClass();
      $node->phylotree = $phylotree;

      $phylotree = chado_expand_var($phylotree,'field','phylotree.comment');
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.phylotree');
      tripal_phylogeny_prepare_tree_viewer($phylotree);

      if ($phylotree->has_nodes) {
        $element[0]['comment'] = array(
          '#type' => 'markup',
          '#markup' => 'Click a species to view its species page.',
        );
        $ajax_loader = url(drupal_get_path('module', 'tripal') . '/theme/images/ajax-loader.gif');
        $element[0]['phylogram'] = array(
          '#type' => 'markup',
          '#markup' => '
            <div id="phylogram">
              <img src="' . $ajax_loader . '" class="phylogram-ajax-loader"/>
            </div>
          '
        );
      }
    }
  }
}
