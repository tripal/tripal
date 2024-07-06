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
 *    description = @Translation("Generate a taxonomy tree from organism stored in Chado"),
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
   * Holds the list of all organisms currently in Chado. This list
   * is needed when checking to see if an organism has already been
   * loaded.
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
      '#title' => t('Tree Name'),
      '#description' => t('If a tree with this name exists, it will be rebuilt,
        otherwise a new tree will be created with this name.'),
      '#default_value' => $default_tree_name,
    ];

if (0) { //@@@ remove
    $form['ncbi_api_key'] = [
      '#type' => 'textfield',
      '#title' => t('(Optional) NCBI API key'),
      '#description' => t('Tripal imports Taxonomy information using NCBI\'s ')
        . Link::fromTextAndUrl('EUtils API', Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25500/'))->toString()
        . t(', which limits users and programs to a maximum of 3 requests per second without an API key. '
          . 'However, NCBI allows users and programs to an increased maximum of 10 requests per second if '
          . 'they provide a valid API key. This is particularly useful in speeding up large taxonomy imports. '
          . 'For more information on NCBI API keys, please ')
        . Link::fromTextAndUrl('see here', Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25497/#chapter2.Coming_in_December_2018_API_Key'), array(
        'attributes' => array(
          'target' => 'blank',
        ),
      ))->toString() . '.',
      '#default_value' => \Drupal::state()->get('tripal_ncbi_api_key', NULL),
      '#ajax' => array(
        'callback' => [$this::class, 'tripal_tree_generator_set_ncbi_api_key'],
        'wrapper' => 'ncbi_api_key',
        'disable-refocus' => true,
      ),
      '#prefix' => '<div id="ncbi_api_key">',
      '#suffix' => '</div>',
    ];
}//@@@

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
    global $user;

    $form_state_values = $form_state->getValues();
