<?php
/**
 * This class can be included at the top of your TripalTestCase to facillitate testing
 * fields, widgets and formatters.
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

  /**
   * Create an instance of TripalFieldTestHelper.
   *
   * Specifcally, initialize the widget class and save it for further testing.
   *
   * @param $bundle_name
   *   The name of the bundle the field should be part of. This bundle must already exist.
   * @param $machine_names
   *   An array of machine names including:
   *    - field_name: the name of the field (REQUIRED)
   *    - widget_name: the name of the widget (Only required for widget testing)
   *    - formatter_name: the name of the formatter (Only required for formatter testing)
   * @param $field_info
   *   The Drupal information for the field you want to test.
   * @param $instance_info
   *   The Drupal information for the field instance you want to test.
   */
  public function __construct($bundle_name, $machine_names, $entity, $field_info, $instance_info) {

    // @debug print "BUNDLE: " .$bundle_name."\n";

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

    $class_name = '\\' . $this->class_name;
    $class_path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_chado')
      . '/includes/TripalFields/'.$machine_names['field_name'].'/'.$this->class_name.'.inc';
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
      $this->bundle = tripal_load_bundle_entity(array('name'=> $bundle_name));

      // The entity from the specified bundle that the field should be attached to.
      $this->entity = $entity;

      // @debug print_r($instance_info);

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
   * Retrieve the field information for a fiven field.
   * @see https://api.drupal.org/api/drupal/modules%21field%21field.info.inc/function/field_info_field/7.x
   *
   * @param $field_name
   *   The name of the field to retrieve. $field_name can only refer to a
   *   non-deleted, active field.
   * @return
   *   The field array as returned by field_info_field() and used when initializing
   *   this class.
   */
  public function getFieldInfo($field_name) {

    if (empty($this->field_info)) {
      $this->field_info = field_info_field($field_name);
    }

    return $this->field_info;
  }

  /**
   * Retrieve the field instance information for a fiven field.
   * @see https://api.drupal.org/api/drupal/modules%21field%21field.info.inc/function/field_info_instance/7.x
   *
   * @param $bundle_name
   *   The name of the bundle you want the field attached to. For example, bio_data_1.
   * @param $field_name
   *   The name of the field to retrieve the instance of. $field_name can only refer to a
   *   non-deleted, active field.
   * @return
   *   The field instance array as returned by field_info_instance() and used when
   *   initializing this class.
   */
  public function getInstanceInfo($bundle_name, $field_name) {

    if (empty($this->instance_info)) {
      $this->instance_info = field_info_instance('TripalEntity', $field_name, $bundle_name);
    }

    return $this->instance_info;
  }

}
