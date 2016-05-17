<?php

/**
 * This class provides fields for adding database cross-references.
 *
 * It supports two types of cross-references. The primary cross reference
 * which is usually in the base table as an FK to the dbxref table, and
 * secondary cross references which are in a linking table that connects
 * the base table with the dbxref table.
 *
 */
class chado_linker__dbxref extends TripalField {
  /**
   * Implements hook_info() for fields.
   *
   * This is a hook provided by the tripal_chado module for offloading the
   * hook_field_info() hook for each field to specify.
   */
  function field_info() {
    return array(
      'label' => t('Cross references'),
      'description' => t('This record can be cross referenced with a record in
          another online database. This field is intended for one or more
          references.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'chado_linker__dbxref_widget',
      'default_formatter' => 'chado_linker__dbxref_formatter',
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

    // If the base table has a 'dbxref_id' then we want to attach
    $attach = FALSE;
    $cardinality = 1;
    $field_name = $table_name . '__dbxref_id';
    $chado_table = $table_name;
    $chado_column = 'dbxref_id';
    $schema = chado_get_schema($table_name);
    if (array_key_exists('dbxref_id', $schema['fields'])){
      $attach = TRUE;
    }

    // If the linker table exists then we want to attach.
    $dbxref_table = $table_name . '_dbxref';
    if (chado_table_exists($dbxref_table)) {
      $schema = chado_get_schema($dbxref_table);
      $pkey = $schema['primary key'][0];
      $attach = TRUE;
      $field_name = $dbxref_table;
      $cardinality = FIELD_CARDINALITY_UNLIMITED;
      $chado_table = $dbxref_table;
      $chado_column = $pkey;
    }

    if ($attach) {
      // Initialize the field array.
      $field_info = array(
        'field_name' => $field_name,
        'field_type' => 'chado_linker__dbxref',
        'widget_type' => 'chado_linker__dbxref_widget',
        'widget_settings' => array(
          'display_label' => 1
        ),
        'description' => '',
        'label' => 'Cross References',
        'is_required' => 0,
        'cardinality' => $cardinality,
        'storage' => 'field_chado_storage',
        'field_settings' => array(
          'chado_table' => $chado_table,
          'chado_column' => $chado_column,
          'base_table' => $table_name,
          'semantic_web' => array(
            'name' => 'database cross reference',
            'accession' => '0000554',
            'ns' => 'SBO',
            'nsurl' => 'http://www.ebi.ac.uk/sbo/main/',
          ),
        ),
      );
    }

    return $field_info;
  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Cross references'),
      'field types' => array('chado_linker__dbxref'),
      'description' => t('This record can be cross referenced with a record
          in another online database. This field is intended for the most
          prominent reference.  At a minimum, the database and accession
          must be provided.'),
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('Cross references'),
      'field types' => array('chado_linker__dbxref'),
      'settings' => array(
      ),
    );
  }
  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity, $field,
      $instance, $langcode, $items, $display) {

    dpm($items);
    foreach ($items as $delta => $item) {
      if ($item['value']) {
        $content = $item['value']['namespace'] . ':' . $item['value']['accession'];
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

  function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];
    dpm($items[$delta]);

    $record = $element['#entity']->chado_record;
    $base_table = $record->tablename;

    // The first form element should be for the local dbxref record.
    if (property_exists($record, 'dbxref_id') and $delta == 0) {
      $this->base_widget_form($widget, $form, $form_state, $field, $instance,
          $langcode, $items, $delta, $element);
    }
    else {
      // The remainindg form elements should be for the linking dbxref table.
      $this->linker_widget_form($widget, $form, $form_state, $field, $instance,
          $langcode, $items, $delta, $element);
    }
  }

  /**
   * @see TripalField::widget_form()
   */
  private function base_widget_form(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $base_table = $element['#entity']->chado_record->tablename;
    $field_table = $base_table;
    $field_column = 'dbxref_id';

    // Get the field defaults.
    $fk_val = '';
    $dbxref_id = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array. This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $fk_val = $items[$delta][$field_table .'__' .$field_column];
      $dbxref_id = $items[$delta]['dbxref__dbxref_id'];
      $db_id = $items[$delta]['dbxref__db_id'];
      $accession = $items[$delta]['dbxref__accession'];
      $version = $items[$delta]['dbxref__version'];
      $description = $items[$delta]['dbxref__description'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state)) {
      $fk_val = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table .'__' .$field_column);
      $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'dbxref__dbxref_id');
      $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__db_id");
      $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__accession");
      $version = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__version");
      $description = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__description");
    }

