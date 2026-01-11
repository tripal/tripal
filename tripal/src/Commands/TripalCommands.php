<?php

namespace Drupal\tripal\Commands;

use Drush\Attributes as CLI;
use Drush\Commands\DrushCommands;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drush\Drush;

/**
 * Drush commands
 */
class TripalCommands extends DrushCommands {

  use StringTranslationTrait;

  /**
   * Makes sure the proper user is set when running the drush command.
   */
  protected function switchUser($uname) {
    if (!$uname) {
      throw new \Exception('The --username argument is required.');
    }

    $user = user_load_by_name($uname);
    if (!$user) {
      throw new \Exception('The --username argument does not specify a valid user.');
    }
    \Drupal::service('account_switcher')->switchTo($user);
  }

  /**
   * Executes one or more jobs in the Tripal Jobs Queue.
   *
   * @command tripal:trp-run-jobs
   * @aliases trp-run-jobs
   * @options parallel
   *   Set to 1 if the job is allowed to run in parallel with other Tripal jobs.
   * @options job_id
   *   The numeric ID of the job. If no job ID is provided then all of the
   *   jobs waiting in the queue will be run.
   * @options max_jobs
   *   The maximum number of jobs that should be run concurrently. If -1 then
   *   unlimited.
   * @options single
   *   Ensures only a single job is run rather then the entire queue.
   * @options username
   *   The name of the user for whom the job run is associated.
   * @usage drush trp-run-jobs --username=[USERNAME]
   *   Executes all jobs waiting in the queue and associates the runs with
   *   the provided user.
   * @usage drush trp-run-jobs --job_id=[JOB_ID] --username=[USERNAME]
   *   Executes a job, using the provided job ID and associates the run with
   *   the provided user.
   */
  public function runJobs($options = ['username' => NULL, 'job_id' => NULL,
    'parallel' => FALSE, 'max_jobs' => -1, 'single' => 0]) {

    $parallel = $options['parallel'];
    $job_id = $options['job_id'];
    $max_jobs = $options['max_jobs'];
    $single = $options['single'];
    $uname = $options['username'];

    $this->switchUser($uname);

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    if ($parallel) {
      $this->output()->writeln("Tripal Job Launcher (in parallel)");
      if ($max_jobs !== -1) {
        $this->output()->writeln("Maximum number of jobs is " . $max_jobs);
      }
      $this->output()->writeln("Running as user '$uname'");
      $this->output()->writeln("-------------------");
      tripal_launch_job($parallel, $job_id, $max_jobs, $single);
    }
    else {
      $this->output()->writeln("Tripal Job Launcher");
      $this->output()->writeln("Running as user '$uname'");
      $this->output()->writeln("-------------------");
      tripal_launch_job(0, $job_id, $max_jobs, $single);
    }
  }

  /**
   * Reruns a jobs in the Tripal Jobs Queue.
   *
   * @command tripal:trp-rerun-job
   * @aliases trp-rerun-job
   * @options parallel
   *   Set to 1 if the job is allowed to run in parallel with other Tripal jobs.
   * @options job_id
   *   The numeric ID of the job. If no job ID is provided then all of the
   *   jobs waiting in the queue will be run.
   * @options max_jobs
   *   The maximum number of jobs that should be run concurrently. If -1 then
   *   unlimited.
   * @options single
   *   Ensures only a single job is run rather then the entire queue.
   * @options username
   *   The name of the user for whom the job run is associated.
   * @usage drush trp-run-job --job_id=[JOB_ID] --username=[USERNAME]
   *   Re-runs a job by first resubmitting it then executing it.
   */
  public function rerunJob($options = ['username' => NULL, 'job_id' => NULL,
    'parallel' => FALSE, 'max_jobs' => -1, 'single' => 0]) {

    $parallel = $options['parallel'];
    $job_id = $options['job_id'];
    $max_jobs = $options['max_jobs'];
    $single = $options['single'];
    $uname = $options['username'];

    if (!$job_id) {
      throw new \Exception('The --job_id argument is required.');
    }

    $this->switchUser($uname);

    $new_job_id = tripal_rerun_job($job_id, FALSE);

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    if ($parallel) {
      $this->output()->writeln("Tripal Job Launcher (in parallel)");
      $this->output()->writeln("Running as user '$username'");
      $this->output()->writeln("-------------------");
      tripal_launch_job($parallel, $new_job_id, $max_jobs, $single);
    }
    else {
      $this->output()->writeln("Tripal Job Launcher");
      $this->output()->writeln("Running as user '$username'");
      $this->output()->writeln("-------------------");
      tripal_launch_job(0, $new_job_id, $max_jobs, $single);
    }
  }

