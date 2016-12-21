<?php
class data__accession_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Accession';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__accession');

  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

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
    $widget['#theme'] = 'data__accession_widget';
    $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id'>";
    $widget['#suffix'] =  "</span>";

    // A temporary element used for theming the fieldset.
    $widget['#theme_settings'] = array(
      '#title' => $element['#title'],
      '#description' =>  $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#theme' => 'data__accession_widget',
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
        'callback' => "data__accession_widget_form_ajax_callback",
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
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
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
}

/**
 * An Ajax callback for the tripal_chado_admin_publish_form..
 */
function data__accession_widget_form_ajax_callback($form, $form_state) {
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

function theme_data__accession_widget($variables) {
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