<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Tree Generator implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_tree_generator",
 *    label = @Translation("Taxonomy Tree Generator"),
 *    description = @Translation("Generate a taxonomy tree from organisms stored in Chado"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Generate Taxonomy Tree"),
 *    file_upload = FALSE,
 *    file_local = FALSE,
 *    file_remote = FALSE,
 *    file_required = FALSE,
 *  )
 */
class TreeGenerator extends ChadoImporterBase {

  /**
   * Holds the list of all organisms currently in Chado.
   */
  protected $all_orgs = [];

  /**
   * The record from the Chado phylotree table that refers to this
   * Taxonomic tree.
   */
  protected $phylotree = NULL;

  /**
   * The temporary tree array used by the Tripal Phylotree API for
   * importing a new tree.
   */
  protected $tree = NULL;

  /**
   * CV term id for local:rank
   */
  protected $rank_cvterm_id = NULL;


  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $form['instructions'] = [
      '#type' => 'fieldset',
      '#title' => 'INSTRUCTIONS',
      '#description' => t('This form is used to generate a phylogenetic
        tree for organisms at exist on this site. The organisms need to
        have been previously prepared using the Taxonomy Importer in order
        to have the lineage properties in place.'),
    ];

    $site_name = \Drupal::config('system.site')->get('name');
    $default_tree_name = trim($site_name . ' Taxonomy Tree');
    $form['tree_name'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('Tree Name'),
      '#description' => t('If a tree with this name exists, it will be rebuilt,
        otherwise a new tree will be created with this name.'),
      '#default_value' => $default_tree_name,
    ];

    $form['root_taxon'] = [
      '#type' => 'textfield',
      '#title' => t('(Optional) Root Taxon'),
      '#description' => t('An optional top level taxon for the tree.
        For NCBI lineage, the top level is "cellular organisms". Specify
        a taxon here to use as the tree root, for example at the order
        or family level. This allows you to generate a tree using
        a subset of the site\'s organisms.'),
      '#default_value' => '',
    ];

    return $form;
  }


  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

  }


  /**
   * Performs the import.
   */
  public function run() {

    $chado = $this->getChadoConnection();

    $arguments = $this->arguments['run_args'];
    $tree_name = $arguments['tree_name'];
    $root_taxon = $arguments['root_taxon'];

    // Get the list of all organisms.
    $sql = "
      SELECT O.*, CVT.name AS type,
      (SELECT X.accession FROM {1:dbxref} X
        LEFT JOIN {1:organism_dbxref} OD ON OD.dbxref_id = X.dbxref_id
        LEFT JOIN {1:db} DB ON X.db_id = DB.db_id
        WHERE OD.organism_id = O.organism_id
        AND DB.name = 'NCBITaxon') AS ncbitaxid,
      (SELECT OP.value from {1:organismprop} OP WHERE
        type_id = (SELECT cvterm_id FROM {1:cvterm} WHERE name = 'lineage'
        AND cv_id = (SELECT cv_id FROM {1:cv} WHERE name = 'local'))
        AND OP.organism_id = O.organism_id) AS lineage,
      (SELECT OP.value from {1:organismprop} OP WHERE
        type_id = (SELECT cvterm_id FROM {1:cvterm} WHERE name = 'lineageex'
        AND cv_id = (SELECT cv_id FROM {1:cv} WHERE name = 'local'))
        AND OP.organism_id = O.organism_id) AS lineageex
      FROM {1:organism} O
        LEFT JOIN {1:cvterm} CVT ON CVT.cvterm_id = O.type_id
      ORDER BY O.genus, O.species, CVT.name, O.infraspecific_name
    ";
    $results = $chado->query($sql);
    while ($item = $results->fetchObject()) {
      $this->all_orgs[] = $item;
    }

    // Get the phylotree object.
    $this->logger->notice('Initializing Tree...');
    $this->phylotree = $this->initTree($tree_name);
    $this->logger->notice('Rebuilding Tree...');
    $this->tree = $this->rebuildTree($root_taxon);

    // Clean out the phylonodes for this tree in the event this is a reload.
    chado_delete_record('phylonode', ['phylotree_id' => $this->phylotree->phylotree_id], NULL, $this->chado_schema_main);

    // Set the number of items to handle.
    $this->setTotalItems(count($this->all_orgs));
    $this->setItemsHandled(0);

    // Update existing records
    $this->logger->notice('Retrieving lineages...');
    $this->retrieveLineage($root_taxon);

    // These are options for the tripal_report_error function. We do not
    // want to log messages to the watchdog but we do for the job and to
    // the terminal.
    $options['message_type'] = 'tripal_phylogeny';
    $options['message_opts'] = [
      'watchdog' => FALSE,
      'print' => TRUE,
    ];
    // Pass through the job, needed for log output to show up on the "jobs page".
    if (property_exists($this, 'job')) {
      $options['message_opts']['job'] = $this->job;
    }

    // This importer imports only species (taxonomy) trees.
    $options['leaf_type'] = 'taxonomy';

    // Now import the tree.
    chado_phylogeny_import_tree($this->tree, $this->phylotree, $options, [], NULL, $this->chado_schema_main);
  }


