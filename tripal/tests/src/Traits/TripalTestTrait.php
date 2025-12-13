<?php

namespace Drupal\Tests\tripal\Traits;

use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalVocabularyInterface;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use PHPUnit\Framework\Attributes\Group;

/**
 * Provides functions related to setting up Tripal test environments
 * and can be used in either Kernel or Functional tests.
 */
#[Group('tripal-testing')]
trait TripalTestTrait {

  /**
   * Creates a Tripal Field for testing purposes.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - term (TripalTerm)
   *    - is_required (boolean)
   *    - cardinality (integer)
   *    - storage_settings (array)
   *
   * @return array
   *   The field details used to create the field.
   */
  public function createTripalField(string $entity_type, array $values = []) {

    $this->suggestRequiredModules(['TripalTerm', 'TripalField']);

    // Setting the default values:
    $random = $this->getRandomGenerator();
    $values['field_name'] = $values['field_name'] ?? $random->word(6) . '_' . $random->word(15);
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    $values['term'] = $values['term'] ?? $this->createTripalTerm();
    $values['cardinality'] = $values['cardinality'] ?? 1;
    $values['storage_plugin_settings'] = $values['storage_plugin_settings'] ?? [];
    $values['is_required'] = $values['is_required'] ?? FALSE;

    $term = $values['term'];

    $field = [
      'name' => $values['field_name'],
      'label' => ucwords($term->getName()),
      'type' => $values['field_type'],
      'description' => $term->getDefinition(),
      'cardinality' => $values['cardinality'],
      'required' => $values['is_required'],
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => $values['storage_plugin_settings'],
      ],
      'settings' => [
        'termIdSpace' => $term->getIdSpace(),
        'termAccession' => $term->getAccession(),
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 5,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 5,
          ],
        ],
      ],
    ];

    /**
     * @var \Drupal\tripal\Services\TripalFieldCollection $tripal_fields
     */
    $tripal_fields = \Drupal::service('tripal.tripalfield_collection');
    $tripal_fields->addBundleField($entity_type, $field);

    return $field;
  }

  /**
   * Creates a Tripal Vocabulary / ID Space / Term for testing purposes.
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - vocab_name (string)
   *    - id_space_name (string)
   *    - term (array)
   *        - name (string)
   *        - definition (string)
   *        - accession (string)
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalTerm
   *   Returns the tripal term that was created.
   */
  public function createTripalTerm(&$values, $idspace_plugin_id, $vocab_plugin_id) {

    $this->suggestRequiredModules(['TripalTerm']);

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~4 latin capitalized words.
    $values['vocab_name'] = $values['vocab_name'] ?? $random->sentences(4, TRUE);
    // Provides a 4 character string.
    $values['id_space_name'] = $values['id_space_name'] ?? $random->word(4);
    $values['term'] = $values['term'] ?? [];
    // Provides a unique string with ~8 characters.
    $values['term']['accession'] = $values['term']['accession'] ?? $random->name(8, TRUE);
    // Provides a title with ~2 latin capitalized words.
    $values['term']['name'] = $values['term']['name'] ?? $random->sentences(2, TRUE);
    // Provides as collection of sentences with ~20 words.
    $values['term']['definition'] = $values['term']['definition'] ?? $random->sentences(20);

    // Create the Vocabulary.
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocabulary = $vmanager->loadCollection($values['vocab_name']);
    if (!$vocabulary) {
      $vocabulary = $vmanager->createCollection($values['vocab_name'], $vocab_plugin_id);
      $this->assertInstanceOf(TripalVocabularyInterface::class, $vocabulary, "Unable to create the Vocabulary.");
    }
    $this->assertInstanceOf(TripalVocabularyInterface::class, $vocabulary, "Unable to load the Vocabulary.");

    // Create the ID Space.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($values['id_space_name']);
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($values['id_space_name'], $idspace_plugin_id);
      $this->assertInstanceOf(TripalIdSpaceInterface::class, $idSpace, "Unable to create the ID Space.");
      $idSpace->setDefaultVocabulary($vocabulary->getName());
    }
    $this->assertInstanceOf(TripalIdSpaceInterface::class, $idSpace, "Unable to load the ID Space.");

    $term = $idSpace->getTerm($values['term']['accession']);
    if (!$term) {
      // Now create the term.
      $values['term']['idSpace'] = $values['id_space_name'];
      $values['term']['vocabulary'] = $values['vocab_name'];
      $term = new TripalTerm($values['term']);
      $this->assertInstanceOf(TripalTerm::class, $term, "Unable to create the term object.");
      // And save it to the ID Space.
      $idSpace->saveTerm($term);
    }

    return $term;
  }

  /**
   * Creates a Tripal Entity (i.e. a piece of Tripal content) for testing purposes.
   *
   * Currently this function creates a real TripalEntity and saves it.
   * Because this is a testing environment, this does get saved in the
   * Drupal prefixed test tables and dropped after the fact.
   *
   * We went this route intially for maximum code coverage in order to catch
   * all deprecation notices posed by Drupal. We may want to replace this approach
   * at a later date.
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - title (string)
   *    - type (string; eg. organism)
   *    - user_id (integer)
   *    - status (boolean; TRUE if published)
   *
   * @return \Drupal\tripal\Entity\TripalEntity
   *   The Tripal Entity object that was created based off the parameters.
   */
  public function createTripalContent($values = []) {

    $this->suggestRequiredModules(['TripalTerm', 'TripalEntity', 'TripalField']);

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~8 latin capitalized words.
    $values['title'] = $values['title'] ?? $random->sentences(8, TRUE);
    $values['id'] = $values['id'] ?? $random->sentences(1, TRUE);

    // Creates a type if one is not provided.
    if (!isset($values['type'])) {
      $content_type = $this->createTripalContentType();
      $values['type'] = $content_type->id();
    }

    $entity = TripalEntity::create($values);
    $this->assertIsObject($entity, "Unable to create a test entity.");

    return $entity;
  }

  /**
   * Creates a Tripal Entity Type object for testing purposes.
   *
   * Currently this function creates a real TripalEntityType and saves it.
   * Because this is a testing environment, this does get saved in the
   * Drupal prefixed test tables and dropped after the fact.
   *
   * We went this route intially for maximum code coverage in order to catch
   * all deprecation notices posed by Drupal. We may want to replace this approach
   * at a later date.
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    -     id (string)
   *    -     label (label; string)
   *    -     termIdSpace (string)
   *    -     termAccession (string)
   *    -     help_text (text)
   *    -     category (string)
   *    -     title_format (string)
   *    -     url_format (string)
   *    -     hide_empty_field (boolean)
   *    -     ajax_field (boolean)
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   *   The Tripal content type that was created based on the parameters.
   */
  public function createTripalContentType($values = []) {

    $this->suggestRequiredModules(['TripalTerm', 'TripalEntity', 'TripalField']);

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~3 latin capitalized words.
    $values['label'] = $values['label'] ?? $random->sentences(3, TRUE);
    $values['id'] = $values['id'] ?? $random->sentences(1, TRUE);
    // Provides a random non-unique 4 character string.
    $values['termIdSpace'] = $values['termIdSpace'] ?? $random->string(4);
    // Provides a random non-unique 10 character string.
    $values['termAccession'] = $values['termAccession'] ?? $random->string(10);
    // Provides a few of sentences with ~50 words total.
    $values['help_text'] = $values['help_text'] ?? $random->sentences(50);
    $values['category'] = $values['category'] ?? 'Testing Types';
    $values['title_format'] = $values['title_format'] ?? 'This is a title format with no tokens';
    $values['url_format'] = $values['url_format'] ?? '/url/format/with/no/tokens';
    $values['hide_empty_field'] = $values['hide_empty_field'] ?? FALSE;
    $values['ajax_field'] = $values['ajax_field'] ?? FALSE;

    // Actually creating the type.
    $entity_type_obj = TripalEntityType::create($values);
    $this->assertIsObject($entity_type_obj, "Unable to create a test content type.");
    $entity_type_obj->save();

    // A quick double check before returning it.
    $this->assertEquals($values['label'], $entity_type_obj->getLabel(), "Unable to retrieve label from the newly created entity type.");

    return $entity_type_obj;
  }

  /**
   * Creates a content type and associated fields using the
   * tripalentitytype_collection and tripalfield_collection configuration.
   *
   * @param string $config_id
   *   The id from a tripalentitytype_collection config file. Fields will also be
   *   added if there is a tripalfield_collection with this same id.
   * @param string $content_type_id
   *   The id of the content type to create. It must exist in the specified YAML.
   * @param bool $createTerms
   *   Not implemented.
   *
   * @return int
   *   The return value is 1 if everything went well and 2 if the content type
   *   was created but the fields were not due to a missing config with matching
   *   id. If the content type was not created then PHPUnit asserts fail.
   */
  protected function createContentTypeFromConfig($config_id, $content_type_id, $createTerms = FALSE) {
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $field_service = \Drupal::service('tripal.tripalfield_collection');
    $config_factory = \Drupal::service('config.factory');
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $storage = \Drupal::entityTypeManager()->getStorage('tripal_entity_type');

    $this->suggestRequiredModules(['TripalTerm', 'TripalEntity', 'TripalField']);

    // FIRST THE CONTENT TYPE.
    $yaml_contentTypes = 'tripal.tripalentitytype_collection.' . $config_id;

    // Check that config is installed.
    $config = $config_factory->get($yaml_contentTypes);
    $this->assertIsObject($config,
      'You need to have called $this->installConfig for the module containing the configuration for the content type you want to use in this test.');
    $specific_config = $config->get('content_types');
    $this->assertIsArray($specific_config,
      'You need to have called $this->installConfig for the module containing the configuration for the content type you want to use in this test.');

    foreach ($specific_config as $content_type) {

      if (!array_key_exists('id', $content_type) or $content_type['id'] != $content_type_id) {
        continue;
      }

      [$termIdSpace, $termAccession] = explode(':', $content_type['term']);
      $idspace = $idsmanager->loadCollection($termIdSpace);
      $this->assertIsObject($idspace, "We were not able to get the id space " . $termIdSpace);
      $term = $idspace->getTerm($termAccession);
      $this->assertIsObject($term, "We were not able to get the term " . $content_type['term']);
      $content_type['term'] = $term;

      // Add the content type.
      $added_content_type = $content_type_service->createContentType($content_type);
      $this->assertIsObject($added_content_type,
        "We were not able to create the $content_type_id content type in the testing environment.");

      // Set the third party setting for base table.
      $base_table = $content_type['settings']['chado_base_table'] ?? NULL;
      $this->assertNotNull($base_table, "There is no YAML base table setting for content type $content_type_id");
      $entity_type = $storage->load($content_type_id);
      $entity_type->setThirdPartySetting('tripal', 'chado_base_table', $base_table);
      $entity_type->save();
      $table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
      $this->assertEquals($base_table, $table, "We did not retrieve the correct third party base table setting for $content_type_id");
    }

    // NOW THE FIELDS.
    $yaml_fields = 'tripal.tripalfield_collection.' . $config_id;

    // Check that config is installed.
    $config = $config_factory->get($yaml_fields);
    if (!is_object($config)) {
      return 2;
    }
    $specific_config = $config->get('fields');
    if (!is_array($specific_config)) {
      return 2;
    }

    foreach ($specific_config as $field) {
      if (array_key_exists('content_type', $field) and $field['content_type'] === $content_type_id) {
        // @debug print "\nAdding Field to Bundle: " . print_r($field,TRUE);
        $field_service->addBundleField($field);
      }
    }

    return 1;
  }

  /**
   * Sets the 'fields' by specifying the top level key of a YAML file.
   *
   * @param string $yaml_file
   *   The full path to a yaml file which follows the format descripbed above.
   *
   * @return array
   *   The first array returned describes the state of the test environment to
   *   be setup and the second describes the scenarios to test. For a
   *   description of the structure of these arrays, see the YAML file directly.
   */
  public function getTestInfoFromYaml(string $yaml_file): array {

    if (!file_exists($yaml_file)) {
      throw new \Exception("Cannot open YAML file $yaml_file.");
    }

    $file_contents = file_get_contents($yaml_file);
    if (empty($file_contents)) {
      throw new \Exception("Unable to retrieve contents for YAML file $yaml_file.");
    }

    $yaml_data = Yaml::parse($file_contents);
    if (empty($yaml_data)) {
      throw new \Exception("Unable to parse YAML file $yaml_file.");
    }

    if (!array_key_exists('system-under-test', $yaml_data)) {
      throw new \Exception("The 'system-under-test' key is missing from the $yaml_file.");
    }
    if (!array_key_exists('scenarios', $yaml_data)) {
      throw new \Exception("The 'scenarios' key is missing from the $yaml_file.");
    }

    return [$yaml_data['system-under-test'], $yaml_data['scenarios']];
  }

  /**
   * Warns test developers if they are missing required modules in a kernel test.
   *
   * This is needed because otherwise the exceptions thrown are not very obvious
   * and complicate debugging kernel tests.
   *
   * @param array $functionality
   *   A list of functionality you need to support. Although this method handles
   *   dependencies, you should include all items in the supported keys below
   *   that you need. This is because in some cases you will want to mock rather
   *   then include in your kernel tests and this way, this method supports that.
   *   Supported keys are:
   *   - TripalTerm
   *   - TripalEntity
   *   - TripalField
   *   - TripalImporter.
   *
   * @return void
   */
  protected function suggestRequiredModules(array $functionality) {
    $suggested_modules = [];

    // We need to do the suggested modules first so that you get better
    // warnings if you do not have the right combination.
    if (in_array('TripalTerm', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['tripal'] = 'tripal';
    }
    if (in_array('TripalEntity', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['path'] = 'path';
      $suggested_modules['path_alias'] = 'path_alias';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['field'] = 'field';
    }
    if (in_array('TripalField', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['field'] = 'field';
    }
    if (in_array('TripalImporter', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['file'] = 'file';
    }

    // Now warn you about your modules array.
    $missing_modules = [];
    foreach ($suggested_modules as $check) {
      if (!in_array($check, static::$modules)) {
        $missing_modules[] = $check;
      }
    }
    $modules_array_code = 'protected static $modules = [\'' . implode("','", $suggested_modules) . '\'];';
    $this->assertEmpty($missing_modules, 'You are missing some modules in your static $modules array. For the functionality you requested, we suggest the following: ' . $modules_array_code);
  }

}
