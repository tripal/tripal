<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Link;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Url;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\tripal\TripalImporter\Attribute\TripalImporter;
use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;

#[TripalImporter(
  id: 'pub_search_query_loader',
  label: new TranslatableMarkup('Publication Loader'),
  description: new TranslatableMarkup('Import Publications into Chado using a Publication Search Query'),
  use_analysis: false,
  require_analysis: false,
  button_text: new TranslatableMarkup('Import Publications'),
  file_upload: false,
  file_remote: false,
  file_local: false,
  file_required: false,
  hidden: true,
)]
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

  /**
   * Connection to the Chado schema
   * @var \Drupal\pgsql\Driver\Database\pgsql\Connection $chado
   */
  protected $chado = NULL;

  /**
   * Publication library manager service
   * @var Drupal\tripal_chado\Plugin\TripalImporter\PubSearchQueryImporter $pub_library_manager
   */
  protected $pub_library_manager = NULL;

  /**
   * db_id value from the chado.db table for the external database
   * @var ?int $db_id
   */
  protected $db_id = NULL;

  /**
   * Stores all of the cvterm_id values for the tripal_pub ontology
   * @var array $cvterm_lookups
   *   Key is CV name, value is cvterm_id
   */
  protected $cvterm_lookups = NULL;

  /**
   * Stores a few cached cvterm_id values for the tripal_contact ontology
   * @var array $contact_lookups
   *   Key is CV name, value is cvterm_id
   */
  protected $contact_lookups = [];

  /**
   * Stores information for retrieved publications keyed by the accession
   * @var array $pub_index
   *   First key is accession, second level keys implemented are:
   *     - 'index' - int, the $publications array index
   *     - 'pub_id' - int, the pkey of the chado.pub table
   *     - 'is_new' - boolean, if TRUE we need to insert this as a new publication
   *     - 'dbxref_id' - int, the pkey of the chado.dbxref table
   */
  protected array $pub_index = [];

  /**
   * Stores accessions of new publications not already stored in chado
   * @var array $new_accessions
   */
  protected array $new_accessions = [];

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
      $do_contact_string = ($criteria_column_array['do_contact'] > 0)?t('yes'):t('no');
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

      // Provide a warning if disabled. (Flag is stored as an integer in the table)
      // The submit button is added later so we can't disable it here.
      if ($disabled > 0) {
        \Drupal::messenger()->addWarning(t('This importer has been marked as "Disabled" so cannot be executed'));
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
    if (is_null($this->pub_library_manager)) {
      $this->pub_library_manager = \Drupal::service('tripal.pub_library');
    }
    $plugins = $this->pub_library_manager->getLibraryOptions();
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
        $query = $public->select('tripal_pub_library_query','tpi')
          ->fields('tpi')
          ->condition('pub_library_query_id', $query_id, '=');
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
      if (is_null($this->pub_library_manager)) {
        $this->pub_library_manager = \Drupal::service('tripal.pub_library');
      }
      $pub_record = $this->pub_library_manager->getSearchQuery($query_id);
      if (!$pub_record) {
        $form_state->setErrorByName('search_query_name',
            t('There is no query with an ID value of @id', ['@id' => $query_id]));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    // After submit, we redirect to the "Manage Publication Search Queries" page
    $response = new RedirectResponse('/admin/tripal/loaders/publications/manage_publication_search_queries');
    $response->send();
  }

  /**
   * Retrieves the pkey value from the chado.db table of the specified databse
   *
   * @param string $db_name
   *   The name of the remote database
   * @return void
   *   Stores the returned value in $this->db_id
   * @throw \Exception
   *   Exception if the database name does not exist
   */
  protected function getRemoteDbId(string $db_name) {
    $query = $this->chado->select('1:db', 'DB');
    $query->condition('"DB".name', $db_name, '=');
    $query->addField('DB', 'db_id');
    $db_id = $query->execute()->fetchField();
    // This is an error only a developer would might when creating a new importer
    if (is_null($db_id)) {
      throw new \Exception('Could not find a db_id for this remote database. A db record must exist in the db table that matches the name ' . $db_name);
    }
    $this->db_id = $db_id;
  }

  /**
   * Caches cvterm_id values for all cvterms in the "tripal_pub"
   * vocabulary, saving them in the class variable $this->cvterm_lookups
   */
  protected function cachePublicationCvterms() {
    $query = $this->chado->select('1:cvterm', 'T');
    $query->leftJoin('1:cv', 'CV', '"T".cv_id="CV".cv_id');
    $query->condition('"CV".name', 'tripal_pub', '=');
    $query->fields('T', ['cvterm_id', 'name']);
    $results = $query->execute();
    while ($values = $results->fetchAssoc()) {
      $this->cvterm_lookups[$values['name']] = $values['cvterm_id'];
    }
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
    $this->pub_index = [];
    foreach ($publications as $index => $publication) {
      $accession = $publication['Publication Dbxref'];
      $all_publications_dbxref[] = $accession;
      // Initialize the publication index with defaults
      $this->pub_index[$accession] = [
        'index' => $index,
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
        $this->pub_index[$values['accession']]['is_new'] = FALSE;
        $this->pub_index[$values['accession']]['dbxref_id'] = $values['dbxref_id'];
        $this->pub_index[$values['accession']]['pub_id'] = $values['pub_id'];
        $n_found++;
      }
    }
    // We return the count of how many publication records we will need to add
    return count($all_publications_dbxref) - $n_found;
  }

  /**
   * Inserts new publication accessions into the dbxref table.
   *
   * @param array $publications
   *   All publications returned by the external database
   *
   * @return int
   *   A count of the number of records inserted
   *   Inserted dbxref_id pkey values are stored in $this->pub_index
   */
  protected function insertMissingPublicationsDbxref(array $publications): int {

    // Create the list of new accessions to add
    $this->getNewPublicationAccessions($publications);

    // Perform inserts in batches
    $n_inserted = 0;
    $batches = array_chunk($this->new_accessions, $this->batch_size);
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

      // Store the dbxref keys for linking to pub later
      for ($i=0; $i<count($batch); $i++) {
        $this->pub_index[$batch[$i]]['dbxref_id'] = $first_dbxref_id + $i;
      }
    }
    return $n_inserted;
  }

  /**
   * Extract a list of new publication accessions that need to be imported
   *
   * @param array $publications
   *   The publications returned from the external database
   * @return void
   *   The list is stored at $this->new_accessions
   */
  protected function getNewPublicationAccessions(array $publications): void {
    $this->new_accessions = [];
    // Create the list of new accessions to be imported
    foreach ($this->pub_index as $accession => $info) {
      if ($info['is_new']) {
        $this->new_accessions[] = $accession;
      }
    }
  }

  /**
   * Inserts new publications into the pub table
   *
   * @param array &$publications
   *   All publications returned by the external database
   * @return int
   *   A count of the number of publications inserted
   */
  protected function insertPublications(array &$publications): int {
    $n_inserted = 0;
    foreach ($publications as $index => $publication) {
      $accession = $publication['Publication Dbxref'];
      if ($this->pub_index[$accession]['is_new']) {

        // Assemble the values for the pub table columns
        $title = $publication['Title'];
        $series_name = trim(explode('(', $publication['Journal Name'] ?? '')[0]);
        $pyear = $publication['Year'];
        // Here for the uniquename field in the pub table we use the citation,
        // which should be unique, and we should have already generated it
        // for all importers, but a simple default is provided as a fallback.
        $uniquename = $publication['Citation']
          ?? trim(str_replace(',', ';', $publication['Authors'] ?? '') . ' ' . $title . ' ' . $series_name . '; ' . $pyear);

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
          $this->pub_index[$accession]['pub_id'] = $pub_id;
          $n_inserted++;
          // Adds the property defining the bundle type
          $this->addBundleTypeProperty('pub_id', $pub_id, 'pubprop', 'TPUB', '0000002', 'publication');
        }
        else {
          // If there is no type_id, we cannot process this publication further. This was logged earlier
          unset($publications[$index]);
          unset($this->pub_index[$accession]);
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
   *   If type is not defined in the publication, or if the type is not available in the tripal_pub ontology
   */
  protected function getPublicationTypeId(array $publication): int {
    $type_id = 0;
    // In the event that the publication has no type, e.g. 39755038,
    // then assign a generic 'Publication' type.
    $type = $publication['Publication Type'] ?? 'Publication';
    $accession = $publication['Publication Dbxref'] ?? 'accession_unknown';
    if ($type) {
      if (is_array($type)) {
        // A publication can have more than one type. We can't support
        // that in the pub table, so just return the first one.
        $type = $type[array_key_first($type)];
      }
      $type_id = $this->cvterm_lookups[$type] ?? 0;
      if (!$type_id) {
        $this->logger->warning('Publication with accession @acc has a type "@type" which is not present'
          . ' in the tripal_pub vocabulary, so will not be imported. Consider adding a term for this type.',
          ['@acc' => $accession, '@type' => $type]);
      }
    }
    return $type_id;
  }

  /**
   * Link publications to the database accessions through pub_dbxref table
   */
  protected function insertPubDbxrefs() {
    $n_inserted = 0;

    // Perform inserts in batches
    $n_inserted = 0;
    $batches = array_chunk($this->new_accessions, $this->batch_size);
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
      $insert->execute();
    }
    return $n_inserted;
  }

  /**
   * Add publication properties to the chado.pubprop table
   *
   * @param array $publications
   *   All publications returned by the external database
   * @return int
   *   A count of the number of properties inserted
   */
  protected function insertPubProps(array $publications): int {

    // Preprocess to handle some special cases for properties, e.g. create a URL from the DOI
    $this->specialCaseProps($publications);

    $n_inserted = 0;
    $missing_cvterms = []; // will keep track of keys that do not have cvterms, helpful for continuous debugging

    // Perform inserts in batches
    $batches = array_chunk($this->new_accessions, $this->batch_size);
    foreach ($batches as $batch) {
      $insert = $this->chado->insert('1:pubprop');
      $insert->fields(['pub_id', 'type_id', 'value', 'rank']);
      foreach ($batch as $accession) {
        $n_in_batch = 0;
        $pub_id = $this->pub_index[$accession]['pub_id'];
        $publication = $publications[$this->pub_index[$accession]['index']];
        foreach ($publication as $key => $value) {
          $values = $this->checkIfSupportedProperty($key, $value, $missing_cvterms);
          if ($values) {
            $delta = 0;
            foreach ($values as $value) {
              $insert->values([
                'pub_id' => $pub_id,
                'type_id' => $this->cvterm_lookups[$key],
                'value' => $value,
                'rank' => $delta,
              ]);
              $n_inserted++;
              $n_in_batch++;
              $delta++;
            }
          }
        }
      }
      if ($n_in_batch > 0) {
        $insert->execute();
      }
    }

    // Status message for properties that we did not have a CV term for
    if (count($missing_cvterms) > 0) {
      $this->logger->notice("[!]   Overall missing CVTERMS for this set of publications: " . implode(', ', array_keys($missing_cvterms)) . "\n");
    }

    return $n_inserted;
  }

  /**
   * Special case handling for specific properties
   *
   * @param array &$publications
   *   The array of publications being imported
   */
  protected function specialCaseProps(&$publications) {
    foreach ($publications as $index => $publication) {

      // Case 1. If there is no URL property, but there
      // is a DOI property, then construct a URL property from that.
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
  protected function checkIfSupportedProperty(string $key, string|array $value, array &$missing_cvterms): array {
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
      // Author list is used to create contacts, so although we remove it, we do not report it
      if ($key != 'Author List') {
        $missing_cvterms[$key] = TRUE;
      }
    }
    return $prop_values;
  }

  /**
   * Add authors to all specified publications
   *
   * @param array $publications
   *   Results from the publication external database query
   * @param bool $do_contact
   *   If TRUE, then create and link an entry in the chado.contact table
   * @return int
   *   Count of number of contacts added
   */
  protected function insertContacts(array $publications, bool $do_contact): int {
    $n_added = 0;
    $batches = array_chunk($this->new_accessions, $this->batch_size);
    foreach ($batches as $batch) {
      foreach ($batch as $accession) {
        $publication = $publications[$this->pub_index[$accession]['index']];
        $author_list = $publication['Author List'] ?? NULL;
        if ($author_list) {
          $n_added += $this->addAuthors($accession, $publication, $do_contact);
        }
      }
    }
    return $n_added;
  }

  /**
   * Add one or more authors to a publication through the
   * pubauthor table, and optionally the pubauthor_contact
   * and contact tables
   *
   * @param string $accession
   *   The external database accession for this publication
   * @param array $publication
   *   A single publication query result.
   * @param bool $do_contact
   *   If TRUE, then create and link an entry in the chado.contact table
   * @return int
   *   A count of the number of records added
   * Example PMID with a suffix: 38266644
   * Example PMID that is a collective: 12773082
   */
  protected function addAuthors(string $accession, array $publication, bool $do_contact = FALSE): int {
    $pub_id = $this->pub_index[$accession]['pub_id'];
    $author_list = $publication['Author List'] ?? [];
    $contact_id_list = [];
    $rank = 0;
    $insert = $this->chado->insert('1:pubauthor');
    $insert->fields(['pub_id', 'rank', 'editor', 'surname', 'givennames', 'suffix']);
    foreach ($author_list as $author) {
      if (($author['valid'] ?? 'Y') != 'N') {
        $surname = substr($author['Surname'] ?? '', 0, 100);
        if ($author['Collective'] ?? NULL) {
          if (!$surname) {
            $surname = substr($author['Collective'], 0, 100);
          }
        }
        $insert->values([
          'pub_id' => $pub_id,
          'rank' => $rank,
          'editor' => 0,
          'surname' => $surname,
          'givennames' => substr($author['Given Name'] ?? '', 0, 100),
          'suffix' => substr($author['Suffix'] ?? '', 0, 100),
        ]);
        $rank++;
      }
    }
    // Skip remainder if author list was empty
    if ($rank > 0) {
      $first_pubauthor_id = $insert->execute();
      if ($do_contact) {
        $delta = 0;
        $contact_insert = $this->chado->insert('1:pubauthor_contact');
        $contact_insert->fields(['pubauthor_id', 'contact_id']);
        foreach ($author_list as $author) {
          if (($author['valid'] ?? 'Y') != 'N') {
            $author_type = 'Person';
            $author_full_name = trim(
              ($author['Given Name'] ?? '') .
              ' ' .
              ($author['Surname'] ?? '') .
              ' ' .
              ($author['Suffix'] ?? '')
            );
            if ($author['Collective'] ?? NULL) {
              $author_full_name = $author['Collective'];
              $author_type = 'Collective';
            }
            $contact_id = $this->getContact($author_full_name, $author_type);
            $contact_insert->values([
              'pubauthor_id' => $first_pubauthor_id + $delta,
              'contact_id' => $contact_id,
            ]);
            $delta++;
          }
        }
        $contact_insert->execute();
      }
    }

    return $rank;
  }

  /**
   * Gets the contact_id of a person, will create the contact if it does not already exist
   *
   * @param string $contact_name
   *   The name of a contact
   * @param string $type
   *   The type of contact in the tripal_contact ontology, most commonly 'Person'
   * @return int
   *   The contact_id value
   */
  protected function getContact(string $contact_name, string $type): int {
    $type_id = $this->getContactType($type);
    $query = $this->chado->select('1:contact', 'C');
    $query->condition('"C".name', $contact_name, 'ILIKE');
    $query->condition('"C".type_id', $type_id, '=');
    $query->addField('C', 'contact_id');
    $contact_id = $query->execute()->fetchField();
    if (!$contact_id) {
      $insert = $this->chado->insert('1:contact');
      $insert->fields([
        'name' => $contact_name,
        'description' => '',
        'type_id' => $type_id,
      ]);
      $contact_id = $insert->execute();
    }
    return $contact_id;
  }

  /**
   * Gets a cvterm_id from the tripal_contact ontology.
   *
   * @param string $type_name
   *   Corresponds to the 'name' column of the cvterm table
   * @return int
   *   Corresponds to the 'cvterm_id' column of the cvterm table.
   */
  protected function getContactType(string $type_name): int {
    $cvterm_id = $this->contact_lookups[$type_name] ?? NULL;
    if (!$cvterm_id) {
      $query = $this->chado->select('1:cvterm', 'T');
      $query->leftJoin('1:cv', 'CV', '"T".cv_id="CV".cv_id');
      $query->condition('T.name', $type_name, '=');
      $query->condition('CV.name', 'tripal_contact', '=');
      $query->addField('T', 'cvterm_id');
      $cvterm_id = $query->execute()->fetchField();
      // Cache even if no match
      $this->contact_lookups[$type_name] = $cvterm_id;
    }
    return $cvterm_id;
  }

  /**
   * Handles initialization for the run() function
   *
   * @return array|NULL
   *   The criteria array, or NULL if an error occurred
   */
  protected function run_init(): ?array {

    $this->logger->notice('Initializing publication importer');

    // Initialize services
    $this->chado = $this->getChadoConnection();
    $this->pub_library_manager = \Drupal::service('tripal.pub_library');

    // We can pass a query_id value, in which case we retrieve a criteria
    // array from the public.tripal_pub_library_query table. Alternatively we
    // can pass a criteria array directly.
    $arguments = $this->arguments['run_args'];
    $criteria = [];
    if ($arguments['criteria'] ?? NULL) {
      $criteria = $arguments['criteria'];
    }
    else {
      $query_id = $arguments['query_id'] ?? NULL;
      if (!$query_id) {
        // This will extract the query id from the query name selected from the autocomplete field
        $search_query_name = $arguments['search_query_name'] ?? '';
        if (preg_match('/\((\d+)\)/', $search_query_name, $matches)) {
          $query_id = $matches[1];
        }
      }
      if (!$query_id) {
        $this->logger->error('A query ID was not supplied, cannot continue');
        return NULL;
      }

      // Use the query_id to retrieve the query information from the database
      $pub_record = $this->pub_library_manager->getSearchQuery($query_id);
      if (!$pub_record) {
        $this->logger->error('There is no search query defined for the supplied identifier "'. $query_id . '"');
        return NULL;
      }
      $criteria = unserialize($pub_record->criteria);
    }
    if (!($criteria['plugin_id'] ?? NULL)) {
      $plugin_id = $criteria['form_state_user_input']['plugin_id'] ?? NULL;
      if (is_null($plugin_id)) {
        $this->logger->error('Could not find the plugin_id, could not find adequate query information');
        return NULL;
      }
      $criteria['plugin_id'] = $plugin_id;
    }
    if (($criteria['disabled'] ?? 0) > 0) {
      $this->logger->error('This query cannot be executed because it is marked as "Disabled"');
      return NULL;
    }

    // Lookup the db_id for the external database for the plugin
    $this->getRemoteDbId($criteria['remote_db']);

    // Preload all tripal_pub ontology terms to $this->cvterm_lookups
    $this->cachePublicationCvterms();

    return $criteria;
  }

  /**
   * @see TripalImporter::run()
   *
   * n.b. the calling function will wrap this in a database transaction
   */
  public function run() {

    $criteria = $this->run_init();
    if (is_null($criteria)) {
      return;
    }

    // This is stored as an integer in the database table, this converts it to a boolean
    $do_contact = (($criteria['do_contact'] ?? 0) > 0);

    // Set up a loop to load publications in batches
    $plugin_id = $criteria['plugin_id'];
    $plugin = $this->pub_library_manager->createInstance($plugin_id, []);
    $page = 0;
    $n_groups = '?';
    $completed = FALSE;
    $prefix = '';

    while (!$completed) {

      // Use the appropriate plugin to run the query and process the results, in groups of 100
      $this->logger->notice($prefix . 'Step 1 of 7: Retrieving publication data from remote database ' . $plugin_id);
      $criteria['page'] = $page;
      $criteria['count'] = $this->batch_size;
      $page_results = $plugin->run($criteria);

      // NULL indicates exception caught
      if (!is_array($page_results)) {
        $this->logger->error('  ✗ ERROR: Unable to connect to external database to lookup publications');
        $completed = TRUE;
      }
      elseif ($page_results['total_records'] == 0) {
        $this->logger->notice('  🗸 No records matched the search criteria');
        $completed = TRUE;
      }
      else {
        $publications = $page_results['pubs'];
        $total_records = $page_results['total_records'];
        $n_groups = intval(($total_records - 1) / $this->batch_size) + 1;
        if ($n_groups > 1) {
          $prefix = 'Group ' . ($page+1) . ' of ' . $n_groups . ', ';
          if ($page == 0) {
            $this->logger->notice('  🗸 @total publications will be imported in @group groups of @size publications each',
              ['@total' => number_format($total_records), '@group' => $n_groups, '@size' => $this->batch_size]);
          }
        }
        $this->logger->notice('  🗸 Importing @count publications', ['@count' => count($publications)]);

        // Determine the number of new publications to be inserted
        $n_to_insert = 0;
        if (count($publications)) {
          $this->logger->notice($prefix . 'Step 2 of 7: Check for already imported publications');
          $n_to_insert = $this->findExistingPublications($publications);
          $this->logger->notice('  🗸 New publications to be inserted: ' . $n_to_insert);
        }

        // If no new publications, then nothing to do
        if (!$n_to_insert) {
          $this->logger->notice('  ✗ No new publications were found, there is nothing to import');
        }
        else {
          $this->logger->notice($prefix . 'Step 3 of 7: Insert new publications');
          $n_inserted = $this->insertPublications($publications);
          $this->logger->notice('  🗸 Inserted: ' . $n_inserted);

          $this->logger->notice($prefix . 'Step 4 of 7: Insert database crossreferences');
          $n_inserted = $this->insertMissingPublicationsDbxref($publications);
          $this->logger->notice('  🗸 Inserted: ' . $n_inserted);

          $this->logger->notice($prefix . 'Step 5 of 7: Link publications to database crossreferences');
          $n_inserted = $this->insertPubDbxrefs();
          $this->logger->notice('  🗸 Inserted: ' . $n_inserted);

          $this->logger->notice($prefix . 'Step 6 of 7: Insert publication properties');
          $n_inserted = $this->insertPubProps($publications);
          $this->logger->notice('  🗸 Inserted: ' . $n_inserted);

          $this->logger->notice($prefix . 'Step 7 of 7: Insert authors');
          $n_added = $this->insertContacts($publications, $do_contact);
          $this->logger->notice('  🗸 Inserted: ' . $n_added);
        }
      }
      // n.b. $page starts at 0 so last page is $n_groups-1
      $page++;
      if ($page == $n_groups) {
        $completed = TRUE;
      }
      else {
        $prefix = 'Group ' . ($page+1) . ' of ' . $n_groups . ', ';
      }
    }
    return;
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {
  }

}
