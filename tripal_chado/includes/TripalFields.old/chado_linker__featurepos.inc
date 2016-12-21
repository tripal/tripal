<?php

class chado_linker__featurepos extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Map positions';

  // The default description for this field.
  public static $default_description = 'Map position of a sequence.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatter_settings_summary()
   */
  public function formatter_settings_summary($field, $instance,
      $view_mode) {

  }

  /**
   * @see TripalField::formatter_settings_form()
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {



  }

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $rows = array();
    $headers = array('Map', 'Mapped Feature', 'Position');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $val = $item['value'];
      $map_name = $val['map_name'];
      $mapped_feature_name = $val['mapped_feature_name'];
      $position = $val['position'];

      $rows[] = array(
        $map_name,
        $mapped_feature_name,
        $position
      );
    }


    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-map-positions',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => 'This record is not found on any map.',
    );

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
      );
    }
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $record = $details['record'];
    $settings = $this->field['settings'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    $options = array(
      'return_array' => TRUE,
      'include_fk' => array(
        'srcfeature_id' => array(
          'type_id' => 1,
        ),
        'feature_id' => array(
          'type_id' => 1
        ),
      )
    );
    $feature = chado_expand_var($record, 'table', 'featurepos', $options);

    // iterate through the results and add them to our featureposes array
    $featureposes = array();
    if (isset($feature->featurepos->feature_id)) {
       foreach ($feature->featurepos->feature_id AS $featurepos) {
         $featuremap_id = $featurepos->featuremap_id;
         $featuremap = chado_generate_var('featuremap', array('featuremap_id' => $featuremap_id));
         $map_feature_id = $featurepos->map_feature_id;
         $map_feature = chado_generate_var('feature', array('feature_id' => $map_feature_id));
         $mappos = $featurepos->mappos;
         $featureposes [] = array (
           'map_name' => $featuremap->name,
           'mapped_feature_name' => $map_feature->name,
           'position' => $mappos
         );
      }
    }
    $i = 0;
    foreach ($featureposes as $position) {
      $entity->{$field_name}['und'][$i]['value'] = $position;
      $i++;
    }
  }

  /**
   * We don't want a widget so override this function.
   */
  public static function widgetInfo() {
    return array();
  }

}

