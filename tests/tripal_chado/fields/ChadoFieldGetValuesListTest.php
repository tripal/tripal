<?php
namespace Tests\tripal_chado\fields;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

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
    $values = $instance->getValueList(array('limit' => 5));

    // Ensure we have values returned!
    $this->assertTrue(
      is_array($values),
      t(
        'No values returned for @field_name (bundle: @bundle_name, bundle base table: @bundle_base_table, chado table: @chado_table, chado column: @chado_column).',
        array(
          '@field_name' => $field_name,
          '@bundle_name' => $bundle_name,
          '@bundle_base_table' => $info['bundle_base_table'],
          '@chado_table' => $info['instance_info']['settings']['chado_table'],
          '@chado_column' => $info['instance_info']['settings']['chado_column'],
        )
      )
    );

    // Ensure there are no more then 5 as specified in the limit above.
    $this->assertLessThanOrEqual(5, sizeof($values),
      t('Returned too many results for @field_name.', array('@field_name' => $field_name)));

    // @todo Ensure a known value is in the list.
    // Note: This requires insertion of data using factories. However, there are not yet
    // factories for all chado tables and I don't know how to test if there is one for the
    // current bundle base table. See issue statonlab/TripalTestSuite#92

  }

  /**
   * DataProvider: a list of fields who store their data in the base table of a bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle stores it's data in.
   *       - base_schema: the Tripal Schema array for the bundle_base_table.
   */
  public function getBaseFields() {

    // Retrieve a list of fields to test.
    // Note: this list is cached to improve performance.
    $fields = $this->retrieveFieldList();

    return $fields['field_chado_storage']['base'];
  }

  /**
   * Test for fields based on columns in the base table that are also foreign keys.
   *
   * @dataProvider getBaseFkFields
   *
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
    $values = $instance->getValueList(array('limit' => 5));

    // Ensure we have values returned!
    $this->assertTrue(
      is_array($values),
      t(
        'No values returned for @field_name with no label string set (bundle: @bundle_name, bundle base table: @bundle_base_table, chado table: @chado_table, chado column: @chado_column).',
        array(
          '@field_name' => $field_name,
          '@bundle_name' => $bundle_name,
          '@bundle_base_table' => $info['bundle_base_table'],
          '@chado_table' => $info['instance_info']['settings']['chado_table'],
          '@chado_column' => $info['instance_info']['settings']['chado_column'],
        )
      )
    );

    // Ensure there are no more then 5 as specified in the limit above.
    $this->assertLessThanOrEqual(5, sizeof($values),
      t('Returned too many results for @field_name.', array('@field_name' => $field_name)));

    // @todo Ensure it works with a label string set.
    // @todo Ensure a known value is in the list.
    // Note: This requires insertion of data using factories. However, there are not yet
    // factories for all chado tables and I don't know how to test if there is one for the
    // current bundle base table. See issue statonlab/TripalTestSuite#92

  }

  /**
   * DataProvider: a list of fields who store their data in the base table of a bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle stores it's data in.
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
    $values = $instance->getValueList(array('limit' => 5));

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
   * DataProvider: a list of fields who store their data in the base table of a bundle.
   *
   * Each element describes a field instance and consists of:
   *   - the machine name of the field (e.g. obi__organism).
   *   - the machine name of the bundle (e.g. bio_data_17).
   *   - an array of additional information including:
   *       - instance_info: information about the field instance.
   *       - field_info: information about the field.
   *       - bundle: the TripalBundle object.
   *       - bundle_base_table: if applicable, the chado base table the bundle stores it's data in.
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

      $this->field_list = array();

      // field_info_instances() retrieves a list of all the field instances in the current site,
      // indexed by the bundle it is attached to.
      // @todo use fake bundles here to make these tests less dependant upon the current site.
      $bundles = field_info_instances('TripalEntity');
      foreach($bundles as $bundle_name => $fields) {

        // Load the bundle object to later determine the chado table.
        $bundle = tripal_load_bundle_entity(array('name'=> $bundle_name));

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
            // If the field and bundle store their data in the same table
            // then it's either a "base" or "foreign key" relationship
            // based on the schema.
            if ($bundle_base_table == $field_table) {

              // We assume it's not a foreign key...
              $rel = 'base';
              // and then check the schema to see if we're wrong :-)
              foreach ($base_schema['foreign keys'] as $schema_info) {
                if (isset($schema_info['columns'][ $field_column ])) { $rel = 'foreign key'; }
              }
            }
          }

          // Store all the info about bundle, field, instance, schema for use in the test.
          $info = array(
            'field_name' => $field_name,
            'bundle_name' => $bundle_name,
            'bundle' => $bundle,
            'bundle_base_table' => $bundle_base_table,
            'base_schema' => $base_schema,
            'field_info' => $field_info,
            'instance_info' => $instance_info,
          );

          // Create a unique key.
          $key = $bundle_name . '--' . $field_name;

          // If this bundle uses chado and we know the fields relationship to the base
          // chado table, then we want to index the field list by that relationship.
          if ($rel) {
            $this->field_list[$storage][$rel][$key] = array(
              $field_name,
              $bundle_name,
              $info
            );
          }
          else {
            $this->field_list[$storage][$key] = array(
              $field_name,
              $bundle_name,
              $info
            );
          }

        }
      }
    }

    return $this->field_list;
  }

}