    // If we are here because our parent was triggered in a form submit
    // then that means an ajax call was made and we don't want the fieldset to
    // be closed when it returns from the ajax call.
    // $collapsed = TRUE;
    // if (array_key_exists('triggering_element', $form_state) and
    // $form_state['triggering_element']['#parents'][0] == $field_name) {
    // $collapsed = FALSE;
    // }
    // if ($dbxref_id) {
    // $collapsed = FALSE;
    // }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    $widget['#element_validate'] = array('chado_base__dbxref_id_widget_validate');
    $widget['#theme'] = 'chado_base__dbxref_id_widget';
    $widget['#prefix'] = "<span id='$field_name-dbxref--db-id'>";
    $widget['#suffix'] = "</span>";

    // A temporary element used for theming the fieldset.
    $widget['#theme_settings'] = array(
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#theme' => 'chado_base__dbxref_id_widget'
      // '#collapsible' => TRUE,
      // '#collapsed' => $collapsed,
    );

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $items[0]['value']
    );

    $widget[$field_table .'__' .$field_column] = array(
      '#type' => 'value',
      '#default_value' => $fk_val

    );

    $widget['dbxref__dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id

    );

    $widget['dbxref__db_id'] = array(
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => array(
        'callback' => "chado_base__dbxref_id_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id",
        'effect' => 'fade',
        'method' => 'replace')

    );
    $widget['dbxref__accession'] = array(
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' .$db_id,
      '#ajax' => array(
        'callback' => "tripal_chado_dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id",
        'effect' => 'fade','method' => 'replace'
      ),
      '#disabled' => $db_id ? FALSE : TRUE

    );
    $widget['dbxref__version'] = array(
      '#type' => 'textfield',
      '#title' => t('Version'),
      '#default_value' => $version,
      '#maxlength' => array_key_exists(
        'length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
      '#size' => 5,
      '#disabled' => $db_id ? FALSE : TRUE

    );
    $widget['dbxref__description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $description,
      '#size' => 20,
      '#disabled' => $db_id ? FALSE : TRUE

    );
    // $widget['links'] = array(
    // '#type' => 'item',
    // '#markup' => l('Add a new database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
    // );
  }

  /**
   * @see TripalField::widget_form()
   */
  private function linker_widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the FK column that links to the base table.
    $chado_table = $field['settings']['chado_table'];
    $base_table = $field['settings']['base_table'];
    $schema = chado_get_schema($chado_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = '';
    $dbxref_id = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $record_id = $items[$delta]['value'];
      $fkey_value = $items[$delta][$field_name . '__' . $fkey];
      $dbxref_id = $items[$delta][$field_name . '__dbxref_id'];
      $db_id = $items[$delta][$field_name . '--dbxref__db_id'];
      $accession = $items[$delta][$field_name . '--dbxref__accession'];
      $version = $items[$delta][$field_name . '--dbxref__version'];
      $description = $items[$delta][$field_name . '--dbxref__description'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name);
      $fkey_value = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__' . $fkey);
      $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__dbxref_id');
      $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
      $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
      $version = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__version');
      $description = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__description');
    }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    $widget['#table_name'] = $chado_table;
    $widget['#fkey_field'] = $fkey;
    $widget['#element_validate'] = array('chado_linker__dbxref_widget_validate');
    $widget['#theme'] = 'chado_linker__dbxref_widget';
    $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id-$delta'>";
    $widget['#suffix'] =  "</span>";


    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items)? $items[$delta]['value'] : '',
    );

    $widget[$field_table . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget[$field_table . '__dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget[$field_table . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget[$field_table . '--dbxref__dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget[$field_table . '--dbxref__db_id'] = array(
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => array(
        'callback' => "chado_linker__dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $widget[$field_table . '--dbxref__accession'] = array(
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
      '#ajax' => array(
        'callback' => "chado_linker__dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget[$field_table . '--dbxref__version'] = array(
      '#type' => 'textfield',
      '#title' => t('Version'),
      '#default_value' => $version,
      '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
      '#size' => 5,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget[$field_table . '--dbxref__description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $description,
      '#size' => 20,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    if (!$db_id) {
      $widget['links'] = array(
        '#type' => 'item',
        '#markup' => l('Add a database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
      );
    }
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
    $base_table = $record->tablename;

    $num_dbxrefs = 0;

    // Set some defauls for the empty record
    if (property_exists($record, 'dbxref_id')) {
      $entity->{$field_name}['und'][0] = array(
        'value' => array(),
        $base_table . '__dbxref_id' => '',
        'dbxref__dbxref_id' => '',
        'dbxref__db_id' => '',
        'dbxref__accession' => '',
        'dbxref__version' => '',
        'dbxref__description' => '',
      );
    }
    else {
      $entity->{$field_name}['und'][0] = array(
        'value' => array(),
        $field_table . '__' . $pkey => '',
        $field_table . '__' . $fkey => '',
        $field_table . '__' . 'dbxref_id' => '',
        $field_table . '--' . 'dbxref__dbxref_id' => '',
        $field_table . '--' . 'dbxref__db_id' => '',
        $field_table . '--' . 'dbxref__accession' => '',
        $field_table . '--' . 'dbxref__version' => '',
        $field_table . '--' . 'dbxref__description' => '',
      );
    }

    // First check to see if a local dbxref_id field has values, if it does
    // then add it.
    if (property_exists($record, 'dbxref_id')) {
      $dbxref = $record->dbxref_id;
      $value = $dbxref->db_id->name . ':' . $dbxref->accession;
      $URL = '';
      if ($dbxref->db_id->urlprefix) {
        $URL = l($value, $dbxref->db_id->urlprefix . '/' . $dbxref->accession);
      }

      $entity->{$field_name}['und'][$num_dbxrefs] = array(
        'value' => array(
          'namespace' => $dbxref->db_id->name,
          'accession' => $dbxref->accession,
          'URL' => $URL,
        ),
        $base_table . '__dbxref_id' => $dbxref->dbxref_id,
        'dbxref__dbxref_id' => $dbxref->dbxref_id,
        'dbxref__db_id' => $dbxref->db_id->db_id,
        'dbxref__accession' => $dbxref->accession,
        'dbxref__version' => $dbxref->version,
        'dbxref__description' => $dbxref->description,
      );
      $num_dbxrefs++;
    }

    // Second, check in the dbxref linker table for values.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    $linker_table = $base_table . '_dbxref';
    $options = array('return_array' => 1);
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    if (count($record->$linker_table) > 0) {
      foreach ($record->$linker_table as $index => $linker) {
        $dbxref = $linker->dbxref_id;
        $entity->{$field_name}['und'][$num_dbxrefs] = array(
          'value' => array(
            'namespace' => $dbxref->db_id->name,
            'accession' => $dbxref->accession,
            'URL' => $URL,
          ),
          $field_table . '__' . $pkey => $linker->$pkey,
          $field_table . '__' . $fkey => $linker->$fkey->$fkey,
          $field_table . '__' . 'dbxref_id' => $dbxref->dbxref_id,
          $field_table . '--' . 'dbxref__dbxref_id' => $dbxref->dbxref_id,
          $field_table . '--' . 'dbxref__db_id' => $dbxref->db_id->db_id,
          $field_table . '--' . 'dbxref__accession' => $dbxref->accession,
          $field_table . '--' . 'dbxref__version' => $dbxref->version,
          $field_table . '--' . 'dbxref__description' => $dbxref->description,
        );
        $num_dbxrefs++;
      }
    }
  }
}
/**
 * Theme function for the linker table dbxrefs.
 *
 * @param $variables
 */
function theme_chado_linker__dbxref_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__db_id']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__accession']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__version']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__description']) . "
        </div>
        <div class=\"secondary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
      </div>
    ";

  return $layout;
}
/**
 * Callback function for validating the chado_linker__dbxref_widget.
 */
function chado_linker__dbxref_widget_validate($element, &$form_state) {

  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__dbxref_id');
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
  $version = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__version');
  $description = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__description');

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (!$db_id and $accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided."));
  }
  if ($db_id and !$accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__accession', t("A database and the accession must both be provided."));
  }
  if (!$db_id and !$accession and ($version or $description)) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided."));
  }

  // If the dbxref_id does not match the db_id + accession then the user
  // has selected a new dbxref record and we need to update the hidden
  // value accordingly.
  if ($db_id and $accession) {
    $fkey_value = $element['#entity']->chado_record_id;
    tripal_chado_set_field_form_values($field_name, $form_state, $fkey_value, $delta, $table_name . '__' . $fkey);

    $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
    if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '__dbxref_id');
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '--dbxref__dbxref_id');
    }
  }
  else {
    // If the db_id and accession are not set, then remove the linker FK value to the base table.
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $fkey);
  }
}

/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__dbxref_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  // Check to see if this dbxref already exists. If not then
  // give a notice to the user that the dbxref will be added.
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
  if ($db_id and $accession) {
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $options = array('is_duplicate' => TRUE);
    $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
    if (!$has_duplicate) {
      drupal_set_message('The selected cross reference is new and will be added for future auto completions.');
    }
  }

  return $form[$field_name]['und'][$delta];
}

/**
 * Theme function for the local base table dbxrefs.
 */
function theme_chado_base__dbxref_id_widget($variables) {
  $element = $variables['element'];

  $layout = "
      <div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__db_id']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__accession']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__version']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__description']) . "
        </div>
        <div class=\"primary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
      </div>
    ";

  return $layout;
}