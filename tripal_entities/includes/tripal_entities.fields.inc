<?php

/**
 * Implements hook_field_info().
 */
function tripal_entities_field_info() {
  $fields = array(
    'organism_id' => array(
      'label' => t('Organism'),
      'description' => t('A field for specifying an organism.'),
      'default_widget' => 'tripal_entities_organism_select_widget',
      'default_formatter' => 'tripal_entities_organism_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_entities',
        'active' => TRUE
      ),
    ),
    'dbxref_id' => array(
      'label' => t('Cross-reference'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'tripal_entities_primary_dbxref_widget',
      'default_formatter' => 'tripal_entities_primary_dbxref_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_entities',
        'active' => TRUE
      ),
    ),
    'md5checksum' => array(
      'label' => t('MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'tripal_entities_md5checksum_checkbox_widget',
      'default_formatter' => 'tripal_entities_md5checksum_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_entities',
        'active' => TRUE
      ),
    ),
    'seqlen' => array(
      'label' => t('Sequence length'),
      'description' => t('A field for calculating the length of a sequence.'),
      'default_widget' => 'tripal_entities_seqlen_hidden_widget',
      'default_formatter' => 'tripal_entities_seqlen_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_entities',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}

/**
 * Implements hook_field_widget_info().
 */
function tripal_entities_field_widget_info() {
  return array(
    'tripal_entities_organism_select_widget' => array(
      'label' => t('Organism Select'),
      'field types' => array('organism_id')
    ),
    'tripal_entities_primary_dbxref_widget' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id'),
      'description' => t('This record can be cross-referenced with a record in another online database. This field is intended for the most prominent reference.  At a minimum, the database and accession must be provided.'),
    ),
    'tripal_entities_md5checksum_checkbox_widget' => array(
      'label' => t('MD5 Checksum Checkbox'),
      'field types' => array('md5checksum')
    ),
    'tripal_entities_seqlen_hidden_widget' => array(
      'field types' => array('seqlen')
    ),
  );
}
/**
 * Implements hook_field_formatter_info().
 */
function tripal_entities_field_formatter_info() {
  return array(
    'tripal_entities_organism_formatter' => array(
      'label' => t('Organism'),
      'field types' => array('organism_id')
    ),
    'tripal_entities_primary_dbxref_formatter' => array(
      'label' => t('Cross-reference'),
      'field types' => array('dbxref_id')
    ),
    'tripal_entities_md5checksum_formatter' => array(
      'label' => t('MD5 checksum'),
      'field types' => array('md5checksum')
    ),
    'tripal_entities_seqlen_formatter' => array(
      'label' => t('Sequence length'),
      'field types' => array('seqlen')
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
function tripal_entities_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  switch ($display['type']) {

    case 'tripal_entities_organism_formatter':
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
    case 'tripal_entities_primary_dbxref_formatter':
      foreach ($items as $delta => $item) {
        $accession = '';
        if ($item['value']) {
          $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $item['value']));
          $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
          if ($dbxref->db_id->urlprefix) {
            $accession = l($accession, $dbxref->db_id->urlprefix . '/' . $dbxref->accession);
          }
        }
        // TODO: add hook here to allow other modules to change this display
        // if they want.
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          '#type' => 'markup',
          '#markup' => $accession,
        );
      }
      break;
    case 'tripal_entities_md5checksum_formatter':
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
    case 'tripal_entities_seqlen_formatter':
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
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_entities_field_widget_form(&$form, &$form_state, $field,
  $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {
    case 'tripal_entities_organism_select_widget':
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
        '#element_validate' => array('tripal_entities_organism_select_widget_validate'),
      );
      $element['value'] = $widget;
      break;
    case 'tripal_entities_primary_dbxref_widget':
      // Get the field defaults from the database if a record exists.
      $dbxref_id = '';
      $db_id = '';
      $accession = '';
      $version = '';
      $description = '';
      if (count($items) > 0 and $items[0]['value']) {
        $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $items[0]['value']));
        $dbxref_id = $dbxref->dbxref_id;
        $db_id = $dbxref->db_id->db_id;
        $accession  = $dbxref->accession;
        $version = $dbxref->version;
        $description = $dbxref->description;
      }

      $schema = chado_get_schema('dbxref');
      $options = tripal_get_db_select_options();
      $widget += array(
        '#element_validate' => array('tripal_entities_primary_dbxref_widget_validate'),
        '#type' => 'fieldset',
        '#title' => $element['#title'],
        '#description' =>  $element['#description'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#theme' => 'tripal_entities_primary_dbxref_widget',
        array(
          $element['#field_name'] => array(
            '#type' => 'hidden',
            '#default_value' => $dbxref_id,
          ),
          'dbxref__db_id' => array(
            '#type' => 'select',
            '#title' => t('Database'),
            '#options' => $options,
            '#required' => $element['#required'],
            '#default_value' => $db_id,
          ),
          'dbxref__accession' => array(
            '#type' => 'textfield',
            '#title' => t('Accession'),
            '#default_value' => $accession,
            '#required' => $element['#required'],
            '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
            '#size' => 15,
          ),
          'dbxref__version' => array(
            '#type' => 'textfield',
            '#title' => t('Version'),
            '#default_value' => $version,
            '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
            '#size' => 5,
          ),
          'dbxref__description' => array(
            '#type' => 'textfield',
            '#title' => t('Description'),
            '#default_value' => $description,
            '#size' => 20,
          ),
        ),
      );
      $element['value'] = $widget;
      break;
    case 'tripal_entities_md5checksum_checkbox_widget':
      $widget += array(
        '#type' => 'checkbox',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#options' => array(0, 1),
        '#default_value' => 1,
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#element_validate' => array('tripal_entities_md5checksum_checkbox_widget_validate'),
      );
      $element['value'] = $widget;
      break;
    case 'tripal_entities_seqlen_hidden_widget':
      $widget += array(
        '#type' => 'hidden',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#element_validate' => array('tripal_entities_seqlen_hidden_widget_validate'),
      );
      $element['value'] = $widget;
      break;
  }
  return $element;
}


