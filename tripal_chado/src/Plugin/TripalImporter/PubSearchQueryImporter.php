<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Symfony\Component\HttpFoundation\RedirectResponse;


/**
 * Chado Pub Search Query Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "pub_search_query_loader",
 *    label = @Translation("Publication Loader"),
 *    description = @Translation("Import Publications into Chado using a Publication Search Query"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import Publications"),
 *    file_upload = False,
 *    file_remote = False,
 *    file_local = False,
 *    file_required = False,
 *    hidden = True,
 *  )
 */
class PubSearchQueryImporter extends ChadoImporterBase {

  // Public connection
  private $public = NULL;
  // Chado connection
  private $chado = NULL;
  private $db_id = NULL;
  private $cvterm_lookups = NULL;

  /**
   * Stores pub_id for inserted accession
   * @var array $pub_index
   *   First key is accession, second level keys implemented are:
   *     - pub_id
   *     - is_new
   *     - dbxref_id
   */
  protected array $pub_index = [];

  /**
   * Stores the maximum size of database query batches
   * @var int $batch_size
   */
  protected int $batch_size = 100;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    // Always call the parent form to ensure selection of Chado schema is handled properly.
    $form = parent::form($form, $form_state);

    $query_id = "";
    $build_args = $form_state->getBuildInfo();

    if (!is_null($build_args['args'][1])) {
      $query_id = $build_args['args'][1];
    }

    $form['query_id'] = [
        '#title' => t('Query ID'),
        '#type' => 'hidden',
        '#required' => TRUE,
        '#value' => $query_id,
        '#description' => t("Required to import the publications based on query id"),
    ];

    // If query_id is unset, we need to display library options and an autocomplete for the search query
    if ($query_id == "") {
      $this->formQueryIdNotSet($form, $form_state);
    }

    // If the query id is set, display the data
    if (!is_null($build_args['args'][1])) {
      $public = \Drupal::service('database');
      $row = $public->select('tripal_pub_library_query', 'tpi')
        ->fields('tpi')
        ->condition('pub_library_query_id', $query_id, '=')
        ->execute()->fetchObject();
      $criteria_column_array = unserialize($row->criteria);
      // Get search string from the criteria data
      $search_string = '';
      foreach ($criteria_column_array['criteria'] as $criteria_row) {
        $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
      }
      // Get the database from the criteria data
      $db_string = $criteria_column_array['remote_db'];
      $do_contact_string = $criteria_column_array['do_contact']?t('yes'):t('no');
      $disabled = $criteria_column_array['disabled'];
      $markup = '<h4>Search Query Details</h4>';
      $markup .= '<ul>';
      $markup .= '<li>Name: ' . $row->name . '</li>';
      $markup .= '<li>Database: ' . $db_string . '</li>';
      $markup .= '<li>Search string: ' . $search_string . '</li>';
      if (array_key_exists('days', $criteria_column_array)) {
        $markup .= '<li>Days since record modified: ' . $criteria_column_array['days'] . '</li>';
      }
      $markup .= '<li>Create contact: ' . $do_contact_string . '</li>';
      $markup .= '</ul>';
      $form['query_info'] = [
        '#markup' => $markup
      ];

      // We don't actually enforce the disabled status, and apparently Tripal 3
      // ignored this field, but at least provide a warning.
      if ($disabled) {
        \Drupal::messenger()->addWarning(t('This importer has been marked as "Disabled"'));
      }

    }

