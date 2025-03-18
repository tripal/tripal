<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Plugin\TripalBackendPublish\ChadoPublish;

/**
 * Tests the publish service for chado-based content types.
 *
 * @group TripalBackendPublish
 * @group ChadoPublish
 */
class ChadoPublishTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'tripal_chado', 'views', 'field'];

  protected $connection;

  protected $chado_publish;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Grab the container.
    $container = \Drupal::getContainer();

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm','TripalEntity']);
    // -- additionally we need tripal_chado config to access the yaml files.
    $this->installConfig('tripal_chado');

    // Get Chado in place
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Update configuration to match tripal/config/install/tripal.settings.yml
    $allowed_title_tags = 'em i strong u';
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.allowed_title_tags', $allowed_title_tags)
      ->save();

    // Create three organisms in chado to be published.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:organism')
        ->fields([
          'genus' => 'Tripalus',
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

    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach(['OBI','local','TAXRANK','NCBITaxon','SIO','schema','data','NCIT','operation','OBCS','SWO','IAO','TPUB'] as $termIdSpace) {
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

      // Finally confirm title and URL are correct
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
}