/**
 * Implements hook_field_is_empty().
 */
function tripal_entities_field_is_empty($item, $field) {
  if (empty($item['value']) && (string) $item['value'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
/**
 * Callback function for validating the tripal_entities_organism_select_widget.
 */
function tripal_entities_organism_select_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }
  $organism_id = tripal_entities_get_field_form_values($field_name, $form_state);

  if (count($organism_id) == 0) {
    form_error($element, t("Please specify an organism that already exists in the database."));
  }
}
/**
 * Callback function for validating the tripal_entities_organism_select_widget.
 */
function tripal_entities_primary_dbxref_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $db_id = tripal_entities_get_field_form_values($field_name, $form_state, "dbxref__db_id");
  $accession = tripal_entities_get_field_form_values($field_name, $form_state, "dbxref__accession");
  $version = tripal_entities_get_field_form_values($field_name, $form_state, "dbxref__version");
  $description = tripal_entities_get_field_form_values($field_name, $form_state, "dbxref__description");

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (count($db_id) == 0 and count($accession) > 0) {
    form_set_error(implode('][', $element ['#parents']) . '][0][dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if (count($db_id) > 0 and count($accession) == 0) {
    form_set_error(implode('][', $element ['#parents']) . '][0][dbxref__accession', t("A database and the accession must both be provided for the primary cross reference."));
  }

  // If user did not select a database, we want to remove dbxref_id from the
  // field.
  if (count($db_id) == 0) {
    tripal_entities_set_field_form_values($field_name, $form_state, '__NULL__', $field_name);
  }
}

/**
 * Callback function for validating the tripal_entities_md5checksum_checkbox_widget.
 */
function tripal_entities_md5checksum_checkbox_widget_validate($element, &$form_state) {  
  $field_name = $element['#field_name'];

  // Calculate the md5 checksum for the sequence only if md5 box is checked and the residues exist
  $residues = tripal_entities_get_field_form_values('feature__residues', $form_state);
  if (count($residues) > 0 && trim($residues[0]) != '') {    
    tripal_entities_set_field_form_values ($field_name, $form_state, md5($residues[0]));
  }
  else {
    // Otherwise, remove the md5 value
    tripal_entities_set_field_form_values ($field_name, $form_state, '__NULL__');
  }
}

/**
 * Callback function for validating the tripal_entities_seqlen_hidden_widget.
 */
function tripal_entities_seqlen_hidden_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // Calculate the md5 checksum for the sequence only if md5 box is checked and the residues exist
  $residues = tripal_entities_get_field_form_values('feature__residues', $form_state);
  if (count($residues) > 0) {
    tripal_entities_set_field_form_values ($field_name, $form_state, strlen($residues[0]));
  }
  else {
    // Otherwise, remove the md5 value
    tripal_entities_set_field_form_values ($field_name, $form_state, '__NULL__');
  }
}

/**
 * Theme function for the primary_dbxref_widget.
 *
 * @param $variables
 */
function theme_tripal_entities_primary_dbxref_widget($variables) {
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
function tripal_entities_get_field_form_values($field_name, $form_state, $child = NULL) {
  $values = array();
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
  return $values;
}

/**
 * Returns the values of the field from the $form_state.
 */
function tripal_entities_set_field_form_values($field_name, &$form_state, $newvalue, $child = NULL) {
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