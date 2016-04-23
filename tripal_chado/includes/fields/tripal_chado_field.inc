<?php

/* TODO: it may be eaiser to help train developers at creating new
 * fields if it was a class rather than a bunch of hooks. Below is
 * the starting point.
 */

/**
 * A base class for all Fields supported by the Tripal Chado module.
 *
 */
class TripalChadoField {
  /**
   * Implements hook_info() for fields.
   *
   * This is a hook provided by the tripal_chado module for offloading the
   * hook_field_info() hook for each field to specify.
   */
  public function info() {
    return array(
    );
  }
  /**
   * Implements hook_widget_info.
   *
   * This is a hook provided by the tripal_chado module for offloading
   * the hook_field_widget_info() hook for each field to specify.
   */
  public function widget_info() {
    return array(
    );
  }
  /**
   * Implements hook_formatter_info.
   *
   * This is a hook provided by the tripal_chado module for
   * offloading the hook_field_formatter_info() for each field
   * to specify.
   *
   */
  public function formatter_info() {
    return array(
    );
  }
  /**
   * Implements hook_formatter_settings_summary.
   *
   * This is a hook provided by the tripal_chado module for
   * offloading the hook_field_formatter_settings_summary() for each field
   * to specify.
   *
   */
  public function formatter_settings_summary($field, $instance, $view_mode) {

  }

  /**
   * Provides a settings form for the formatter.
   *
   * This is a hook provided by the tripal_chado module for
   * offloading the hook_field_formatter_settings_form() for each field
   * to specify.
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }

  /**
   *
   */
  public function formatter(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
  }

  /**
   * Loads the field values with appropriate data.
   *
   * This function is called by the tripal_chado_field_storage_load() for
   * each property managed by the field_chado_storage storage type.  This is
   * an optional hook function that is only needed if the field has
   * multiple form elements.
   *
   * @param $field
   * @param $entity
   * @param $base_table
   * @param $record
   */
  public function load($field, $entity, $base_table, $record) {

  }

  /**
   * Implements hook_ws_formatter().
   */
  public function ws_formatter(&$element, $entity_type, $entity,
      $field, $instance, $items) {

  }

  /**
   *  Implements hook_widget().
   */
  public function widget(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }
  /**
   * Callback function for validating the widget_.
   */
  static function widget_validate($element, &$form_state) {

  }

  /**
   *
   */
  public function attach_info($entity_type, $bundle) {

  }
}