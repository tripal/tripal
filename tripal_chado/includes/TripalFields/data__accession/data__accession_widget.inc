<?php

class data__accession_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Site Accession';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__accession'];

  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field defaults.
    $dbxref_id = '';
    $db_id = '';
    $accession = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $dbxref_id = $items[$delta]['chado-' . $field_table . '__' . $field_column];
      $db_id = $items[$delta]['db_id'];
      $accession = $items[$delta]['accession'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $dbxref_id = isset($form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column]) ? $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column] : '';
      $db_id = isset($form_state['values'][$field_name]['und'][$delta]['db_id']) ? $form_state['values'][$field_name]['und'][$delta]['db_id'] : '';
      $accession = isset($form_state['values'][$field_name]['und'][$delta]['accession']) ? $form_state['values'][$field_name]['und'][$delta]['accession'] : '';
    }

    $schema = chado_get_schema('dbxref');
    $options = chado_get_db_select_options();

    //$widget['#element_validate'] = array('chado_base__dbxref_id_widget_validate');
    $widget['#prefix'] = "<span id='$field_name-dbxref--db-id'>";
    $widget['#suffix'] = "</span>";

    $widget['value'] = [
      '#type' => 'value',
      '#value' => $dbxref_id,
    ];

    $widget['chado-' . $field_table . '__' . $field_column] = [
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    ];

    $widget['db_id'] = [
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => [
        'callback' => "data__accession_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id",
        'effect' => 'fade',
        'method' => 'replace',
      ],
    ];
    $widget['accession'] = [
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
      '#disabled' => $db_id ? FALSE : TRUE,
    ];
  }

  /**
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $dbxref_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'];
    $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
    $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];

    // Is this field required?
    if ($element['#required'] and !$db_id) {
      form_set_error($field_name . '][und][0][db_id', "A database for the accession must be provided.");
    }
    if ($element['#required'] and !$accession) {
      form_set_error($field_name . '][und][0][accession', "An accession number must be provided.");
    }

    // If user did not select a database, we want to remove dbxref_id from the
    // field. We use '__NULL__' because this field is part of the base table
    // and this tells the Chado backend to insert a null rather than an empty
    // string.
    if (!$db_id) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'] = '__NULL__';
    }
    // If the dbxref_id does not match the db_id + accession then the user
    // has selected a new dbxref record and we need to update the hidden
    // value accordingly.
    if ($db_id and $accession) {
      $dbxref = chado_generate_var('dbxref', [
        'db_id' => $db_id,
        'accession' => $accession,
      ]);
      if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'] = $dbxref->dbxref_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $dbxref->dbxref_id;
      }
    }
  }

  /**
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $dbxref_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'];
    $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
    $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];

    // If the accession doesn't exist then add it.
    if ($db_id and $accession) {
      $dbxref = chado_generate_var('dbxref', [
        'db_id' => $db_id,
        'accession' => $accession,
      ]);
      if (!$dbxref) {
        $values = [
          'db_id' => $db_id,
          'accession' => $accession,
        ];
        $dbxref = chado_insert_dbxref($values);
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'] = $dbxref->dbxref_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $dbxref->dbxref_id;
      }
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {
    $layout = "
      <div class=\"primary-dbxref-widget\">
        <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element['db_id']) . "
        </div>
        <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element['accession']) . "
        </div>
      </div>
    ";

    $fieldset = [
      '#title' => $element['#title'],
      '#value' => '',
      '#description' => $element['#description'],
      '#children' => $layout,
    ];

    return theme('fieldset', ['element' => $fieldset]);
  }
}

/**
 * An Ajax callback for the tripal_chado_admin_publish_form..
 */
function data__accession_widget_form_ajax_callback($form, $form_state) {

  $instance = $form['#instance'];
  $field_name = $form_state['triggering_element']['#parents'][0];
  $dbxref_id = $form_state['input'][$field_name]['und'][0]['chado-' . $field_table . '__dbxref_id'];
  $db_id = $form_state['input'][$field_name]['und'][0]['db_id'];
  $accession = $form_state['input'][$field_name]['und'][0]['accession'];

  // If we don't have a match then this must be new accession. Because
  // this is a database defined access we will automatically add the
  // accession.
  if ($db_id and $accession) {
    $dbxref = chado_generate_var('dbxref', [
      'db_id' => $db_id,
      'accession' => $accession,
    ]);
    if (!$dbxref) {
      drupal_set_message('The accession provided does not exist in the database and will be added.', 'warning');
    }
  }

  return $form[$field_name];
}

