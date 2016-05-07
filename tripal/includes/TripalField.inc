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
 * AJAX callbacks, validate functions and theme functions cannot be part of
 * the implementation of this class.  Those functions should be added to
 * the bottom of the file where the child class is housed.
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
   * Provides an array that allows Tripal to attach a field to an entity.
   *
   * @todo: This function needs better documentation.
   *
   */
  public function attach_info($entity_type, $bundle, $settings) {

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
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
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
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the enity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($field, $entity, $details) {

  }

  /**
   * Provides a form for the 'Field Settings' of the field management page.
   *
   * This is an optional hook function and is similar to the
   * hook_field_settings_form function().
   *
   * @param $field
   *   The field structure being configured.
   * @param $instance
   *   The instance structure being configured.
   * @param $has_data
   *   TRUE if the field already has data, FALSE if not.
   */
  public function settings_form($field, $instance, $has_data) {
    $settings = $field['settings'];
    $element = array();

    $element['semantic_web'] = array(
      '#type' => 'fieldset',
      '#title' => 'Semantic Web',
      '#description' => t('Each field must be associated with a term
          from a controlled vocabulary.  This allows computer programs to understand
          the data provided on this site.  Please be cautions changing these
          values.  Defaults are set by Tripal and sites that use the same
          terms can exchange information.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#tree' => TRUE,
    );

    $element['semantic_web']['name'] = array(
      '#type' => 'textfield',
      '#title' => 'Term Name',
      '#description' => t('The name of the term (e.g. gene, mRNA, organism, etc.).'),
      '#default_value' => $settings['semantic_web']['name'],
    );
    $element['semantic_web']['accession'] = array(
      '#type' => 'textfield',
      '#title' => 'Term ID',
      '#description' => t('The unique term ID (accession) in the vocabulary (e.g. ' .
          'for the term "gene" in the Sequence Ontology, the accession is "0000704").'),
      '#default_value' => $settings['semantic_web']['accession'],
    );
    $element['semantic_web']['ns'] = array(
      '#type' => 'textfield',
      '#title' => 'Vocabulary Namespace',
      '#description' => t('The namespace for the vocabulary to which this term ' .
          'belongs (e.g. the namespace for the Sequence Ontology is "SO"). This '.
          'value is prefixed to the Term ID to form the complete id (e.g. "SO:0000704")'),
      '#default_value' => $settings['semantic_web']['ns'],
    );
    $element['semantic_web']['nsurl'] = array(
      '#type' => 'textfield',
      '#title' => 'Vocabulary URL',
      '#description' => t('The web URL of the vocabulary..'),
      '#default_value' => $settings['semantic_web']['nsurl'],
    );
    return $element;
  }

  /**
   * Describes this fields "data tables" to Views.
   *
   * This function is the equivalent of the hook_views_data() function of
   * the Drupal Views API.  It provides the necessary details to allow
   * Views to integrate the field.
   *
   * @return
   *   An associative array describing the data structure of the field.
   */
  public function views_data_alter(&$data, $field, $entity_info) {

  }

}