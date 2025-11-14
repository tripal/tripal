<?php

namespace Drupal\tripal_chado\Services;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal\Services\TripalEntityLookup;

/**
 * Methods used for accessing phylogenetic trees stored in chado.
 *
 * The loadPhylotreeById() method loads a phylotree record from its
 * primary key phylotree_id and returns an associative array.
 *
 * The getTreeStructure() method returns the phylotree representation
 * in a PHP nested array.
 *
 * The getTreeNewick() method is used to generate a newick representation
 * of the phylonodes in a tree, which can then be used to prepare a
 * visualization of the tree.
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
   * @param Drupal\tripal\Services\TripalEntityLookup $entity_lookup_manager
   *   The tripal entity lookup service.
   */
  public function __construct(ChadoConnection $chado_connection, TripalEntityLookup $entity_lookup_manager) {
    $this->chado_connection = $chado_connection;
    $this->entity_lookup_manager = $entity_lookup_manager;
    $this->initializeCvTerms();
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
  protected function loadPhylonodesByTreeId(int $phylotree_id) {
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
   * Generates a newick representation of all the nodes in a phylotree.
   *
   * @param int $phylotree_id
   *   The ID of the phylotree table record.
   * @param array $options
   *   - no_normalize = FALSE, set to TRUE to return node lengths
   *     exactly as they are stored in the phylonode records.
   *
   * @return string
   *   The phylotree encoded in newick format.
   *   Returns an empty string if the specified tree does not
   *   exist, or if it has no phylonodes attached.
   */
  public function getTreeNewick(int $phylotree_id, array $options = []): string {
    $structure = $this->getTreeStructure($phylotree_id, $options);
    $newick = '';
    if ($structure) {
      $newick = $this->getNodeNewick($structure) . ';';
    }
    return $newick;
  }

  /**
   * A recursive function to process nodes into newick format.
   *
   * @param array $node
   *   A leaf, internal, or root node.
   *
   * @return string
   *   The tree or tree subset in newick format.
   */
  protected function getNodeNewick(array $node): string {
    $newick = '';
    if (!($node['children'] ?? [])) {
      // Process a leaf node.
      $newick = $this->quote($node['name']);
    }
    else {
      // Process an internal node.
      $children_strings = [];
      foreach ($node['children'] as $child) {
        $children_strings[] = $this->getNodeNewick($child);
      }
      $name = $this->quote($node['name'] ?? '');
      $newick = '(' . implode(',', $children_strings) . ')' . $name;
    }

    // Include annotations from Chado if present.
    $newick .= $this->encodeAnnotations($node);

    // Include length if present.
    if (isset($node['length'])) {
      $newick .= ':' . $node['length'];
    }

    return $newick;

  }

  /**
   * Encode any additional annotations in a json string.
   *
   * The json string will be enclosed in curly braces {}.
   * Any key or value that is not an integer will be quoted.
   *
   * Example: '{"cvterm_name":"phylo_leaf","organism_id":7,"entity_id":11}'.
   *
   * @param array $node
   *   One node from the phylotree.
   *
   * @return string
   *   Encoded properties, or an empty string if there are none.
   */
  protected function encodeAnnotations(array $node): string {
    $exclude = ['length', 'phylonode_id', 'parent_phylonode_id', 'name', 'children'];
    $annotations = [];
    foreach ($node as $key => $value) {
      if (!in_array($key, $exclude)) {
        if (isset($value) && strlen($value)) {
          $annotations[] = $this->quote($key) . ':' . $this->quote($value);
        }
      }
    }
    $annotation_string = '';
    if ($annotations) {
      $annotation_string = '{' . implode(',', $annotations) . '}';
    }
    return $annotation_string;
  }

  /**
   * Quote if the string is anything other than an integer.
   *
   * @param string $string
   *   The string to processed.
   *
   * @return string
   *   The string with surrounding double quotes if necessary.
   */
  protected function quote(string $string): string {
    if (preg_match('/[^0-9]/', $string)) {
      $string = '"' . $string . '"';
    }
    return $string;
  }

  /**
   * Generates a php representation of all the nodes in a phylotree.
   *
   * @param int $phylotree_id
   *   The ID of the phylotree table record.
   * @param array $options
   *   - no_normalize = FALSE, set to TRUE to return node lengths
   *     exactly as they are stored in the phylonode records.
   *
   * @return array
   *   The phylotree array.
   *   Returns an empty array if the specified tree does not
   *   exist, or if it has no phylonodes attached.
   */
  public function getTreeStructure(int $phylotree_id, array $options = []): array {

    // Stores the function return value.
    $root_phylonode_ref = [];

    /** @var array */
    $phylotree = $this->loadPhylotreeById($phylotree_id);
    if (!$phylotree) {
      return $root_phylonode_ref;
    }

    /** @var Drupal\Core\Database\StatementWrapperIterator */
    $nodes = $this->loadPhylonodesByTreeId($phylotree_id);
    if (!$nodes) {
      return $root_phylonode_ref;
    }

    // Fetch all the phylonodes into an associative array indexed by
    // phylonode_id. Convert from database query record to array,
    // casting to appropriate datatypes.
    $phylonodes = [];
    while ($r = $nodes->fetchObject()) {
      $phylonode_id = (int) $r->phylonode_id;

      // Expect all nodes to have these properties.
      $node = [
        'phylonode_id' => $phylonode_id,
        'parent_phylonode_id' => (int) $r->parent_phylonode_id,
        'length' => (double) $r->length,
        'cvterm_name' => $r->cvterm_name,
      ];

      // Tripal 3 ignored the length for taxonomic trees and set it to 0.001.
      // Because phylotree.js hardcodes tick marks to 2 significant digits,
      // we will now use 0.01. However, if the node already has a length
      // that is greater than this, then use that. Can be overridden by
      // option 'no_normalize' => TRUE.
      if (($phylotree['type_id'] ?? 0) == $this->cv_terms['Species tree']) {
        if (!($options['no_normalize'] ?? FALSE)) {
          if ($node['length'] < 0.01) {
            $node['length'] = 0.01;
          }
        }
      }

      // Other properties may not exist for internal nodes.
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
        $node['entity_id'] = (int) $entity_id;
      }

      // Add in the organism fields when they are available via the
      // phylonode_organism table.
      if ($r->organism_id) {
        $node['organism_id'] = (int) $r->organism_id;
        $node['common_name'] = $r->common_name;
        $node['abbreviation'] = $r->abbreviation;
        $node['entity_id'] = $this->entity_lookup_manager->getEntityId($r->organism_id, 'OBI', '0100026', 'organism');

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
        // @todo these next two are not used, use name code from above?
        $node['fo_genus'] = $r->fo_genus;
        $node['fo_species'] = $r->fo_species;
        $node['entity_id'] = $this->entity_lookup_manager->getEntityId($r->fo_organism_id, 'OBI', '0100026', 'organism');
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

    return $root_phylonode_ref;
  }

}
