<?php

class chado_base__dbxref_id extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Accession'),
      'description' => t('This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.'),
      'default_widget' => 'chado_base__dbxref_id_widget',
      'default_formatter' => 'chado_base__dbxref_id_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::can_attach()
   */
  protected function can_attach($entity_type, $bundle, $details) {

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    // Check the schema for the data table if it does not have
    // a 'dbxref_id' column then we don't want to attach this field.
    $schema = chado_get_schema($table_name);
    if (!$schema) {
      return FALSE;
    }
    if (array_key_exists('dbxref_id', $schema['fields'])) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * @see TripalField::create_info()
   */
  function create_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    };

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__dbxref_id',
      'type' => 'chado_base__dbxref_id',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'dbxref_id',
        'semantic_web' => 'data:2091',
      ),
    );
  }

  /**
   * @see TripalField::create_instance_info()
   */
  function create_instance_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__dbxref_id',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Accession',
      'description' => 'This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'chado_base__dbxref_id_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_base__dbxref_id_formatter',
          'settings' => array(),
        ),
      ),
    );
  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Accession'),
      'field types' => array('chado_base__dbxref_id'),
      'description' => t('This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.'),
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('Accession'),
      'field types' => array('chado_base__dbxref_id'),
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
      if ($item['value']) {
        $content = $item['value']['vocabulary'] . ':' . $item['value']['accession'];
        if ($item['value']['URL']) {
          $content = l($item['value']['URL'], $item['value']['URL']);
        }
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }
  }
  /**
   * @see TripalField::widget_form()
   */
  function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the field defaults.
    $fk_val = '';
    $dbxref_id = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $fk_val = $items[$delta][$field_table . '__' . $field_column];
      $dbxref_id = $items[$delta][$field_table . '__' . $field_column . '--dbxref_id'];
      $db_id = $items[$delta][$field_table . '__' . $field_column . '--db_id'];
      $accession = $items[$delta][$field_table . '__' . $field_column . '--accession'];
      $version = $items[$delta][$field_table . '__' . $field_column . '--version'];
      $description = $items[$delta][$field_table . '__' . $field_column . '--description'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state)) {
      $fk_val = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column);
      $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column . '--dbxref_id');
      $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column . '--db_id');
      $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column . '--accession');
      $version = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column . '--version');
      $description = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column . '--description');
    }

    // If we are here because our parent was triggered in a form submit
    // then that means an ajax call was made and we don't want the fieldset to
    // be closed when it returns from the ajax call.
  //   $collapsed = TRUE;
  //   if (array_key_exists('triggering_element', $form_state) and
  //       $form_state['triggering_element']['#parents'][0] == $field_name) {
  //     $collapsed = FALSE;
  //   }
  //   if ($dbxref_id) {
  //     $collapsed = FALSE;
  //   }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    $widget['#element_validate'] = array('chado_base__dbxref_id_widget_validate');
    $widget['#theme'] = 'chado_base__dbxref_id_widget';
    $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id'>";
    $widget['#suffix'] =  "</span>";

    // A temporary element used for theming the fieldset.
    $widget['#theme_settings'] = array(
      '#title' => $element['#title'],
      '#description' =>  $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#theme' => 'chado_base__dbxref_id_widget',
      //'#collapsible' => TRUE,
      //'#collapsed' => $collapsed,
    );

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget[$field_table . '__' . $field_column] = array(
      '#type' => 'value',
      '#default_value' => $fk_val,
    );

    $widget[$field_table . '__' . $field_column . '--dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );

    $widget[$field_table . '__' . $field_column . '--db_id'] = array(
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => array(
        'callback' => "chado_base__dbxref_id_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $widget[$field_table . '__' . $field_column . '--accession'] = array(
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
      '#ajax' => array(
        'callback' => "tripal_chado_dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget[$field_table . '__' . $field_column . '--version'] = array(
      '#type' => 'textfield',
      '#title' => t('Version'),
      '#default_value' => $version,
      '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
      '#size' => 5,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget[$field_table . '__' . $field_column . '--description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $description,
      '#size' => 20,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget['links'] = array(
      '#type' => 'item',
      '#markup' => l('Add a new database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
    );
  }

  /**
   * @see TripalField::load()
   */
  function load($field, $entity, $details) {

    $record = $details['record'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];


    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      $field_table . '__' . $field_column => '',
      $field_table . '__dbxref_id--dbxref_id' => '',
      $field_table . '__dbxref_id--db_id' => '',
      $field_table . '__dbxref_id--accession' => '',
      $field_table . '__dbxref_id--version' => '',
      $field_table . '__dbxref_id--description' => '',
    );

    // Get the primary dbxref record (if it's not NULL).  Because we have a
    // dbxref_id passed in by the base record, we will only have one record.
    if ($record->$field_column) {
      $dbxref = $record->$field_column;
      $value = $dbxref->db_id->name . ':' . $dbxref->accession;
      $URL = '';
      if ($dbxref->db_id->urlprefix) {
        $URL = l($value, $dbxref->db_id->urlprefix . '/' . $dbxref->accession);
      }

      $entity->{$field_name}['und'][0] = array(
        'value' => array(
          'vocabulary' => $dbxref->db_id->name,
          'accession' => $dbxref->accession,
          'URL' => $URL,
        ),
        $field_table . '__' . $field_column => $record->$field_column->$field_column,
        $field_table . '__' . $field_column . '--dbxref_id'   => $dbxref->dbxref_id,
        $field_table . '__' . $field_column . '--db_id'       => $dbxref->db_id->db_id,
        $field_table . '__' . $field_column . '--accession'   => $dbxref->accession,
        $field_table . '__' . $field_column . '--version'     => $dbxref->version,
        $field_table . '__' . $field_column . '--description' => $dbxref->description,
      );
    }
  }
}

/**
 * Callback function for validating the tripal_chado_dbxref_select_widget.
 */
function chado_base__dbxref_id_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];
  $field = $form_state['field'][$field_name]['und']['field'];
  $settings = $field['settings'];
  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];
  $field_prefix = $field_table . '__' . $field_column;


  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $fk_value = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__' . $field_column);
  $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--dbxref_id');
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--accession');
  $version = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--version');
  $description = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--description');

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (!$db_id and $accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $field_prefix . '--db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if ($db_id and !$accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $field_prefix . '--accession', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if (!$db_id and !$accession and ($version or $description)) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $field_prefix . '--db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }

  // If user did not select a database, we want to remove dbxref_id from the
  // field.
  if (!$db_id) {
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, $field_prefix . '--dbxref_id');
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__', 0, $field_table . '__' . $field_column);
  }
  // If the dbxref_id does not match the db_id + accession then the user
  // has selected a new dbxref record and we need to update the hidden
  // value accordingly.
  if ($db_id and $accession) {
    $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
    if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, 0, $field_table . '__' . $field_column);
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, 0, $field_prefix . '--dbxref_id');
    }
  }
}

