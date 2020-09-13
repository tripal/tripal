<?php

class TripalFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Tripal Field.';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = [];

  /**
   *  The list of default settings for this formatter.
   */
  public static $default_settings = [];


  /**
   * Instantiates a new TripalFieldFormatter object.
   *
   * @param $field
   *   An array containing the field data as returned by field_info_field()
   * @param $instance
   *   (Optional). Set the instance of this field when one is available. This
   *   is necessary when working with instance specific functions such as the
   *   formatterSettingsForm, widgetForm, etc.
   */
  public function __construct($field, $instance = NULL) {
    $this->field = $field;
    $this->instance = $instance;
  }

  /**
   * Provides information about the formatter for this field.
   *
   * This function corresponds to the hook_field_formatter_info() function of
   * the Drupal Field API.
   *
   * This is a static function as it provides default values for all of the
   * formatters for this field type, and thus we don't need an instantiated
   * object to provide this information.
   *
   * @return
   *   An associative array with key/value paris compatible with those from the
   *   hook_field_formatter_info() function of the Drupal Field API.
   *
   */
  public static function info() {
    $class = get_called_class();
    return [
      'label' => $class::$default_label,
      'field types' => $class::$field_types,
      'settings' => $class::$default_settings,
      'TripalFieldFormatter' => TRUE,
    ];
  }

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
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
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   * This function provides the display for a field when it is viewed on
   * as a full page, teaser, indexing for searching, etc.  The content
   * returned by the formatter should only include what is present in the
   * $items[$delta]['values] array. This way, the contents that are displayed
   * on the page, via web services and downloaded into a CSV file will
   * always be identical.  The view need not show all of the data in the
   * 'values' array.
   *
   * @param $element
   *   A renderable array for the $items, as an array of child elements keyed
   *   by numeric indexes starting from 0.  When implemented as a child
   *   class, this argument is set for the display.
   * @param $entity_type
   *   The type of $entity.
   * @param $entity
   *   The entity object.
   * @param $langcode
   *   The language associated with $items.
   * @param $items
   *   Array of values for this field.
   * @param $display
   *   The display settings to use, as found in the 'display' entry of instance
   *   definitions. The array notably contains the following keys and values;
   *     - type: The name of the formatter to use.
   *     - settings: The array of formatter settings.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    foreach ($items as $delta => $item) {
      $element[$delta] = [
        '#type' => 'markup',
        '#markup' => $item['value'],
      ];
    }
  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
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
  public function settingsSummary($view_mode) {

  }

  /**
   * Updates a pager generated by theme('pager') for use with AJAX.
   *
   * Because fields are meant to be updated by AJAX we need clicks in the pager
   * to not reload the entire page but rather to only reload the field.
   * Therefore the links in the pager must be adjusted to support this.
   *
   * @param $pager
   *   The pager as created by theme('pager')
   * @param $entity
   *   The entity object.
   */
  protected function ajaxifyPager($pager, $entity) {

    $field_id = 'tripal-entity-' . $entity->id . '--' . $this->field['field_name'];

    $pager = preg_replace('/href="\/.+\?page=(.+?)"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', $1)"', $pager);
    $pager = preg_replace('/href="\/.+"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', 0)"', $pager);

    $pager = '<img src="/' . drupal_get_path('module', 'tripal') . '/theme/images/ajax-loader.gif" id="' . $field_id . '-spinner" class="tripal-field-ajax-spinner">' . $pager;
    return $pager;
  }

}
