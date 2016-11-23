<?php

class chado_base__dbxref_id extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Accession';

  // The default description for this field.
  public static $default_description = 'This field specifies the unique stable accession (ID) for
        this record. It requires that this site have a database entry.';

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

    foreach ($items as $delta => $item) {
      if ($item['value']) {
        $content = $item['value']['vocabulary'] . ':' . $item['value']['accession'];
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field defaults.
    $fk_val = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $fk_val = $items[$delta]['chado-' . $field_table . '__' . $field_column];
      $db_id = $items[$delta]['db_id'];
      $accession = $items[$delta]['accession'];
      $version = $items[$delta]['version'];
      $description = $items[$delta]['description'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state)) {
      $fk_val = isset($form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column]) ? $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column] : '';
      $db_id = isset($form_state['values'][$field_name]['und'][$delta]['db_id']) ? $form_state['values'][$field_name]['und'][$delta]['db_id'] : '';
      $accession = isset($form_state['values'][$field_name]['und'][$delta]['accession']) ? $form_state['values'][$field_name]['und'][$delta]['accession'] : '';
      $version = isset($form_state['values'][$field_name]['und'][$delta]['version']) ? $form_state['values'][$field_name]['und'][$delta]['version'] : '';
      $description = isset($form_state['values'][$field_name]['und'][$delta]['description']) ? $form_state['values'][$field_name]['und'][$delta]['description'] : '';
    }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    //$widget['#element_validate'] = array('chado_base__dbxref_id_widget_validate');
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

    $widget['chado-' . $field_table . '__' . $field_column] = array(
      '#type' => 'value',
      '#default_value' => $fk_val,
    );

    $widget['db_id'] = array(
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
    $widget['accession'] = array(
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
    $widget['links'] = array(
      '#type' => 'item',
      '#markup' => l('Add a new database', 'admin/tripal/legacy/tripal_db/add', array('attributes' => array('target' => '_blank')))
    );
  }


  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field values.
    foreach ($items as $delta => $values) {
      $fk_val = $values['chado-' . $field_table . '__' . $field_column];
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
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
      if (!$db_id and !$accession and ($version or $description)) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided for the primary cross reference."),
          'error' => 'chado_base__dbxref',
        );
      }
    }
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    $fk_val = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $field_column]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $field_column] : '';
    $db_id = isset($form_state['values'][$field_name][$langcode][$delta]['db_id']) ? $form_state['values'][$field_name][$langcode][$delta]['db_id'] : '';
    $accession = isset($form_state['values'][$field_name][$langcode][$delta]['accession']) ? $form_state['values'][$field_name][$langcode][$delta]['accession'] : '';
    $version = isset($form_state['values'][$field_name][$langcode][$delta]['version']) ? $form_state['values'][$field_name][$langcode][$delta]['version'] : '';

    // If user did not select a database, we want to remove dbxref_id from the
    // field.
    if (!$db_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id'] = '__NULL__';
    }
    // If the dbxref_id does not match the db_id + accession then the user
    // has selected a new dbxref record and we need to update the hidden
    // value accordingly.
    if ($db_id and $accession) {
      $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
      if ($dbxref and $dbxref->dbxref_id != $fk_val) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id'] = $dbxref->dbxref_id;
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


    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        'vocabulary' => '',
        'accession' => '',
        'URL' => '',
      ),
      'chado-' . $field_table . '__' . $field_column => '',
      'db_id' => '',
      'accession' => '',
      'version' => '',
      'description' => '',
    );

    // Get the primary dbxref record (if it's not NULL).  Because we have a
    // dbxref_id passed in by the base record, we will only have one record.
    if ($record->$field_column) {
      $dbxref = $record->$field_column;
      $value = $dbxref->db_id->name . ':' . $dbxref->accession;
      $entity->{$field_name}['und'][0] = array(
        'value' => array(
          'vocabulary' => $dbxref->db_id->name,
          'accession' => $dbxref->accession,
          'URL' => tripal_get_dbxref_url($dbxref),
        ),
        'chado-' . $field_table . '__' . $field_column => $record->$field_column->$field_column,
        'db_id'       => $dbxref->db_id->db_id,
        'accession'   => $dbxref->accession,
        'version'     => $dbxref->version,
        'description' => $dbxref->description,
      );
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
  $field_prefix = 'chado-' . $field_table . '__' . $field_column;

//   $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--db_id');
//   $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_prefix . '--accession');
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
  $field_prefix = 'chado-' . $field_table . '__' . $field_column;

  $layout = "
      <div class=\"primary-dbxref-widget\">
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['db_id']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['accession']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['version']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['description']) . "
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
