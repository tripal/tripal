<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\field\Entity\FieldStorageConfig;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests the publish service for chado-based content types.
 *
 * @group TripalBackendPublish
 * @group ChadoPublish
 */
#[Group('tripal-publish')]
#[Group('chado-publish')]
class ChadoPublishTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'tripal_chado', 'views', 'field'];

  protected $connection;

  /**
   * Connection to the drupal database.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $public;

  protected $chado_publish;

  /**
   * The most recent error message from the mocked tripal logger.
   *
   * @var string
   */
  protected string $mock_warning = '';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a mocked logger so we can access error messages from the Tripal logger
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['warning'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function($message, $context, $options) {
          $this->mock_warning .= str_replace(array_keys($context), $context, $message);
          return NULL;
        });
    $container->set('tripal.logger', $mock_logger);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm','TripalEntity']);
    // -- additionally we need tripal_chado config to access the yaml files.
    $this->installConfig('tripal_chado');

    // Get connection to drupal database in place.
    $this->public = \Drupal::service('database');

    // Get Chado in place
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Update entity settings to match tripal/config/install/tripal.settings.yml
    $allowed_title_tags = 'em i strong u';
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.allowed_title_tags', $allowed_title_tags)
      ->save();

    // Create three organisms in chado to be published.
    // Note: We added HTML to the genus (not approved tag) to confirm that
    // unallowed tags are being filtered and allowed ones are being kept.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:organism')
        ->fields([
          'genus' => '<p>Tripalus</p>',
          'species' => 'databasica ' . $i,
          'comment' => "Entry $i: we are adding a comment to ensure that we do have working fields that are not required.",
        ])->execute();
    }

    // Create three projects in chado to be published.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:project')
        ->fields([
          'name' => 'Project No. ' . $i,
          'description' => "Entry $i: we are adding a comment to ensure that we do have working fields that are not required.",
        ])->execute();
    }

    // Create three contacts in chado to be published.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:contact')
        ->fields([
          'name' => 'Contact No. ' . $i,
          'description' => "Entry $i: we are adding a comment to ensure that we do have working fields that are not required.",
        ])->execute();
    }

    // Create one analysis which will have lots of linked publications.
    $analysis_id = $this->connection->insert('1:analysis')
      ->fields([
        'name' => 'Analysis One',
        'program' => 'Tripal',
        'programversion' => '4.x',
      ])->execute();

    // Create many linked publications in chado, greater than max_delta.
    for ($i = 1; $i <= 150; $i++) {
      $pub_id = $this->connection->insert('1:pub')
        ->fields([
          'type_id' => 1,
          'uniquename' => 'Publication No. ' . $i,
          'title' => 'Publication No. ' . $i,
        ])->execute();
      $this->connection->insert('1:analysis_pub')
        ->fields([
          'analysis_id' => $analysis_id,
          'pub_id' => $pub_id,
        ])->execute();
    }
    // Add a title to the null publication to avoid a warning message.
    $this->connection->update('1:pub')
        ->fields([
          'title' => 'Null Publication',
        ])->condition('pub_id', 1, '=')->execute();

    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach(['OBI','local','TAXRANK','NCBITaxon','SIO','schema','data','NCIT','operation','OBCS','SWO','IAO','TPUB','rdfs'] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach(['obi','local','taxonomic_rank','ncbitaxon','SIO','schema','EDAM','ncit','OBCS','swo','IAO','tripal_pub'] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

    // Create terms for organism_dbxref since it seems to be missing.
    $term_details = [
      'vocab_name' => 'sbo',
      'id_space_name' => 'SBO',
      'term' => [
        'name' => 'reference annotation',
        'definition' => 'Additional information that supplements existing data, usually in a document, by providing a link to more detailed information, which is held externally, or elsewhere.',
        'accession' => '0000552',
      ],
    ];
    $this->createTripalTerm($term_details, 'chado_id_space', 'chado_vocabulary');

    $term_details = [
      'vocab_name' => 'ero',
      'id_space_name' => 'ERO',
      'term' => [
        'name' => 'database',
        'definition' => 'A database is an organized collection of data, today typically in digital form.',
        'accession' => '0001716',
      ],
    ];
    $this->createTripalTerm($term_details, 'chado_id_space', 'chado_vocabulary');

    // Create the content types + fields that we need.
    $this->createContentTypeFromConfig('general_chado', 'organism', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'project', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'contact', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'pub', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'analysis', TRUE);

    $publish_service = \Drupal::service('tripal.backend_publish');
    $this->chado_publish = $publish_service->createInstance('chado_storage', []);

  }

  /**
   * A very simple test to run the publish job and check it created entities
   * and populated fields.
   *
   * This test is not ideal but is better than nothing ;-)
   *
   * We are doing the test here to avoid mocking anything and to test
   * publishing of chado-focused content types.
   */
  public function testTripalPublishServiceSingleJob() {
    $publish_options = ['bundle' => 'organism', 'datastore' => 'chado_storage', 'schema_name' => $this->testSchemaName];
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(3, $published_entities,
      "We did not publish the expected number of entities.");

    // confirm the entities are added.
    $confirmed_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'organism']);
    $this->assertCount(3, $confirmed_entities,
      "We expected there to be the same number of organism entities as we inserted.");

    // confirm they have the expected titles and URL aliases
    $i = 1;
    foreach ($confirmed_entities as $key => $entity) {
      $expected_title = "<em>Tripalus databasica $i</em>";
      $expected_url = "/organism/$i";
      $title_string = $confirmed_entities[$key]->getTitle();
      $url_string = $confirmed_entities[$key]->toUrl()->toString();
      $this->assertEquals($expected_title, $title_string, "Published title differs from the expected value");
      $this->assertEquals($expected_url, $url_string, "Published URL differs from the expected value");
      $i++;
    }

    // Change the title and URL templates and confirm that all entity fields work.
    // Because of the length limit on the template, do a few at a time.
    $templates = [
      4 => '1[TripalEntityType__entity_id]2[TripalBundle__bundle_id]3',
      5 => '1[TripalEntityType__label]2[TripalEntityType__term_namespace]3',
      6 => '1[TripalEntityType__term_accession]2[TripalEntityType__term_label]3',
      7 => '1[TripalEntity__entity_id]2',
    ];
    $expected = [
      4 => '1organism2organism3',
      5 => '1Organism2OBI3',
      6 => '101000262organism3',
      7 => '172',
    ];
    foreach ($templates as $key => $template) {
      $this->connection->insert('1:organism')
        ->fields([
          'genus' => 'Tripalus',
          'species' => 'databasica ' . $key,
            'comment' => "Entry $key: we are adding a comment to ensure that we do have working fields that are not required.",
          ])->execute();
      $test_title_format = '=T=' . $template;
      $test_url_format = '/U/' . $template;
      $organism_bundle = \Drupal\tripal\Entity\TripalEntityType::load('organism');
      $organism_bundle->setTitleFormat($test_title_format);
      $organism_bundle->setURLFormat($test_url_format);
      $organism_bundle->save();
      $published_entities = $this->chado_publish->publish($publish_options);
      $this->assertCount(1, $published_entities,
        "We did not publish the expected number of entities.");
      // confirm the entities are added.
      $all_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'organism']);
      $this->assertCount($key, $all_entities,
        "We expected there to be the same total number of organism entities as we inserted so far.");

      // confirm title and URL are correct
      $expected_title = '=T=' . $expected[$key];
      $expected_url = '/U/' . $expected[$key];
      $title_string = $all_entities[$key]->getTitle();
      $url_string = $all_entities[$key]->toUrl()->toString();
      // Publish does not currently support the [TripalEntity__entity_id] on a new entity, see PR #2154
      if ($key != 7) {
        $this->assertEquals($expected_title, $title_string, "Published title differs from the expected value");
      }
      $this->assertEquals($expected_url, $url_string, "Published URL differs from the expected value");
      $i++;
    }

    // This section checks that max_delta is being enforced.
    $field_table = 'tripal_entity__analysis_pub';
    $publish_options = [
      'bundle' => 'analysis',
      'datastore' => 'chado_storage',
      'schema_name' => $this->testSchemaName,
      'republish' => 1,
    ];

    // First check using default value (100).
    $this->mock_warning = '';
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(1, $published_entities,
      'We did not publish the single expected analysis.');
    $this->assertStringContainsString('only 101 records will be published', $this->mock_warning,
      'We did not see the expected warning message from publish');
    $this->assertEquals(101, $this->countFieldTable($field_table),
      'The drupal field table does not contain the expected number of publications for the analysis');

    // Now explicitly set the global max_delta to a higher number, 110.
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.publish_global_max_delta', 110)
      ->set('tripal_entity_type.publish_global_max_delta_inhibit', 0)
      ->save();
    $this->mock_warning = '';
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(1, $published_entities,
      'We did not republish the analysis.');
    $this->assertStringContainsString('only 111 records will be published', $this->mock_warning,
      'We did not see the expected warning message from publish');
    $this->assertEquals(111, $this->countFieldTable($field_table),
      'The drupal field table does not contain the expected number of publications for the analysis');

    // Increase to 120, but set the inhibit flag, nothing should get published.
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.publish_global_max_delta', 120)
      ->set('tripal_entity_type.publish_global_max_delta_inhibit', 1)
      ->save();
    $this->mock_warning = '';
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertStringContainsString('no records will be published', $this->mock_warning,
      'We did not see the expected warning message from publish');
    $this->assertEquals(111, $this->countFieldTable($field_table),
      'The drupal field table does not contain the expected number of publications for the analysis');

    // Set the cardinality on the field, this should override other settings.
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.publish_global_max_delta', 100)
      ->set('tripal_entity_type.publish_global_max_delta_inhibit', 0)
      ->save();

    // This loop will test cardinality values near the actual number of existing
    // published records (111). A cardinality less than this will not publish
    // anything, but existing records will not be removed.
    $test_n = [100, 110, 111, 112, 130];
    foreach ($test_n as $i) {
      $this->mock_warning = '';
      $field_storage = FieldStorageConfig::loadByName('tripal_entity', 'analysis_pub');
      $this->assertIsObject($field_storage, 'Failed to retrieve field storage object');
      $field_storage->setCardinality($i);
      $field_storage->save();
      $published_entities = $this->chado_publish->publish($publish_options);
      $this->assertStringContainsString('only ' . ($i + 1) . ' records will be published', $this->mock_warning,
        'We did not see the expected warning message from publish');
      $expected = $i + 1;
      if ($expected < 111) {
        $expected = 111;
      }
      $this->assertEquals($expected, $this->countFieldTable($field_table),
        'The drupal field table does not contain the expected number of publications for the analysis');
    }
    // Tests unpublishing.
    // Try to unpublish only orphaned entities, but since there are no
    // orphaned entities this should do absolutely nothing.
    $publish_options = ['bundle' => 'organism', 'datastore' => 'chado_storage', 'schema_name' => $this->testSchemaName, 'unpublish' => TRUE];
    $initial_chado_records = $this->getChadoTableRecords('organism', 'organism_id');
    $initial_drupal_records = $this->getPublicTableRecords('tripal_entity__organism_genus', 'organism_genus_record_id');
    $unpublished_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(0, $unpublished_entities, 'No orphans, should not have unpublished any records');
    $final_chado_records = $this->getChadoTableRecords('organism', 'organism_id');
    $final_drupal_records = $this->getPublicTableRecords('tripal_entity__organism_genus', 'organism_genus_record_id');
    $this->assertEquals($initial_chado_records, $final_chado_records, 'Unexpected change to chado organism table');
    $this->assertEquals($initial_drupal_records, $final_drupal_records, 'Unexpected change to drupal field table tripal_entity__organism_genus');

    // Delete one chado record and confirm that we can now unpublish it as
    // an orphaned record.
    $n = $this->connection->delete('1:organism')
      ->condition('organism_id', 2, '=')
      ->execute();
    $this->assertEquals(1, $n, 'Did not delete organism from chado where organism_id=2');
    $unpublished_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(1, $unpublished_entities, 'Did not unpublish one orphaned organism entity');
    $final_chado_records = $this->getChadoTableRecords('organism', 'organism_id');
    $final_drupal_records = $this->getPublicTableRecords('tripal_entity__organism_genus', 'organism_genus_record_id');
    $this->assertFalse(array_key_exists(2, $final_chado_records), 'The chado "2" record was not removed');
    $this->assertFalse(array_key_exists(2, $final_drupal_records), 'The field "2" record was not unpublished');

    // Unpublish all remaining non-orphaned organism entities.
    // Chado will not be touched.
    $initial_chado_records = $this->getChadoTableRecords('organism', 'organism_id');
    $publish_options['orphaned'] = FALSE;
    $unpublished_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(6, $unpublished_entities, 'Did not unpublish the 6 remaining organism entities');
    $final_chado_records = $this->getChadoTableRecords('organism', 'organism_id');
    $final_drupal_records = $this->getPublicTableRecords('tripal_entity__organism_genus', 'organism_genus_record_id');
    $this->assertEquals($initial_chado_records, $final_chado_records, 'Unexpected change to chado organism table');
    $this->assertEmpty($final_drupal_records, 'There are field records remaining that were not unpublished');
  }

  /**
   * A very simple test to run TWO publish jobs and check it created entities
   * and populated fields.
   *
   * @see https://github.com/tripal/tripal/issues/1716
   *
   * This test is not ideal but is better than nothing ;-)
   *
   * We are doing the test here to avoid mocking anything and to test
   * publishing of chado-focused content types.
   */
  public function testTripalPublishService2Jobs() {
    $publish_options = ['bundle' => 'project', 'datastore' => 'chado_storage', 'schema_name' => $this->testSchemaName];
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertCount(3, $published_entities,
      "We did not publish the expected number of entities.");

    // confirm the entities are added.
    $confirmed_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'project']);
    $this->assertCount(3, $confirmed_entities,
      "We expected there to be the same number of project entities as we inserted.");

    // Submit the Tripal job by calling the callback directly.
    $publish_options = ['bundle' => 'contact', 'datastore' => 'chado_storage', 'schema_name' => $this->testSchemaName];
    $published_entities = $this->chado_publish->publish($publish_options);
    $this->assertContains(count($published_entities), [3, 4],
      "We did not publish the expected number of entities.");

    // confirm the entities are added. Chado defines a default "null" contact, which
    // may also get published, so expect 4 instead of 3. (Issue #1809)
    $confirmed_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'contact']);
    $this->assertContains(count($confirmed_entities), [3, 4],
      "We expected there to be the same number of contact entities as we inserted plus the null contact.");

    // Verify that a field table was populated
    // Because this is a test environment, we know that the entity IDs
    // that we just published will start with 1, but because of the
    // "null" contact, we will just check the project table.
    for ($i=1; $i <= 3; $i++) {
      $query = \Drupal::entityQuery('tripal_entity')
        ->condition('type', 'project')
        ->condition('project_name.record_id', $i, '=')
        ->accessCheck(TRUE);
      $ids = $query->execute();
      $this->assertEquals(1, count($ids), 'We did not retrieve the project name field');
      $entity_id = reset($ids);
      $this->assertEquals($i, $entity_id, 'We did not retrieve the expected project entity id from its field');
    }
  }

  /**
   * Returns a count of number of entries in a Drupal field table.
   *
   * @param string $table_name
   *   The name of the table in the public schema.
   *
   * @return int
   *   The count of records in the table.
   */
  protected function countFieldTable(string $table_name): int {
    $query = $this->public->select($table_name);
    $query->addExpression('COUNT(*)', 'count');
    $count = $query->execute()->fetchField();
    return $count;
  }

  /**
   * Returns all the records from a Drupal field table.
   *
   * @param string $table_name
   *   The name of the table in the public schema.
   * @param string $key
   *   The name of the column for the key of the returned array.
   *
   * @return array
   *   An array of record objects from the table, keyed by $key column.
   */
  protected function getPublicTableRecords(string $table_name, string $key): array {
    $query = $this->public->select($table_name, 't');
    $query->fields('t');
    $records = $query->execute()->fetchAllAssoc($key);
    return $records;
  }

  /**
   * Returns all the records from a Chado table.
   *
   * @param string $table_name
   *   The name of the table in the chado schema.
   * @param string $key
   *   The name of the column for the key of the returned array.
   *
   * @return array
   *   An array of record objects from the table, keyed by $key column.
   */
  protected function getChadoTableRecords(string $table_name, string $key): array {
    $query = $this->connection->select('1:' . $table_name, 't');
    $query->fields('t');
    $records = $query->execute()->fetchAllAssoc($key);
    return $records;
  }

}
