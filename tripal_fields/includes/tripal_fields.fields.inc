<?php

/**
 * Implements hook_field_info().
 */
function tripal_fields_field_info() {
  $fields = array(
    'organism_id' => array(
      'label' => t('Organism'),
      'description' => t('A field for specifying an organism.'),
      'default_widget' => 'tripal_fields_organism_select_widget',
      'default_formatter' => 'tripal_fields_organism_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'dbxref_id' => array(
      'label' => t('Cross-reference'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'tripal_fields_primary_dbxref_widget',
      'default_formatter' => 'tripal_fields_primary_dbxref_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'residues' => array(
      'label' => t('Residues'),
      'description' => t('A field for managing nucleotide and protein residues.'),
      'default_widget' => 'tripal_fields_residue_textarea_widget',
      'default_formatter' => 'tripal_fields_residues_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'md5checksum' => array(
      'label' => t('MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'tripal_fields_md5checksum_checkbox_widget',
      'default_formatter' => 'tripal_fields_md5checksum_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    'seqlen' => array(
      'label' => t('Sequence length'),
      'description' => t('A field for calculating the length of a sequence.'),
      'default_widget' => 'tripal_fields_seqlen_hidden_widget',
      'default_formatter' => 'tripal_fields_seqlen_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
    // The field provides a widget for adding new properties
    // to an entity that is connected to a base table that has a prop table
    // in Chado.
    'kvproperty' => array(
      'label' => t('Add a Property Type'),
      'description' => t('This record may have any number of properties. Use this field to first add the type.'),
      'default_widget' => 'tripal_fields_kvproperty_widget',
      'default_formatter' => 'tripal_fields_kvproperty_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_fields',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}

/**
 * Implements hook_field_widget_info().
 */
function tripal_fields_field_widget_info() {
  return array(
    'tripal_fields_organism_select_widget' => array(
      'label' => t('Organism Select'),
      'field types' => array('organism_id')
    ),
    'tripal_fields_primary_dbxref_widget' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
    ),
    'tripal_fields_md5checksum_checkbox_widget' => array(
      'label' => t('MD5 Checksum Checkbox'),
      'field types' => array('md5checksum'),
    ),
    'tripal_fields_residues_textarea_widget' => array(
      'label' => t('Residues'),
      'field types' => array('residues'),
    ),
    'tripal_fields_seqlen_hidden_widget' => array(
      'label' => t('Sequence Length'),
      'field types' => array('seqlen'),
    ),
    'tripal_fields_kvproperty_widget' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty'),
    ),
  );
}
/**
 * Implements hook_field_formatter_info().
 */
function tripal_fields_field_formatter_info() {
  return array(
    'tripal_fields_organism_formatter' => array(
      'label' => t('Organism'),
      'field types' => array('organism_id')
    ),
    'tripal_fields_primary_dbxref_formatter' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id')
    ),
    'tripal_fields_md5checksum_formatter' => array(
      'label' => t('MD5 checksum'),
      'field types' => array('md5checksum')
    ),
    'tripal_fields_residues_formatter' => array(
      'label' => t('Residues'),
      'field types' => array('residues')
    ),
    'tripal_fields_seqlen_formatter' => array(
      'label' => t('Sequence length'),
      'field types' => array('seqlen')
    ),
    'tripal_fields_kvproperty_formatter' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty')
    ),
  );
}
/**
 * Implements hook_field_formatter_view().
 *
 * Two formatters are implemented.
 * - field_example_simple_text just outputs markup indicating the color that
 *   was entered and uses an inline style to set the text color to that value.
 * - field_example_color_background does the same but also changes the
 *   background color of div.region-content.
 *
 * @see field_example_field_formatter_info()
 */
function tripal_fields_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {
  $element = array();
  switch ($display['type']) {

    case 'tripal_fields_organism_formatter':
      foreach ($items as $delta => $item) {
        $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $item['value']));
        // TODO: add hook here to allow other modules to change this display
        // if they want.
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          // "<p>Genus Species</p>".
          // See theme_html_tag().
          '#type' => 'markup',
          '#markup' => '<i>' . $organism[0]->genus .' ' . $organism[0]->species . '</i>',
        );
      }
      break;
    case 'tripal_fields_primary_dbxref_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/tripal_fields/fields/primary_dbxref');
      $content = tripal_fields_primary_dbxref_formatter($entity_type, $entity, $field,
        $instance, $langcode, $items, $display);

      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
      break;
    case 'tripal_fields_md5checksum_formatter':
      module_load_include('inc', 'tripal_fields', 'includes/tripal_fields/fields/md5checksum');
      $content = tripal_fields_md5checksum_checkbox_formatter($entity_type, $entity, $field,
        $instance, $langcode, $items, $display);
      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
      break;
    case 'tripal_fields_residues_formatter':
      foreach ($items as $delta => $item) {
        // TODO: add hook here to allow other modules to change this display
        // if they want.
        $residues = key_exists('value', $item) ? $item['value'] : '';
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          '#type' => 'markup',
          '#markup' =>  '<pre>' . $residues . '</pre>',
        );
      }
      break;
    case 'tripal_fields_seqlen_formatter':
      foreach ($items as $delta => $item) {
        // TODO: add hook here to allow other modules to change this display
        // if they want.
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          '#type' => 'markup',
          '#markup' => key_exists('value', $item) ? $item['value'] : '',
        );
      }
      break;
    case 'tripal_fields_kvproperty_formatter':
      // Do nothing. This field is only used in the form.
      $element[$delta] = '';
      break;
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_fields_field_widget_form(&$form, &$form_state, $field,
  $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $widget['#delta'] = $delta;
  $field_name = $field['field_name'];

  switch ($instance['widget']['type']) {
    case 'tripal_fields_organism_select_widget':
      $options = tripal_get_organism_select_options();
      $widget += array(
        '#type' => 'select',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#options' => $options,
        '#default_value' => count($items) > 0 ? $items[0]['value'] : 0,
        '#required' => $element['#required'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#element_validate' => array('tripal_fields_organism_select_widget_validate'),
      );
      $element['value'] = $widget;
      break;
    case 'tripal_fields_primary_dbxref_widget':
      // Make sure the include files get parsed now and for the form submits.
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/fields/primary_dbxref');
      module_load_include('inc', 'tripal_fields', 'includes/fields/primary_dbxref');
      // Update the widget with the new field.
      tripal_fields_primary_dbxref_widget($field_name, $widget, $form,
        $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_md5checksum_checkbox_widget':
      form_load_include($form_state, 'inc', 'tripal_fields', 'includes/tripal_fields/fields/md5checksum');
      module_load_include('inc', 'tripal_fields', 'includes/tripal_fields/fields/md5checksum');
      tripal_fields_md5checksum_checkbox_widget($field_name, $widget,
          $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      $element['value'] = $widget;
      break;
    case 'tripal_fields_residues_textarea_widget':
        $widget += array(
          '#type' => 'textarea',
          '#title' => $element['#title'],
          '#description' => $element['#description'],
          '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
          '#default_value' => count($items) > 0 ? $items[0]['value'] : 0,
          '#delta' => $delta,
          '#element_validate' => array('tripal_fields_residues_textarea_widget_validate'),
        );
        $element['value'] = $widget;
        break;
    case 'tripal_fields_seqlen_hidden_widget':
      $widget += array(
        '#type' => 'hidden',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#element_validate' => array('tripal_fields_seqlen_hidden_widget_validate'),
      );
      $element['value'] = $widget;
      break;
    case 'tripal_fields_kvproperty_widget':
      $widget += array(
        '#element_validate' => array('tripal_fields_kvproperty_widget_validate'),
        '#type' => 'fieldset',
        '#title' => $element['#title'],
        '#description' =>  $element['#description'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#group' => 'entity_vetical_tabs',
        array(
          'kvproperty' => array(
            '#title' => t('Property Type'),
            '#type' => 'textfield',
            '#description' => t("Please enter the type of property that you want to add.  As you type, suggestions will be provided."),
            '#autocomplete_path' => "eadmin/tripal/chado/tripal_cv/cvterm/auto_name/",
          ),
          'kvproperty_add' => array(
            '#value' => t('Add fields for property type'),
            '#type' => 'button',
            '#name' => 'kvproperty_add',
          ),
        ),
      );
      $element['value'] = $widget;
      break;
  }
  return $element;
}


/**
 * Implements hook_field_is_empty().
 */
function tripal_fields_field_is_empty($item, $field) {
  if (empty($item['value']) && (string) $item['value'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
/**
 * Callback function for validating the tripal_fields_organism_select_widget.
 */
function tripal_fields_organism_select_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }
  $organism_id = tripal_fields_get_field_form_values($field_name, $form_state);

  if (count($organism_id) == 0) {
    form_error($element, t("Please specify an organism that already exists in the database."));
  }
}

/**
 * Callback function for validating the tripal_fields_kvproperty_widget.
 */
function tripal_fields_kvproperty_widget_validate($element, &$form_state) {
  // Add the new field to the entity
  $form_state['rebuild'] = TRUE;
}


/**
 * Callback function for validating the tripal_fields_residues_textarea_widget.
 */
function tripal_fields_residues_textarea_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // Remove any white spaces.
  $residues = tripal_fields_get_field_form_values($field_name, $form_state);
  if (count($residues) > 0) {
    $residues = preg_replace('/\s/', '', $residues[0]);
    tripal_fields_set_field_form_values($field_name, $form_state, $residues);
  }
}

/**
 * Callback function for validating the tripal_fields_seqlen_hidden_widget.
 */
function tripal_fields_seqlen_hidden_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // Get the residues so we can calculate teh length.
  $residues = tripal_fields_get_field_form_values('feature__residues', $form_state);

  // Remove any white spaces.
  if (count($residues) > 0) {
    $residues = preg_replace('/\s/', '', $residues[0]);
    tripal_fields_set_field_form_values($field_name, $form_state, strlen($residues));
  }
  else {
    // Otherwise, remove the md5 value
    tripal_fields_set_field_form_values ($field_name, $form_state, '__NULL__');
  }
}

/**
 * Theme function for the primary_dbxref_widget.
 *
 * @param $variables
 */
function theme_tripal_fields_primary_dbxref_widget($variables) {
  $element = $variables['element'];
  $layout = "
    <div class=\"primary-dbxref-widget\">
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[0]['dbxref__db_id']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[0]['dbxref__accession']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[0]['dbxref__version']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[0]['dbxref__description']) . "
      </div>
    </div>
  ";
  return $layout;
}


/**
 * Returns the values of the field from the $form_state.
 */
function tripal_fields_get_field_form_values($field_name, $form_state, $child = NULL) {
  $values = array();
  if (!array_key_exists('values', $form_state)) {
    return $values;
  }
  if (array_key_exists($field_name, $form_state['values'])) {
    foreach ($form_state['values'][$field_name] as $langcode => $items) {
      foreach ($items as $delta => $value) {
        if ($child and array_key_exists($child, $value['value'][0]) and $value['value'][0][$child]) {
          $values[] = $value['value'][0][$child];
        }
        else if (!$child and $value['value']) {
          $values[] = $value['value'];
        }
      }
    }
  }
  return $values;
}

/**
 * Returns the values of the field from the $form_state.
 */
function tripal_fields_set_field_form_values($field_name, &$form_state, $newvalue, $child = NULL) {
  $values = array();
  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    foreach ($items as $delta => $value) {
      if ($child and array_key_exists($child, $value['value'][0]) and $value['value'][0][$child]) {
        $form_state['values'][$field_name][$langcode][$delta]['value'][0][$child] = $newvalue;
      }
      else if (!$child) {
        $form_state['values'][$field_name][$langcode][$delta]['value'] = $newvalue;
      }
    }
  }
  return $values;
}

