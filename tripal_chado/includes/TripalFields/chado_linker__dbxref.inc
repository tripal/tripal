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

  // The default lable for this field.
  public static $default_label = 'Cross references';

  // The default description for this field.
  public static $default_description = 'This record can be cross referenced with a record in
          another online database. This field is intended for one or more
          references.  At a minimum, the database and accession must be provided.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $chado_table = $this->field['settings']['chado_table'];
    $content = '';
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $content = $item['value']['vocabulary'] . ':' . $item['value']['accession'];
      if ($item['value']['URL']) {
        $content = l($content, $item['value']['URL'], array('attributes' => array('target' => '_blank')));
      }
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }

    if (count($element) == 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => '',
      );
    }
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the FK column that links to the base table.
    $chado_table = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($chado_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity']->chado_record_id;
    $dbxref_id = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey, $fkey_value);
      $dbxref_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__dbxref_id', $dbxref_id);
      $db_id = tripal_get_field_item_keyval($items, $delta, 'db_id', $db_id);
      $accession = tripal_get_field_item_keyval($items, $delta, 'accession', $accession);
      $version = tripal_get_field_item_keyval($items, $delta, 'version', $version);
      $description = tripal_get_field_item_keyval($items, $delta, 'description', $description);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta][$field_table . '__' . $pkey];
      $fkey_value = $form_state['values'][$field_name]['und'][$delta][$field_table . '__' . $fkey];
      $dbxref_id = $form_state['values'][$field_name]['und'][$delta][$field_table . '__dbxref_id'];
      $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
      $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];
      $version = $form_state['values'][$field_name]['und'][$delta]['version'];
      $description = $form_state['values'][$field_name]['und'][$delta]['description'];
    }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    $widget['#table_name'] = $chado_table;
    $widget['#fkey_field'] = $fkey;
    //$widget['#element_validate'] = array('chado_linker__dbxref_widget_validate');
    $widget['#theme'] = 'chado_linker__dbxref_widget';
    $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id-$delta'>";
    $widget['#suffix'] =  "</span>";


    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $field_table . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $field_table . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $field_table . '__dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget['dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget['db_id'] = array(
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
    $widget['accession'] = array(
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
    $widget['version'] = array(
      '#type' => 'textfield',
      '#title' => t('Version'),
      '#default_value' => $version,
      '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
      '#size' => 5,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $description,
      '#size' => 20,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    if (!$db_id) {
      $widget['links'] = array(
        '#type' => 'item',
        '#markup' => l('Add a database', 'admin/tripal/legacy/tripal_db/add', array('attributes' => array('target' => '_blank')))
      );
    }
  }

  /**
   * @see TripalField::widgetFormValidate()
   */
  public function widgetFormValidate($entity_type, $entity, $field, $items, &$errors) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->field['settings']['chado_table'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];


    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $dbxref_id = $values['chado-' . $field_table . '__dbxref_id'];
      $db_id = $values['db_id'];
      $accession = $values['accession'];
      $version = $values['version'];
      $description = $values['description'];

      // Make sure that if a database ID is provided that an accession is also
      // provided.  Here we use the form_set_error function rather than the
      // form_error function because the form_error will add a red_highlight
      // around all of the fields in the fieldset which is confusing as it's not
      // clear to the user what field is required and which isn't. Therefore,
      // we borrow the code from the 'form_error' function and append the field
      // so that the proper field is highlighted on error.
      if (!$db_id and $accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
      if (!$db_id and !$accession and ($version or $description)) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
    }
  }
  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->field['settings']['chado_table'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $dbxref_id = $values['chado-' . $field_table . '__dbxref_id'];
      $db_id = $values['db_id'];
      $accession = $values['accession'];
      $version = $values['version'];
      $description = $values['description'];

      // If the dbxref_id does not match the db_id + accession then the user
      // has selected a new dbxref record and we need to update the hidden
      // value accordingly.
      if ($db_id and $accession) {
        $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
        if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
          $items[$delta]['chado-' . $table_name . '__dbxref_id'] = $dbxref->dbxref_id;
          $items[$delta]['dbxref_id'] = $dbxref->dbxref_id;
        }
      }
      // If the db_id and accession are not set, then remove the linker FK
      // value to the base table, but leave the primary key so the record
      // can be deleted.
      else {
        $items[$delta]['chado-' . $table_name . '__' . $fkey] = '';
        $items[$delta]['chado-' . $table_name . '__dbxref_id'] = '';
      }
    }
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $record = $details['record'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $record->tablename;

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey => '',
      'chado-' . $field_table . '__dbxref_id' => '',
      'dbxref_id' => '',
      'db_id' => '',
      'accession' => '',
      'version' => '',
      'description' => '',
    );

    $linker_table = $base_table . '_dbxref';
    $options = array('return_array' => 1);
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    if (count($record->$linker_table) > 0) {
      $i = 0;
      foreach ($record->$linker_table as $index => $linker) {
        $dbxref = $linker->dbxref_id;
        $URL = tripal_get_dbxref_url($dbxref);
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'vocabulary' => $dbxref->db_id->name,
            'accession' => $dbxref->accession,
            'URL' => $URL,
          ),
          'chado-' . $field_table . '__' . $pkey => $linker->$pkey,
          'chado-' . $field_table . '__' . $fkey => $linker->$fkey->$fkey,
          'chado-' . $field_table . '__dbxref_id' => $dbxref->dbxref_id,
          'dbxref_id' => $dbxref->dbxref_id,
          'db_id' => $dbxref->db_id->db_id,
          'accession' => $dbxref->accession,
          'version' => $dbxref->version,
          'description' => $dbxref->description,
        );
        $i++;
      }
    }
  }
}
/**
 * Theme function for the dbxref_id_widget.
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
      <div class=\"secondary-dbxref-widget\">
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['db_id']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['accession']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['version']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['description']) . "
        </div>
        <div class=\"secondary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
      </div>
    ";

  return $layout;
}

/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__dbxref_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];
  $field = field_info_field($field_name);
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];
  $field_prefix = 'chado-' . $field_table . '__dbxref_id';

  // Check to see if this dbxref already exists. If not then
  // give a notice to the user that the dbxref will be added.
  $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
  $db_id = $form_state['values'][$field_name]['und'][$delta]['accession'];
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