/**
 * An Ajax callback for the tripal_chado_admin_publish_form..
 */
function chado_base__dbxref_id_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  $field = field_info_field($field_name);
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];
  $field_prefix = $field_table . '__' . $field_column;

  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--accession');
  if ($db_id and $accession) {
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $options = array('is_duplicate' => TRUE);
    $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
    if (!$has_duplicate) {
      drupal_set_message('The selected cross reference is new and will be added for future auto completions.', 'warning');
    }
  }

  return $form[$field_name];
}

function theme_chado_base__dbxref_id_widget($variables) {
  $element = $variables['element'];
  $field_name = $element['#field_name'];
  $field = field_info_field($field_name);
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];
  $field_prefix = $field_table . '__' . $field_column;

  $layout = "
      <div class=\"primary-dbxref-widget\">
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[$field_prefix . '--db_id']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[$field_prefix . '--accession']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[$field_prefix . '--version']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element[$field_prefix . '--description']) . "
        </div>
        <div class=\"primary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
      </div>
    ";

  //   $classes = array();
  //   $classes[] = 'collapsible';
  //   $theme_settings = $element['#theme_settings'];
  //   if ($theme_settings['#collapsed'] == FALSE) {
  //     $classes[] = 'collapsed';
  //   }
  $fieldset = array(
    '#title' => $element['#title'],
    '#value' => '',
    '#description' => $element['#description'],
    '#children' => $layout,
    //    '#attributes' => array('class' => $classes),
  );

  return theme('fieldset', array('element' => $fieldset));
}
