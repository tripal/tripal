<?php

namespace Tests\tripal_chado\fields;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
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
   *  project_relationship: subject_project_id, type_id, object_project_id,
   *  rank.
   *
   * @returns
   *   Returns an array where each item to be tested has the paramaters
   *   needed for initializeWidgetClass(). Specfically, $bundle_name,
   *   $field_name, $widget_name, $entity_ids, $expect.
   */
  public function provideEntities() {
    $data = [];

    foreach (['organism', 'stock', 'project'] as $base_table) {

      $field_name = 'sbo__relationship';
      $widget_name = 'sbo__relationship_widget';

      // Find a bundle which stores it's data in the given base table.
      // This will work on Travis since Tripal creates matching bundles by default.
      $bundle_details = db_query(
        "
         SELECT bundle_id, type_column, type_id
         FROM chado_bundle b
         WHERE data_table=:table AND type_linker_table=''
         ORDER BY bundle_id ASC LIMIT 1", [':table' => $base_table]
      )->fetchObject();
      if (isset($bundle_details->bundle_id)) {
        $bundle_id = $bundle_details->bundle_id;
      }
      else {
        continue;
      }

      $bundle_name = 'bio_data_' . $bundle_id;

      // Create some entities so that we know there are some available to find.
      if ($bundle_details->type_column == 'type_id') {
        $chado_records = factory('chado.' . $base_table, 2)->create(
          ['type_id' => $bundle_details->type_id]
        );
      }
      else {
        $chado_records = factory('chado.' . $base_table, 2)->create();
      }
      // Then publish them so we have entities.
      $this->publish($base_table);

      // Find our fake entities from the above bundle.
      $entity_ids = [];
      $entity_ids[] = db_query(
        'SELECT entity_id FROM chado_' . $bundle_name
        . ' WHERE record_id=:chado_id',
        [':chado_id' => $chado_records[0]->{$base_table . '_id'}]
      )->fetchField();
      $entity_ids[] = db_query(
        'SELECT entity_id FROM chado_' . $bundle_name
        . ' WHERE record_id=:chado_id',
        [':chado_id' => $chado_records[1]->{$base_table . '_id'}]
      )->fetchField();

      // Set variables to guide testing.
      $expect = [
        'has_rank' => TRUE,
        'has_value' => FALSE,
        'subject_key' => 'subject_id',
        'object_key' => 'object_id',
        'base_table' => $base_table,
        'relationship_table' => $base_table . '_relationship',
      ];
      if ($base_table == 'organism') {
        $expect['has_rank'] = FALSE;
      }
      if ($base_table == 'stock') {
        $expect['has_value'] = TRUE;
      }
      if ($base_table == 'project') {
        $expect['subject_key'] = 'subject_project_id';
        $expect['object_key'] = 'object_project_id';
      }

      $data[] = [$bundle_name, $field_name, $widget_name, $entity_ids, $expect];
    }
    return $data;
  }

  /**
   * Test that we can initialize the widget properly.
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetClassInitialization() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      $entity_id = $entity_ids[0];

      // Load the entity.
      $entity = entity_load('TripalEntity', [$entity_id]);
      $entity = $entity[$entity_id];

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Check we have the variables we initialized.
      $this->assertNotEmpty(
        $helper->bundle, "Could not load the bundle."
      );
      $this->assertNotEmpty(
        $helper->getFieldInfo($field_name),
        "Could not lookup the field information."
      );
      $this->assertNotEmpty(
        $helper->getInstanceInfo($bundle_name, $field_name),
        "Could not lookup the instance information."
      );
      $this->assertNotEmpty(
        $widget_class, "Couldn't create a widget class instance."
      );
      $this->assertNotEmpty(
        $entity, "Couldn't load an entity."
      );

      // Check a little deeper...
      $this->assertEquals(
        $helper->instance_info['settings']['chado_table'],
        $expect['relationship_table'],
        "Instance settings were not initialized fully."
      );
    }
  }

  /**
   * Test the widget Form.
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetForm() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      $entity_id = $entity_ids[0];

      // Load the entity.
      $entity = entity_load('TripalEntity', [$entity_ids]);
      $entity = $entity[$entity_id];

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity, $field_info, $instance_info
      );
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
        'chado-' . $base_table . '_relationship__organism_relationship_id' => '',
        'chado-' . $base_table . '_relationship__subject_id' => '',
        'chado-' . $base_table . '_relationship__object_id' => '',
        'chado-' . $base_table . '_relationship__type_id' => '',
        'object_name' => '',
        'subject_name' => '',
        'type_name' => '',
      ];

      // Execute the form method.
      $widget_class->form(
        $widget, $form, $form_state, $langcode, $items, $delta, $element
      );

      // Check the resulting for array.
      $this->assertArrayHasKey(
        'subject_name', $widget,
        "The form for $bundle_name($base_table) does not have a subject element."
      );
      $this->assertArrayHasKey(
        'type_name', $widget,
        "The form for $bundle_name($base_table) does not have a type element."
      );
      $this->assertArrayHasKey(
        'object_name', $widget,
        "The form for $bundle_name($base_table) does not have a object element."
      );

      // Check the subject/object keys were correctly determined.
      $this->assertEquals(
        $expect['subject_key'], $widget['#subject_id_key'],
        "The form didn't determine the subject key correctly."
      );
      $this->assertEquals(
        $expect['object_key'], $widget['#object_id_key'],
        "The form didn't determine the object key correctly."
      );
    }
  }

  /**
   * Case: WidgetValidate on existing relationship.
   *
   * @group        widget
   * @group        sbo__relationship
   */
  public function testWidgetValidate_existing() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      // Load the entities.
      $entities = entity_load('TripalEntity', $entity_ids);
      $entity1 = $entities[$entity_ids[0]];
      $entity2 = $entities[$entity_ids[1]];
      $base_table = $entity1->chado_table;

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity1, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Set some initial values.
      $cvterm = factory('chado.cvterm')->create();
      $initial_values = [
        'subject_name' => $entity2->chado_record->name,
        'type_name' => $cvterm->name,
        'vocabulary' => $cvterm->cv_id,
        'object_name' => $entity1->chado_record->name,
        // Both the form and load set the chado values
        // so we will set them here as well.
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'] => $entity2->chado_record->{$base_table . '_id'},
        'chado-' . $base_table . '_relationship__type_id' => $cvterm->cvterm_id,
        'chado-' . $base_table . '_relationship__' . $expect['object_key'] => $entity1->chado_record->{$base_table . '_id'},
      ];
      if ($base_table == 'organism') {
        $initial_values['subject_name'] = $entity2->chado_record->species;
        $initial_values['object_name'] = $entity1->chado_record->species;
      }

      // Mock objects.
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
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the subject_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the subject.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__' . $expect['object_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the object_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the object.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__type_id',
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the type_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the type.'
      );

      // Check for errors.
      $errors = form_get_errors();
      // @debug print "Errors: " . print_r($errors, TRUE)."\n";

      $this->assertEmpty(
        $errors,
        "There should be no form errors when subject and object are pre-existing and both are supplied. Initial values: "
        . print_r($initial_values, TRUE) . " But these were registered: "
        . print_r($errors, TRUE)
      );

      // Clean up after ourselves by removing any errors we logged.
      form_clear_error();
    }
  }

  /**
   * Case: WidgetValidate on new relationship filled out properly.
   *
   * @group        widget
   * @group        sbo__relationship
   */
  public function testWidgetValidate_create() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      // Load the entities.
      $entities = entity_load('TripalEntity', $entity_ids);
      $entity1 = $entities[$entity_ids[0]];
      $entity2 = $entities[$entity_ids[1]];
      $base_table = $entity1->chado_table;

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity1, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Set some initial values.
      $cvterm = factory('chado.cvterm')->create();
      $initial_values = [
        'subject_name' => $entity2->chado_record->name,
        'type_name' => $cvterm->name,
        'vocabulary' => $cvterm->cv_id,
        'object_name' => $entity1->chado_record->name,
        // These are not set on the creation form.
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'] => NULL,
        'chado-' . $base_table . '_relationship__type_id' => NULL,
        'chado-' . $base_table . '_relationship__' . $expect['object_key'] => NULL,
      ];
      if ($base_table == 'organism') {
        $initial_values['subject_name'] = $entity2->chado_record->species;
        $initial_values['object_name'] = $entity1->chado_record->species;
      }

      // Mock objects.
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
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the subject_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the subject.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__' . $expect['object_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the object_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the object.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__type_id',
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the type_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the type.'
      );

      // Check for errors.
      $errors = form_get_errors();
      // @debug print "Errors: " . print_r($errors, TRUE)."\n";

      $this->assertEmpty(
        $errors,
        "There should be no form errors when subject and object are pre-existing and both are supplied. Initial values: "
        . print_r($initial_values, TRUE) . " But these were registered: "
        . print_r($errors, TRUE)
      );

      // Clean up after ourselves by removing any errors we logged.
      form_clear_error();
    }
  }

  /**
   * Case: WidgetValidate on new relationship missing subject.
   *
   * @group        widget
   * @group        sbo__relationship
   */
  public function testWidgetValidate_nosubject() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      // Load the entities.
      $entities = entity_load('TripalEntity', $entity_ids);
      $entity1 = $entities[$entity_ids[0]];
      $entity2 = $entities[$entity_ids[1]];
      $base_table = $entity1->chado_table;

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity1, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Set some initial values.
      $cvterm = factory('chado.cvterm')->create();
      $initial_values = [
        'subject_name' => '',
        'type_name' => $cvterm->name,
        'vocabulary' => $cvterm->cv_id,
        'object_name' => $entity1->chado_record->name,
        // Both the form and load set the chado values
        // so we will set them here as well.
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'] => NULL,
        'chado-' . $base_table . '_relationship__type_id' => $cvterm->cvterm_id,
        'chado-' . $base_table . '_relationship__' . $expect['object_key'] => $entity1->chado_record->{$base_table . '_id'},
      ];
      if ($base_table == 'organism') {
        $initial_values['object_name'] = $entity1->chado_record->species;
      }

      // Mock objects.
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
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the subject_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the subject.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__' . $expect['object_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the object_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the object.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__type_id',
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the type_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the type.'
      );

      // Check for errors.
      $errors = form_get_errors();
      // @debug print "Errors: " . print_r($errors, TRUE)."\n";

      $this->assertNotEmpty(
        $errors,
        "There should be form errors when subject is not supplied. Initial values: "
        . print_r($initial_values, TRUE) . " But these were registered: "
        . print_r($errors, TRUE)
      );

      // Clean up after ourselves by removing any errors we logged.
      form_clear_error();
    }
  }

  /**
   * Case: WidgetValidate on new relationship missing object.
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetValidate_noobject() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      // Load the entities.
      $entities = entity_load('TripalEntity', $entity_ids);
      $entity1 = $entities[$entity_ids[0]];
      $entity2 = $entities[$entity_ids[1]];
      $base_table = $entity1->chado_table;

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity1, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Set some initial values.
      $cvterm = factory('chado.cvterm')->create();
      $initial_values = [
        'subject_name' => $entity2->chado_record->name,
        'type_name' => $cvterm->name,
        'vocabulary' => $cvterm->cv_id,
        'object_name' => '',
        // Both the form and load set the chado values
        // so we will set them here as well.
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'] => $entity2->chado_record->{$base_table . '_id'},
        'chado-' . $base_table . '_relationship__type_id' => $cvterm->cvterm_id,
        'chado-' . $base_table . '_relationship__' . $expect['object_key'] => NULL,
      ];
      if ($base_table == 'organism') {
        $initial_values['subject_name'] = $entity2->chado_record->species;
      }

      // Mock objects.
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
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the subject_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the subject.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__' . $expect['object_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the object_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the object.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__type_id',
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the type_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the type.'
      );

      // Check for errors.
      $errors = form_get_errors();
      // @debug print "Errors: " . print_r($errors, TRUE)."\n";

      $this->assertNotEmpty(
        $errors,
        "There should be form errors when the object is not supplied. Initial values: "
        . print_r($initial_values, TRUE) . " But these were registered: "
        . print_r($errors, TRUE)
      );

      // Clean up after ourselves by removing any errors we logged.
      form_clear_error();
    }
  }

  /**
   * Case: WidgetValidate on new relationship missing type.
   *
   * @group widget
   * @group sbo__relationship
   */
  public function testWidgetValidate_notype() {
    foreach ($this->provideEntities() as $dataEntry) {
      list(
        $bundle_name, $field_name, $widget_name, $entity_ids, $expect
        ) = $dataEntry;
      // Load the entities.
      $entities = entity_load('TripalEntity', $entity_ids);
      $entity1 = $entities[$entity_ids[0]];
      $entity2 = $entities[$entity_ids[1]];
      $base_table = $entity1->chado_table;

      // Initialize the widget class via the TripalFieldTestHelper class.
      $machine_names = [
        'field_name' => $field_name,
        'widget_name' => $widget_name,
      ];
      $field_info = field_info_field($field_name);
      $instance_info = field_info_instance(
        'TripalEntity', $field_name, $bundle_name
      );
      $helper = new \TripalFieldTestHelper(
        $bundle_name, $machine_names, $entity1, $field_info, $instance_info
      );
      $widget_class = $helper->getInitializedClass();

      // Set some initial values.
      $initial_values = [
        'subject_name' => $entity2->chado_record->name,
        'type_name' => '',
        'vocabulary' => NULL,
        'object_name' => $entity1->chado_record->name,
        // Both the form and load set the chado values
        // so we will set them here as well.
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'] => $entity2->chado_record->{$base_table . '_id'},
        'chado-' . $base_table . '_relationship__type_id' => NULL,
        'chado-' . $base_table . '_relationship__' . $expect['object_key'] => $entity1->chado_record->{$base_table . '_id'},
      ];
      if ($base_table == 'organism') {
        $initial_values['subject_name'] = $entity2->chado_record->species;
        $initial_values['object_name'] = $entity1->chado_record->species;
      }

      // Mock objects.
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
        'chado-' . $base_table . '_relationship__' . $expect['subject_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the subject_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the subject.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__' . $expect['object_key'],
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the object_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the object.'
      );
      $this->assertArrayHasKey(
        'chado-' . $base_table . '_relationship__type_id',
        $form_state['values'][$field_name][$langcode][$delta],
        'Failed to find the type_id in the processed values (Base: '
        . $base_table
        . '). This implies the validate function was not able to validate the type.'
      );

      // Check for errors.
      $errors = form_get_errors();
      // @debug print "Errors: " . print_r($errors, TRUE)."\n";

      $this->assertNotEmpty(
        $errors,
        "There should be form errors when type is not supplied. Initial values: "
        . print_r($initial_values, TRUE) . " But these were registered: "
        . print_r($errors, TRUE)
      );

      // Clean up after ourselves by removing any errors we logged.
      form_clear_error();
    }
  }
}
