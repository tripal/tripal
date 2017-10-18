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

    // Get the site name where the data came from.
    $site_id_ws = $this->instance['settings']['data_info']['remote_site'];
    $site = db_select('tripal_sites', 'ts')
      ->fields('ts', array('name', 'url'))
      ->condition('ts.id', $site_id_ws)
      ->execute()
      ->fetchObject();

    $content = '';
    if (count($items) > 0) {
      $remote_entity_label = $items[0]['remote_entity']['label'];
      $remote_entity_page = $items[0]['remote_entity']['ItemPage'];
      $content = t('The data below about !label was obtained from the !site database.',
        array('!label' => l($remote_entity_label, $remote_entity_page),
          '!site' => l($site->name, $site->url)));
    }
    else {
      $content = t('There is no data about this record from the !site database.',
          array('!site' => l($site->name, $site->url)));
    }
    foreach ($items as $index => $item) {
      $value = $item['value'];
      if (is_array($value)) {
        $headers = array('');

        // If this is a collection then handle it as a list of members.
        if (array_key_exists('members', $value)) {
          foreach ($value['members'] as $subvalue) {
            $content .= $this->createTable($subvalue);
          }
        }
        else {
          $content .= $this->createTable($subvalue);
        }
      }
      else {
        $content .= $this->createDL($value);
      }
    }

    // Return the content for this field.
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }

  /**
   * A recursive function for displaying an item in a table.
   *
   * @param $item
   *   An item from the $items array passed to the view() function.
   * @return
   *   An HTML formatted Table.
   */
  private function createTable($item, &$pkey = '', &$rows = array(), $depth = 0) {
    foreach ($item as $key => $value) {
      // Skip JSON-LD keys.
      if (preg_match('/^\@/', $key)) {
        continue;
      }
      $key = preg_replace('/_/', ' ', $key);
      $key = ucwords($key);
      if ($pkey) {
        $key = $pkey . ' ' . $key;
      }
      if (is_array($value)) {
        $this->createTable($value, $key, $rows, $depth + 1);
      }
      else {
        $rows[] = array(
          'data'=> array(
            $key,
            $value
          ),
          'no_striping' => TRUE,
        );
      }
    }
    if ($depth == 0) {
      $headers = array('Data Type', 'Value');
      return theme_table(array(
        'header' => $headers,
        'rows' => $rows,
        'attributes' => array(
          'class' => 'tripal-remote--data-field-table',
        ),
        'sticky' => FALSE,
        'caption' => "",
        'colgroups' => array(),
        'empty' => 'There are no results.',
      ));
    }
  }

  /**
   * A recursive function for creating an HTML dictionary list from
   * the results for the item provided.
   *
   * @param $item
   *   An item from the $items array passed to the view() function.
   * @return
   *   An HTML formatted DL.
   */
  private function createDL($item, &$pkey = '', &$content= '', $depth = 0) {
    if ($depth == 0) {
      $content = '<dl class="tripal-remote--data-field-dl">';
    }
    foreach ($item as $key => $value) {
      // Skip JSON-LD keys.
      if (preg_match('/^\@/', $key)) {
        continue;
      }

      $key = preg_replace('/_/', ' ', $key);
      $key = ucwords($key);
      if ($pkey) {
        $key = $pkey . ' ' . $key;
      }
      if (is_array($value)) {
        $this->createDL($value, $key, $content, $depth + 1);
      }
      else {
        $content .= '<dt>' . $key . '&nbsp;:&nbsp;</dt><dd>' . $value . '</dd>';
      }
    }
    if ($depth == 0) {
      $content .= '</dl>';
      return $content;
    }
  }

  /**
   * A recursive function for creating an HTML dictionary list from
   * the results for the item provided.
   *
   * @param $item
   *   An item from the $items array passed to the view() function.
   * @return
   *   An HTML formatted DL.
   */
  private function createNestedDL($item) {
    $content = '<dl>';
    foreach ($item as $key => $value) {
      // Skip JSON-LD keys.
      if (preg_match('/^\@/', $key)) {
        continue;
      }

      $key = preg_replace('/_/', ' ', $key);
      $key = ucwords($key);
      if (is_array($value)) {
        $value = $this->createDL($value);
      }
      $content .= '<dt>' . $key . '</dt><dd>' . $value . '</dd>';
    }
    $content .= '</dl>';
    return $content;
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