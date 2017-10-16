<?php

class remote__data_formatter extends WebServicesFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Remote Data';
  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('remote__data');
  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );
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
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];
    $field_name = $this->field['field_name'];

    //Determine if single or multiple values returned.
    $count = count($items[0]['value']['members']);

    // Case #1: A single scalar value (string, number).
    if ($count == 1) {
      //Array key is based on the label, grab label to specify array in $items.
      $array_key = $items[0]['value']['members'][0]['label'];
      foreach($items[$array_key]['value'] as $index => $array_item){
        //Ignore all the extra ws info coming through.
        if($index == '@context' || $index == '@id' || $index == '@type'){
          continue;
        }
        else {
          $content = $array_item;
        }

      }
      $element[$array_key] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
    /*
    * Case #2: An array of scalar values (1-level deep, e.g. the obi__organism
    * field on the gene or mRNA content type).
    * Case #3: An array of nested scalar values (2 or more levels deep.  e.g.
    * the sbo__relationship field on all data types, but easily visible on mRNA content tpyes).
    */
    else if ($count > 1) {


    }


    //Check that the load function returned content.
    /*if (!empty($items[0]['value'])){
      $list = array();
      foreach ($items as $index => $item) {
        $list[$index] = $item['value'];
      }
      // If more than one value has been found display all values in an unordered
      // list.
      if (count($list) > 1) {
        $content = theme_item_list(array(
          'items' => $list,
          'title' => '',
          'attributes' => array('class' => array($entity->bundle . '-remote-data-list', 'remote-data-field-list')),
          'type' => 'ul'
        ));
      }
      else {
        $content = $list[0];
      }

      if(array_key_exists('members', $content)){
        $header = array($items[0]['value']['label']);
        $members = $content['members'];
        $rows = array();
        // Wrap each item in an array so that
        // it works with the table_theme
        foreach($members as $index => $member){
          if(is_array($member)){
            $rows[] = array('data' => array($index, $member));
            foreach($member as $array_index => $array_member_item){
              $rows[] = array('data' => array($array_index, $array_member_item));
            }
          }
          else{
            $rows[] = array('data' => array($index, $member));
          }
        }
        //Remove the context information which is not needed.
        unset($rows[0]);
      }
      else {
        $header = array($items[0]['value']['label']);
        $rows = array();
        // Wrap each item in an array so that
        // it works with the table_theme.
        foreach($content as $index => $item){
          $rows[] = array('data' => array($index, $item));
        }
        //Remove the context information which is not needed.
        unset($rows[0]);
      }


      $table = array(
        'header' => $header,
        'rows' => $rows,
        'attributes' => array(
          'id' => 'tripal_table-remote-data-object',
          'class' => 'tripal-data-table'
        ),
        'sticky' => FALSE,
        'caption' => "",
        'colgroups' => array(),
        'empty' => 'There is no remote data available.',
      );

      $content = theme_table($table);

      if (count($items) > 0) {
        // once we have our table array structure defined, we call Drupal's theme_table()
        // function to generate the table.
        $element[0] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }*/
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
}