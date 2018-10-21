<?php
namespace Tests\tripal_chado\fields;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('php', 'tripal_chado', '../tests/TripalFieldTestHelper');

class sbo__relationship_widgetTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Data Provider: provides entities matching important test cases.
   *
   * Specifically, we will cover three relationship tables, which represent
   * the diversity in the chado schema v1.3:
   *  organism_relationship: subject_id, type_id, object_id,
   *  stock_relationship: subject_id, type_id, object_id, value, rank,
   *  project_relationship: subject_project_id, type_id, object_project_id, rank
   *
   * @returns
   *   Returns an array where each item to be tested has the paramaters
   *   needed for initializeWidgetClass(). Specfically, $bundle_name,
   *   $field_name, $widget_name, $entity_id.
   */
  public function provideEntities() {
     $data = [];

     foreach (['organism', 'stock', 'project'] as $base_table) {

       $field_name = 'sbo__relationship';
       $widget_name = 'sbo__relationship_widget';

       // find a bundle which stores it's data in the given base table.
       // This will work on Travis since Tripal creates matching bundles by default.
       // @todo ideally we would create a fake bundle here.
       $bundle_id = db_query("
         SELECT bundle_id
         FROM chado_bundle b
         LEFT JOIN tripal_entity e ON e.bundle='bio_data_'||b.bundle_id
         WHERE data_table=:table AND id IS NOT NULL LIMIT 1",
           array(':table' => $base_table))->fetchField();

       if (!$bundle_id) {
         continue;
       }

       $bundle_name = 'bio_data_'.$bundle_id;

       // Find an entity from the above bundle.
       // @todo find a way to create a fake entity for use here.
       $entity_id = db_query('SELECT id FROM tripal_entity WHERE bundle=:bundle LIMIT 1',
         array(':bundle' => $bundle_name))->fetchField();

       // set variables to guide testing.
       $expect = [
         'has_rank' => TRUE,
         'has_value' => FALSE,
         'subject_key' => 'subject_id',
         'object_key' => 'object_id',
         'base_table' => $base_table,
         'relationship_table' => $base_table.'_relationship'
       ];
       if ($base_table == 'organism') { $expect['has_rank'] = FALSE; }
       if ($base_table == 'stock') { $expect['has_value'] = TRUE; }
       if ($base_table == 'project') {
         $expect['subject_key'] = 'subject_project_id';
         $expect['object_key'] = 'object_project_id';
       }

       $data[] = [$bundle_name, $field_name, $widget_name, $entity_id, $expect];
     }
     return $data;
  }

  /**
   * Test that we can initialize the widget properly.
   *
   * @dataProvider provideEntities()
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetClassInitialization($bundle_name, $field_name, $widget_name, $entity_id, $expect) {

    // Load the entity.
    $entity = entity_load('TripalEntity', [$entity_id]);
    $entity = $entity[$entity_id];

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = array(
      'field_name' => $field_name,
      'widget_name' => $widget_name,
    );
    $field_info = field_info_field($field_name);
    $instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);
    $helper = new \TripalFieldTestHelper($bundle_name, $machine_names, $entity, $field_info, $instance_info);
    $widget_class = $helper->getInitializedClass();

    // Check we have the variables we initialized.
    $this->assertNotEmpty($helper->bundle,
      "Could not load the bundle.");
    $this->assertNotEmpty($helper->getFieldInfo(),
      "Could not lookup the field information.");
    $this->assertNotEmpty($helper->getInstanceInfo(),
      "Could not lookup the instance information.");
    $this->assertNotEmpty($widget_class,
      "Couldn't create a widget class instance.");
    $this->assertNotEmpty($entity,
      "Couldn't load an entity.");

    // Check a little deeper...
    $this->assertEquals($helper->instance_info['settings']['chado_table'], $expect['relationship_table'],
      "Instance settings were not initialized fully.");

  }

  /**
   * Test the widget Form.
   *
   * @dataProvider provideEntities()
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetForm($bundle_name, $field_name, $widget_name, $entity_id, $expect) {

    // Load the entity.
    $entity = entity_load('TripalEntity', [$entity_id]);
    $entity = $entity[$entity_id];

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = array(
      'field_name' => $field_name,
      'widget_name' => $widget_name,
    );
    $field_info = field_info_field($field_name);
    $instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);
    $helper = new \TripalFieldTestHelper($bundle_name, $machine_names, $entity, $field_info, $instance_info);
    $widget_class = $helper->getInitializedClass();

    $base_table = $entity->chado_table;

    // Stub out a fake objects.
    $delta = 1;
    $langcode = LANGUAGE_NONE;
    $widget = $helper->mockElement($delta, $langcode);
    $form = $helper->mockForm($delta, $langcode);
    $form_state = $helper->mockFormState($delta, $langcode);
    $element = $helper->mockElement($delta, $langcode);

    $items = [
      'value' => '',
      'chado-'.$base_table.'_relationship__organism_relationship_id' => '',
      'chado-'.$base_table.'_relationship__subject_id' => '',
      'chado-'.$base_table.'_relationship__object_id' => '',
      'chado-'.$base_table.'_relationship__type_id' => '',
      'object_name' => '',
      'subject_name' => '',
      'type_name' => '',
    ];

    // Execute the form method.
    $widget_class->form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // Check the resulting for array
    $this->assertArrayHasKey('subject_name', $widget,
      "The form for $bundle_name($base_table) does not have a subject element.");
    $this->assertArrayHasKey('type_name', $widget,
      "The form for $bundle_name($base_table) does not have a type element.");
    $this->assertArrayHasKey('object_name', $widget,
      "The form for $bundle_name($base_table) does not have a object element.");

    // Check the subject/object keys were correctly determined.
    $this->assertEquals($expect['subject_key'], $widget['#subject_id_key'],
      "The form didn't determine the subject key correctly.");
    $this->assertEquals($expect['object_key'], $widget['#object_id_key'],
      "The form didn't determine the object key correctly.");

  }

  /**
   * DataProvider: Provides datasets to validate.
   */
  public function provideThings2Validate() {
    $data = [];

    foreach (['organism', 'stock', 'project'] as $base_table) {

      $base_table = $base_table;
      $field_name = 'sbo__relationship';
      $widget_name = 'sbo__relationship_widget';

      // find a bundle which stores it's data in the given base table.
      // This will work on Travis since Tripal creates matching bundles by default.
      // @todo ideally we would create a fake bundle here.
      $bundle_id = db_query("
        SELECT bundle_id
        FROM chado_bundle b
        LEFT JOIN tripal_entity e ON e.bundle='bio_data_'||b.bundle_id
        WHERE data_table=:table AND id IS NOT NULL LIMIT 1",
          array(':table' => $base_table))->fetchField();

      if (!$bundle_id) {
        continue;
      }
      $bundle_name = 'bio_data_'.$bundle_id;

      // set variables to guide testing.
      $expect = [
        'has_rank' => TRUE,
        'has_value' => FALSE,
        'subject_key' => 'subject_id',
        'object_key' => 'object_id',
        'base_table' => $base_table,
        'relationship_table' => $base_table.'_relationship',
      ];
      if ($base_table == 'organism') { $expect['has_rank'] = FALSE; }
      if ($base_table == 'stock') { $expect['has_value'] = TRUE; }
      if ($base_table == 'project') {
        $expect['subject_key'] = 'subject_project_id';
        $expect['object_key'] = 'object_project_id';
      }

      // Create 5 fake records and publish them.
      $records = factory('chado.'.$base_table, 5)->create();
      $this->publish($base_table);

      $cvterm = factory('chado.cvterm')->create();

      // Find an entity from the above bundle.
      $ids = db_query('SELECT id FROM {tripal_entity} WHERE bundle=:bundle LIMIT 2',
        array(':bundle' => $bundle_name))->fetchCol();
        $entities = entity_load('TripalEntity', $ids);
        $entity = array_pop($entities);
        $entity2 = array_pop($entities);

      // Now Build our test cases for this base table.
      foreach (['user_create', 'existing', 'no_subject', 'no_object', 'no_type'] as $case) { 
        $expect['test_case'] = $case;

        // First assume "existing" (later we will modify based on case).
        $values = [
          'subject_name' => $entity2->chado_record->name,
          'type_name' => $cvterm->name,
          'vocabulary' => $cvterm->cv_id,
          'object_name' => $entity->chado_record->name,
          // Both the form and load set the chado values
          // so we will set them here as well.
          'chado-'.$base_table.'_relationship__'.$expect['subject_key'] => $entity2->chado_record->{$base_table.'_id'},
          'chado-'.$base_table.'_relationship__type_id' => $cvterm->cvterm_id,
          'chado-'.$base_table.'_relationship__'.$expect['object_key'] => $entity->chado_record->{$base_table.'_id'},
        ];
        if ($base_table == 'organism') {
          $values['subject_name'] = $entity2->chado_record->species;
          $values['object_name'] = $entity->chado_record->species;
        }

        $expect['num_errors'] = 0;

        // Now modify based on the case.
        switch ($case) {
          case 'user_create':
            $values[ 'chado-'.$base_table.'_relationship__'.$expect['subject_key'] ] = NULL;
            $values[ 'chado-'.$base_table.'_relationship__type_id' ] = NULL;
            $values[ 'chado-'.$base_table.'_relationship__'.$expect['object_key'] ] = NULL;
            break;
          case 'no_subject':
            $values['subject_name'] = '';
            $values[ 'chado-'.$base_table.'_relationship__'.$expect['subject_key'] ] = NULL;
            $expect['num_errors'] = 1;
            break;
          case 'no_object':
            $values['object_name'] = '';
            $values[ 'chado-'.$base_table.'_relationship__'.$expect['object_key'] ] = NULL;
            $expect['num_errors'] = 1;
            break;
          case 'no_type':
            $values['type_name'] = '';
            $values['vocabulary'] = NULL;
            $values[ 'chado-'.$base_table.'_relationship__type_id' ] = NULL;
            $expect['num_errors'] = 1;
            break;
        }

        $data[] = [
          [
            'field_name' => $field_name,
            'widget_name' => $widget_name,
            'bundle_id' => $bundle_id,
            'bundle_name' => $bundle_name,
          ],
          $entity,
          $values,
          $expect,
        ];
      }
    }

    return $data;
  }

  /**
   * Test sbo__relationship_widget->validate().
   *
   * @dataProvider provideThings2Validate()
   *
   * @group lacey-wip
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetValidate($info, $entity, $initial_values, $expect) {

    $base_table = $entity->chado_table;

    // Initialize the widget class via the TripalFieldTestHelper class.
    $machine_names = array(
      'field_name' => $info['field_name'],
      'widget_name' => $info['widget_name'],
    );
    $field_info = field_info_field($info['field_name']);
    $instance_info = field_info_instance('TripalEntity', $info['field_name'], $info['bundle_name']);
    $helper = new \TripalFieldTestHelper($info['bundle_name'], $machine_names, $entity, $field_info, $instance_info);
    $widget_class = $helper->getInitializedClass();

    // Mock objects.
    $field_name = $info['field_name'];
    $delta = 1;
    $langcode = LANGUAGE_NONE;
    $widget = $helper->mockElement($delta, $langcode);
    $form = $helper->mockForm($delta, $langcode);
    $form_state = $helper->mockFormState($delta, $langcode, $initial_values);
    $element = $helper->mockElement($delta, $langcode);

    $widget_class->validate($element, $form, $form_state, $langcode, $delta);

    // @debug print_r($form_state['values'][$field_name][$langcode][$delta]);

    // Ensure the chado-table__column entries are there.
    $this->assertArrayHasKey(
      'chado-'.$base_table.'_relationship__'.$expect['subject_key'],
      $form_state['values'][$field_name][$langcode][$delta],
      'Failed to find the subject_id in the processed values (Base: '.$base_table.'). This implies the validate function was not able to validate the subject.'
    );
    $this->assertArrayHasKey(
      'chado-'.$base_table.'_relationship__'.$expect['object_key'],
      $form_state['values'][$field_name][$langcode][$delta],
      'Failed to find the object_id in the processed values (Base: '.$base_table.'). This implies the validate function was not able to validate the object.'
    );
    $this->assertArrayHasKey(
      'chado-'.$base_table.'_relationship__type_id',
      $form_state['values'][$field_name][$langcode][$delta],
      'Failed to find the type_id in the processed values (Base: '.$base_table.'). This implies the validate function was not able to validate the type.'
    );

    // Check for errors.
    $errors = form_get_errors();
    // @debug print "Errors: " . print_r($errors, TRUE)."\n";

    if ($expect['num_errors'] === 0) {
      $this->assertEmpty($errors,
        "There should be no form errors for the following initial values: ".print_r($initial_values,TRUE)." But these were registered: ".print_r($errors, TRUE));
    }
    else {
      $this->assertEquals($expect['num_errors'], sizeof($errors),
        "The number of errors didn't match what we expected for the following initial values: ".print_r($initial_values,TRUE)." Here are the errors: ".print_r($errors, TRUE));
    }

    // Clean up after ourselves by removing any errors we logged.
    form_clear_error();
  }
}
