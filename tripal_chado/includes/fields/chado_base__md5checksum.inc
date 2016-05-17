<?php

class chado_base__md5checksum  extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'chado_base__md5checksum_widget',
      'default_formatter' => 'chado_base__md5checksum_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }

  /**
   * @see TripalField::attach_info()
   */
  function attach_info($entity_type, $bundle, $settings) {
    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

    $md5_fieldname = '';
    $schema = chado_get_schema($table_name);
    // Only attach to tables that have md5checksum columns. Currently for
    // Chado v1.3 this is only the  feature  and expression tables.
    if (array_key_exists('md5checksum', $schema['fields'])) {
      if ($table_name == 'feature') {
        $md5_fieldname = 'residues';
      }
      else if ($table_name == 'expression') {
        $md5_fieldname = 'uniquename';
      }
      else {
        return $field_info;
      }
    }
    else {
      return $field_info;
    }



    // Create an array with information about this field.
    $field_info = array(
      'field_name' => 'base__md5checksum',
      'field_type' => 'chado_base__md5checksum',
      'widget_type' => 'chado_base__md5checksum_widget',
      'description' => 'The MD5 checksum for the sequence.',
      'label' => 'MD5 Checksum',
      'is_required' => 0,
      'storage' => 'field_chado_storage',
      'widget_settings' => array(
        'display_label' => 1,
        'md5_fieldname' => $md5_fieldname,
      ),
      'field_settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'md5checksum',
        'semantic_web' => array(
          'name' => 'md5_checksum',
          'accession' => 'md5_checksum',
          'ns' => 'local',
          'nsurl' => '',
        ),
      ),
    );
    return $field_info;
  }

  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('MD5 Checksum Checkbox'),
      'field types' => array('chado_base__md5checksum'),
    );
  }

  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('MD5 Checksum'),
      'field types' => array('chado_base__md5checksum'),
      'settings' => array(
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity, $field,
      $instance, $langcode, $items, $display) {
    foreach ($items as $delta => $item) {
      $content = key_exists('value', $item) ? $item['value'] : '';
      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }

  /**
   * @see TripalField::widget_form()
   */
  function widget_form(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {
    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    $form['value'] = array(
      '#type' => 'value',
      '#value' => $items[0]['value'],
    );

    $widget[$field_table . '__md5checksum'] = array(
      '#type' => 'checkbox',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => array(0, 1),
      '#default_value' => '',
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#element_validate' => array('chado_base__md5checksum_widget_validate'),
    );
  }


  /**
   * @see TripalField::load()
   */
  function load($field, $entity, $details) {
    $record = $details['record'];
    $settings = $field['settings'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    $entity->{$field_name}['und'][0]['value'] = $record->md5checksum;
    $entity->{$field_name}['und'][0][$field_table . '__md5checksum'] = $record->md5checksum;
  }
}

/**
 * Callback function for validating the chado_base__md5checksum_widget.
 */
function chado_base__md5checksum_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];
  $field = $form_state['field'][$field_name]['und']['field'];
  $settings = $field['settings'];
  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];

  // Calculate the md5 checksum for the sequence only if md5 box is checked and
  // the residues exist.
  $residues = tripal_chado_get_field_form_values('feature__residues', $form_state, 0, 'feature__residues');
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, md5($residues), 0, $field_table . '__md5checksum');
  }
  else {
    // Otherwise, remove the md5 value
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, $field_table . '__md5checksum');
  }
}