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
    $content = '';
    
    // Get the settings
    $settings = $display['settings'];
    $field_name = $this->field['field_name'];

    // Get any subfields and the header label.  Shift the array because the
    // results should already be the value of the fisrt entry.
    $rd_field_name = $this->instance['settings']['data_info']['rd_field_name'];
    $subfields = explode(',', $rd_field_name);
    $header_label = $this->getHeaderLabel($subfields);
    $flabel = array_shift($subfields);

    // Get the site logo if one is provided
    $site_logo = $this->instance['settings']['data_info']['site_logo'];
    if ($site_logo) {
      $site_logo = file_load($site_logo);
    }

    // Get the site name where the data came from.
    $site_id_ws = $this->instance['settings']['data_info']['remote_site'];
    $site = db_select('tripal_sites', 'ts')
      ->fields('ts', array('name', 'url'))
      ->condition('ts.id', $site_id_ws)
      ->execute()
      ->fetchObject();

    // Iterate through the results and create a generic table.
    $rows = array();
    $headers = array('');
    foreach ($items as $index => $item) {
      $remote_entity_label = $item['remote_entity']['label'];
      $remote_entity_page = $item['remote_entity']['ItemPage'];
      $link = t('View !data on %site',
          array('!data' => l('this content', $remote_entity_page, array('attributes' => array('target' => '_blank'))),
            '%site' => $site->name));
      $value = $item['value'];
      if (!$value) {
        continue;
      }
      $error = $item['error'];
      if ($error) {
        $rows[] = [['error' => $value]];
        continue;
      }

      // If this is a collection then handle it as a list of members.
      if (array_key_exists('members', $value)) {
        foreach ($value['members'] as $subvalue) {
          $subvalue = $this->refineSubValue($subvalue, $subfields, $header_label);
          $rows[] = array($subvalue);
        }
      }
      else {
        if (count($subfields) > 0) {
          $subvalue = $this->refineSubValue($value, $subfields, $header_label);
          $rows[] = array($subvalue);
        }
        else {
          if (array_key_exists($flabel, $value)) {
            $rows[] = array($value[$flabel]);
          }
          else {
            $value['Link'] = l('View content on ' . $site->name, $remote_entity_page, array('attributes' => array('target' => '_blank')));
            $rows[] = array($value);
          }
        }
      }
    }

    $has_sub_tables = FALSE;
    for ($i = 0; $i < count($rows); $i++) {
      if (is_array($rows[$i][0])) {
        $rows[$i][0] = $this->createTable($rows[$i]);
        $has_sub_tables = TRUE;
      }
    }

    // If we don't have  tables for each row then we'll put everything into
    // a table.
    if (!$has_sub_tables) {
      $headers = array($header_label . '(s)');
      $content .= theme_table(array(
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
    else {
      for ($i = 0; $i < count($rows); $i++) {
        if (count($rows) > 1) {
          $content .= '<span class="tripal-remote--data-field-table-label">' . $header_label . ' ' . ($i + 1) . '</span>';
        }
        $content .= $rows[$i][0];
      }
    }
    
    $content .= '<p>';
    if (is_object($site_logo)) {
      $content .= '<img class="tripal-remote--data-field-logo" src="' . file_create_url($site_logo->uri) . '"><br/>';
    }
    $content .=  t('This content provided by !site.',
      array('!site' => l($site->name, $site->url, array('attributes' => array("target" => '_blank')))));
    $content .= '</p>';
    
    // Return the content for this field.
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => '<div class="tripal-remote--data-field">' . $content . '</div>',
    );
  }


  /**
   * Retrieves the header label given the subfields criteria.
   *
   * @param $subfields
   *   An array of the sequence of subfields.
   */
  private function getHeaderLabel($subfields) {
     $subfield = array_shift($subfields);
     $header_label = ucwords(preg_replace('/_/', ' ', $subfield));
     if (count($subfields) > 0) {
       $header_label .= ' ' . $this->getHeaderLabel($subfields);
     }
     return $header_label;
  }
  /**
   * Adjusts the items array to contain only the section/subsection desired.
   *
   * The field settings can indicate a field with sub fields that should
   * be displayed (e.g. organism,genus or relationship,clause).  We want
   * to adjust the item to only include what the user requested.
   *
   * @param $values
   * @param $subfields
   */
  private function refineSubValue($values, $subfields) {

    // Remove unwanted elements.
    unset($values['@id']);
    unset($values['@context']);
    unset($values['@type']);
    unset($values['remote_entity']);

    $subfield = array_shift($subfields);
    if (array_key_exists($subfield, $values)) {
      if (is_array($values[$subfield]) and count($subfields) > 0) {
        return  $this->refineSubvalue($values[$subfield], $subfields);
      }
      else {
        return $values[$subfield];
      }
    }
    else {
      return $values;
    }
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