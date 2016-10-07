<?php

class chado_linker__pub extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Publications';

  // The default description for this field.
  public static $default_description = 'Associates a publication (e.g. journal article,
            conference proceedings, book chapter, etc.) with this record.';

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

    $list_items = array();
    $chado_table = $this->field['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      if ($item['chado-' . $chado_table . '__pub_id']) {
        $pub = chado_generate_var('pub', array('pub_id' => $item['chado-' . $chado_table . '__pub_id']));
        $list_items[$pub->pyear] = $pub->uniquename;
      }
    }

    krsort($list_items, SORT_NUMERIC);

    $list = array(
      'title' => '',
      'items' => $list_items,
      'type' => 'ol',
      'attributes' => array(),
    );

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_item_list($list),
      );
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $entity = $form['#entity'];
    $field_name = $this->field['field_name'];

    // Get the FK column that links to the base table.
    $table_name = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity']->chado_record_id;
    $pub_id = '';
    $title = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $record_id);
      $pub_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__pub_id', $pub_id);
      $title = tripal_get_field_item_keyval($items, $delta, 'uniquename', $title);
    }

    $schema = chado_get_schema('pub');

    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#theme'] = 'chado_linker__pub_widget';
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id,
    );

    $widget['uniquename'] = array(
      '#type' => 'textfield',
      '#title' => t('Publication'),
      '#default_value' => $title,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/pub',
      '#ajax' => array(
        'callback' => "chado_linker__pub_widget_form_ajax_callback",
        'wrapper' => "$table_name-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#maxlength' => 100000,
    );
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {

    // Get the FK column that links to the base table.
    $table_name = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field values.
    foreach ($items as $delta => $values) {
      $fkey_value = $values['value'];
      $pub_id = $values['chado-' . $table_name . '__pub_id'];
      $uname = $values['uniquename'];

      // If the user provided a uniquename then we want to set the foreign key
      // value to be the chado_record_id
      if ($uname and !$pub_id) {
        $pub = chado_generate_var('pub', array('uniquename' => $uname));
        $items[$delta]['chado-' . $table_name . '__pub_id'] = $pub->pub_id;
      }

      // In the widgetFrom function we automatically add the foreign key
      // record.  But if the user did not provide a publication we want to take
      // it out so that the Chado field_storage infrastructure won't try to
      // write a record.
      if (!$uname and !$pub_id) {
        $items[$delta]['chado-' . $table_name . '__' . $fkey] = '';
      }

      // If the user removed the publication from the pub_uniquename field
      // then we want to clear out the rest of the hidden values.
      if (!$uname and $pub_id) {
        $items[$delta]['chado-' . $table_name . '__' . $fkey] = '';
        $items[$delta]['chado-' . $table_name . '__' . $pkey] = '';
        $items[$delta]['chado-' . $table_name . '__pub_id'] = '';
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
    $base_table = $this->field['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'pub_id' => '',
      'uniquename' => '',
    );

    $linker_table = $base_table . '_pub';
    $options = array(
      'return_array' => 1,
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);

    if (count($record->$linker_table) > 0) {
      $i = 0;
      foreach ($record->$linker_table as $index => $linker) {
        $pub = $linker->pub_id;
        $pub_details = tripal_get_minimal_pub_info($pub);
        $pub_details['@type'] = $pub->type_id->dbxref_id->db_id->name . ':' . $pub->type_id->dbxref_id->accession;
        $pub_details['publication']['type'] = $pub->type_id->name;

        $entity->{$field_name}['und'][$i]['value'] = $pub_details;
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $pkey] = $linker->$pkey;
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $fkey_lcolumn] = $linker->$fkey_lcolumn->$fkey_lcolumn;
        $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . 'pub_id'] = $pub->pub_id;
        $entity->{$field_name}['und'][$i]['uniquename'] =  $pub->uniquename;

        if (property_exists($pub, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $pub->entity_id;
        }
        $i++;
      }
    }
  }
}

/**
 * An Ajax callback for the pub widget.
 */
function chado_linker__pub_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_chado_linker__pub_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
        drupal_render($element['uniquename']) . "
        </div>
      </div>
    ";

  return $layout;
}