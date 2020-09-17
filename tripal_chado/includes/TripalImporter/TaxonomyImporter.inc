<?php

class TaxonomyImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado NCBI Taxonomy Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_taxonomy';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Imports new organisms from NCBI using taxonomy IDs, or loads taxonomic details about existing organisms.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = [];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = '';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'File Upload';

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * The method that Tripal attempts to use for this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = FALSE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = FALSE;

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import from NCBI Taxonomy';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => FALSE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  /**
   * Indicates if the file must be provided.  An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   */
  public static $file_required = FALSE;


  /**
   * The array of arguments used for this loader.  Each argument should
   * be a separate array containing a machine_name, name, and description
   * keys.  This information is used to build the help text for the loader.
   */
  public static $argument_list = [];


  /**
   * Indicates how many files are allowed to be uploaded.  By default this is
   * set to allow only one file.  Change to any positive number. A value of
   * zero indicates an unlimited number of uploaded files are allowed.
   */
  public static $cardinality = 0;

  /**
   * Holds the list of all orgainsms currently in Chado. This list
   * is needed when checking to see if an organism has already been
   * loaded.
   */
  private $all_orgs = [];

  /**
   * The record from the Chado phylotree table that refers to this
   * Taxonomic tree.
   */
  private $phylotree = NULL;

  /**
   * The temporary tree array used by the Tripal Phylotree API for
   * importing a new tree.
   */
  private $tree = NULL;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $form['instructions'] = [
      '#type' => 'fieldset',
      '#title' => 'instructions',
      '#description' => t('This form is used to import species from the NCBI
        Taxonomy database into this site. Alternatively, it can import details
        about organisms from the NCBI Taxonomy database for organisms that
        already exist on this site.  This loader will also construct
        the taxonomic tree for the species loaded.'),
    ];

    $form['ncbi_api_key'] = [
      '#type' => 'textfield',
      '#title' => t('(Optional) NCBI API key:'),
      '#description' => t('Tripal imports Taxonomy information using NCBI\'s ')
        . l('EUtils API', 'https://www.ncbi.nlm.nih.gov/books/NBK25500/')
        . t(', which limits users and programs to a maximum of 3 requests per second without an API key. '
          . 'However, NCBI allows users and programs to an increased maximum of 10 requests per second if '
          . 'they provide a valid API key. This is particularly useful in speeding up large taxonomy imports. '
          . 'For more information on NCBI API keys, please ')
        . l('see here', 'https://www.ncbi.nlm.nih.gov/books/NBK25497/#chapter2.Coming_in_December_2018_API_Key', array(
        'attributes' => array(
          'target' => 'blank',
        ),
      )) . '.',
      '#default_value' => variable_get('tripal_taxon_importer_ncbi_api_key', NULL),
      '#ajax' => array(
        'callback' => 'tripal_taxon_importer_set_ncbi_api_key',
        'wrapper' => 'ncbi_api_key',
      ),
      '#prefix' => '<div id="ncbi_api_key">',
      '#suffix' => '</div>',
    ];

    $form['taxonomy_ids'] = [
      '#type' => 'textarea',
      '#title' => 'Taxonomy ID',
      '#description' => t('Please provide a list of NCBI taxonomy IDs separated
        by spaces, tabs or new lines.
        The information about these organisms will be downloaded and new organism
        records will be added to this site.'),
    ];

    $form['import_existing'] = [
      '#type' => 'checkbox',
      '#title' => 'Import details for existing species.',
      '#description' => t('The NCBI Taxonomic Importer examines the organisms
        currently present in the database and queries NCBI for the
        taxonomic details.  If the importer is able to match the
        genus and species with NCBI the species details will be imported,
        and a page containing the taxonomic tree will be created.'),
      '#default_value' => 1,
    ];
    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    global $user;

    $import_existing = $form_state['values']['import_existing'];
    $taxonomy_ids = $form_state['values']['taxonomy_ids'];

    // make sure that we have numeric values, one per line.
    if ($taxonomy_ids) {
      $tax_ids = preg_split("/[\s\n\t\r]+/", $taxonomy_ids);
      $bad_ids = [];
      foreach ($tax_ids as $tax_id) {
        $tax_id = trim($tax_id);
        if (!preg_match('/^\d+$/', $tax_id)) {
          $bad_ids[] = $tax_id;
        }
      }
      if (count($bad_ids) > 0) {
        form_set_error('taxonomy_ids',
          t('Taxonomy IDs must be numeric. The following are not valid: "@ids".',
            ['@ids' => implode('", "', $bad_ids)]));
      }
    }
  }

  /**
   * Performs the import.
   */
  public function run() {
    global $site_name;


    $arguments = $this->arguments['run_args'];
    $taxonomy_ids = $arguments['taxonomy_ids'];
    $import_existing = $arguments['import_existing'];

    // Get the list of all organisms as we'll need this to lookup existing
    // organisms.
    if (chado_get_version() > 1.2) {
      $sql = "
        SELECT O.*, CVT.name as type
        FROM {organism} O
         LEFT JOIN {cvterm} CVT ON CVT.cvterm_id = O.type_id
        ORDER BY O.genus, O.species
      ";
    }
    else {
      $sql = "
        SELECT O.*, '' as type
        FROM {organism} O
        ORDER BY O.genus, O.species
      ";
    }
    $results = chado_query($sql);
    while ($item = $results->fetchObject()) {
      $this->all_orgs[] = $item;
    }

    // Get the phylotree object.
    $this->logMessage('Initializing Tree...');
    $this->phylotree = $this->initTree();
    $this->logMessage('Rebuilding Tree...');
    $this->tree = $this->rebuildTree();

    // Clean out the phnylondes for this tree in the event this is a reload
    chado_delete_record('phylonode', ['phylotree_id' => $this->phylotree->phylotree_id]);

    // Get the taxonomy IDs provided by the user (if any).
    $tax_ids = [];
    if ($taxonomy_ids) {
      $tax_ids = preg_split("/[\s\n\t\r]+/", $taxonomy_ids);
    }

    // Set the number of items to handle.
    if ($taxonomy_ids and $import_existing) {
      $this->setTotalItems(count($this->all_orgs) + count($tax_ids));
    }
    if ($taxonomy_ids and !$import_existing) {
      $this->setTotalItems(count($tax_ids));
    }
    if (!$taxonomy_ids and $import_existing) {
      $this->setTotalItems(count($this->all_orgs));
    }
    $this->setItemsHandled(0);

    // If the user wants to import new taxonomy IDs then do that.
    if ($taxonomy_ids) {
      $this->logMessage('Importing Taxonomy IDs...');
      $api_key = variable_get('tripal_taxon_importer_ncbi_api_key', NULL);
      $sleep_time = 333334;
      if (!empty($api_key)) {
        $sleep_time = 100000;
      }

      foreach ($tax_ids as $tax_id) {
        $start = microtime(TRUE);
        $tax_id = trim($tax_id);
        $result = $this->importRecord($tax_id);

        // Only addItemsHandled if the importRecord was a success.
        if ($result) {
          $this->addItemsHandled(1);
        }

        $remaining_sleep = $sleep_time - ((int) (1e6 * (microtime(TRUE) - $start)));
        if ($remaining_sleep > 0) {
          usleep($remaining_sleep);
        }
      }
    }

    // If the user wants to update existing records then do that.
    if ($import_existing) {
      $this->logMessage('Updating Existing...');
      $this->updateExisting();
    }

    // Now import the tree.
    $options = ['taxonomy' => 1];
    chado_phylogeny_import_tree($this->tree, $this->phylotree, $options);
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
  private function initTree() {
    // Add the taxonomy tree record into the phylotree table. If the tree
    // already exists then don't insert it again.
    $site_name = variable_get('site_name');
    $tree_name = $site_name . 'Taxonomy Tree';
    $phylotree = chado_select_record('phylotree', ['*'], ['name' => $tree_name]);
    if (count($phylotree) == 0) {
      // Add the taxonomic tree.
      $phylotree = [
        'name' => $site_name . 'Taxonomy Tree',
        'description' => 'A phylogenetic tree based on taxonomic rank.',
        'leaf_type' => 'taxonomy',
        'tree_file' => '/dev/null',
        'format' => 'taxonomy',
        'no_load' => TRUE,
      ];
      $errors = [];
      $warnings = [];
      $success = tripal_insert_phylotree($phylotree, $errors, $warnings);
      if (!$success) {
        throw new Exception("Cannot add the Taxonomy Tree record.");
      }
      $phylotree = (object) $phylotree;
    }
    else {
      $phylotree = $phylotree[0];
    }
    return $phylotree;
  }


  /**
   * Iterates through all existing organisms and rebuilds the taxonomy tree.
   *
   * The phloytree API doesn't support adding nodes to existing trees only
   * importing whole trees. So, we must rebuild the tree using the current
   * organisms and then we can add to it.
   *
   */
  private function rebuildTree() {
    $lineage_nodes[] = [];

    // Get the "rank" cvterm. It requires that the TAXRANK vocabulary is loaded.
    $rank_cvterm = chado_get_cvterm([
      'name' => 'rank',
      'cv_id' => ['name' => 'local'],
    ]);

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
      $sci_name = chado_get_organism_scientific_name($organism);
      //$this->logMessage("- " . ($j++) . " of $total. Adding @organism", array('@organism' => $sci_name));

      // First get the phylonode record for this organism.
      $sql = "
        SELECT P.*
        FROM {phylonode} P
         INNER JOIN {phylonode_organism} PO on PO.phylonode_id = P.phylonode_id
        WHERE P.phylotree_id = :phylotree_id AND PO.organism_id = :organism_id
      ";
      $args = [
        ':phylotree_id' => $this->phylotree->phylotree_id,
        ':organism_id' => $organism->organism_id,
      ];
      $result = chado_query($sql, $args);
      if (!$result) {
        continue;
      }
      $phylonode = $result->fetchObject();

      // Next get the lineage for this organism.
      $lineage = $this->getProperty($organism->organism_id, 'lineage');
      if (!$lineage) {
        continue;
      }
      $lineage_depth = preg_split('/;\s*/', $lineage->value);

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
          $phylonode = chado_select_record('phylonode', $columns, $values);
          if (count($phylonode) == 0) {
            $lineage_nodes[$child] = NULL;
            $lineage_good = FALSE;
            continue;
          }
          $phylonode = $phylonode[0];
          $lineage_nodes[$child] = $phylonode;


          $values = [
            'phylonode_id' => $phylonode->phylonode_id,
            'type_id' => $rank_cvterm->cvterm_id,
          ];
          $columns = ['*'];
          $phylonodeprop = chado_select_record('phylonodeprop', $columns, $values);
        }
        $name = $child;
        $node_rank = (string) $child->Rank;
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
            $rank_cvterm->cvterm_id => $phylonodeprop[0]->value,
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
      $sci_name = chado_get_organism_scientific_name($organism);
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
          $rank_cvterm->cvterm_id => $rank_type,
        ],
      ];
      $this->addTaxonomyNode($tree, $node, $lineage_depth);

      // Set the indecies for the tree.
      chado_assign_phylogeny_tree_indices($tree);
    }

    return $tree;
  }

  /**
   * Imports details from NCBI Taxonomy for organisms that alrady exist.
   */
  private function updateExisting() {

    $total = count($this->all_orgs);
    $api_key = variable_get('tripal_taxon_importer_ncbi_api_key', NULL);
    $sleep_time = 333334;
    if (!empty($api_key)) {
      $sleep_time = 100000;
    }

    foreach ($this->all_orgs as $organism) {
      // If the organism record is marked as new then let's skip it because
      // it was newly added and should have the updated information already.
      if ($organism->is_new) {
        continue;
      }

      // TODO: we should check if the organism already has a taxonomy ID.
      // if so we should use that instead of the scientific name.

      $start = microtime(TRUE);
      // Build the query string to get the information about this species.
      $sci_name = chado_get_organism_scientific_name($organism);
      $sci_name = urlencode($sci_name);
      $search_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" .
        "db=taxonomy" .
        "&term=$sci_name";

      if (!empty($api_key)) {
        $search_url .= "&api_key=" . $api_key;
      }

      // Get the search response from NCBI.
      $rfh = fopen($search_url, "r");
      $xml_text = '';

      if (!$rfh) {

        $this->logMessage("Could not look up !sci_name", ['!sci_name' => $sci_name], TRIPAL_WARNING);
        continue;
      }
      while (!feof($rfh)) {
        $xml_text .= fread($rfh, 255);
      }
      fclose($rfh);

      $remaining_sleep = $sleep_time - ((int) (1e6 * (microtime(TRUE) - $start)));
      if ($remaining_sleep > 0) {
        usleep($remaining_sleep);
      }

      // Parse the XML to get the taxonomy ID
      $result = FALSE;
      $start = microtime(TRUE);
      $xml = new SimpleXMLElement($xml_text);
      if ($xml) {
        $taxid = (string) $xml->IdList->Id;
        if ($taxid) {
          $result = $this->importRecord($taxid, $organism);
        }
      }

      if ($result) {
        $this->addItemsHandled(1);
      }

      $remaining_sleep = $sleep_time - ((int) (1e6 * (microtime(TRUE) - $start)));
      if ($remaining_sleep > 0) {
        usleep($remaining_sleep);
      }
    }
  }

  /**
   * Checks the Chado database to see if the organism already exists.
   *
   * @param $taxid
   *   The taxonomic ID for the organism.
   * @param $sci_name
   *   The scientific name for the organism as returned by NCBI
   */
  private function findOrganism($taxid, $sci_name) {
    $organism = NULL;

    // First check the taxid to see if it's present and associated with an
    // organism already.
    $values = [
      'db_id' => [
        'name' => 'NCBITaxon',
      ],
      'accession' => $taxid,
    ];
    $columns = ['dbxref_id'];
    $dbxref = chado_select_record('dbxref', $columns, $values);
    if (count($dbxref) > 0) {
      $columns = ['organism_id'];
      $values = ['dbxref_id' => $dbxref[0]->dbxref_id];
      $organism_dbxref = chado_select_record('organism_dbxref', $columns, $values);
      if (count($organism_dbxref) > 0) {
        $organism_id = $organism_dbxref[0]->organism_id;
        $columns = ['*'];
        $values = ['organism_id' => $organism_id];
        $organism = chado_select_record('organism', $columns, $values);
        if (count($organism) > 0) {
          $organism = $organism[0];
        }
      }
    }

    // If the caller did not provide an organism then we want to try and
    // add one. But, it only makes sense to add one if this record
    // is of rank species.
    // First check if the full name (including the infrasepcific name)
    // are all present in the genus and species name.  This would have
    // been the Chado v1.2 (or less) of storing species.
    if (!$organism) {
      $sql = "
          SELECT organism_id
          FROM {organism}
          WHERE concat(genus, ' ', species) = :sci_name
        ";
      $results = chado_query($sql, [':sci_name' => $sci_name]);
      $item = $results->fetchObject();
      if ($item) {
        $columns = ['*'];
        $values = ['organism_id' => $item->organism_id];
        $organism = chado_select_record('organism', $columns, $values);
        if (count($organism) > 0) {
          $organism = $organism[0];
        }
      }
    }
    // Second, check if the full name includes the infraspecific name.
    if (!$organism) {
      foreach ($this->all_orgs as $item) {
        $internal_sci_name = chado_get_organism_scientific_name($item);
        if ($sci_name == $internal_sci_name) {
          $organism = $item;
        }
      }
    }
    return $organism;
  }

  /**
   * Adds a new organism record to Chado.
   *
   * @param sci_name
   *   The scientific name as provied by NCBI Taxonomy.
   * @param $rank
   *   The rank of the organism as provied by NCBI Taxonomy.
   */
  private function addOrganism($sci_name, $rank) {

    $organism = NULL;
    $matches = [];
    $genus = '';
    $species = '';
    $infra = '';
    $values = [];

    // Check if the scientific name has an infraspecific part or is just
    // a species name.
    if (preg_match('/^(.+?)\s+(.+?)\s+(.+)$/', $sci_name, $matches)) {
      $genus = $matches[1];
      $species = $matches[2];
      $infra = $matches[3];

      // Get the CV term for the rank.
      $type = chado_get_cvterm([
        'name' => preg_replace('/ /', '_', $rank),
        'cv_id' => ['name' => 'taxonomic_rank'],
      ]);

      // Remove the rank from the infraspecific name.
      $abbrev = chado_abbreviate_infraspecific_rank($rank);
      $infra = preg_replace("/$abbrev/", "", $infra);
      $infra = trim($infra);

      $values = [
        'genus' => $genus,
        'species' => $species,
        'abbreviation' => $genus[0] . '. ' . $species,
        'type_id' => $type->cvterm_id,
        'infraspecific_name' => $infra,
      ];
      $organism = chado_insert_record('organism', $values);
      $organism = (object) $organism;
      $organism->type = $rank;
    }
    else {
      if (preg_match('/^(.+?)\s+(.+?)$/', $sci_name, $matches)) {
        $genus = $matches[1];
        $species = $matches[2];
        $infra = '';
        $values = [
          'genus' => $genus,
          'species' => $species,
          'abbreviation' => $genus[0] . '. ' . $species,
        ];
        $organism = chado_insert_record('organism', $values);
        $organism = (object) $organism;
      }
    }
    if ($organism) {
      $organism->is_new = TRUE;
      $this->all_orgs[] = $organism;
    }

    return $organism;
  }

  /**
   * Imports an organism from the NCBI taxonomy DB by its taxonomy ID
   *
   * @param $taxid
   *   The NCBI Taxonomy ID.
   * @param $organism
   *   The organism object to which this taxonomy belongs.  If the organism
   *   is NULL then it will be created.
   */
  private function importRecord($taxid, $organism = NULL) {
    $adds_organism = $organism ? FALSE : TRUE;

    // Get the "rank" cvterm. It requires that the TAXRANK vocabulary is loaded.
    $rank_cvterm = chado_get_cvterm([
      'name' => 'rank',
      'cv_id' => ['name' => 'local'],
    ]);

    // Get the details for this taxonomy.
    $fetch_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" .
      "db=taxonomy" .
      "&id=$taxid";

    $api_key = variable_get('tripal_taxon_importer_ncbi_api_key', NULL);
    if (!empty($api_key)) {
      $fetch_url .= "&api_key=" . $api_key;
    }

    // Get the search response from NCBI.
    $xml = FALSE;
    $rfh = fopen($fetch_url, "r");
    if ($rfh) {
      $xml_text = '';
      while (!feof($rfh)) {
        $xml_text .= fread($rfh, 255);
      }
      fclose($rfh);

      $xml = new SimpleXMLElement($xml_text);
    }

    if ($xml) {
      $taxon = $xml->Taxon;

      // Get the genus and species from the xml.
      $parent = (string) $taxon->ParentTaxId;
      $rank = (string) $taxon->Rank;
      $sci_name = (string) $taxon->ScientificName;
      //$this->logMessage(' - Importing @sci_name', array('@sci_name' => $sci_name));

      // If we don't have an organism record provided then see if there
      // is one provided by Chado, if not, the try to add one.
      if (!$organism) {
        $organism = $this->findOrganism($taxid, $sci_name);
        if (!$organism) {
          $organism = $this->addOrganism($sci_name, $rank);
          if (!$organism) {
            throw new Exception(t('Cannot add organism: @sci_name', ['@sci_name' => $sci_name]));
          }
        }
      }

      // Associate the Dbxref with the organism.
      $this->addDbxref($organism->organism_id, $taxid);

      // Get properties for this organism.
      $lineage = (string) $taxon->Lineage;
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
              // If we had to add the organism then include the commone name too.
              if ($adds_organism) {
                $organism->common_name = $name;
                $values = ['organism_id' => $organism->id];
                chado_update_record('organism', $values, $organism);
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

      // Generate a nested array structure that can be used for importing the tree.
      $lineage_depth = preg_split('/;\s*/', $lineage);
      $parent = $this->tree;
      $i = 1;
      foreach ($taxon->LineageEx->children() as $child) {
        $tid = (string) $child->TaxID;
        $name = (string) $child->ScientificName;
        $node_rank = (string) $child->Rank;
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
            $rank_cvterm->cvterm_id => $node_rank,
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
          $rank_cvterm->cvterm_id => $rank,
        ],
      ];
      $this->addTaxonomyNode($this->tree, $node, $lineage_depth);

      // Set the indecies for the tree.
      chado_assign_phylogeny_tree_indices($this->tree);
      return TRUE;
    }
    return FALSE;
  }

  /**
   *
   */
  private function addTaxonomyNode(&$tree, $node, $lineage_depth) {

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
      for ($j = 0; $j < count($branch_set); $j++) {
        // If this node already exists in the tree then return.
        if ($branch_set[$j]['name'] == $node['name'] and
          $branch_set[$j]['depth'] = $node['depth']) {
          return;
        }
        // Otherwise, set the branch to be the current branch and continue.
        if ($branch_set[$j]['name'] == $lineage_depth[$i - 1]) {
          $branch_set = &$branch_set[$j]['branch_set'];
          break;
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
   *   present in the 'organism_property' cv.
   * @param $rank
   *   The order for this property. The first instance of this term for
   *   this organism should be zero. Defaults to zero.
   *
   * @return
   *   The property object.
   */
  private function getProperty($organism_id, $term_name, $rank = 0) {
    $record = [
      'table' => 'organism',
      'id' => $organism_id,
    ];
    $property = [
      'type_name' => $term_name,
      'cv_name' => 'organism_property',
      'value' => $value,
      'rank' => $rank,
    ];

    return chado_get_property($record, $property);
  }

  /**
   * Adds a property to an organism node.
   *
   * @param $organism_id
   *   The organism ID to which the property is added.
   * @param $term_name
   *   The name of the organism property term.  This term must be
   *   present in the 'organism_property' cv.
   * @param $value
   *   The value of the property.
   * @param $rank
   *   The order for this property. The first instance of this term for
   *   this organism should be zero. Defaults to zero.
   */
  private function addProperty($organism_id, $term_name, $value, $rank = 0) {
    if (!$value) {
      return;
    }

    $record = [
      'table' => 'organism',
      'id' => $organism_id,
    ];
    $property = [
      'type_name' => $term_name,
      'cv_name' => 'organism_property',
      'value' => $value,
    ];
    // Delete all properties of this type if the rank is zero.
    if ($rank == 0) {
      chado_delete_property($record, $property);
    }
    chado_insert_property($record, $property);
  }

  /**
   *
   * @param unknown $organism_id
   * @param unknown $taxId
   */
  private function addDbxref($organism_id, $taxId) {
    $db = chado_get_db(['name' => 'NCBITaxon']);
    $values = [
      'db_id' => $db->db_id,
      'accession' => $taxId,
    ];
    $dbxref = chado_insert_dbxref($values);

    $values = [
      'dbxref_id' => $dbxref->dbxref_id,
      'organism_id' => $organism_id,
    ];

    if (!chado_select_record('organism_dbxref', ['organism_dbxref_id'], $values)) {
      chado_insert_record('organism_dbxref', $values);
    }
  }
}

/**
 * Ajax callback for the TaxonomyImporter::form() function.
 *
 * It is called when the user makes a change to the NCBI API key field and then
 * moves their cursor out of the field.
 *
 * @param $form
 *   The new form element.
 * @param $form_state
 *   The state of the new form element.
 *
 * @return array
 *   The new api key field.
 */
function tripal_taxon_importer_set_ncbi_api_key($form, $form_state) {
  variable_set('tripal_taxon_importer_ncbi_api_key', check_plain($form_state['values']['ncbi_api_key']));
  drupal_set_message('NCBI API key has been saved successfully!');
  return $form['ncbi_api_key'];
}