  /**
   * Parses NCBI Taxonomy lineage details for organisms at this site.
   * This is information previously generated in Chado by the taxonomy importer.
   */
  protected function retrieveLineage($root_taxon = NULL) {

    $total = count($this->all_orgs);
    $omitted_organisms = [];
    $outside_root_taxon = 0;

    foreach ($this->all_orgs as $organism) {
      $status = $this->addOrganismNode($organism, $root_taxon);
      // 1=added, 2=not within root_taxon, 3=no lineage available
      if ($status == 1) {
        $this->addItemsHandled(1);
      }
      elseif ($status == 2) {
        $outside_root_taxon++;
      }
      elseif ($status == 3) {
        // Save a list of problematic organisms for a final warning message.
        $message = 'id:' . $organism->organism_id . ' name:' . chado_get_organism_scientific_name($organism);
        $omitted_organisms[] = $message;
      }
    }
    if ($outside_root_taxon) {
      $this->logger->notice('@count organisms were outside the specified root taxon "@root_taxon"'
                          . ' and have not been included in the tree.',
        ['@count' => $outside_root_taxon, '@root_taxon' => $root_taxon]);
    }
    if (count($omitted_organisms)) {
      $omitted_list = implode('", "', $omitted_organisms);
      $this->logger->warning('The following @count organisms do not have a taxonomic lineage stored in Chado,'
                           . ' and have not been included in the tree: "@omitted_list"',
        ['@count' => count($omitted_organisms), '@omitted_list' => $omitted_list]);
    }
  }


  /**
   * Create the taxonomic tree in Chado.
   *
   * If the tree already exists it will not be recreated.
   *
   * @throws Exception
   * @return
   *   Returns the phylotree object.
   */
  protected function initTree($tree_name) {
    // Add the taxonomy tree record into the phylotree table. If the tree
    // already exists then don't insert it again.
    $phylotree = chado_select_record('phylotree', ['*'], ['name' => $tree_name], NULL, $this->chado_schema_main);
    if (count($phylotree) == 0) {
      // Add the taxonomic tree.
      $phylotree = [
        'name' => $tree_name,
        'description' => 'A phylogenetic tree based on taxonomic rank.',
        'leaf_type' => 'taxonomy',
        'tree_file' => '/dev/null',
        'format' => 'taxonomy',
        'no_load' => TRUE,
      ];
      $errors = [];
      $warnings = [];
      $success = chado_insert_phylotree($phylotree, $errors, $warnings, $this->chado_schema_main);
      if (!$success) {
        throw new \Exception("Cannot add the Taxonomy Tree record.");
      }
      $phylotree = (object) $phylotree;
    }
    else {
      $phylotree = $phylotree[0];
    }
    return $phylotree;
  }


