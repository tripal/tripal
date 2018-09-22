<?php
namespace Tests\tripal_chado\fields;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class sbo__relationship_widgetTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Create a fake sbo__relationship_widget field?
   */
  private function initializeWidgetClass($bundle_name, $field_name, $widget_name, $entity_id) {
    $vars = [];
    $vars['bundle'] = $vars['field_info'] = $vars['instance_info'] = NULL;
    $vars['widget_class'] = $vars['entity'] = NULL;

    // First include the appropriate class.
    $widget_class_path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_chado')
      . '/includes/TripalFields/sbo__relationship/sbo__relationship_widget.inc';
    if ((include_once($widget_class_path)) == TRUE) {

      // Load the bundle and field/instance info.
      $vars['bundle'] = tripal_load_bundle_entity(array('name'=> $bundle_name));
      $vars['field_info'] = field_info_field($field_name);
      $vars['instance_info'] = field_info_instance('TripalEntity', $field_name, $bundle_name);

      // Create an instance of the widget class.
      $vars['widget_class'] = new \sbo__relationship_widget($this->field_info, $this->instance_info);

      // load an entity to pretend the widget is modifying.
      $vars['entity'] = entity_load('TripalEntity', [$entity_id]);
    }

    return $vars;
  }

  /**
   * Test that we can initialize the widget properly.
   */
  public function testWidgetClassInitialization() {

    // Initialize our variables.
    $vars = $this->initializeWidgetClass('bio_data_1', 'sbo__relationship', 'sbo__relationship_widget', 8);

    // Check we have the variables we initialized.
    $this->assertNotEmpty($vars['bundle'], "Could not load the bundle.");
    $this->assertNotEmpty($vars['field_info'], "Could not lookup the field information.");
    $this->assertNotEmpty($vars['instance_info'], "Could not lookup the instance informatiob.");
    $this->assertNotEmpty($vars['widget_class'], "Couldn't create a widget class instance.");
    $this->assertNotEmpty($vars['entity'], "Couldn't load an entity.");

  }

  /**
   * Test the widget Form.
   *
   * @group lacey
   */
  public function testWidgetForm() {

    $field_name = 'sbo__relationship';
    $bundle_name = 'bio_data_1';
    $widget_name = 'sbo__relationship_widget';
    $entity_id = 8;
    $vars = $this->initializeWidgetClass($bundle_name, $field_name, $widget_name, $entity_id);

    // Stub out a fake $widget object.
    $widget = [
      '#entity_type' => 'TripalEntity',
      '#entity' => $vars['entity'],
      '#bundle' => $vars['bundle'],
      '#field_name' => $field_name,
      '#language' => LANGUAGE_NONE,
      '#field_parents' => [],
      '#columns' => [],
      '#title' => '',
      '#description' => '',
      '#required' => FALSE,
      '#delta' => 0,
      '#weight' => 0, //same as delta.
      'value' => [
        '#type' => 'value',
        '#value' => '',
      ],
      '#field' => $vars['field_info'],
      '#instance' => $vars['instance_info'],
      '#theme' => 'tripal_field_default',
      'element_validate' => ['tripal_field_widget_form_validate']
    ];

    // Stub out the form and form_state.
    $form = [
      '#parents' => [],
      '#entity' => $vars['entity'],
    ];
    $form_state = [
      'build_info' => [
        'args' => [
          0 => NULL,
          1 => $vars['entity']
        ],
        'form_id' => 'tripal_entity_form',
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'temporary' => [],
      'submitted' => FALSE,
    ];

    // stub out the data for the field.
    $langcode = LANGUAGE_NONE;
    $items = [
      'value' => '',
      'chado-organism_relationship__organism_relationship_id' => '',
      'chado-organism_relationship__subject_id' => '',
      'chado-organism_relationship__object_id' => '',
      'chado-organism_relationship__type_id' => '',
      'chado-organism_relationship__rank' => '',
      'object_name' => '',
      'subject_name' => '',
      'type_name' => '',
    ];
    $delta = 0;

    // Stub out the widget element.
    $element = [
      '#entity_type' => 'TripalEntity',
      '#entity' => $vars['entity'],
      '#bundle' => $bundle_name,
      '#field_name' => $field_name,
      '#language' => LANGUAGE_NONE,
      '#field_parents' => [],
      '#columns' => [],
      '#title' => '',
      '#description' => '',
      '#required' => FALSE,
      '#delta' => 0,
      '#weight' => 0,
    ];

    // Execute the form method.
    $vars['widget_class']->form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // Check the resulting for array
    $this->assertArrayHasKey('subject_name', $widget, 'The form does not have a subject element.'); 
    $this->assertArrayHasKey('type_name', $widget, 'The form does not have a type element.');
    $this->assertArrayHasKey('object_name', $widget, 'The form does not have a object element.');

  }
}