  /**
   * Returns the current version of Tripal that is installed
   *
   * @command tripal:version
   * @aliases trp-version
   * @usage drush trp-version
   *   Returns the current Tripal version string.
   */
  public function tripalVersion() {
    $this->output()->writeln(tripal_version());
  }

  /**
   * Imports publications.
   */
  #[CLI\Command(name: 'tripal:trp-import-pub', aliases: ['trp-import-pub'])]
  #[CLI\Option(name: 'username', description: 'Required, the name of the user for whom the import is associated.')]
  #[CLI\Option(name: 'name', description: 'The name of an existing publication search query.')]
  #[CLI\Option(name: 'id', description: 'The ID number of an existing publication search query.')]
  #[CLI\Option(name: 'pmid', description: 'The PubMed ID of one or more publications to import, comma-delimited.')]
  #[CLI\Option(name: 'schema-name', description: 'pmid only: The name of the chado schema to use (defaults to "chado")')]
  #[CLI\Option(name: 'api-key', description: 'pmid only: Optional NCBI API key for faster requests.')]
  #[CLI\Option(name: 'create-contact', description: 'pmid only: Set to 1 to create contact records for authors (default: 0).')]
  #[CLI\Usage(
    name: 'drush trp-import-pub --name="Query Name" --username=[USERNAME]',
    description: 'Performs the search and imports publications from the search query with the name "Query Name".',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --id=1 --username=[USERNAME]',
    description: 'Performs the search and imports publications from the search query with the record ID of 1 in the tripal_pub_library_query table.',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --pmid=12345678 --username=[USERNAME]',
    description: 'Imports a single publication with PMID 12345678',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --pmid=12345678 --create-contact=1 --api-key=[API_KEY] --username=[USERNAME]',
    description: 'Imports publication with contact creation and API key for faster processing.',
  )]
  public function tripalImportPublication(
    $options = [
      'username' => NULL,
      'name' => NULL,
      'id' => NULL,
      'pmid' => NULL,
      'schema-name' => 'chado',
      'api-key' => NULL,
      'create-contact' => 0,
    ],
  ) {

    $username = $options['username'] ?? NULL;
    $name = $options['name'] ?? NULL;
    $id = $options['id'] ?? NULL;
    $pmid = $options['pmid'] ?? NULL;
    $chado_schema_name = $options['schema-name'] ?? 'chado';
    $api_key = $options['api-key'] ?? NULL;
    $create_contact = $options['create-contact'] ?? 0;

    if (!$username) {
      $this->logger->error($this->t('The --username argument is required.'));
      return;
    }
    if (!$name && !$id && !$pmid) {
      $this->logger->error($this->t('Either the --name, --id, or --pmid argument is required.'));
      return;
    }

    if ($name) {
      $this->tripalImportPublicationByName($name, $chado_schema_name, $username);
    }
    if ($id) {
      $this->tripalImportPublicationById($id, $chado_schema_name, $username);
    }
    if ($pmid) {
      $this->tripalImportPublicationByPmid($pmid, $chado_schema_name, $username, $api_key, $create_contact);
    }
  }

  /**
   * Helper function to import by pub search query name.
   *
   * @param string $name
   *   The name of an existing publication search query.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByName (string $name, string $chado_schema_name, string $username): void {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $all_queries = $pub_library_manager->getSearchQueries();
    $queries = [];
    foreach ($all_queries as $query) {
      if ($query->name == $name) {
        $queries[] = $query;
      }
    }
    if (count($queries) == 0) {
      $this->logger->error($this->t('No pub search query matches the supplied name "@name"',
        ['@name' => $name]));
      return;
    }
    // Note that there is no unique constraint on name, so if more than one
    // matches, then do them all.
    foreach ($queries as $query) {
      $id = $query->pub_library_query_id;
      $this->tripalImportPublicationById($id, $chado_schema_name, $username);
    }
  }

  /**
   * Helper function to import by pub search query ID.
   *
   * @param int $id
   *   Primary key of the query in the tripal_pub_library_query table.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationById (int $id, string $chado_schema_name, string $username): void {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $importer_manager = \Drupal::service('tripal.importer');
    $importer = $importer_manager->createInstance('pub_search_query_loader');

    $ids = explode(',', $id);
    foreach ($ids as $id) {
      $id = trim($id);
      if ($id) {
        $query = $pub_library_manager->getSearchQuery($id);
        if (!$query) {
          $this->logger->error($this->t('No pub search query matches the supplied ID "@id"',
            ['@id' => $id]));
        }
        else {
          $disabled = $query->disabled;
          if ($disabled != 0) {
            $this->logger->error($this->t('Pub search query "@id" is marked as disabled',
              ['@id' => $id]));
            }
          else {
            $criteria = unserialize($query->criteria);
            $do_contact = $query->do_contact;

            // Set up the importer arguments.
            $importer_args = [
              'run_args' => [
                'criteria' => $criteria,
                'schema_name' => $chado_schema_name,
              ],
            ];
            $importer->setArguments($importer_args);

            $this->output()->writeln('CP73 Importing publications');
            $importer->run();
          }
        }
      }
    }
  }

  /**
   * Helper function to import into chado by pubmed ID.
   *
   * @param string $pmid
   *   The PubMed ID of one or more publications to import.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   * @param string $api_key
   *   Optional NCBI API key for faster requests.
   * @param int $create_contact
   *   Set to 1 to create contact records for authors (default: 0).
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByPmid (string $pmid, string $chado_schema_name, string $username, string $api_key, int $create_contact): void {
    $pmids = explode(',', $pmid);
    $criteria = [];
    foreach ($pmids as $index => $id) {
      $id = trim($id);
      if ($id) {
        // Validate that each PMID is numeric.
        if (!is_numeric($id)) {
          $this->logger->error($this->t('A PMID must be a numeric value. Invalid value supplied was "@id"',
            ['@id' => $id]));
          return;
        }
        // Criteria array is expected to start at 1.
        $criteria[$index + 1] = [
          'search_terms' => $id,
          'scope' => 'id',
          'is_phrase' => 0,
          'operation' => $criteria ? 'OR' : '',
        ];
      }
    }

    $this->switchUser($username);

    try {
/**
      // Get services.
      $pub_library_manager = \Drupal::service('tripal.pub_library');

      $this->output()->writeln("Importing publication with PMID: " . $options['pmid'] . " from PubMed...");

      // Create criteria array to search for specific PMID.
      $criteria = [
        'plugin_id' => 'tripal_pub_library_PMID',
        'remote_db' => 'PMID',
        'num_criteria' => 1,
        'count' => 1,
        'page' => 0,
        'do_contact' => $create_contact ? 1 : 0,
        'disabled' => 0,
        'criteria' => $criteria;
          1 => [
            'search_terms' => $id,
            'scope' => 'id',
            'is_phrase' => 0,
            'operation' => '',
          ],
        ],
      ];

      // Add API key if provided.
      if ($options['api-key']) {
        $criteria['ncbi_api_key'] = $options['api-key'];
        $criteria['form_state_user_input']['ncbi_api_key'] = $options['api-key'];
      }

      // Create PubMed plugin instance.
      $plugin = $pub_library_manager->createInstance('tripal_pub_library_PMID', []);

      // Retrieve publication data.
      $this->output()->writeln("Fetching publication data from PubMed...");
      $page_results = $plugin->run($criteria);

      if (!is_array($page_results) || empty($page_results['pubs'])) {
        throw new \Exception("Failed to retrieve publication data for PMID: " . $options['pmid']);
      }

      if ($page_results['total_records'] == 0) {
        $this->output()->writeln("No publication found with PMID: " . $options['pmid']);
        return;
      }

      $publications = $page_results['pubs'];
      $this->output()->writeln("Found publication: " . ($publications[0]['Title'] ?? 'Unknown Title'));
*/

      // Use the importer manager to create the PubSearchQueryImporter instance.
      $importer_manager = \Drupal::service('tripal.importer');
      $importer = $importer_manager->createInstance('pub_search_query_loader');

      // Set up the importer arguments.
      $importer_args = [
        'run_args' => [
          'criteria' => [
            'plugin_id' => 'tripal_pub_library_PMID',
            'remote_db' => 'PMID',
            'num_criteria' => 1,
            'count' => 1,
            'page' => 0,
            'do_contact' => $create_contact ? 1 : 0,
            'disabled' => 0,
            'criteria' => $criteria,
          ],
          'schema_name' => $chado_schema_name,
        ],
      ];
      $importer->setArguments($importer_args);

      $this->output()->writeln("CP31 Importing publication into Chado database...");
      $x = $importer->run();
