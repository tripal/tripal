<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;


/**
 * Tests the basic functions of the TripalFieldCollection Service..
 *
 * @group Tripal
 * @group Tripal Content
 */
class TripalFieldCollectionTest extends TripalTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'tripal',
    'tripal_test',
  ];


  /**
   * Tests the TripalContentTypes class public functions.
   */
  public function testTripalFieldCollection() {

    //\Drupal::state()->set('is_a_test_environment', TRUE);

    // Create the vocabulary term needed for testing the content type.
    // We'll use the default Tripal IdSpace and Vocabulary plugins.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idspace = $idsmanager->createCollection('OBI', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('OBI', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'organism',
      'idSpace' => 'OBI',
      'vocabulary' => 'OBI',
      'accession' => '0100026',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);

    // Create a good content type array.
    $ct_details = [
      'label' => 'Organism',
      'term' => $term,
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];
    /** @var \Drupal\tripal\Services\TripalContentTypes $content_type_setup **/
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $content_type = $content_type_service->createContentType($ct_details);
    $this->assertTrue(!is_null($content_type), "Failed to create a content type with avalid definition.");


    /** @var \Drupal\tripal\Services\TripalFieldCollection $fields_service **/
    $fields_service = \Drupal::service('tripal.tripalfield_collection');

    $idspace = $idsmanager->createCollection('TAXRANK', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('TAXRANK', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'genus',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'TAXRANK',
      'accession' => '0000005',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $field_def = [
      'name' => 'organism_genus',
      'content_type' => 'organism',
      'label' => 'Genus',
      'type' => 'tripal_string_type',
      'description' => "The genus name of the organism.",
      'cardinality' => 1,
      'required' => TRUE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings'=> [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'TAXRANK',
        'termAccession' => "0000005",
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 15
          ],
        ],
        'form' => [
          'default'=> [
            'region'=> 'content',
            'weight' => 15
          ],
        ],
      ],
    ];

    // Test that the setFieldDefDefaults() function returns all expected
    // default elements.
    $defaults = $fields_service->setFieldDefDefaults();

    // Check the first-level elements.
    $this->assertTrue(array_key_exists('name', $defaults), "The field default array is missing the name element.");
    $this->assertTrue(array_key_exists('content_type', $defaults), "The field default array is missing the content_type element.");
    $this->assertTrue(array_key_exists('label', $defaults), "The field default array is missing the label element.");
    $this->assertTrue(array_key_exists('type', $defaults), "The field default array is missing the type element.");
    $this->assertTrue(array_key_exists('description', $defaults), "The field default array is missing the description element.");
    $this->assertTrue(array_key_exists('required', $defaults), "The field default array is missing the required element.");
    $this->assertTrue(array_key_exists('revisionable', $defaults), "The field default array is missing the revisionable element.");
    $this->assertTrue(array_key_exists('translatable', $defaults), "The field default array is missing the translatable element.");
    $this->assertTrue(array_key_exists('cardinality', $defaults), "The field default array is missing the cardinality element.");

    // Check the storage settings
    $this->assertTrue(array_key_exists('storage_settings', $defaults), "The field default array is missing the storage_settings element.");
    $this->assertTrue(array_key_exists('storage_plugin_id', $defaults['storage_settings']), "The field default array is missing the storage_settings > storage_plugin_id element.");
    $this->assertTrue(array_key_exists('storage_plugin_settings', $defaults['storage_settings']), "The field default array is missing the storage_settinsg > storage_plugin_settings element.");

    // Check the settings.
    $this->assertTrue(array_key_exists('settings', $defaults), "The field default array is missing the settings element.");
    $this->assertTrue(array_key_exists('termIdSpace', $defaults['settings']), "The field default array is missing the termIdSpace settings element.");
    $this->assertTrue(array_key_exists('termAccession', $defaults['settings']), "The field default array is missing the termAccession settings element.");

    // Check the displays
    $this->assertTrue(array_key_exists('display', $defaults), "The field default array is missing the display element.");
    $this->assertTrue(array_key_exists('view', $defaults['display']), "The field default array is missing the view display element.");
    $this->assertTrue(array_key_exists('form', $defaults['display']), "The field default array is missing the form display element.");

    // Check the view display
    $this->assertTrue(array_key_exists('default', $defaults['display']['view']), "The field default array is missing display > view > default element.");
    $this->assertTrue(array_key_exists('region', $defaults['display']['view']['default']), "The field default array is missing the display > view > default > region element.");
    $this->assertTrue(array_key_exists('label', $defaults['display']['view']['default']), "The field default array is missing the display > view > default > label element.");
    $this->assertTrue(array_key_exists('weight', $defaults['display']['view']['default']), "The field default array is missing the display > view > default > weight element.");
    $this->assertTrue(array_key_exists('label', $defaults['display']['view']['teaser']), "The field default array is missing the display > view > teaser > label element.");

    // check the form display
    $this->assertTrue(array_key_exists('default', $defaults['display']['form']), "The field default array is missing the display > form > default element.");
    $this->assertTrue(array_key_exists('region', $defaults['display']['form']['default']), "The field default array is missing the display > form > default > region element.");
    $this->assertTrue(array_key_exists('weight', $defaults['display']['form']['default']), "The field default array is missing the display > form > default > weight element.");

    $this->assertIsBool($defaults['required'], 'The required element is not boolean');
    $this->assertIsBool($defaults['revisionable'], 'The revisionable element is not boolean');
    $this->assertIsBool($defaults['translatable'], 'The translatable element is not boolean');
    $this->assertIsInt($defaults['cardinality'], 'The cardinality element is not an integer');
    $this->assertIsArray($defaults['storage_settings']['storage_plugin_settings'], 'The storage_plugin_settings element should be an array');

    // Make sure the setFieldDefDefaults functions doesn't change any valid values
    $new_def = $fields_service->setFieldDefDefaults($field_def);
    $this->assertTrue($field_def['name'] == $new_def['name'], "The name element changed after setting defaults.");
    $this->assertTrue($field_def['content_type'] == $new_def['content_type'], "The content_type element changed after setting defaults.");
    $this->assertTrue($field_def['label'] == $new_def['label'], "The label element changed after setting defaults.");
    $this->assertTrue($field_def['description'] == $new_def['description'], "The description element changed after setting defaults.");
    $this->assertTrue($field_def['required'] == $new_def['required'], "The required element changed after setting defaults.");
    $this->assertTrue($field_def['cardinality'] == $new_def['cardinality'], "The cardinality element changed after setting defaults.");
    $this->assertTrue($field_def['storage_settings']['storage_plugin_id'] == $new_def['storage_settings']['storage_plugin_id'],
        "The storage_settings > storage_plugin_id  element changed after setting defaults.");
    $this->assertTrue(empty($new_def['storage_settings']['storage_plugin_settings']),
        "The storage_settings > storage_plugin_settings element changed after setting defaults.");
    $this->assertTrue($new_def['storage_settings']['max_length'] == $new_def['storage_settings']['max_length'],
        "The storage_settings > max_length element changed after setting defaults.");
    $this->assertTrue($field_def['settings']['termIdSpace'] == $new_def['settings']['termIdSpace'],
        "The settings > termIdSpace element changed after setting defaults.");
    $this->assertTrue($field_def['settings']['termAccession'] == $new_def['settings']['termAccession'],
        "The settings > termAccession element changed after setting defaults.");
    $this->assertTrue($field_def['display']['view']['default']['region'] == $new_def['display']['view']['default']['region'],
        "The display > view > default > region element changed after setting defaults.");
    $this->assertTrue($field_def['display']['view']['default']['label'] == $new_def['display']['view']['default']['label'],
        "The display > view > default > label element changed after setting defaults.");
    $this->assertTrue($field_def['display']['view']['default']['weight'] == $new_def['display']['view']['default']['weight'],
        "The display > view > default > weight element changed after setting defaults.");
    $this->assertTrue($field_def['display']['form']['default']['region'] == $new_def['display']['form']['default']['region'],
        "The display > form > default > region element changed after setting defaults.");
    $this->assertTrue($field_def['display']['form']['default']['region'] == $new_def['display']['form']['default']['region'],
        "The display > form > default > region element changed after setting defaults.");

    // Make sure we have some newly added defaults that were missing
    $this->assertTrue(array_key_exists('translatable', $new_def), "Missing the translatable element after setting defaults.");
    $this->assertTrue(array_key_exists('revisionable', $new_def), "Missing the revisionable element after setting defaults.");
    $this->assertTrue(array_key_exists('teaser', $new_def['display']['view']), "Missing the display > view > teaser element after setting defaults.");

    // Make sure the termIdSpace and termAccession got copied to the storage settings.
    $this->assertTrue(array_key_exists('termIdSpace', $new_def['storage_settings']),
        "Missing the storage_settings > termIdSpace element after setting defaults.");
    $this->assertTrue(array_key_exists('termAccession', $new_def['storage_settings']),
        "Missing the storage_settings > termAccession element after setting defaults.");
    $this->assertTrue($new_def['storage_settings']['termIdSpace'] == $field_def['settings']['termIdSpace'],
        "The storage_settings > termIdSpace was not copied correctly.");
    $this->assertTrue($new_def['storage_settings']['termAccession'] == $field_def['settings']['termAccession'],
        "The storage_settings > termAccession was not copied correctly.");

    // Check that our field details array passes validation.
    $is_valid = $fields_service->validate($field_def);
    $this->assertTrue($is_valid, "A good field definition was invalid.");

    // Check that a good field definition can be added to a content type.
    $is_added = $fields_service->addBundleField($field_def);
    $this->assertTrue($is_added, "A good field definition failed to be added to the content type.");

    // Make sure a bad field definition cannot be added.
    $bad = $field_def;
    $bad['name'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'name' should have not been added.");

    $bad = $field_def;
    $bad['content_type'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'content_type' should have not been added.");

    $bad = $field_def;
    $bad['type'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'type' should have not been added.");

    $bad = $field_def;
    $bad['storage_settings'] = [];
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'storage_settings' should have not been added.");

    $bad = $field_def;
    $bad['storage_settings']['storage_plugin_id'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'storage_plugin_id' should have not been added.");

    $bad = $field_def;
    $bad['settings']['termIdSpace'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'termIdSpace' should have not been added.");

    $bad = $field_def;
    $bad['settings']['termAccession'] = '';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition missing the 'termAccession' should have not been added.");

    $bad = $field_def;
    $bad['settings']['termIdSpace'] = 'XYZPDQ';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition with an incorrect 'termIdSpace' should have not been added.");

    $bad = $field_def;
    $bad['settings']['termAccession'] = 'XYZPDQ';
    $is_added = $fields_service->addBundleField($bad);
    $this->assertFalse($is_added, "A bad field definition with an incorrect 'termAccession' should have not been added.");

    // -------------------------------------------------------------
    // Tests for field discovery
    // -------------------------------------------------------------

    // Test that the discover function works by providing the default keys.
    $discovered_fields = $fields_service->discover($content_type);
    $this->assertTrue(array_key_exists('new', $discovered_fields), "Missing the 'new' key in the array returned by the discover() function.");
    $this->assertTrue(array_key_exists('existing', $discovered_fields), "Missing the 'existing' key in the array returned by the discover() function.");
    $this->assertTrue(array_key_exists('invalid', $discovered_fields), "Missing the 'invalid' key in the array returned by the discover() function.");

    // The TripalTestTextTypeItem is a test field provided in
    // the `tripal_test` module which is included as a new module
    // in testing. There we'll find a discover method that should
    // return various types of fields.  We'll make sure we see
    // those fields as expected.

    // Make sure we see a valid field
    $this->assertTrue(array_key_exists('organism_test_field', $discovered_fields['new']), "Missing the 'organism_test_field' key in the 'new' array returned by the discover() function.");

    // Make sure we see a properly truncated field name with
    // spaces and unicode character replaced with underscores
    $this->assertTrue(array_key_exists('organism__test_field_but_with__1', $discovered_fields['new']), "Missing the 'organism__test_field_but_with__1' key in the 'new' array returned by the discover() function.");

    // Same as above, but cvterm_id was not passed. The field name should
    // end with a unique id
    $found = FALSE;
    foreach ($discovered_fields['new'] as $field) {
      if (preg_match('/^organism__test_fie_[0-9a-f]{13}$/', $field['name'])) {
        $found = TRUE;
      }
    }
    $this->assertTrue($found, "Missing the 'organism__test_fie_[0-9a-f]{13}' key in the 'new' array returned by the discover() function.");

    // Make sure we have an invalid field. We don't need to test every
    // case where a field may be invalid. That happens above. We just need to
    // make sure that if a field is invalid that it is listed in the invalid
    // section with the reason included.
    $this->assertTrue(array_key_exists('organism_test_field4', $discovered_fields['invalid']), "The 'organism_test_field4' key should be in the 'invalid' array returned by the discover() function.");

    // Now add the valid field to the content type. and check to make sure
    // it is listed as `existing` afterwards when discover() is called again.
    $is_added = $fields_service->addBundleField($discovered_fields['new']['organism_test_field']);
    $this->assertTrue($is_added, "A valid field definition from the discover() method failed to be added to the content type.");
    $discovered_fields = $fields_service->discover($content_type);
    $this->assertTrue(array_key_exists('organism_test_field', $discovered_fields['existing']), "Missing the 'organism_test_field' key in the 'existing' array returned by the discover() function.");

  }


}