    return $form;
  }

  /**
   * Helper function for form(), code to handle the case
   * where the query ID is not yet set.
   *
   * @param array &$form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  protected function formQueryIdNotSet(&$form, $form_state) {
    // Get list of database/libraries
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $plugins = $pub_library_manager->getLibraryOptions();
    $form_state_values = $form_state->getValues();

    $form['database'] = [
      '#title' => t('Database'),
      '#type' => 'select',
      '#required' => FALSE,
      '#options' => $plugins,
      '#empty_option' => t('- Select -'),
      '#description' => 'Select the database of the search query to'
        . ' limit "Search query name" to only queries for that database.',
      '#ajax' => [
        'callback' =>  [$this::class, 'database_on_change'],
        'wrapper' => 'edit-output',
      ],
    ];

    $url = Link::fromTextAndUrl('create new or edit existing search queries.',
        Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries'))->toString();
    $form['search_query_name'] = [
      '#title' => t('Existing search query name'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#autocomplete_path' => 'admin/tripal/autocomplete/pubsearchqueryname',
      '#autocomplete_route_name' => 'tripal.pubsearchqueryname_autocomplete',
      '#autocomplete_query_parameters' => ['db' => '*'],
      '#description' => t('Enter the name of an existing search query. You can also ') . $url,
      '#prefix' => '<div id="edit-search-query-name">',
      '#suffix' => '</div>',
    ];

    // In 2024 we only have one database option, make it the default.
    // We can remove this section later if we create a second importer.
    if (count($plugins) == 1) {
      $form['database']['#default_option'] = array_key_first($plugins);
      unset($form['database']['#empty_option']);
      $form['search_query_name']['#autocomplete_query_parameters']['db'] = array_key_first($plugins);
    }

    $form['button_view_query_details'] = [
      '#type' => 'button',
      '#button_type' => 'button',
      '#value' => 'Preview query details'
    ];

    if (isset($form_state_values['op'])) {
      $op = $form_state_values['op'];
      if ($op == 'Preview query details') {
        $query_id = -1;
        if ($form_state_values['query_id'] != '') {
          $query_id = $form_state_values['query_id'];
        }
        else {
          $search_query_name = $form_state_values['search_query_name'] ?? '';
          if (preg_match('/\((\d+)\)/', $search_query_name, $matches)) {
            $query_id = $matches[1];
          }
        }
        $headers = [
          'Importer Name',
          'Database',
          'Search String',
          'Disabled',
          'Create Contact',
        ];
        $form['pub_query_details'] = [
          '#type' => 'table',
          '#header' => $headers,
          '#prefix' => '<div id="pub_manager_table">',
          '#suffix' => '</div>',
        ];

        $public = \Drupal::database();
        $query = $public->select('tripal_pub_library_query','tpi')->fields('tpi')->condition('pub_library_query_id', $query_id, '=');
        $results = $query->execute();
        foreach ($results as $pub_query) {
          $criteria_column_array = unserialize($pub_query->criteria);

          $search_string = "";
          foreach ($criteria_column_array['criteria'] as $criteria_row) {
            $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
          }

          $disabled = ($criteria_column_array['disabled'] <= 0) ? 'No' : 'Yes';

          $do_contact = ($criteria_column_array['do_contact'] <= 0) ? 'No' : 'Yes';

          $row = [];

          // This should contain edit test and import pubs links @TODO

          $row['col-1'] = [
            '#markup' => $pub_query->name
          ];
          $row['col-2'] = [
            '#markup' => $criteria_column_array['remote_db']
          ];

          // Search string
          $row['col-3'] = [
            '#markup' => $search_string
          ];

          // Disabled
          $row['col-4'] = [
            '#markup' => $disabled
          ];

          // Create contact
          $row['col-5'] = [
            '#markup' => $do_contact
          ];

          $form['pub_query_details'][] = $row;
        }
      }
    }
  }

  public static function test_click_on_change(array &$form, $form_state) {
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#pub-query-details', 'WOW'));
    return $response;
  }

  public static function database_on_change(array &$form, $form_state) {
    $user_input = $form_state->getUserInput();

    // database / library value when changed
    $database = $user_input['database'];
    $response = new AjaxResponse();

    // This adjusts the autocomplete path for search query name
    $autocomplete_path = $form['search_query_name']['#autocomplete_path'];
    $autocomplete_path_parts = explode('db=', $autocomplete_path);
    $autocomplete_path = base_path() . $autocomplete_path_parts[0]. '?db=' . $database;
    $response->addCommand(new InvokeCommand('#edit-search-query-name input', 'attr', ['data-autocomplete-path', $autocomplete_path]));


    return $response;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $form_state_values = $form_state->getValues();
    $query_id = $form_state_values['query_id'] ?? NULL;
    if (!$query_id) {
      $search_query_name = $form_state_values['search_query_name'] ?? '';
      // This will extract the query id from the query name selected from the autocomplete field
      if (preg_match('/\((\d+)\)/', $search_query_name, $matches)) {
        $query_id = $matches[1];
      }
    }
    if (!$query_id) {
      $form_state->setErrorByName('search_query_name',
          t('The query name must include its ID value in parentheses'));
    }
    else {
      $pub_library_manager = \Drupal::service('tripal.pub_library');
      $pub_record = $pub_library_manager->getSearchQuery($query_id);
      if (!$pub_record) {
        $form_state->setErrorByName('search_query_name',
            t('There is no query with an ID value of @id', ['@id' => $query_id]));
      }
    }
  }

  /**
   * Retrieves the database pkey value
   *
   * @param string $db_name
   *   The name of the remote database
   * @return void
   *   Stores the returned value in $this->db_id
   */
  protected function getRemoteDbId(string $db_name) {
    $query = $this->chado->query('1:db', 'DB');
    $query->condition('"DB".name', $db_name, '=');
    $query->addField('DB', 'name');
    $db_id = $query-execute()->fetchField();
    if (is_null($db_id)) {
      throw new \Exception('Could not find a db_id for this remote database. A db record must exist in the db table that matches the name ' . $db_name);
    }
print "CP01 db_id=$db_id\n";//@@@
    $this->db_id = $db_id;
  }

  /**
   * Caches cvterm_id values for all cvterms in the "tripal_pub"
   * vocabulary in the class variable $this->cvterm_lookups
   */
  function cachePublicationCvterms() {
    $sql = 'SELECT T.cvterm_id, T.name FROM {1:cvterm} T WHERE T.cv_id ='
         . ' (SELECT cv_id FROM {1:cv} WHERE name = :name)';
    $args = [
      ':name' => 'tripal_pub',
    ];
    $result = $this->chado->query($sql, $args);
    foreach ($result as $row) {
      $cvterm_id = $row->cvterm_id;
      $cvterm_name = $row->name;
      $this->cvterm_lookups[$cvterm_name] = $cvterm_id;
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {
    $this->public = \Drupal::database();
    $public = $this->public;
    $arguments = $this->arguments['run_args'];

    $query_id = NULL;
    if (isset($arguments['query_id']) and !empty($arguments['query_id'])) {
      $query_id = $arguments['query_id'];
    }
    else {
      $search_query_name = $arguments['search_query_name'];

      // This will extract the query id from the query name selected from the autocomplete field
      if (preg_match('/\((\d+)\)/', $search_query_name, $matches)) {
        $query_id = $matches[1];
      }
    }

    // Retrieve plugin_id from the database
    $criteria = NULL;
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_record = $pub_library_manager->getSearchQuery($query_id);

    $criteria = unserialize($pub_record->criteria);
    $plugin_id = $criteria['form_state_user_input']['plugin_id'] ?? NULL;

    if (is_null($criteria) || is_null($plugin_id)) {
      $this->logger->error('Could not find criteria or plugin_id, could not find adequate query information');
      return;
    }

    if ($criteria['disabled']) {
      $this->logger->error('This query cannot be executed because it is marked as "Disabled"');
      return;
    }
    $do_contact = $criteria['do_contact'];
    $n_steps = $do_contact?9:8;

    // Initialize chado variable (used in other helper functions within this class)
    $this->chado = $this->getChadoConnection();
    $this->logger->notice("Step 1 of $n_steps: Find db_id for remote database (table: db) ...");
    $this->getRemoteDbId();
    $this->logger->notice("               🗸 Found db_id: " . $this->db_id);

    $this->logger->notice("Step 2 of $n_steps: CVTERMs lookup and caching ...            ");
    $this->cachePublicationCvterms();
    $this->logger->notice("               🗸 Cached cvterms: " . count($this->cvterm_lookups));

    // Run a query to the remote database and return publications in an array
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $plugin = $pub_library_manager->createInstance($plugin_id, []);
    $this->logger->notice("Step 3 of $n_steps: Retrieving publication data from remote database ...");
    $publications = $plugin->run($query_id);
    if (!is_array($publications)) {
      $this->logger->error("               🗸 ERROR: Unable to connect to NCBI to lookup publications!");
      return FALSE;
    }
    $this->logger->notice("               🗸 Found publications: " . count($publications));

    // Now we may make changes, so start a transaction
    $transaction_chado = $this->chado->startTransaction();
    try {

      $this->logger->notice("Step 4 of $n_steps: Check for already imported publications ...         ");
      $n_to_insert = $this->findExistingPublications($publications);
      $this->logger->notice("               🗸 Missing publications to be inserted: " . $n_to_insert);

      // Insert missingPublicationsDbxref
      $this->logger->notice("Step 5 of $n_steps: Insert new publication dbxrefs ...                ");
      $n_inserted = $this->insertMissingPublicationsDbxref();
      $this->logger->notice("               🗸 Inserted: " . $n_inserted);

      $this->logger->notice("Step 6 of $n_steps: Insert new publications ...                       ");
      $n_inserted = $this->insertPublications($publications);
      $this->logger->notice("               🗸 Inserted: " . $n_inserted);

      $this->logger->notice("Step 7 of $n_steps: Insert new pub_dbxrefs ...                        ");
      $inserted_pub_dbxref_ids = [];
      $inserted_pub_dbxref_ids = $this->insertPubDbxrefs();
      $this->logger->notice("               🗸 Inserted: " . count($inserted_pub_dbxref_ids));

      $this->logger->notice("Step 8 of $n_steps: Insert new publications properties ...            ");
      $pub_props_count = $this->insertPubProps($publications);
      $this->logger->notice("               🗸 Inserted: " . $pub_props_count);

      if ($do_contact) {
        $this->logger->notice("Step 9 of $n_steps: Insert author contacts ...");
print "CP78 missing_publications_dbxref "; print_r($missing_publications_dbxref); //@@@
        $n_added = $this->insertContacts($missing_publications_dbxref, $publications);
        $this->logger->notice("               🗸 Inserted: " . $n_added);
      }
    }
    catch (\Exception $e) {
      $transaction_chado->rollback();
      throw $e;
    }

  }

  function insertPubProps($inserted_pub_ids, $missing_publications_dbxref, &$publications) {
    // Handle some special cases for properties, e.g. create a URL from the DOI
    $this->specialCaseProps($publications);

    $init_sql = "INSERT INTO {1:pubprop} (pub_id, type_id, value, rank) ";
    $init_sql .= "VALUES \n";
    $i = 0;
    $total = 0;
    $prop_count = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $missing_cvterms = []; // will keep track of keys that do not have cvterms, helpful for continuous debugging

    foreach ($publications as $publication) {
      // Get pub id from inserted_pub_ids
      $pub_id = $inserted_pub_ids[$total];
      $total++;

      // Go through each publication array keys => values
      foreach ($publication as $key => $value) {
        $values = $this->checkIfSupportedProperty($key, $value, $missing_cvterms);
        if ($values) {
          $delta = 0;
          foreach ($values as $value) {
            $i++;
            $prop_count++; // keep count of inserted prop (return this just for details)
            $sql .= " (:pub_id_$i, :type_id_$i, :value_$i, :rank_$i), ";
            $args[":pub_id_$i"] = $pub_id;
            $args[":type_id_$i"] = $this->cvterm_lookups[$key];
            $args[":value_$i"] = $value;
            $args[":rank_$i"] = $delta;
            $delta++;
          }

          if ($i >= $this->batch_size) {
            $sql = rtrim($sql, ", ");
            $sql = $init_sql . $sql;
            $this->chado->query($sql, $args);

            $batch_num++;
            // Now reset all of the variables for the next batch.
            $sql = '';
            $i = 0;
            $args = [];
          }
        }
      }

    }
    if ($sql != '') {
      $sql = rtrim($sql, ", ");
      $sql = $init_sql . $sql;
      $this->chado->query($sql, $args);
    }

    if (count($missing_cvterms) > 0) {
      $this->logger->notice("[!]   Overall missing CVTERMS for this set of publications: " . implode(', ', array_keys($missing_cvterms)) . "\n");
    }

    return $prop_count;
  }
  /**
   * Special case handling for specific properties
   *
   * @param array &$publications
   *   The array of publications to be imported
   */
  protected function specialCaseProps(&$publications) {
    foreach ($publications as $index => $publication) {

      // Case 1. If there is no URL property, but there
      // is a DOI property, then construct a URL from that.
      if (!array_key_exists('URL', $publication)) {
        if (array_key_exists('DOI', $publication)) {
          $publications[$index]['URL'] =  'https://doi.org/' . $publication['DOI'];
        }
      }

    }
  }

  /**
   * Helper function for insertPubProps to determine if the
   * property CV term is supported.
   *
   * @param string $key
   *   The property key, which is a CV term.
   * @param string|array $value
   *   The property value or values.
   * @param array &$missing_cvterms
   *   Array keys define a list of non-supported CV terms.
   *
   * @return array
   *   An array of values to be saved, or NULL if the $key is not supported.
   */
  private function checkIfSupportedProperty(string $key, string|array $value, array &$missing_cvterms): array {
    $prop_values = [];
    if (isset($this->cvterm_lookups[$key])) {
      if (is_array($value)) {
        $prop_values = $value;
      }
      else {
        $prop_values[] = $value;
      }
    }
    else {
      // Author list is used to create contacts, so we allow even though it has no CV term
      if ($key != 'Author List') {
        $missing_cvterms[$key] = TRUE;
      }
    }
    return $prop_values;
  }

  /**
   * Link publications to the database accessions
   */
  function insertPubDbxrefs(
#@@@    $inserted_pub_ids, $inserted_dbxref_ids
) {
    $n_inserted = 0;

    // Create the list of new accessions to add
    $new_publications_dbxref = [];
    foreach ($this->pub_index as $accession => $info) {
      if ($info['is_new']) {
        $new_publications_dbxref[] = $accession;
      }
    }

    // Perform inserts in batches
    $n_inserted = 0;
    $batches = array_chunk($new_publications_dbxref, $this->batch_size);
    foreach ($batches as $batch) {
      $insert = $this->chado->insert('1:pub_dbxref');
      $insert->fields(['pub_id', 'dbxref_id']);
      foreach ($batch as $accession) {
        $dbxref_id = $this->pub_index[$accession]['dbxref_id'];
        $pub_id = $this->pub_index[$accession]['pub_id'];
        $insert->values([
          'pub_id' => $pub_id,
          'dbxref_id' => $dbxref_id,
        ]);
        $n_inserted++;
      }
    }
    return $n_inserted;
  }

  /**
   * Add authors to all specified publications
   *
   * @param array $missing_publications_dbxref
   *   A list of publication accessions that were inserted
   * @param array $publications
   *   Results from the publication external database query
   * @return int
   *   Count of number of contacts added
   */
  protected function insertContacts(array $missing_publications_dbxref, $publications): int {
    $n_added = 0;
print "CP39 insertContacts\n";//@@@
    foreach ($publications as $publication) {
      $accession = $publication['Publication Dbxref'];
      if (in_array($accession, $missing_publications_dbxref)) {
print "CP40\n";//@@@
#print "CP40 publication="; print_r($publication); //@@@
        $n_added += $this->addAuthors($publication);
      }
    }
    return $n_added;
  }

  /**
   * Add one or more authors to a publication
   *
   * @param array $publication
   *   A single publication query result.
   * @return int
   *   A count of the number of records added
   */
  protected function addAuthors(array $publication): int {
    $author_list = $publication['Author List'] ?? [];
    $rank = 0;
print "CP41 authors="; print_r($author_list);//@@@
    // First remove any of the existing pubauthor entries.
    return 0;//@@@
  }

  /**
   * Inserts new publications into the pub table
   *
   * @param array $publications
   *   All publications returned by the external database
   * @return int
   *   A count of the number of publications inserted
   */
  function insertPublications(array $publications): int {
    $n_inserted = 0;
    foreach ($publications as $publication) {
      $accession = $publication['Publication Dbxref'];
      if ($this->pub_index[$accession]['is_new']) {

        // Assemble the values for the pub table columns
        $title = $publication['Title'];
        $series_name = trim(explode('(', $publication['Journal Name'])[0]);
        $pyear = $publication['Year'];
        // Here for the uniquename field in the pub table we use the citation,
        // which should be unique, and we should have already generated it for
        // for all importers, but a simple default is provided as a fallback.
        $uniquename = $publication['Citation']
          ?? str_replace(',', ';', @$publication['Authors']) . ' ' . $title . ' ' . $series_name . '; ' . $pyear;
        $type_id = $this->getPublicationTypeId($publication);

        if ($type_id) {
          $insert = $this->chado->insert('1:pub');
          $insert->fields([
            'title' => $title,
            'series_name' => $series_name,
            'pyear' => $pyear,
            'uniquename' => $uniquename,
            'type_id' => $type_id,
          ]);
          $pub_id = $insert->execute();
print "CP93 pub_id inserted=$pub_id\n";//@@@
          $this->pub_index[$accession]['pub_id'] = $pub_id;
          $n_inserted++;
        }
      }
    }
    return $n_inserted;
  }

  /**
   * Get the cvterm_id for the publication type
   *
   * @param array $publication
   *   One publication record returned by the external database
   * @return int
   *   The corresponding cvterm_id value
   * @throw \Exception
   *   If term is not defined, or if it is not available in the ontology
   */
  protected function getPublicationTypeId(array $publication): int {
    $type_id = 0;
    $type = $publication['Publication Type'] ?? NULL;
    if ($type) {
      if (is_array($type)) {
        // A publication can have more than one type. We can't support
        // that in the pub table, so just return the first one.
        $type = $type[array_key_first($type)];
      }
      $type_id = $this->cvterm_lookups[$type] ?? 0;
      // @todo change to just issue warning so we can skip over this publication
      if (!$type_id) {
        throw new \Exception('Type ID for Publication Type: ' . $type . ' is not present in the tripal_pub vocabulary');
      }
    }
    else {
      throw new \Exception('Publication is missing a type: ' . print_r($publication, TRUE));
    }
    return $type_id;
  }

  /**
   * Inserts new publication accessions into the dbxref table.
   *
   * Inserted dbxref_id pkey values are stored in $this->pub_index
   *
   * @return int
   *   A count of the number of records inserted
   */
  protected function insertMissingPublicationsDbxref() {

    // Create the list of new accessions to add
    $new_publications_dbxref = [];
    foreach ($this->pub_index as $accession => $info) {
      if ($info['is_new']) {
        $new_publications_dbxref[] = $accession;
      }
    }

    // Perform inserts in batches
    $n_inserted = 0;
    $batches = array_chunk($new_publications_dbxref, $this->batch_size);
    foreach ($batches as $batch) {
      $insert = $this->chado->insert('1:dbxref');
      $insert->fields(['db_id', 'accession', 'version']);
      foreach ($batch as $accession) {
        $insert->values([
          'db_id' => $this->db_id,
          'accession' => $accession,
          'version' => ''
        ]);
        $n_inserted++;
      }
      $first_dbxref_id = $insert->execute();
print "CP51 first dbxref id=$first_dbxref_id\n"; //@@@

      // Store the dbxref keys for linking to pub later
      for ($i=0; $i<count($batch); $i++) {
        $this->pub_index[$batch[$i]]['dbxref_id'] = $first_dbxref_id + $i;
      }
    }
    return $n_inserted;
  }


  /**
   * Finds publication accessions that already are stored in Chado.
   *
   * Results are stored in $this->pub_index
   *
   * @param array $publications
   *   Publications loaded from the external database
   * @return int
   *   The number of publications not currently stored in Chado
   */
  public function findExistingPublications(array $publications) {

    // Get all publication accessions
    $all_publications_dbxref = [];
    foreach ($publications as $publication) {
      $accession = $publication['Publication Dbxref'];
      $all_publications_dbxref[] = $accession;
      // Initialize the publication index with defaults
      $this->pub_index[$accession] = [
        'is_new' => TRUE,
        'dbxref_id' => NULL,
        'pub_id' => NULL,
      ];
    }

    // Query for existing records in batches
    $n_found = 0;
    $batches = array_chunk($all_publications_dbxref, $this->batch_size);
    foreach ($batches as $batch) {
      $select = $this->chado->select('1:pub', 'P');
      $select->leftJoin('1:pub_dbxref', 'PX', '"P".pub_id="PX".pub_id');
      $select->leftJoin('1:dbxref', 'X', '"PX".dbxref_id="X".dbxref_id');
      $select->addField('P', 'pub_id', 'pub_id');
      $select->addField('X', 'dbxref_id', 'dbxref_id');
      $select->addField('X', 'accession', 'accession');
      $select->condition('X.accession', $batch, 'IN');
      $results = $select->execute();
      while ($values = $results->fetchAssoc()) {
        $this->pub_index[$values['accession']] = [
          'is_new' => FALSE,
          'dbxref_id' => $values['dbxref_id'],
          'pub_id' => $values['pub_id'],
        ];
        $n_found++;
      }
    }
    // We return the count of how many publication records we will need to add
    return count($all_publications_dbxref) - $n_found;
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
    // After submit, we redirect to the "Manage Publication Search Queries" page
    $response = new RedirectResponse('/admin/tripal/loaders/publications/manage_publication_search_queries');
    $response->send();
  }
}
