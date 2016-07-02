<?php

/**
 * Generates a page that contains the taxonomy view.
 */
function tripal_phylogeny_taxonomy_view() {
  $values = array(
    'type_id' => array(
      'name' => 'taxonomy',
    ),
  );
  $phylotree = chado_generate_var('phylotree', $values);
  $node = new stdClass();
  $node->phylotree = $phylotree;

  return theme('tripal_phylogeny_taxonomic_tree', array('node' => $node));
}
/**
 *
 */
function tripal_phylogeny_taxonomy_load_form($form, &$form_state) {
  $form['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('The NCBI Taxonmic Importer examines the organisms
      currently present in the database and queries NCBI for the
      taxonomic details.  If the importer is able to match the
      genus and species with NCBI the species details will be imported,
      and a page containing the taxonomic tree will be created.'),
  );
  $form['import_ncbi'] = array(
    '#type' => 'submit',
    '#name' => 'import_sync',
    '#value' => 'Import NCBI taxonomic data'
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_phylogeny_taxonomy_load_form_submit($form, &$form_state) {
  global $user;
  $sync = $form_state['values']['do_sync'];
  $args = array();
  tripal_add_job("Import NCBI Taxonomy", 'tripal_phylogeny',
    'tripal_phylogeny_ncbi_taxonomy_import', $args, $user->uid);
}

/**
 *
 * @param unknown $job_id
 */
function tripal_phylogeny_ncbi_taxonomy_import($job_id) {

  print "\nNOTE: Importing of NCBI taxonomy data is performed using a database transaction. \n" .
    "If the load fails or is terminated prematurely then the entire set of \n" .
    "insertions/updates is rolled back and will not be found in the database\n\n";

  $transaction = db_transaction();
  try {
    // TDDO: there should be an API function named tripal_insert_analysis().
    // But until then we have to insert the analysis manually.
    // Get the version of this module for the analysis record:
    $info = system_get_info('module', 'tripal_phylogeny');
    $version = $info['version'];
    $analysis_name = 'NCBI Taxonomy Tree Import';

    // If the analysis record already exists then don't add it again.
    $analysis = chado_select_record('analysis', array('*'), array('name' => $analysis_name));
    if (count($analysis) == 0) {
      $values = array(
        'name' => 'NCBI Taxonomy Tree Import',
        'description' => 'Used to import NCBI taxonomy details for organisms in this database.',
        'program' => 'Tripal Phylogeny Module NCBI Taxonomy Importer',
        'programversion' => $version,
        'sourcename' => 'NCBI Taxonomy',
        'sourceuri' => 'http://www.ncbi.nlm.nih.gov/taxonomy',
      );
      $analysis = chado_insert_record('analysis', $values);
      if (!$analysis) {
        throw new Exception("Cannot add NCBI Taxonomy Tree Import Analysis.");
      }
    }
    else {
      $analysis = $analysis[0];
    }

    // If the tree already exists then don't insert it again.
    global $site_name;
    $tree_name = $site_name . 'Taxonomy Tree';
    $phylotree = chado_select_record('phylotree', array('*'), array('name' => $tree_name));
    if (count($phylotree) == 0) {
      // Add the taxonomic tree.
      $options = array(
        'name' =>  $site_name . 'Taxonomy Tree',
        'description' => 'The taxonomic tree of species present on this site.',
        'leaf_type' => 'taxonomy',
        'analysis_id' => $analysis->analysis_id,
        'tree_file' => '/dev/null',
        'format' => 'taxonomy',
        'no_load' => TRUE,
      );
      $errors = array();
      $warnings = array();
      $success = tripal_insert_phylotree($options, $errors, $warnings);
      if (!$success) {
        throw new Exception("Cannot add the Taxonomy Tree record.");
      }
      $phylotree = (object) $options;
    }
    else {
      $phylotree = $phylotree[0];
    }

    // Clean out the phylotree in the event this is a reload
    chado_delete_record('phylonode', array('phylotree_id' => $phylotree->phylotree_id));

    // The taxonomic tree must have a root, so create that first.
    $tree = array(
      'name' => 'root',
      'depth' => 0,
      'is_root' => 1,
      'is_leaf' => 0,
      'is_internal' => 0,
      'left_index' => 0,
      'right_index' => 0,
      'branch_set' => array(),
    );

    // Get the "rank" cvterm. It requires that the TAXRANK vocabulary is loaded.
    $rank_cvterm = tripal_get_cvterm(array(
      'name' => 'rank',
      'cv_id' => array('name' => 'tripal_phylogeny')
    ));

    // Get the list of orgnaisms
    $sql = "SELECT O.* FROM {organism} O";
    $organisms = chado_query($sql);
    while ($organism = $organisms->fetchObject()) {
      // Build the query string to get the information about this species.
      $term = $organism->genus . ' ' . $organism->species;
      $term = urlencode($term);
      $search_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?".
        "db=taxonomy" .
        "&term=$term";

      // Get the search response from NCBI.
      $rfh = fopen($search_url, "r");
      $xml_text = '';
      while (!feof($rfh)) {
        $xml_text .= fread($rfh, 255);
      }
      fclose($rfh);

      // Parse the XML to get the taxonomy ID
      $xml = new SimpleXMLElement($xml_text);
      if ($xml) {
        $taxid = (string) $xml->IdList->Id;
        if ($taxid) {
          print "$taxid\t$organism->genus $organism->species\n";
          // If we have a taxonomy ID we can now get the details.
          $fetch_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?".
            "db=taxonomy" .
            "&id=$taxid";
          // Get the search response from NCBI.
          $rfh = fopen($fetch_url, "r");
          $xml_text = '';
          while (!feof($rfh)) {
            $xml_text .= fread($rfh, 255);
          }
          fclose($rfh);

          $xml = new SimpleXMLElement($xml_text);
          if ($xml) {
            $taxon = $xml->Taxon;

            // Add in the organism properties
            $lineage = (string) $taxon->Lineage;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'lineage', $lineage);

            $genetic_code = (string) $taxon->GeneticCode->GCId;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'genetic_code', $genetic_code);

            $genetic_code_name = (string) $taxon->GeneticCode->GCName;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'genetic_code_name', $genetic_code_name);

            $mito_genetic_code = (string) $taxon->MitoGeneticCode->MGCId;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'mitochondrial_genetic_code', $mito_genetic_code);

            $mito_genetic_code_name = (string) $taxon->MitoGeneticCode->MGCName;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'mitochondrial_genetic_code_name', $mito_genetic_code_name);

            $division = (string) $taxon->Division;
            tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'division', $division);

            $name_ranks = array();
            foreach ($taxon->OtherNames->children() as $child) {
              $type = $child->getName();
              $name = (string) $child;
              if (!array_key_exists($type, $name_ranks)) {
                $name_ranks[$type] = 0;
              }
              switch ($type) {
                case 'GenbankCommonName':
                  tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'genbank_common_name', $name, $name_ranks[$type]);
                  break;
                case 'Synonym':
                  tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'synonym', $name, $name_ranks[$type]);
                  break;
                case 'CommonName':
                case 'Includes':
                  tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'other_name', $name, $name_ranks[$type]);
                  break;
                case 'EquivalentName':
                  tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'equivalent_name', $name, $name_ranks[$type]);
                  break;
                case 'Anamorph':
                  tripal_phylogeny_taxonomy_add_organism_property($organism->organism_id, 'anamorph', $name, $name_ranks[$type]);
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

            // Generate a nested array structure that can be used for importing the tree.
            $parent = (string) $taxon->ParentTaxId;
            $rank = (string) $taxon->Rank;
            $sci_name = (string) $taxon->ScientificName;
            $lineage_depth = preg_split('/;\s*/', $lineage);
            $parent = $tree;
            $i = 1;
            foreach ($taxon->LineageEx->children() as $child) {
              $tid = (string) $child->TaxID;
              $name = (string) $child->ScientificName;
              $node_rank = (string) $child->Rank;
              $node = array(
                'name' => $name,
                'depth' => $i,
                'is_root' => 0,
                'is_leaf' => 0,
                'is_internal' => 1,
                'left_index' => 0,
                'right_index' => 0,
                'parent' => $parent,
                'branch_set' => array(),
                'parent' => $parent['name'],
                'properties' => array(
                  $rank_cvterm->cvterm_id => $node_rank,
                ),
              );
              $parent = $node;
              tripal_phylogeny_taxonomy_import_add_node($tree, $node, $lineage_depth);
              $i++;
            }
            // Now add in the leaf node
            $node = array(
              'name' => $sci_name,
              'depth' => $i,
              'is_root' => 0,
              'is_leaf' => 1,
              'is_internal' => 0,
              'left_index' => 0,
              'right_index' => 0,
              'parent' => $parent['name'],
              'organism_id' => $organism->organism_id,
              'properties' => array(
                $rank_cvterm->cvterm_id => $rank,
              ),
            );
            tripal_phylogeny_taxonomy_import_add_node($tree, $node, $lineage_depth);

            // Set the indecies for the tree.
            tripal_phylogeny_assign_tree_indices($tree);
          } // end: if ($xml) { ...
        } // end: if ($taxid) { ...
      } // end: if ($xml) { ...
    } // end: while ($organism = $organisms->fetchObject()) { ...
    // print json_encode(($tree));

    // Now add the tree
    $options = array('taxonomy' => 1);
    tripal_phylogeny_import_tree($tree, $phylotree, $options);

    // If ther user requested to sync the tree then do it.
    //if ($sync) {
      chado_node_sync_records('phylotree', FALSE, FALSE,
        array(), $ids = array($phylotree->phylotree_id));
    //}
  }
  catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('tripal_phylogeny', $e);
    print "FAILED: Rolling back database changes...\n";
  }
}

/**
 *
 * @param unknown $node
 */
function tripal_phylogeny_taxonomy_import_add_node(&$tree, $node, $lineage_depth) {

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
       if ($branch_set[$j]['name'] == $lineage_depth[$i-1]) {
         $branch_set = &$branch_set[$j]['branch_set'];
         break;
       }
     }
   }
   // Add the node to the last branch set.  This should be where this node goes.
   $branch_set[] = $node;
}

/**
 *
 * @param unknown $organism_id
 * @param unknown $term_name
 * @param unknown $value
 */
function tripal_phylogeny_taxonomy_add_organism_property($organism_id, $term_name, $value, $rank = 0) {
  if (!$value) {
    return;
  }

  $record = array(
    'table' => 'organism',
    'id' => $organism_id
  );
  $property = array(
    'type_name' => $term_name,
    'cv_name' => organism_property,
    'value' => $value
  );
  // Delete all properties of this type if the rank is zero.
  if ($rank == 0) {
    chado_delete_property($record, $property);
  }
  chado_insert_property($record, $property);
}