//@@@ maybe can delete the validation
  }

  /**
   * Performs the import.
   */
  public function run() {

    $chado = $this->getChadoConnection();

    $arguments = $this->arguments['run_args'];
    $tree_name = $arguments['tree_name'];
    $root_taxon = $arguments['root_taxon'];

    // Get the list of all organisms as we'll need this to lookup existing
    // organisms. Include lookup of NCBI taxid, if present.
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
        AND OP.organism_id = O.organism_id) AS lineage
      FROM {1:organism} O
        LEFT JOIN {1:cvterm} CVT ON CVT.cvterm_id = O.type_id
      ORDER BY O.genus, O.species, CVT.name, O.infraspecific_name
    ";
    $results = $chado->query($sql);
    while ($item = $results->fetchObject()) {
      // If $root_taxon is specified, and lineage is already stored in chado,
      // then we can filter out undesired organisms here when processing a
      // sub-tree, and avoid an unnecessary query to NCBI later.
      if (($root_taxon) and ($item->lineage) and !preg_match("/$root_taxon/i", $item->lineage)) {
        continue;
      }
      $this->all_orgs[] = $item;
    }

    // Get the phylotree object.
    $this->logger->notice('Initializing Tree...');
    $this->phylotree = $this->initTree($tree_name);
    $this->logger->notice('Rebuilding Tree...');
    $this->tree = $this->rebuildTree($root_taxon);

    // Clean out the phylonodes for this tree in the event this is a reload.
    chado_delete_record('phylonode', ['phylotree_id' => $this->phylotree->phylotree_id], NULL, $this->chado_schema_main);

    // Get the taxonomy IDs provided by the user (if any).
    $tax_ids = [];
    if ($taxonomy_ids) {
      $tax_ids = preg_split("/[\s\n\t\r]+/", $taxonomy_ids);
    }

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
   * Retrieves NCBI Taxonomy lineage details for organisms at this site.
   */
  protected function retrieveLineage($root_taxon = NULL) {

    $total = count($this->all_orgs);
    $omitted_organisms = [];

    foreach ($this->all_orgs as $organism) {
      $error = $this->addOrganismNode($organism, $root_taxon);
      // 
      if (!$error) {
        $this->addItemsHandled(1);
      }
      else {
        // Save a list of problematic organisms for a final warning message.
        $omitted_organisms[] = $error;
      }
    }
    if (count($omitted_organisms)) {
      $omitted_list = implode('", "', $omitted_organisms);
      $this->logger->warning('The following @count organisms do not have an NCBI taxonomy ID,'
                           . ' and have not been included in the tree: "@omitted_list"',
        ['@count' => count($omitted_organisms), '@omitted_list' => $omitted_list]
      );
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
   * organisms and then we can add to it.
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
      if (!$result) { // POSSIBLE BUG: MIGHT BE A BUG???
        continue;
      }
      $phylonode = $result->fetchObject(); // POSSIBLE BUG: Why would we fetch if the result is empty? based on the previous if empty clause

      // Next get the lineage for this organism. If missing, we cannot
      // add this organism to the tree.
      $lineageprop = $this->getProperty($organism->organism_id, 'lineage');
      if (!$lineageprop) {
        continue;
      }
      $lineage = $lineageprop->value;

      // If a root taxon is specified, remove everything above it in
      // the lineage. This taxon will then become the tree root.
      if ($root_taxon) {
        $lineage = preg_replace("/^.*($root_taxon)/i", '$1', $lineage);
      }
      $lineage_depth = preg_split('/;\s*/', $lineage);

      // Now rebuild the tree by first creating the nodes for the full
      // lineage and then adding the organism as a leaf node.
      $parent = $tree;
      $i = 1;
      $lineage_good = TRUE;
      foreach ($lineage_depth as $child) {
        // We need to find the node in the phylotree for this level of the
        // lineage, but there's a lot of repeats and we don't want to keep
        // doing the same queries over and over, so we store the nodes
        // we've already seen in the $lineage_nodes array for fast lookup.
        if (array_key_exists($child, $lineage_nodes)) {
          $phylonode = $lineage_nodes[$child];
          if (!$phylonode) {
            $lineage_good = FALSE;
            continue;
          }
        }
        else {
          $values = [
            'phylotree_id' => $this->phylotree->phylotree_id,
            'label' => $child,
          ];
          $columns = ['*'];
          $phylonode = chado_select_record('phylonode', $columns, $values, NULL, $this->chado_schema_main);
          if (count($phylonode) == 0) {
            $lineage_nodes[$child] = NULL;
            $lineage_good = FALSE;
            continue;
          }
          $phylonode = $phylonode[0];
          $lineage_nodes[$child] = $phylonode;

          $values = [
            'phylonode_id' => $phylonode->phylonode_id,
            'type_id' => $this->rank_cvterm_id,
          ];
          $columns = ['*'];
          $phylonodeprop = chado_select_record('phylonodeprop', $columns, $values, NULL, $this->chado_schema_main);
        }
        $name = $child;
        // $node_rank = (string) $child->Rank; // Removed because it's unused
        $node = [
          'name' => $name,
          'depth' => $i,
          'is_root' => 0,
          'is_leaf' => 0,
          'is_internal' => 1,
          'left_index' => 0,
          'right_index' => 0,
          'parent' => $parent,
          'branch_set' => [],
          'parent' => $parent['name'],
          'properties' => [
            $rank_cvterm_id => $phylonodeprop[0]->value,
          ],
        ];
        $parent = $node;
        $this->addTaxonomyNode($tree, $node, $lineage_depth);
        $i++;
      } // end foreach ($lineage_depth as $child) { ...

      // If $stop is set then we had problems setting the lineage so
      // skip adding the leaf node below.
      if (!$lineage_good) {
        continue;
      }

      $rank_type = 'species';
      if (property_exists($organism, 'type_id') and $organism->type_id) {
        $rank_type = $organism->type;
      }

      // Now add in the leaf node
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
          $rank_cvterm_id => $rank_type,
        ],
      ];
      $this->addTaxonomyNode($tree, $node, $lineage_depth);

      // Set the indices for the tree.
      chado_assign_phylogeny_tree_indices($tree);
    }

    return $tree;
  }

  /**
   * Imports an organism from the NCBI taxonomy DB by its taxonomy ID
   *
   * @param $organism
   *   An organism object.
   * @param $root_taxon
   *   An optional taxon name for the root node if generating a sub-tree.
   */
  protected function addOrganismNode($organism, $root_taxon = NULL) : string {
    $adds_organism = $organism ? FALSE : TRUE;

    // Get the "rank" cvterm. It requires that the TAXRANK vocabulary is loaded.
    $query = $chado->select('1:cvterm', 't');
    $query->leftJoin('1:cv', 'cv', 't.cv_id = cv.cv_id');
    $query->fields('t', ['cvterm_id']);
    $query->condition('t.name', 'rank', '=');
    $query->condition('cv.name', 'local', '=');
    $results = $query->execute();
    $rank_cvterm_id = $results->fetchObject()->cvterm_id;

    if ($xml) {
      $taxon = $xml->Taxon;

      // Get the genus and species from the xml.
      $parent = (string) $taxon->ParentTaxId;
      $rank = (string) $taxon->Rank;
      $lineage = (string) $taxon->Lineage;

      // If an alternate root node taxon is specified, check for its
      // presence in the lineage. If absent, this organism will not be
      // included in the tree.
      if (($root_taxon) and !preg_match("/$root_taxon/i", $lineage)) {
        return FALSE;
      }

      // NCBI cultivars may contain escaped single quotes.
      // Here we just delete them.
      $sci_name = (string) $taxon->ScientificName;
      $sci_name = preg_replace('/&#039;/', '', $sci_name);

      // If we have a heterotypic synonym, the $sci_name from NCBI may
      // be different than what is stored in chado. To keep the site
      // consistent, use the name from chado for the tree.
      if ($organism) {
        $chado_name = chado_get_organism_scientific_name($organism, $this->chado_schema_main);
        if ($chado_name != $sci_name) {
          $this->logger->warning("Substituting site taxon \"@chado_name\" for NCBI taxon \"@sci_name\","
                               . " taxid @taxid, organism_id @organism_id",
            ['@chado_name' => $chado_name, '@sci_name' => $sci_name,
             '@taxid' => $taxid, '@organism_id' => $organism->organism_id]
          );
          $sci_name = $chado_name;
        }
      }
      //$this->logMessage(' - Importing @sci_name', array('@sci_name' => $sci_name));

      // If we don't have an organism record provided then see if there
      // is one provided by Chado, if not, then try to add one.
      if (!$organism) {
        $organism = $this->findOrganism($taxid, $sci_name);
        if (!$organism) {
          $organism = $this->addOrganism($sci_name, $rank);
          if (!$organism) {
            throw new \Exception(t('Cannot add organism: @sci_name', ['@sci_name' => $sci_name]));
          }
        }
      }

      // Associate the dbxref with the organism.
      $this->addDbxref($organism->organism_id, $taxid);

      // Get properties for this organism.
      $genetic_code = (string) $taxon->GeneticCode->GCId;
      $genetic_code_name = (string) $taxon->GeneticCode->GCName;
      $mito_genetic_code = (string) $taxon->MitoGeneticCode->MGCId;
      $mito_genetic_code_name = (string) $taxon->MitoGeneticCode->MGCName;
      $division = (string) $taxon->Division;

      // Add in the organism properties.
      $this->addProperty($organism->organism_id, 'division', $division);
      $this->addProperty($organism->organism_id, 'mitochondrial_genetic_code_name', $mito_genetic_code_name);
      $this->addProperty($organism->organism_id, 'mitochondrial_genetic_code', $mito_genetic_code);
      $this->addProperty($organism->organism_id, 'genetic_code_name', $genetic_code_name);
      $this->addProperty($organism->organism_id, 'lineage', $lineage);
      $this->addProperty($organism->organism_id, 'genetic_code', $genetic_code);

      $name_ranks = [];
      if ($taxon->OtherNames->children) {
        foreach ($taxon->OtherNames->children() as $child) {
          $type = $child->getName();
          $name = (string) $child;
          if (!array_key_exists($type, $name_ranks)) {
            $name_ranks[$type] = 0;
          }
          switch ($type) {
            case 'GenbankCommonName':
              $this->addProperty($organism->organism_id, 'genbank_common_name', $name, $name_ranks[$type]);
              break;
            case 'Synonym':
            case 'GenbankSynonym':
              $this->addProperty($organism->organism_id, 'synonym', $name, $name_ranks[$type]);
              break;
            case 'CommonName':
              // If we had to add the organism then include the common name too.
              if ($adds_organism) {
                $organism->common_name = $name;
                $values = ['organism_id' => $organism->id];
                chado_update_record('organism', $values, $organism, NULL, $this->chado_schema_main);
              }
            case 'Includes':
              $this->addProperty($organism->organism_id, 'other_name', $name, $name_ranks[$type]);
              break;
            case 'EquivalentName':
              $this->addProperty($organism->organism_id, 'equivalent_name', $name, $name_ranks[$type]);
              break;
            case 'Anamorph':
              $this->addProperty($organism->organism_id, 'anamorph', $name, $name_ranks[$type]);
              break;
            case 'Name':
              // skip the Name stanza
              break;
            default:
              print "NOTICE: Skipping unrecognzed name type: $type\n";
            // do nothing for unrecognized types
          }
          $name_ranks[$type]++;
        }
      }

      // If a root taxon is specified, remove everything above it in
      // the lineage. This taxon will then become the tree root.
      if ($root_taxon) {
        $lineage = preg_replace("/^.*($root_taxon)/i", '$1', $lineage);
      }

      // Generate a nested array structure that can be used for importing the tree.
      $lineage_depth = preg_split('/;\s*/', $lineage);
      $parent = $this->tree;
      $i = 1;
      $passed_root = $root_taxon?0:1;
      foreach ($taxon->LineageEx->children() as $child) {
        $tid = (string) $child->TaxID;
        $name = (string) $child->ScientificName;
        $node_rank = (string) $child->Rank;
        // If $root_taxon is defined, skip higher ranks until it is seen.
        if (($root_taxon) and ($name == $root_taxon)) {
          $passed_root = 1;
        }
        if (!$passed_root) {
          continue;
        }
        $node = [
          'name' => $name,
          'depth' => $i,
          'is_root' => 0,
          'is_leaf' => 0,
          'is_internal' => 1,
          'left_index' => 0,
          'right_index' => 0,
          'parent' => $parent,
          'branch_set' => [],
          'parent' => $parent['name'],
          'properties' => [
            $rank_cvterm_id => $node_rank,
          ],
        ];
        $parent = $node;
        $this->addTaxonomyNode($this->tree, $node, $lineage_depth);
        $i++;
      }
      // Now add in the leaf node
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
          $rank_cvterm_id => $rank,
        ],
      ];
      $this->addTaxonomyNode($this->tree, $node, $lineage_depth);

      // Set the indices for the tree.
      chado_assign_phylogeny_tree_indices($this->tree);
      return TRUE;
    }
    else {
      $this->logger->warning("Error contacting NCBI to look up taxid @taxid",
        ['@taxid' => $taxid]
      );
      return FALSE;
    }
  }

  /**
   *
   */
  protected function addTaxonomyNode(&$tree, $node, $lineage_depth) {

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
          if (isset($branch_set[$j]['name']) and ($branch_set[$j]['name'] == $lineage_depth[$i - 1])) {
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
   * Retrieves a property for a given organism.
   *
   * @param $organism_id
   *   The organism ID to which the property is added.
   * @param $term_name
   *   The name of the organism property term.  This term must be
   *   present in the 'local' cv.
   * @param $rank
   *   The order for this property. The first instance of this term for
   *   this organism should be zero. Defaults to zero.
   *
   * @return
   *   The property object.
   */
  protected function getProperty($organism_id, $term_name, $rank = 0) {
    $record = [
      'table' => 'organism',
      'id' => $organism_id,
    ];
    $property = [
      'type_name' => $term_name,
      'cv_name' => 'local',
      'rank' => $rank,
    ];
    return chado_get_property($record, $property, $this->chado_schema_main);
  }

  /**
   * Adds a property to an organism node.
   *
   * @param $organism_id
   *   The organism ID to which the property is added.
   * @param $term_name
   *   The name of the organism property term.  This term must be
   *   present in the 'local' cv.
   * @param $value
   *   The value of the property.
   * @param $rank
   *   The order for this property. The first instance of this term for
   *   this organism should be zero. Defaults to zero.
   */
  protected function addProperty($organism_id, $term_name, $value, $rank = 0) {
    if (!$value) {
      return;
    }

    $record = [
      'table' => 'organism',
      'id' => $organism_id,
    ];
    $property = [
      'type_name' => $term_name,
      'cv_name' => 'local',
      'value' => $value,
    ];

    // Delete all properties of this type if the rank is zero.
    if ($rank == 0) {
      chado_delete_property($record, $property, $this->chado_schema_main);
    }
    chado_insert_property($record, $property, [], $this->chado_schema_main);
  }

  /**
   *
   * @param unknown $organism_id
   * @param unknown $taxId
   */
  protected function addDbxref($organism_id, $taxId) {
    $chado = $this->getChadoConnection();
    $db = chado_get_db(['name' => 'NCBITaxon'], [], $this->chado_schema_main);
    $values = [
      'db_id' => $db->db_id,
      'accession' => $taxId,
    ];
    $dbxref = chado_insert_dbxref($values, [], $this->chado_schema_main);

    $values = [
      'dbxref_id' => $dbxref->dbxref_id,
      'organism_id' => $organism_id,
    ];

    if (!chado_select_record('organism_dbxref', ['organism_dbxref_id'], $values, NULL, $this->chado_schema_main)) {
      // chado_insert_record('organism_dbxref', $values);
      $chado->insert('1:organism_dbxref')
      ->fields($values)
      ->execute();
    }
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
