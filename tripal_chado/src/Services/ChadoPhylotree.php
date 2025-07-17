<?php

namespace Drupal\tripal_chado\Services;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal\Services\TripalEntityLookup;

/**
 * Handle visualization of a phylogenetic tree.
 */
class ChadoPhylotree {

  /**
   * A connection to a chado database.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * Tripal entity lookup service.
   *
   * @var Drupal\tripal\Services\TripalEntityLookup
   */
  protected TripalEntityLookup $entity_lookup_manager;

  /**
   * Stores the bundle terms for phylotree content types.
   *
   * @var array
   */
  protected array $cv_terms = [];

  /**
   * Instantiates a new ChadoPhylotree object.
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection $chado_connection
   *   The chado connection used to query chado.
   */
  public function __construct(ChadoConnection $chado_connection, TripalEntityLookup $entity_lookup_manager) {
    $this->chado_connection = $chado_connection;
    $this->entity_lookup_manager = $entity_lookup_manager;
    $this->initializeCvTerms();
  }

  /**
   * Creates json for ChadoPhylotree visualization.
   *
   * This will generate a phylotree in json format for tree visualization.
   *
   * @param int $phylotree_id
   *   The pkey value from the chado.phylotree table.
   * @param string|null $chado_schema
   *   Optional. The chado schema where the phylotree table is located.
   *   If no schema is specified then the default schema is used.
   *
   * @return void
   *   No return value.
   */
  public function create(int $phylotree_id, ?string $chado_schema = NULL): void {

  }

  /**
   * Loads CV terms used for phylotree bundles.
   *
   * @return void
   *   No return value.
   */
  protected function initializeCvTerms(): void {
    $query = $this->chado_connection->select('1:cvterm', 't');
    $query->join('1:cv', 'cv', 't.cv_id = cv.cv_id');
    $query->condition('cv.name', 'EDAM', '=');
    $query->condition('t.name', ['Phylogenetic tree', 'Species tree'], 'IN');
    $query->fields('t', ['name', 'cvterm_id']);
    $result = $query->execute();
    while ($r = $result->fetchAssoc()) {
      $this->cv_terms[$r['name']] = $r['cvterm_id'];
    }
  }

  /**
   * Loads a phylotree record from its primary key phylotree_id.
   *
   * @param int $phylotree_id
   *   The pkey value of the phylotree.
   *
   * @return array|bool
   *   An associative array of values from the phylotree table.
   *   Returns FALSE if the passed phylotree_id does not exist.
   */
  public function loadPhylotreeById(int $phylotree_id): array|bool {
    $query = $this->chado_connection->select('1:phylotree', 't');
    $query->condition('t.phylotree_id', $phylotree_id, '=');
    $query->fields('t');
    $phylotree = $query->execute();
    if ($phylotree) {
      return $phylotree->fetchAssoc();
    }
    else {
      return FALSE;
    }
  }

  /**
   * Loads all phylonode records for one phylotree.
   *
   * @param int $phylotree_id
   *   The pkey value of the phylotree.
   *
   * @return Drupal\Core\Database\StatementWrapperIterator|bool
   *   The results object from the database query.
   *   Returns FALSE if the passed phylotree_id does not
   *   exist or has no nodes.
   */
  public function loadPhylonodesById(int $phylotree_id) {
    $query = $this->chado_connection->select('1:phylonode', 'n');
    $query->condition('n.phylotree_id', $phylotree_id, '=');
    $query->leftJoin('1:cvterm', 'cvt', 'n.type_id = cvt.cvterm_id');
    $query->leftJoin('1:feature', 'f', 'n.feature_id = f.feature_id');
    $query->leftJoin('1:organism', 'fo', 'f.organism_id = fo.organism_id');
    $query->leftJoin('1:phylonode_organism', 'po', 'po.phylonode_id = n.phylonode_id');
    $query->leftJoin('1:organism', 'o', 'po.organism_id = o.organism_id');
    $query->leftJoin('1:stock_feature', 'sf', 'sf.feature_id = f.feature_id');
    $query->addField('n', 'phylonode_id', 'phylonode_id');
    $query->addField('n', 'parent_phylonode_id', 'parent_phylonode_id');
    $query->addField('n', 'label', 'name');
    $query->addField('n', 'distance', 'length');
    $query->addField('f', 'feature_id', 'feature_id');
    $query->addField('f', 'name', 'feature_name');
    $query->addField('cvt', 'name', 'cvterm_name');
    $query->addField('o', 'organism_id', 'organism_id');
    $query->addField('o', 'common_name', 'common_name');
    $query->addField('o', 'abbreviation', 'abbreviation');
    $query->addField('o', 'genus', 'genus');
    $query->addField('o', 'species', 'species');
    $query->addField('sf', 'stock_id', 'stock_id');
    $query->addField('fo', 'organism_id', 'fo_organism_id');
    $query->addField('fo', 'common_name', 'fo_common_name');
    $query->addField('fo', 'abbreviation', 'fo_abbreviation');
    $query->addField('fo', 'genus', 'fo_genus');
    $query->addField('fo', 'species', 'fo_species');
    $results = $query->execute();
    return $results;
  }

