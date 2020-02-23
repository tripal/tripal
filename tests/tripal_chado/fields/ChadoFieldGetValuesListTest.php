<?php

namespace Tests\tripal_chado\fields;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use StatonLab\TripalTestSuite\Database\Factory;

/**
 * Test ChadoField->getValueList() Method.
 */
class ChadoFieldGetValuesListTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  // Stores a list of field instances to be tested including their storage method and instance info.
  private $field_list = NULL;

  /**
   * Test getValueList for fields based on columns in the base table.
   *
   * @dataProvider getBaseFields
   *
   * @group fields
   * @group getValueList
   */
  public function testBaseTableColumns($field_name, $bundle_name, $info) {
    include_once(drupal_get_path('tripal_chado', 'module') . '/includes/TripalFields/ChadoField.inc');

    // Construct the Field instance we want the values for.
    // Specifying "ChadoField" here ensures we are only testing our
    // implementation of getValueList() and not the custom version for any
    // given field.
    // YOU SHOULD TEST CUSTOM FIELD IMPLEMENTATIONS SEPARATELY.
    $instance = new \ChadoField($info['field_info'], $info['instance_info']);

    // Retrieve the values.
    // $values will be an array containing the distinct set of values for this field instance.
    $values = $instance->getValueList(['limit' => 5]);

    // Ensure we have values returned!
    $this->assertTrue(
      is_array($values),
      t(
        'No values returned for @field_name (bundle: @bundle_name, bundle base table: @bundle_base_table, chado table: @chado_table, chado column: @chado_column).',
        [
          '@field_name' => $field_name,
          '@bundle_name' => $bundle_name,
          '@bundle_base_table' => $info['bundle_base_table'],
          '@chado_table' => $info['instance_info']['settings']['chado_table'],
          '@chado_column' => $info['instance_info']['settings']['chado_column'],
        ]
      )
    );

    // Ensure there are no more then 5 as specified in the limit above.
    $this->assertLessThanOrEqual(5, sizeof($values),
      t('Returned too many results for @field_name.', ['@field_name' => $field_name]));

    // Ensure a known value is in the list.
    // Note: The following generates fake data with a fixed value for the column this
    // field is based on. This allows us to check that fixed value is one of those
    // returned by ChadoField->getValueList().
    $fake_value = $this->generateFakeData($info['bundle_base_table'], $info['base_schema'], $info['instance_info']['settings']['chado_column']);
    if ($fake_value !== FALSE) {

      // Re-generate the values...
      $values = $instance->getValueList(['limit' => 200]);

      // And ensure our fake value is in the returned list.
      // We can only check this if all the results are returned.
      // As such, we set the limit quite high above and if we have
      // less then the limit, we will go ahead with the test.
      // @note: this tests all fields on TravisCI since there is no pre-existing data.
      if (sizeof($values) < 200) {
        $this->assertContains($fake_value, $values, "\nThe following array should but does not contain our fake value ('$fake_value'): '" . implode("', '", $values) . '.');
      }
    }

  }

  /**
   * DataProvider: a list of fields who store their data in the base table of a
   * bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle
   * stores it's data in.
   *       - base_schema: the Tripal Schema array for the bundle_base_table.
   */
  public function getBaseFields() {

    // Retrieve a list of fields to test.
    // Note: this list is cached to improve performance.
    $fields = $this->retrieveFieldList();

    return $fields['field_chado_storage']['base'];
  }

  /**
   * Test for fields based on columns in the base table that are also foreign
   * keys.
   *
   * @dataProvider getBaseFkFields
   * @group current
   * @group fields
   * @group getValueList
   */
  public function testBaseTableForeignKey($field_name, $bundle_name, $info) {
    include_once(drupal_get_path('tripal_chado', 'module') . '/includes/TripalFields/ChadoField.inc');

    // Construct the Field instance we want the values for.
    // Specifying "ChadoField" here ensures we are only testing our
    // implementation of getValueList() and not the custom version for any
    // given field.
    // YOU SHOULD TEST CUSTOM FIELD IMPLEMENTATIONS SEPARATELY.
    $instance = new \ChadoField($info['field_info'], $info['instance_info']);

    // Retrieve the values using defaults.
    // $values will be an array containing the distinct set of values for this field instance.
    $values = $instance->getValueList(['limit' => 5]);

    // Ensure we have values returned!
    $this->assertTrue(
      is_array($values),
      t(
        'No values returned for @field_name with no label string set (bundle: @bundle_name, bundle base table: @bundle_base_table, chado table: @chado_table, chado column: @chado_column).',
        [
          '@field_name' => $field_name,
          '@bundle_name' => $bundle_name,
          '@bundle_base_table' => $info['bundle_base_table'],
          '@chado_table' => $info['instance_info']['settings']['chado_table'],
          '@chado_column' => $info['instance_info']['settings']['chado_column'],
        ]
      )
    );

    // Ensure there are no more then 5 as specified in the limit above.
    $this->assertLessThanOrEqual(5, sizeof($values),
      t('Returned too many results for @field_name.', ['@field_name' => $field_name]));

    // Ensure it works with a label string set.
    // Ensure a known value is in the list.
    // Note: The following generates fake data with a fixed value for the column this
    // field is based on. This allows us to check that fixed value is one of those
    // returned by ChadoField->getValueList().
    $fake_fk_record = $this->generateFakeData($info['bundle_base_table'], $info['base_schema'], $info['instance_info']['settings']['chado_column'], $info['fk_table']);
    if ($fake_fk_record !== FALSE) {

      // We also want to test the label string functionality.
      // Grab two columns at random from the related table...
      $schema = chado_get_schema($info['fk_table']);
      $fk_table_fields = array_keys($schema['fields']);
      $use_in_label = array_rand($fk_table_fields, 2);
      $column1 = $fk_table_fields[$use_in_label[0]];
      $column2 = $fk_table_fields[$use_in_label[1]];
      // The label string consists of tokens of the form [column_name].
      $label_string = '[' . $column2 . '] ([' . $column1 . '])';

      // Re-generate the values...
      $values = $instance->getValueList([
        'limit' => 200,
        'label_string' => $label_string,
      ]);

      // And ensure our fake value is in the returned list.
      // We can only check this if all the results are returned.
      // As such, we set the limit quite high above and if we have
      // less then the limit, we will go ahead with the test.
      // @note: this tests all fields on TravisCI since there is no pre-existing data.
      if (sizeof($values) < 200) {
        $fixed_key = $fake_fk_record->{$info['fk_table'] . '_id'};
        $this->assertArrayHasKey($fixed_key, $values, "\nThe following array should but does not contain our fake record: " . print_r($fake_fk_record, TRUE));

        // Now test the label of the fake record option is what we expect
        // based on the label string we set above.
        $expected_label = $fake_fk_record->{$column2} . ' (' . $fake_fk_record->{$column1} . ')';
        $this->assertEquals($expected_label, $values[$fixed_key], "\nThe label should have been '$label_string' with the column values filled in.");
      }
    }

  }

  /**
   * DataProvider: a list of fields who store their data in the base table of a
   * bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle
   * stores it's data in.
   *       - base_schema: the Tripal Schema array for the bundle_base_table.
   */
  public function getBaseFkFields() {

    // Retrieve a list of fields to test.
    // Note: this list is cached to improve performance.
    $fields = $this->retrieveFieldList();

    return $fields['field_chado_storage']['foreign key'];
  }

  /**
   * Test for fields based on tables besides the base one for the bundle.
   * CURRENTLY RETRIEVING VALUES FOR THESE TABLES IS NOT SUPPORTED.
   *
   * @dataProvider getNonBaseFields
   *
   * @group fields
   * @group getValueList
   */
  public function testNonBaseTable($field_name, $bundle_name, $info) {
    include_once(drupal_get_path('tripal_chado', 'module') . '/includes/TripalFields/ChadoField.inc');

    // Construct the Field instance we want the values for.
    // Specifying "ChadoField" here ensures we are only testing our
    // implementation of getValueList() and not the custom version for any
    // given field.
    // YOU SHOULD TEST CUSTOM FIELD IMPLEMENTATIONS SEPARATELY.
    $instance = new \ChadoField($info['field_info'], $info['instance_info']);

    // Supress tripal errors
    putenv("TRIPAL_SUPPRESS_ERRORS=TRUE");
    ob_start();

    try {

      // Retrieve the values.
      // $values will be an array containing the distinct set of values for this field instance.
      $values = $instance->getValueList(['limit' => 5]);

      // @todo Check that we got the correct warning message.
      // Currently we can't check this because we need to supress the error in order to keep it from printing
      // but once we do, we can't access it ;-P

    } catch (Exception $e) {
      $this->fail("Although we don't support values lists for $field_name, it still shouldn't produce an exception!");
    }

    // Clean the buffer and unset tripal errors suppression
    ob_end_clean();
    putenv("TRIPAL_SUPPRESS_ERRORS");

    $this->assertFalse($values, "We don't support retrieving values for $field_name since it doesn't store data in the base table.");

  }

  /**
   * DataProvider: a list of fields who store their data in the base table of a
   * bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle
   * stores it's data in.
   *       - base_schema: the Tripal Schema array for the bundle_base_table.
   */
  public function getNonBaseFields() {

    // Retrieve a list of fields to test.
    // Note: this list is cached to improve performance.
    $fields = $this->retrieveFieldList();

    return $fields['field_chado_storage']['referring'];
  }

  /**
   * Returns a list of Fields sorted by their backend, etc. for use in tests.
   */
  private function retrieveFieldList() {
    if ($this->field_list === NULL) {

      $this->field_list = [];

      // field_info_instances() retrieves a list of all the field instances in the current site,
      // indexed by the bundle it is attached to.
      // @todo use fake bundles here to make these tests less dependant upon the current site.
      $bundles = field_info_instances('TripalEntity');
      foreach ($bundles as $bundle_name => $fields) {

        // Load the bundle object to later determine the chado table.
        $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

        // For each field instance...
        foreach ($fields as $field_name => $instance_info) {
          $bundle_base_table = $base_schema = NULL;

          // Load the field info.
          $field_info = field_info_field($field_name);

          // Determine the storage backend.
          $storage = $field_info['storage']['type'];

          // If this field stores it's data in chado...
          // Determine the relationship between this field and the bundle base table.
          $rel = NULL;
          if ($storage == 'field_chado_storage') {

            // We need to know the table this field stores it's data in.
            $bundle_base_table = $bundle->data_table;
            // and the schema for that table.
            $base_schema = chado_get_schema($bundle_base_table);
            // and the table this field stores it's data in.
            $field_table = $instance_info['settings']['chado_table'];
            $field_column = $instance_info['settings']['chado_column'];

            // By default we simply assume there is some relationship.
            $rel = 'referring';
            $rel_table = NULL;
            // If the field and bundle store their data in the same table
            // then it's either a "base" or "foreign key" relationship
            // based on the schema.
            if ($bundle_base_table == $field_table) {

              // We assume it's not a foreign key...
              $rel = 'base';
              // and then check the schema to see if we're wrong :-)
              foreach ($base_schema['foreign keys'] as $schema_info) {
                if (isset($schema_info['columns'][$field_column])) {
                  $rel = 'foreign key';
                  $rel_table = $schema_info['table'];
                }
              }
            }
          }

          // Store all the info about bundle, field, instance, schema for use in the test.
          $info = [
            'field_name' => $field_name,
            'bundle_name' => $bundle_name,
            'bundle' => $bundle,
            'bundle_base_table' => $bundle_base_table,
            'base_schema' => $base_schema,
            'field_info' => $field_info,
            'instance_info' => $instance_info,
            'fk_table' => $rel_table,
          ];

          // Create a unique key.
          $key = $bundle_name . '--' . $field_name;

          // If this bundle uses chado and we know the fields relationship to the base
          // chado table, then we want to index the field list by that relationship.
          if ($rel) {
            $this->field_list[$storage][$rel][$key] = [
              $field_name,
              $bundle_name,
              $info,
            ];
          }
          else {
            $this->field_list[$storage][$key] = [
              $field_name,
              $bundle_name,
              $info,
            ];
          }

        }
      }
    }

    return $this->field_list;
  }

  /**
   * Generate fake data for a given bundle.
   *
   * If only the first parameter is provided this function adds fake data to
   * the indicated chado table. If the third parameter is provided the
   * generated fake data will have a fixed value for the indicated column.
   *
   * @return
   *   Returns FALSE if it was unable to create fake data.
   */
  private function generateFakeData($chado_table, $schema, $fixed_column = FALSE, $fk_table = FALSE) {
    $faker = \Faker\Factory::create();

    // First, do we have a factory? We can't generate data without one...
    if (!Factory::exists('chado.' . $chado_table)) {
      return FALSE;
    }

    // Create fake data -TripalTestSuite will use faker for all values.
    if ($fixed_column === FALSE) {
      factory('chado.' . $chado_table, 50)->create();
      return TRUE;
    }


    // Attempt to create a fixed fake value.
    // This needs to match the column type in the chado table and if the column is a
    // foreign key, this value should match a fake record in the related table.
    $fake_value = NULL;

    // If we weren't told the related table then we assume this is a simple column (not a foreign key).
    if ($fk_table === FALSE) {
      $column_type = $schema[$fixed_column]['type'];
      if (($column_type == 'int')) {
        $fake_value = $faker->randomNumber();
      }
      elseif (($column_type == 'varchar') OR ($column_type == 'text')) {
        $fake_value = $faker->words(2, TRUE);
      }

      if ($fake_value !== NULL) {
        factory('chado.' . $chado_table)->create([
          $fixed_column => $fake_value,
        ]);
        return $fake_value;
      }
    }
    // Otherwise, we need to create a fixed fake record in the related table and then
    // use it in our fake data for the chado table.
    else {
      // Create our fixed fake record in the related table.
      $fake_table_record = factory('chado.' . $fk_table)->create();

      // Again, if we don't have a factory :-( there's nothing we can do.
      if (!Factory::exists('chado.' . $fk_table)) {
        return FALSE;
      }

      // Now create our fake records.
      if (isset($fake_table_record->{$fk_table . '_id'})) {
        factory('chado.' . $chado_table)->create([
          $fixed_column => $fake_table_record->{$fk_table . '_id'},
        ]);

        return $fake_table_record;
      }
    }

    return FALSE;

  }
}
