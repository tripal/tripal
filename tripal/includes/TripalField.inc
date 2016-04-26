<?php


/**
 * A base class for all Fields supported by the Tripal Chado module.
 *
 * This class provides all of the necessary functions for a TripalField field.
 * It helps simplify and unify the process of creating fields for Tripal.  This
 * class simply defines the function prototypes. It is up to the class that
 * extends this class to implement the functions.
 *
 * Each module that creates new fields should use the normal Field API hooks
 * (e.g. hook_field_info(), hook_field_widget_form(), etc.) to instantiate the
 * appropriate TripalField class.
 *
 * Because of the way Drupal handles callbacks, form validate functions,
 * AJAX callbacks and theme functions cannot be part of the implementation of
 * this class.  Those functions should be added to the bottom of the file
 * where the child class is housed..
 *
 */
class TripalField {
  /**
   * Provides information about this field.
   *
   * @return array
   *   An associative array with key/value pairs compatible with those from
   *   the hook_field_info() function of the Drupal Field API.
   */
  public function field_info() {
    return array(
    );
  }
  /**
   * Provides information about the widget for this field.
   *
   * @return array
   *   An associative array with key/value paris compatible with those from the
   *   hook_field_widget_info() function of the Drupal Field API.
   */
  public function widget_info() {
    return array(
    );
  }
  /**
   * Provides information about the formatter for this field.
   *
   * @return
   *   An associative array with key/value paris compatible with those from the
   *   hook_field_formatter_info() function of the Drupal Field API.
   *
   */
  public function formatter_info() {
    return array(
    );
  }
  /**
   * Provides a summary of the formatter settings.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function formatter_settings_summary($field, $instance, $view_mode) {

  }

  /**
   * Provides the field's setting form.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   * @param $form
   * @param $form_state
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.
   *
   *  @param $element
   *  @param $entity_type
   *  @param $entity
   *  @param $field
   *  @param $instance
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
  }

  /**
   * Provides the form for editing of this field.
   *
   * This form is diplayed when the user creates a new entity or edits an
   * existing entity.  If the field is attached to the entity then the form
   * provided by this function will be displayed.
   *
   * @param $widget
   * @param $form
   * @param $form_state
   * @param $field
   * @param $instance
   * @param $langcode
   * @param $items
   * @param $delta
   * @param $element
   *
   * @return
   *   A Drupal form. See the hook_field_widget_form() function for more information.
   */
  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }
  /**
   * Loads the field values from the underlying data store.
   *
   * This function is called by the tripal_chado_field_storage_load() for
   * each property managed by the field_chado_storage storage type.  This is
   * an optional hook function that is only needed if the field has
   * multiple form elements.
   *
   * This function must set the value for the field in the entity object. For
   * example:
   *
   * @code
   *   $field_name = $field['field_name'];
   *   $field_type = $field['type'];
   *   $language = 'und';
   *   $delta = 0;
   *   $entity->{$field_name}[$language][$delta]['value'] = TRUE;
   * @endcode
   *
   * The field in the entity is an associative array where the first level is
   * the field name, followed by the language.  The 'und' value indicates
   * that the language is undefined and is the default.  Next is the 'delta'
   * value. For field with a cardinality of 1, the delta value will always be
   * 0.  For fields with a cardinality greater than 1 then the delta should
   * increment for each value.  Next is a list of key/value pairs one of which
   * should have the name 'value'.  The 'value' key should always contain the
   * primary value that should be displayed to the user.  It can be a single
   * value, or an array.  Any other number of keys can be present to help
   * with the display. These keys also correspond to the names of the form
   * fields specified by the widget() function of this class.
   *
   * @param $field
   * @param $entity
   * @param $base_table
   * @param $record
   *
   */
  public function load($field, $entity, $details) {

  }

  /**
   * Formats the field for dipslay via web services.
   *
   * The tripal_ws module will look for this funcion to give the field the
   * opportunity fo customize the value or array that is present in the
   * JSON output of web services.
   *
   * @return
   *   A single value or an array of values that should be used for this field.
   */
  public function ws_formatter($entity_type, $entity, $field, $instance, $items) {

  }

  /**
   * Provides an array that allows Tripal to attach a field to an entity.
   *
   */
  public function attach_info($entity_type, $bundle, $settings) {

  }
}