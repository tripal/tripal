<?php

class chado_linker__contact extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Contacts';

  // The default description for this field.
  public static $default_description = 'Associates an indviddual or organization with
          this record';

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
    // Get the settings
    $settings = $display['settings'];

    $headers = array('Name', 'Description', 'Type');
    $rows = array();

    foreach ($items as $delta => $item) {
      $contact = $item['value'];
      if (!$contact) {
        continue;
      }

      // Get the field values
      $contact_name = $contact['name'];
      $description = $contact['description'];
      $type = $contact['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $contact_name = l($contact_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($contact_name, $description, $type);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-contact-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'No contacts available',
    );
    $content = theme_table($table);

    if (count($items) > 0) {
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }


  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);

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
    $contact_id = '';
    $name = '';
    
    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $record_id);
      $contact_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__contact_id', $contact_id);
      if ($contact_id) {
        $contact = chado_generate_var('contact', array('contact_id' => $contact_id));
        $name = $contact->name;
      }
    }

    $schema = chado_get_schema('contact');

    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#theme'] = 'chado_linker__contact_widget';
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
    $widget['chado-' . $table_name . '__contact_id'] = array(
      '#type' => 'value',
      '#default_value' => $contact_id,
    );

    $widget['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Contact'),
      '#default_value' => $name,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/contact',
      '#ajax' => array(
        'callback' => "chado_linker__contact_widget_form_ajax_callback",
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
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

    // Get the FK column that links to the base table.
    $table_name = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    $field_name = $this->field['field_name'];

    // Get the field values.
    $fkey_value = isset($form_state['values'][$field_name][$langcode][$delta]['value']) ? $form_state['values'][$field_name][$langcode][$delta]['value'] : '';
    $contact_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] : '';
    $name = isset($form_state['values'][$field_name][$langcode][$delta]['name']) ? $form_state['values'][$field_name][$langcode][$delta]['name'] : '';

    // If the user provided a name then we want to set the foreign key
    // value to be the chado_record_id
    if ($name and !$contact_id) {
      $contact = chado_generate_var('contact', array('name' => $name));
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] = $contact->contact_id;
    }

    // In the widgetForm function we automatically add the foreign key
    // record.  But if the user did not provide a contact we want to take
    // it out so that the Chado field_storage infrastructure won't try to
    // write a record.
    if (!$name and !$contact_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
    }

    // If the user removed the contact from the contact_name field
    // then we want to clear out the rest of the hidden values.
    // Leave the primary key so the record can be deleted.
    if (!$name and $contact_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] = '';
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

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'contact_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_contact';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'contact_id' => array(
          'type_id' => array(
            'dbxref_id' => array(
              'db_id' => TRUE,
            ),
          ),
        ),
        $fkey_lcolumn => TRUE,
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'type' => $contact->type_id ? $contact->type_id->name : '',
            'name' => $contact->name,
            'description' => $contact->description,
          ),
          // Add in the semantic web settings.  This array is expected by
          // other Tripal modules that handle semantic web for fields.
          'semantic_web' => array(
            'type' => $contact->type_id ? $contact->type_id->dbxref_id->db_id->name . ':' . $contact->type_id->dbxref_id->accession : '',
            'name' => tripal_get_chado_semweb_term('contact', 'name'),
            'description' => tripal_get_chado_semweb_term('contact', 'description'),
          ),
          // Add in subfield mapping to Chado tables. This is used by the
          // chado_field_storage for performing queries on sub element values.
          // It should be a comma-separated list (no spacing) of the field names
          // as foreign keys are followed starting from the Chado table to which
          // this field maps.
          'chado_mapping' => array(
            'type' => 'type_id,name',
            'name' => 'contact_id,name',
            'description' => 'contact_id,name'
          ),
          'chado-' . $field_table . '__' . $pkey => $contact_linker->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $contact_linker->$fkey_lcolumn->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'contact_id' => $contact->contact_id
        );

        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
        }
      }
    }
  }
}

/**
 * An Ajax callback for the pub widget.
 */
function chado_linker__contact_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_chado_linker__contact_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
        drupal_render($element['name']) . "
        </div>
      </div>
    ";

  return $layout;
}