  /**
   * Iterates through all existing organisms and builds the taxonomy tree.
   *
   * The phloytree API doesn't support adding nodes to existing trees, only
   * importing whole trees. So, we must rebuild the tree using the current
   * organisms.
   *
   */
  protected function rebuildTree($root_taxon = NULL) {
    $chado = $this->getChadoConnection();
    $lineage_nodes[] = [];

    // Get the "local:rank" cvterm.
    if (!$this->rank_cvterm_id) {
      $query = $chado->select('1:cvterm', 't');
      $query->leftJoin('1:cv', 'cv', 't.cv_id = cv.cv_id');
      $query->fields('t', ['cvterm_id']);
      $query->condition('t.name', 'rank', '=');
      $query->condition('cv.name', 'local', '=');
      $results = $query->execute();
      $this->rank_cvterm_id = $results->fetchObject()->cvterm_id;
    }

    // The taxonomic tree must have a root, so create that first.
    $tree = [
      'name' => 'root',
      'depth' => 0,
      'is_root' => 1,
      'is_leaf' => 0,
      'is_internal' => 0,
      'left_index' => 0,
      'right_index' => 0,
      'branch_set' => [],
    ];

    $total = count($this->all_orgs);
    $j = 1;
    foreach ($this->all_orgs as $organism) {
      $sci_name = chado_get_organism_scientific_name($organism, $this->chado_schema_main);
      //$this->logMessage("- " . ($j++) . " of $total. Adding @organism", array('@organism' => $sci_name));

      // First get the phylonode record for this organism.
      $sql = "
        SELECT P.*
        FROM {1:phylonode} P
          INNER JOIN {1:phylonode_organism} PO on PO.phylonode_id = P.phylonode_id
        WHERE P.phylotree_id = :phylotree_id AND PO.organism_id = :organism_id
      ";
      $args = [
        ':phylotree_id' => $this->phylotree->phylotree_id,
        ':organism_id' => $organism->organism_id,
      ];
      $result = $chado->query($sql, $args);
      if (!$result) {
        continue;
      }
      $phylonode = $result->fetchObject();

      // Next get the lineage for this organism. If missing, we cannot
      // add this organism to the tree. lineageex if available includes
      // ranks for each element.
      $lineage = $organism->lineageex;
      if (!$lineage) {
        $lineage = $organism->lineage;
      }
      $lineage_elements = $this->trimLineage($lineage, $root_taxon);
      if (!$lineage_elements) {
        continue;
      }
      // Omit if not part of root taxon.
      if ($root_taxon and !in_array($root_taxon, $lineage_elements)
          and !preg_grep('/:'.$root_taxon.'$/', $lineage_elements)) {
        continue;
      }

      // Now rebuild the branch for this organism by first creating
      // the nodes for the full lineage, and finally adding the
      // organism as a leaf node.
      $parent = $tree;
      $i = 1;
      $lineage_good = TRUE;
      foreach ($lineage_elements as $element) {
        // If we have lineageex available from NCBI, it will include rank terms (order, family, etc.)
        $subelements = explode(':', $element, 3);
        $node_rank = NULL;
        $node_name = $subelements[0];
        if (count($subelements) == 3) {
          $node_rank = $subelements[0];
          $node_name = $subelements[2];
        }
        // We need to find the node in the phylotree for this level of the
        // lineage, but there's a lot of repeats and we don't want to keep
        // doing the same queries over and over, so we store the nodes
        // we've already seen in the $lineage_nodes array for fast lookup.
        if (array_key_exists($element, $lineage_nodes)) {
          $phylonode = $lineage_nodes[$node_name];
          if (!$phylonode) {
            $lineage_good = FALSE;
            continue;
          }
        }
        else {
          $values = [
            'phylotree_id' => $this->phylotree->phylotree_id,
            'label' => $node_name,
          ];
          $columns = ['*'];
          $phylonode = chado_select_record('phylonode', $columns, $values, NULL, $this->chado_schema_main);
          if (count($phylonode) == 0) {
            $lineage_nodes[$node_name] = NULL;
            $lineage_good = FALSE;
            continue;
          }
          $phylonode = $phylonode[0];
          $lineage_nodes[$node_name] = $phylonode;

          $values = [
            'phylonode_id' => $phylonode->phylonode_id,
            'type_id' => $this->rank_cvterm_id,
          ];
          $columns = ['*'];
          $phylonodeprop = chado_select_record('phylonodeprop', $columns, $values, NULL, $this->chado_schema_main);
        }
        // If we have lineageex available from NCBI, it will include rank terms (order, family, etc.)
        $subelements = explode(':', $element, 3);
        $node_rank = NULL;
        $node_name = $subelements[0];
        if (count($subelements) == 3) {
          $node_rank = $subelements[0];
          $node_name = $subelements[2];
        }

        $node = [
          'name' => $node_name,
          'depth' => $i,
          'is_root' => 0,
          'is_leaf' => 0,
          'is_internal' => 1,
          'left_index' => 0,
          'right_index' => 0,
          'parent' => $parent,
          'branch_set' => [],
          'parent' => $parent['name'],
        ];
        if ($phylonodeprop) {
          $node['properties'] = [$this->rank_cvterm_id => $phylonodeprop[0]->value];
        }
        $parent = $node;
        $this->addTaxonomyNode($tree, $node, $lineage_elements);
        $i++;
      } // end foreach ($lineage_elements as $element) { ...

      // If $stop is set then we had problems setting the lineage so
      // skip adding the leaf node below.
      if (!$lineage_good) {
        continue;
      }

      $leaf_rank = 'species';
      if (property_exists($organism, 'type_id') and $organism->type_id and ($organism->type != 'no_rank')) {
        $leaf_rank = $organism->type;
      }

      // Now add in the leaf node, which is the organism.
      $sci_name = chado_get_organism_scientific_name($organism, $this->chado_schema_main);
      $node = [
        'name' => $sci_name,
        'depth' => $i,
        'is_root' => 0,
        'is_leaf' => 1,
        'is_internal' => 0,
        'left_index' => 0,
        'right_index' => 0,
        'parent' => $parent['name'],
        'organism_id' => $organism->organism_id,
        'properties' => [
          $this->rank_cvterm_id => $leaf_rank,
        ],
      ];
      $this->addTaxonomyNode($tree, $node, $lineage_elements);

      // Set the indices for the tree.
      chado_assign_phylogeny_tree_indices($tree);
    }

    return $tree;
  }