print "CP5 ";var_dump($x);//@@@

      $this->logger->success($this->t('CP37 Successfully imported publication(s) with PMID: ' . $pmid));

      // Show citation.
#      if (!empty($publications[0]['Citation'])) {
#        $this->output()->writeln("Citation: " . $publications[0]['Citation']);
#      }
    }
    catch (\Exception $e) {
      throw new \Exception("Failed to import publication: " . $e->getMessage());
    }
  }

  /**
   * Imports a collection of Tripal Content Types and associated fields
   * for a specific collection id.
   *
   * @command tripal:trp-import-types
   * @aliases trp-import-types
   * @options collection_id
   *   The id specified in the YAML file for the particular TripalEntityType-Collection
   *   you would like to import. Note: fields will also be added automatically if the
   *   TripalField-Collection YAML file has the same id.
   * @options username
   *   The name of the user for whom the content types created are associated.
   * @usage drush trp-import-types --username=[USERNAME] --collection_id=genomic_chado
   *   Runs a job importing the genomic content types focused on a Chado backend.
   */
  public function tripalImportContentTypes($options = ['username' => NULL, 'collection_id' => NULL]) {

    if (!$options['username']) {
      throw new \Exception('The --username argument is required.');
    }
    if (!$options['collection_id']) {
      throw new \Exception('The --collection_id argument is required.');
    }

    $content_type_setup = \Drupal::service('tripal.tripalentitytype_collection');


    // Check that the id supplied is valid.
    $collections = $content_type_setup->getTypeCollections();
    if (!array_key_exists($options['collection_id'], $collections)) {
      Drush::logger()->notice('The following are the found collection ids:');
      foreach($collections as $id => $details) {
        Drush::logger()->notice('  - ' . $id . ' (' . $details['description'] . ')');
      }
      throw new \Exception('The collection ID you provided was not valid. Please try again with one of the above listed ids (e.g. general_chado).');
    }

    $chosen_collection_ids = [ $options['collection_id'] ];

    // Import the content types
    $content_type_setup->install($chosen_collection_ids);

    // Import the fields.
    $fields = \Drupal::service('tripal.tripalfield_collection');
    $fields->install($chosen_collection_ids);

  }

  /**
   * Ensures the Drupal field table schema is up to date for all fields.
   *
   * @command tripal:trp-sync-field-schema
   */
  public function tripalSyncFieldSchema() {

    $this->output()->writeln("\nChecking Tripal Entity types for discrepancies between field schema definitions and the underlying Drupal tables...\n");

    $columns_added = \Drupal::service('tripal.sync_tripal_field_storage')
      ->resolveDifferences();

    foreach ($columns_added as $field_name => $field_differences) {
      $this->output()->writeln("$field_name needed " . count($field_differences) . " difference(s) fixed.");
    }

    $fields_needing_updates = count($columns_added);
    $num_columns_added = array_sum(array_map("count", $columns_added));
    if ($fields_needing_updates > 0) {
      $this->output()->writeln("\nAdded $num_columns_added columns across $fields_needing_updates fields.\n");
    }
    else {
      $this->output()->writeln("\nNo discrepancies found.\n");
    }
  }
}