  /**
   * Get a json representation of all the nodes in a phylotree.
   *
   * @param int $phylotree_id
   *   The ID of the phylotree table record.
   *
   * @return string
   *   The phylotree in json format.
   */
  public function getTreeJson(int $phylotree_id): string {
    $json = '';

    $phylotree = $this->loadPhylotreeById($phylotree_id);
    if (!$phylotree) {
      return $json;
    }

    $nodes = $this->loadPhylonodesById($phylotree_id);
    if (!$nodes) {
      return $json;
    }

    // Fetch all the phylonodes into an associative array indexed by
    // phylonode_id. Convert from database query record to array,
    // casting to appropriate datatypes.
    $phylonodes = [];
    $root_phylonode_ref = NULL;

    while ($r = $nodes->fetchObject()) {
      $phylonode_id = (int) $r->phylonode_id;

      // Expect all nodes to have these properties.
      $node = [
        'phylonode_id' => $phylonode_id,
        'parent_phylonode_id' => (int) $r->parent_phylonode_id,
        'length' => (double) $r->length,
        'cvterm_name' => $r->cvterm_name,
      ];

      // If the nodes are taxonomic then set an equal distance.
      if (($phylotree['type_id'] ?? 0) == $this->cv_terms['Species tree']) {
        $node['length'] = 0.001;
      }

      // Other properties may exist only for leaf nodes.
      if ($r->name) {
        $node['name'] = $r->name;
      }

      // If this node is associated with a feature, then add in the details.
      if ($r->feature_id) {
        $node['feature_id'] = (int) $r->feature_id;
        $node['feature_name'] = $r->feature_name;
        // If not linked directly to a feature, leaf nodes can also be linked to
        // stock entities through an intermediate feature.
        if ($r->stock_id) {
          $entity_id = $this->entity_lookup_manager->getEntityId($r->stock_id, NULL, NULL, 'stock');
        }
        else {
          $entity_id = $this->entity_lookup_manager->getEntityId($r->feature_id, NULL, NULL, 'feature');
        }
        $node['feature_eid'] = (int) $entity_id;
      }

      // Add in the organism fields when they are available via the
      // phylonode_organism table.
      if ($r->organism_id) {
        $node['organism_id'] = (int) $r->organism_id;
        $node['common_name'] = $r->common_name;
        $node['abbreviation'] = $r->abbreviation;
        $node['organism_eid'] = $this->entity_lookup_manager->getEntityId($r->organism_id, 'OBI', '0100026', 'organism');

        // If the node does not have a name but is linked to an organism
        // then set the name to be that of the genus and species.
        if (!$r->name) {
          $node['name'] = chado_get_organism_scientific_name(chado_get_organism(['organism_id' => $r->organism_id], []));
        }
      }

      // Add in the organism fields when they are available via the
      // phylonode.feature_id FK relationship.
      if ($r->fo_organism_id) {
        $node['fo_organism_id'] = (int)$r->fo_organism_id;
        $node['fo_common_name'] = $r->fo_common_name;
        $node['fo_abbreviation'] = $r->fo_abbreviation;
        $node['fo_genus'] = $r->fo_genus;
        $node['fo_species'] = $r->fo_species;
        $node['fo_organism_eid'] = $this->entity_lookup_manager->getEntityId($r->fo_organism_id, 'OBI', '0100026', 'organism');
      }

      // Add this node to the list, organized by ID.
      $phylonodes[$phylonode_id] = $node;
    }

    // Populate the children[] arrays for each node.
    foreach ($phylonodes as $key => &$node) {
      if ($node['parent_phylonode_id'] !== 0) {
        $parent_ref = &$phylonodes[$node['parent_phylonode_id']];
        // Append node reference to children.
        $parent_ref['children'][] = &$node;
      }
      else {
        $root_phylonode_ref = &$node;
      }
    }

    // Convert datastructure to json.
    $json = json_encode($root_phylonode_ref);

    return $json;
  }


/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */

function tripal_phylogeny_admin_phylotrees_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Storage', 'admin/tripal/storage');
  $breadcrumb[] = l('Chado', 'admin/tripal/storage/chado');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_phylogeny_admin_phylotree', 'default');
  if (isset($view)) {
    $output .= $view;
  } else {
    $output .= '<p>The Phylotree module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Phylotree View', 'admin/tripal/extension/tripal_phylogeny/views/phylotree/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}

/**
 *
 * @param unknown $variables
 */
function theme_tripal_phylogeny_admin_org_color_tables($variables) {
  $fields = $variables['element'];
  $num_orgs = $fields['num_orgs']['#value'];
  $headers = ['Organism', 'Color', ''];
  $rows = [];
  for ($i = 0; $i < $num_orgs; $i++) {
    $add_button = ($i == $num_orgs - 1) ? drupal_render($fields['add']) : '';
    $del_button = ($i == $num_orgs - 1 and $i != 0) ? drupal_render($fields['remove']) : '';
    $rows[] = [
      drupal_render($fields['organism_' . $i]),
      drupal_render($fields['color_' . $i]),
      $add_button . $del_button,
    ];
  }
  $table_vars = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'colgroups' => [],
    'empty' => '',
  ];
  $form['orgs']['num_orgs'] = $fields['num_orgs'];
  return theme('table', $table_vars);
}


}