  /**
   * Adds an organism to the taxonomy tree using its lineage.
   *
   * @param $organism
   *   An organism object.
   * @param $root_taxon
   *   An optional taxon name for the root node if generating a sub-tree.
   *
   * @return int
   *   Status. 1 = added
   *           2 = not added, because not part of $root_taxon (if specified)
   *           3 = no lineage available for this organism
   */
  protected function addOrganismNode($organism, $root_taxon = NULL) : int {
    $chado = $this->getChadoConnection();
    $adds_organism = $organism ? FALSE : TRUE;

    if (property_exists($organism, 'lineage') and $organism->lineage) {

      $leaf_rank = $organism->type;
      if (!$leaf_rank or ($leaf_rank = 'no_rank')) {
        $leaf_rank = 'species';
      }
      $lineage = $organism->lineageex;
      if (!$lineage) {
        $lineage = $organism->lineage;
      }
      $lineage_elements = $this->trimLineage($lineage, $root_taxon);

      // If a root node taxon was specified, check for its
      // presence in the lineage. If absent, this organism will
      // not be included in the tree, which is indicated by status=2.
      if ($root_taxon and !in_array($root_taxon, $lineage_elements) 
          and !preg_grep('/:'.$root_taxon.'$/', $lineage_elements)) {
        return 2;
      }

      $sci_name = chado_get_organism_scientific_name($organism, $this->chado_schema_main);
      // $this->logger->notice(' - Importing @sci_name', array('@sci_name' => $sci_name));

      // Generate a nested array structure that can be used for importing the tree.
      $parent = $this->tree;
      $i = 1;
      foreach ($lineage_elements as $element) {
        // If we have lineageex available from NCBI, it will include rank terms (order, family, etc.)
        $subelements = explode(':', $element, 3);
        $node_rank = NULL;
        $node_name = $subelements[0];
        if (count($subelements) == 3) {
          $node_rank = $subelements[0];
          $node_name = $subelements[2];
        }
print "CPX1 node_name=$node_name\n"; //@@@
        $node = [
          'name' => $node_name,
          'depth' => $i,
          'is_root' => 0,
          'is_leaf' => 0,
          'is_internal' => 1,
          'left_index' => 0,
          'right_index' => 0,
          'parent' => $parent,
          'branch_set' => [],
          'parent' => $parent['name']
        ];
        if ($node_rank) {
          $node['properties'] = [
            $this->rank_cvterm_id => $node_rank,
          ];
        }
        $parent = $node;
        $this->addTaxonomyNode($this->tree, $node, $lineage_elements);
        $i++;
      }
      // Now add in the leaf node, which is the organism.
      $node = [
        'name' => $sci_name,
        'depth' => $i,
        'is_root' => 0,
        'is_leaf' => 1,
        'is_internal' => 0,
        'left_index' => 0,
        'right_index' => 0,
        'parent' => $parent['name'],
        'organism_id' => $organism->organism_id,
        'properties' => [
          $this->rank_cvterm_id => $leaf_rank,
        ],
      ];
      $this->addTaxonomyNode($this->tree, $node, $lineage_elements);

      // Set the indices for the tree.
      chado_assign_phylogeny_tree_indices($this->tree);
      return 1;
    }
    else {
      // No lineage present indicated by status=3
      return 3;
    }
  }

