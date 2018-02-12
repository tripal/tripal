<?php

class remote__data_widget extends WebServicesFieldWidget {

  // The default label for this field.
  public static $default_label = 'Remote Data';
  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('remote__data');
  /**
   * Provides the form for editing of this field.
   *
   * This function corresponds to the hook_field_widget_form()
   * function of the Drupal Field API.
   *
   * This form is diplayed when the user creates a new entity or edits an
   * existing entity.  If the field is attached to the entity then the form
   * provided by this function will be displayed.
   *
   * At a minimum, the form must have a 'value' element.  For Tripal, the
   * 'value' element of a field always corresponds to the value that is
   * presented to the end-user either directly on the page (with formatting)
   * or via web services, or some other mechanism.  However, the 'value' is
   * sometimes not enough for a field.  For example, the Tripal Chado module
   * maps fields to table columns and sometimes those columns are foreign keys
   * therefore, the Tripal Chado modules does not just use the 'value' but adds
   * additional elements to help link records via FKs.  But even in this case
   * the 'value' element must always be present in the return form and in such
   * cases it's value should be set equal to that added in the 'load' function.
   *
   * @param $widget
   * @param $form
   *   The form structure where widgets are being attached to. This might be a
   *   full form structure, or a sub-element of a larger form.
   * @param $form_state
   *   An associative array containing the current state of the form.
   * @param $langcode
   *   The language associated with $items.
   * @param $items
   *   Array of default values for this field.
   * @param $delta
   *   The order of this item in the array of subelements (0, 1, 2, etc).
   * @param $element
   * A form element array containing basic properties for the widget:
   *  - #entity_type: The name of the entity the field is attached to.
   *  - #bundle: The name of the field bundle the field is contained in.
   *  - #field_name: The name of the field.
   *  - #language: The language the field is being edited in.
   *  - #field_parents: The 'parents' space for the field in the form. Most
   *    widgets can simply overlook this property. This identifies the location
   *    where the field values are placed within $form_state['values'], and is
   *    used to access processing information for the field through the
   *    field_form_get_state() and field_form_set_state() functions.
   *  - #columns: A list of field storage columns of the field.
   *  - #title: The sanitized element label for the field instance, ready for
   *    output.
   *  - #description: The sanitized element description for the field instance,
   *    ready for output.
   *  - #required: A Boolean indicating whether the element value is required;
   *    for required multiple value fields, only the first widget's values are
   *    required.
   *  - #delta: The order of this item in the array of subelements; see
   *    $delta above
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    // Get the field settings.
   /* $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    // Get the setting for the option for how this widget.
    $instance = $this->instance;
    $settings = '';
    $site_list = '';

    $tokens = array();
    // Get the form info from the bundle about to be saved.
    $bundle_info = tripal_load_bundle_entity(array('name' => $form_state['build_info']['args']['0']['bundle']));
    // Retrieve all available tokens.
    $tokens = tripal_get_entity_tokens($bundle_info);
    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.

    // FORM PROPER
    $widget['#prefix'] =  "<span id='$field_name-remote_data-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['data_info'] = array(
      '#type' => 'fieldset',
      '#title' => 'Remote Data Settings',
      '#description' => 'Provide the site name, query and description for the remote data source.',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#prefix' => "<div id='set_titles-fieldset'>",
      '#suffix' => '</div>',
    );

    // Get the site info from the tripal_sites table.
      // Get the field groups associated with this bundle.
    $sites = db_select('tripal_sites', 's')
      ->fields('s')
      ->execute()->fetchAll();

    foreach ($sites as $site) {
      $rows[] = $site->name;
    }

    $widget['data_info']['site'] = array(
      '#type' => 'select',
      '#title' => t('Site'),
      '#options' => $rows,
      '#default_value' => $site_list,
    );

    $widget['data_info']['query'] = array(
      '#type' => 'textarea',
      '#title' => 'Query',
      '#description' => 'Build the query string that should be appended after the url. The tokens
       listed below may be used in your query build.',
      '#default_value' => $this->instance['settings']['data_info']['query'],
      '#rows' => 5
    );

    $widget['set_titles']['token_display']['tokens'] = array(
      '#type' => 'hidden',
      '#value' => serialize($tokens)
    );

    $widget['data_info']['token_display'] = array(
      '#type' => 'fieldset',
      '#title' => 'Available Tokens',
      '#description' => 'Copy the token and paste it into the "Query" text field above.',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE
    );

    $widget['data_info']['token_display']['content'] = array(
      '#type' => 'item',
      '#markup' => theme_token_list($tokens),
    );

    $widget['data_info']['description'] = array(
      '#type' => 'textarea',
      '#title' => 'Description',
      '#description' => 'Describe the data being pulled in.',
      '#default_value' =>  $this->instance['settings']['data_info']['description'],
      '#rows' => 1
    );
*/
    //TODO Add test button to ensure query returns info.
  }
  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.
   * The 'value' key of this field must be set in the $form_state['values']
   * array anytime data is entered by the user.  It may be the case that there
   * are other fields for helping select a value. In the end those helper
   * fields must be used to set the 'value' field.
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    //TODO validate the tokens, site, and query. Test that query returns data.

  }
}
