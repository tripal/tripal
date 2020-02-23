<?php

/**
 * This class can be included at the top of your TripalTestCase to facillitate
 * testing fields, widgets and formatters.
 */
class TripalFieldTestHelper {

  // This is for the initialized field, widget or formatter as indicated by
  // $machine_names in the constructor.
  public $initialized_class;

  // The name of the class being initialized.
  public $class_name;

  // Information for the field.
  public $field_info;

  // Information for the field instance.
  public $instance_info;

  // The loaded bundle the field is attached to.
  public $bundle;

  // The entity the field is attached to.
  public $entity;

  // The type of class we are initializing.
  // One of 'field', 'widget', or 'formatter'.
  public $type;

  // The name of the field for the class being tested.
  public $field_name;

  /**
   * Create an instance of TripalFieldTestHelper.
   *
   * Specifcally, initialize the widget class and save it for further testing.
   *
   * @param $bundle_name
   *   The name of the bundle the field should be attached to. This bundle must
   *   already exist.
   * @param $machine_names
   *   An array of machine names including:
   *    - field_name: the name of the field (REQUIRED)
   *    - widget_name: the name of the widget (Only required for widget
   *   testing)
   *    - formatter_name: the name of the formatter (Only required for
   *   formatter testing)
   * @param $field_info
   *   The Drupal information for the field you want to test.
   * @param $instance_info
   *   The Drupal information for the field instance you want to test.
   */
  public function __construct($bundle_name, $machine_names, $entity, $field_info, $instance_info) {


    // What type of class are we initializing?
    $this->type = 'field';
    $this->class_name = $machine_names['field_name'];
    if (isset($machine_names['widget_name'])) {
      $this->type = 'widget';
      $this->class_name = $machine_names['widget_name'];
    }
    elseif (isset($machine_names['formatter_name'])) {
      $this->type = 'formatter';
      $this->class_name = $machine_names['formatter_name'];
    }
    $this->field_name = $machine_names['field_name'];

    $class_name = '\\' . $this->class_name;
    $class_path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_chado')
      . '/includes/TripalFields/' . $machine_names['field_name'] . '/' . $this->class_name . '.inc';
    if ((include_once($class_path)) == TRUE) {

      // Save the field information.
      if (!$field_info) {
        $field_info = $this->getFieldInfo($machine_names['field_name']);
      }
      $this->field_info = $field_info;

      // Save the field instance information.
      if (!$instance_info) {
        $instance_info = $this->getFieldInfo($bundle_name, $machine_names['field_name']);
      }
      $this->instance_info = $instance_info;

      // Load the bundle.
      $this->bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

      // The entity from the specified bundle that the field should be attached to.
      $this->entity = $entity;


      // Initialize the class.
      $this->initialized_class = new $class_name($this->field_info, $this->instance_info);
    }

  }

  /**
   * Retrieve the initialized class for testing!
   */
  public function getInitializedClass() {
    return $this->initialized_class;
  }

  /**
   * Retrieve the field information for a given field.
   *
   * @see https://api.drupal.org/api/drupal/modules%21field%21field.info.inc/function/field_info_field/7.x
   *
   * @param $field_name
   *   The name of the field to retrieve. $field_name can only refer to a
   *   non-deleted, active field.
   *
   * @return
   *   The field array as returned by field_info_field() and used when
   *   initializing this class.
   */
  public function getFieldInfo($field_name) {

    if (empty($this->field_info)) {
      $this->field_info = field_info_field($field_name);
    }

    return $this->field_info;
  }

  /**
   * Retrieve the field instance information for a given field.
   *
   * @see https://api.drupal.org/api/drupal/modules%21field%21field.info.inc/function/field_info_instance/7.x
   *
   * @param $bundle_name
   *   The name of the bundle you want the field attached to. For example,
   *   bio_data_1.
   * @param $field_name
   *   The name of the field to retrieve the instance of. $field_name can only
   *   refer to a non-deleted, active field.
   *
   * @return
   *   The field instance array as returned by field_info_instance() and used
   *   when initializing this class.
   */
  public function getInstanceInfo($bundle_name, $field_name) {

    if (empty($this->instance_info)) {
      $this->instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);
    }

    return $this->instance_info;
  }

  /**
   * Create a fake version of the $element parameter used in many field methods
   * (e.g. TripalFieldWidget::form).
   *
   * @param $delta
   *   The delta for the $element you want to fake.
   * @param $langcode
   *   The language code for the field/widget. This should usually be
   *   LANGUAGE_NONE.
   * @param $required
   *   True if the widget is required and false otherwise.
   *
   * @return
   *   A fake $element variable for use in testing.
   */
  public function mockElement($delta = 0, $langcode = LANGUAGE_NONE, $required = FALSE) {
    return [
      '#entity_type' => 'TripalEntity',
      '#entity' => $this->entity,
      '#bundle' => $this->bundle,
      '#field_name' => $this->field_name,
      '#language' => $langcode,
      '#field_parents' => [],
      '#columns' => [],
      '#title' => '',
      '#description' => '',
      '#required' => $required,
      '#delta' => $delta,
      '#weight' => $delta, //same as delta.
      'value' => [
        '#type' => 'value',
        '#value' => '',
      ],
      '#field' => $this->field_info,
      '#instance' => $this->instance_info,
      '#theme' => 'tripal_field_default',
      'element_validate' => ['tripal_field_widget_form_validate'],
    ];
  }

  /**
   * Create a fake version of the create/edit content form with the
   * current entity attached.
   *
   * @return
   *   A fake $form array for use in testing.
   */
  public function mockForm() {
    return [
      '#parents' => [],
      '#entity' => $this->entity,
    ];
  }

  /**
   * Create a fake version of the create/edit content form_state
   * with the current entity attached.
   *
   * @param $delta
   *   The delta for the $element you want to fake.
   * @param $langcode
   *   The language code for the field/widget. This should usually be
   *   LANGUAGE_NONE.
   * @param $values
   *    An array of values where the key is the form element name and the value
   *   is the fake user submmitted value.
   *
   * @return
   *   A fake $form_state array for use in testing.
   */
  public function mockFormState($delta = 0, $langcode = LANGUAGE_NONE, $values = NULL) {
    $form_state = [
      'build_info' => [
        'args' => [
          0 => NULL,
          1 => $entity,
        ],
        'form_id' => 'tripal_entity_form',
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'temporary' => [],
      'submitted' => FALSE,
    ];

    if ($values !== NULL) {
      $form_state['values'][$this->field_name][$langcode][$delta] = $values;
    }

    return $form_state;
  }

}