  /**
   *
   */
  protected function addTaxonomyNode(&$tree, $node, $lineage_elements) {
    // Get the branch set for the tree root.
    $branch_set = &$tree['branch_set'];

    // Iterate through the tree up until the depth where this node will
    // be placed.
    $node_depth = $node['depth'];
    for ($i = 1; $i <= $node_depth; $i++) {
      // Iterate through any existing nodes in the branch set to see if
      // the node name matches the correct name for the lineage at this
      // depth. If it matches then it is inside of this branch set that
      // we will place the node.
      // Skip if branch_set is NULL, this can be the case if we are
      // processing the first subspecies for a given species which had
      // been defined earlier.
      if ($branch_set) {
        for ($j = 0; $j < count($branch_set); $j++) {
          // If this node already exists in the tree then return.
          if ($branch_set[$j]['name'] == $node['name'] and
            $branch_set[$j]['depth'] == $node['depth']) {
            return;
          }
          // Otherwise, set the branch to be the current branch and continue.
if ($i > count($lineage_elements)) {
  print "i=$i count lineage_elements="; print count($lineage_elements); print "\n"; //@@@
  var_dump($lineage_elements); 
}
          if (isset($branch_set[$j]['name']) and ($branch_set[$j]['name'] == $lineage_elements[$i - 1])) {
            $branch_set = &$branch_set[$j]['branch_set'];
            break;
          }
        }
      }
    }
    // Add the node to the last branch set.  This should be where this node goes.
    $branch_set[] = $node;
  }


  /**
   * Removes any part of a lineage above an optional root taxon,
   * and returns the resulting lineage as an array.
   *
   * @param string $lineage
   *   The semicolon-delimited taxonomic lineage.
   *   If is lineageex it has colon-delimited parts.
   * @param string $root_taxon
   *   The root taxon, e.g. a family, or NULL.
   *
   * @return array $lineage_elements
   *   Lineage string exploded into an array, starting at root taxon if specified.
   **/
  protected function trimLineage($lineage, $root_taxon = NULL) : array {
    // Convert semicolon-delimited lineage into array
    $lineage_elements = preg_split('/;\s*/', $lineage);

    // If a root taxon is specified, remove everything above it in
    // the lineage. This root_taxon will then become the tree root.
    if ($root_taxon) {
      // Look for the lineageex element, if present
      $matched_root_taxon = preg_grep('/:'.$root_taxon.'$/', $lineage_elements);
      if (count($matched_root_taxon) != 0) {
        $root_taxon = reset($matched_root_taxon);
      }
      $index = array_search($root_taxon, $lineage_elements);
      if ($index !== FALSE) {
        $lineage_elements = array_slice($lineage_elements, $index, NULL, FALSE);
      }
    }
    return $lineage_elements;
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {

  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {

  }

}
