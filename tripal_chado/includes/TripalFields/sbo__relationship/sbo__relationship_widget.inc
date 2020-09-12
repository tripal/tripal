<?php

class sbo__relationship_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Relationship';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sbo__relationship'];

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------

  // This field depends heavily on the schema of the relationship and base
  // table. The following variables cache the schema to greatly speed up
  // this field.
  // Note: both are ChadoSchema objects.
  protected $schema;

  protected $base_schema;

  // The column which indicated the subject/object_id in the current
  // relationship table. This allows us to support exceptions in the common
  // chado naming conventions.
  protected $subject_id_column;

  protected $object_id_column;

  // An array of columns to use as the "name" of the subject and object.
  // For example, for the feature table, this will be the name,
  // whereas, for the organism table this will be the genus & species.
  protected $base_name_columns;

  // One of 'type_id', or 'table_name'. Not all base tables have a type_id so
  // this setting allows us to better handle these cases.
  protected $base_type_column;

  // The field instance for this widget. This allows us to use some of the
  // field methods and info in the widget.
  protected $field_instance;

  /**
   * Extends TripalField::__construct().
   */
  public function __construct($field, $instance) {
    parent::__construct($field, $instance);

    module_load_include('inc', 'tripal_chado', 'includes/TripalFields/sbo__relationship/sbo__relationship');
    $this->field_instance = new sbo__relationship($field, $instance);

    // Retrieve the schema's.
    $this->schema = $this->field_instance->getRelTableSchema();
    $this->base_schema = $this->field_instance->getBaseTableSchema();

    // Retrieve the subject/object column names.
    $this->subject_id_column = $this->field_instance->getSubjectIdColumn();
    $this->object_id_column = $this->field_instance->getObjectIdColumn();

    // Retrieve the columns to use for name/type.
    $this->base_name_columns = $this->field_instance->getBaseNameColumns();
    $this->base_type_column = $this->field_instance->getBaseTypeColumn();

  }

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // Get the field settings.
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $widget['#table_name'] = $field_table;


    // Get the primary key of the relationship table
    $pkey = $this->schema['primary key'][0];

    // 'nd_reagent_relationship' and 'project_relationship' have different column names from
    // subject_id/object_id. Retrieve those determined in the constructor.
    $subject_id_key = $this->subject_id_column;
    $object_id_key = $this->object_id_column;
    // And save them in the widget for use in testing/debugging.
    $widget['#subject_id_key'] = $subject_id_key;
    $widget['#object_id_key'] = $object_id_key;

    // Default Values:
    //----------------
    $record_id = '';
    $subject_id = '';
    $object_id = '';
    $type_id = '';
    $value = '';
    $rank = '';
    $subject_label = '';
    $object_label = '';
    $type = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {

      // Sometimes empty/initialized items are getting through.
      // To determine if it this one of them, the type_id must always be there.
      $type_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__type_id', $type_id);
      if (!empty($type_id)) {

        // Check for element values that correspond to fields in the Chado table.
        $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
        $subject_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $subject_id_key, $subject_id);
        $object_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $object_id_key, $object_id);
        $type_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__type_id', $type_id);

        // Not all Chado tables have a value and rank.  So we'll only get
        // those if applicable.
        if (array_key_exists('value', $this->schema['fields'])) {
          $value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__value', $value);
        }
        if (array_key_exists('rank', $this->schema['fields'])) {
          $rank = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__rank', $rank);
        }

        // Get element values added to help support insert/updates.
        $object_label = tripal_get_field_item_keyval($items, $delta, 'object_name', $object_label);
        $subject_label = tripal_get_field_item_keyval($items, $delta, 'subject_name', $subject_label);
        $type = tripal_get_field_item_keyval($items, $delta, 'type_name', $type);

      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $subject_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $subject_id_key];
      $object_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $object_id_key];
      $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];
      if (array_key_exists('value', $this->schema['fields'])) {
        $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
      }
      if (array_key_exists('rank', $this->schema['fields'])) {
        $rank = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'];
      }
      $object_label = $form_state['values'][$field_name]['und'][$delta]['object_name'];
      $subject_label = $form_state['values'][$field_name]['und'][$delta]['subject_name'];
      $type = $form_state['values'][$field_name]['und'][$delta]['type_name'];

    }

    // Getting default values for the relationship type element.
    $default_voc = '';
    if (isset($form_state['field'][$field_name]['und']['instance']['default_value'][0]['vocabulary'])) {
      $default_voc = $form_state['field'][$field_name]['und']['instance']['default_value'][0]['vocabulary'];
    }
    $default_term = '';
    if (isset($form_state['field'][$field_name]['und']['instance']['default_value'][0]['type_name'])) {
      $default_term = $form_state['field'][$field_name]['und']['instance']['default_value'][0]['type_name'];
    }

    $default_type_id = $type_id;
    if (!$type_id && isset($form_state['field'][$field_name]['und']['instance']['default_value'][0]['type_id'])) {
      $default_type_id = $form_state['field'][$field_name]['und']['instance']['default_value'][0]['type_id'];
    }

    // Check if we have autocomplete available for this base table
    $autocomplete_path = "admin/tripal/storage/chado/auto_name/$base_table";
    $has_autocomplete = db_query('SELECT 1 FROM menu_router WHERE path=:path',
      [':path' => $autocomplete_path . '/%'])->fetchField();

    // Save some values for later...
    $widget['#table_name'] = $field_table;

    $widget['#fkeys'] = $this->schema['foreign keys'];
    $widget['#base_table'] = $base_table;
    $widget['#chado_record_id'] = isset($form['#entity']) ? $form['#entity']->chado_record_id : '';
    //$widget['#element_validate'] = array('sbo__relationship_validate');
    $widget['#prefix'] = "<span id='$field_table-$delta'>";
    $widget['#suffix'] = "</span>";

    // Save the values needed by the Chado Storage API.
    //-------------------------------------------------
    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-' . $field_table . '__' . $pkey] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $field_table . '__' . $subject_id_key] = [
      '#type' => 'value',
      '#default_value' => $subject_id,
    ];
    $widget['chado-' . $field_table . '__type_id'] = [
      '#type' => 'value',
      '#default_value' => $type_id,
    ];
    $widget['chado-' . $field_table . '__' . $object_id_key] = [
      '#type' => 'value',
      '#default_value' => $object_id,
    ];
    if (array_key_exists('value', $this->schema['fields'])) {
      $widget['chado-' . $field_table . '__value'] = [
        '#type' => 'value',
        '#default_value' => $value,
      ];
    }
    if (array_key_exists('rank', $this->schema['fields'])) {
      $widget['chado-' . $field_table . '__rank'] = [
        '#type' => 'value',
        '#default_value' => $rank,
      ];
    }

    // Subject:
    //----------
    $widget['subject_name'] = [
      '#type' => 'textfield',
      '#title' => t('Subject'),
      '#default_value' => $subject_label,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists($subject_id_key, $this->schema['fields']) && array_key_exists('length', $this->schema['fields'][$subject_id_key]) ? $this->schema['fields'][$subject_id_key]['length'] : 255,
      '#size' => 35,
    ];
    // Add autocomplete if we have one for this base table.
    if ($has_autocomplete) {
      $widget['subject_name']['#autocomplete_path'] = $autocomplete_path;
    }

    // Type:
    //-------
    $rtype_options = $this->get_rtype_select_options();
    if ($rtype_options) {

      $widget['type_id'] = [
        '#type' => 'select',
        '#title' => t('Relationship Type'),
        '#options' => $rtype_options,
        '#empty_option' => 'Select a Type',
        '#default_value' => $default_type_id,
      ];
      if ($type_id && !key_exists($type_id, $rtype_options)) {
        form_set_error($this->field['field_name'] . '[' . $langcode . '][' . $delta . '][type_id]', 'Illegal option detected for Relationship Type. Please contact site administrator to fix the problem');
      }
    }
    // Default option:
    // If we were determine an type_id option set...
    // then we will need to provide a cv + type autocomplete.
    else {
      // Set up available cvterms for selection
      $vocs = [];
      $vocs = chado_get_cv_select_options();
      unset($vocs[0]);
      $cv_id = isset($form_state['values'][$field_name][$langcode][$delta]['vocabulary']) ? $form_state['values'][$field_name][$langcode][$delta]['vocabulary'] : 0;
      // Try getting the cv_id from cvterm for existing records
      if (!$cv_id && $type_id) {
        $cvterm = chado_get_cvterm(['cvterm_id' => $type_id]);
        if (isset($cvterm->cv_id->cv_id)) {
          $cv_id = $cvterm->cv_id->cv_id;
          $default_term = $cvterm->name;
        }
      }
      if (!$cv_id) {
        $cv_id = $default_voc;
      }
      $widget['vocabulary'] = [
        '#type' => 'select',
        '#title' => t('Vocabulary'),
        '#options' => $vocs,
        '#required' => $element['#required'],
        '#default_value' => $cv_id,
        '#empty_option' => 'Select a Vocabulary',
        '#ajax' => [
          'callback' => "sbo__relationship_widget_form_ajax_callback",
          'wrapper' => "$field_table-$delta",
          'effect' => 'fade',
          'method' => 'replace',
        ],
      ];
      $widget['type_name'] = [
        '#type' => 'textfield',
        '#title' => t('Relationship Type'),
        '#size' => 15,
        '#default_value' => $default_term,
        '#disabled' => TRUE,
        '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv_id",
      ];
      if ($cv_id) {
        $widget['type_name']['#disabled'] = FALSE;
      }
    }

    // Object:
    //--------
    $widget['object_name'] = [
      '#type' => 'textfield',
      '#title' => t('Object'),
      '#default_value' => $object_label,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists($object_id_key, $this->schema['fields']) && array_key_exists('length', $this->schema['fields'][$object_id_key]) ? $this->schema['fields'][$object_id_key]['length'] : 255,
      '#size' => 35,
    ];
    // Add autocomplete if we have one for this base table.
    if ($has_autocomplete) {
      $widget['object_name']['#autocomplete_path'] = $autocomplete_path;
    }
  }

  /**
   *
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $chado_record_id = array_key_exists('#entity', $element) ? $element['#entity']->chado_record_id : NULL;

    $fkeys = $this->schema['foreign keys'];

    // The tables 'nd_reagent_relationship' and 'project_relationship' have
    // different column names from subject_id/object_id. Retrieve the column
    // names determined in the form.
    $subject_id_key = $this->subject_id_column;
    $object_id_key = $this->object_id_column;

    // Retrieve the values from the form for the current $delta.
    $voc_id = array_key_exists('vocabulary', $form_state['values'][$field_name][$langcode][$delta]) ? $form_state['values'][$field_name][$langcode][$delta]['vocabulary'] : '';
    $type_name = array_key_exists('type_name', $form_state['values'][$field_name][$langcode][$delta]) ? $form_state['values'][$field_name][$langcode][$delta]['type_name'] : '';
    $subject_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] : '';
    $object_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] : '';
    $type_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] : '';

    $subject_name = isset($form_state['values'][$field_name][$langcode][$delta]['subject_name']) ? $form_state['values'][$field_name][$langcode][$delta]['subject_name'] : '';
    $object_name = isset($form_state['values'][$field_name][$langcode][$delta]['object_name']) ? $form_state['values'][$field_name][$langcode][$delta]['object_name'] : '';

    // Validation:
    //------------
    // If the row is empty then skip this one, there's nothing to validate.
    if (!($type_id || $type_name) && !$subject_name && !$object_name) {
      return;
    }

    // Catch the case where a user is trying to remove a relationship. All
    // widget fields are empty but the $type_id is not.  We must therefore
    // make sure everything is empty.
    if (!$type_name && !$subject_name && !$object_name) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__value'] = '';
      if (array_key_exists('rank', $this->schema['fields'])) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = '';
      }
      return;
    }

    // Do not proceed if subject ID or object ID does not exist.
    if (!key_exists($subject_id_key, $fkeys[$base_table]['columns']) ||
      !key_exists($object_id_key, $fkeys[$base_table]['columns'])) {
      return;
    }

    // Validation is occurring in the field::validate() but we need to know if
    // it finds errors. As such, I'm calling it here to check. Also,
    // field::validate() doesn't seem to always show it's errors OR stop form
    // submission so we need to ensure that happens here.
    // sbo__relationship::validate($entity_type, $entity, $langcode, $items, &$errors)
    $errors = $this->field_instance->validateItem($form_state['values'][$field_name][$langcode][$delta], $element['#chado_record_id']);
    if ($errors) {
      foreach ($errors as $error) {
        switch ($error['element']) {
          case 'subject':
            form_set_error('sbo__relationship][' . $langcode . '][' . $delta . '][subject_name', $error['message']);
            break;
          case 'type':
            form_set_error('sbo__relationship][' . $langcode . '][' . $delta, $error['message']);
            break;
          case 'object':
            form_set_error('sbo__relationship][' . $langcode . '][' . $delta . '][object_name', $error['message']);
            break;
          default:
            form_set_error('sbo__relationship][' . $langcode . '][' . $delta, $error['message']);
        }
      }
    }
    // If there are no other validation errors then ensure data is prepared for
    // the storage back-end.
    else {

      // Get the relationship type record.
      if ($type_name && $voc_id) {
        $val = [
          'cv_id' => $voc_id,
          'name' => $type_name,
        ];
        $cvterm = chado_generate_var('cvterm', $val);

        if (isset($cvterm->cvterm_id)) {
          $type_id = $cvterm->cvterm_id;
        }
      }

      // Get the subject ID.
      $subject_id = '';
      $fkey_rcolumn = $fkeys[$base_table]['columns'][$subject_id_key];
      $matches = [];
      // First check if it's in the textfield due to use of the autocomplete.
      if (preg_match('/\[id: (\d+)\]/', $subject_name, $matches)) {
        $subject_id = $matches[1];
      }
      // Otherwise we need to look it up using the name field determined in the
      // constructor for the current field. There may be more then one name field
      // (e.g. organism: genus + species) so we want to check both.
      else {
        $sql = 'SELECT ' . $fkey_rcolumn . ' FROM {' . $base_table . '} WHERE ' . implode('||', $this->base_name_columns) . '=:keyword';
        $subject = chado_query($sql, [':keyword' => $subject_name])->fetchAll();
        if (count($subject) > 0) {
          $subject_id = $subject[0]->$fkey_rcolumn;
        }
      }

      // Get the object ID.
      $object_id = '';
      $fkey_rcolumn = $fkeys[$base_table]['columns'][$object_id_key];
      $matches = [];
      // First check if it's in the textfield due to use of the autocomplete.
      if (preg_match('/\[id: (\d+)\]/', $object_name, $matches)) {
        $object_id = $matches[1];
      }
      // Otherwise we need to look it up using the name field determined in the
      // constructor for the current field. There may be more then one name field
      // (e.g. organism: genus + species) so we want to check both.
      else {
        $sql = 'SELECT ' . $fkey_rcolumn . ' FROM {' . $base_table . '} WHERE ' . implode('||', $this->base_name_columns) . '=:keyword';
        $object = chado_query($sql, [':keyword' => $object_name])->fetchAll();
        if (count($object) > 0) {
          $object_id = $object[0]->$fkey_rcolumn;
        }
      }

      // If we have all three values required for a relationship
      // then set them as the chado field storage expects them to be set.
      if ($subject_id && $object_id && $type_id) {
        // Set the IDs according to the values that were determined above.
        $form_state['values'][$field_name][$langcode][$delta]['value'] = 'value must be set but is not used';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = $subject_id;
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = $object_id;
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = $type_id;
        if (array_key_exists('rank', $this->schema['fields'])) {
          $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = $form_state['values'][$field_name][$langcode][$delta]['_weight'];
        }
      }
      // Otherwise, maybe we are creating the entity...
      // The storage API sohuld handle this case and automagically add the key in once
      // the chado record is created... so all we need to do is set the other columns.
      elseif ($subject_name && $object_id && $type_id) {
        $form_state['values'][$field_name][$langcode][$delta]['value'] = 'value must be set but is not used';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = $object_id;
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = $type_id;
        if (array_key_exists('rank', $this->schema['fields'])) {
          $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = $form_state['values'][$field_name][$langcode][$delta]['_weight'];
        }
      }
      elseif ($subject_id && $object_name && $type_id) {
        $form_state['values'][$field_name][$langcode][$delta]['value'] = 'value must be set but is not used';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = $subject_id;
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = $type_id;
        if (array_key_exists('rank', $this->schema['fields'])) {
          $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = $form_state['values'][$field_name][$langcode][$delta]['_weight'];
        }
      }
      // Otherwise, we don't have a vallue to insert so leave them blank.
      else {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = '';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = '';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = '';
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__value'] = '';
        if (array_key_exists('rank', $this->schema['fields'])) {
          $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = '';
        }
      }
    }

    return $errors;
  }

  /**
   * Theme function for the sbo__relationship_widget.
   */
  public function theme($element) {
    $layout = "
      <div class=\"chado-linker--relationship-widget\">
        <div class=\"chado-linker--relationship-widget-item\">" .
      drupal_render($element['subject_name']) . "
        </div>
        <div class=\"chado-linker--relationship-widget-item\">" .
      drupal_render($element['vocabulary']) . "
        </div>
        <div class=\"chado-linker--relationship-widget-item\">" .
      drupal_render($element['type_name']) . "
        </div>
        <div class=\"chado-linker--relationship-widget-item\">" .
      drupal_render($element['type_id']) . "
        </div>
        <div>" .
      drupal_render($element['object_name']) . "
        </div>
      </div>
    ";
    return $layout;
  }

  /**
   * Retrieve options for the type drop-down for the relationship widget.
   */
  public function get_rtype_select_options() {

    // This is slated for Release 2 of this widget.
    // It still needs extensive functional and automated testing.
    // Thus for now we are falling back on the Default option:
    // Form will provide a type autocomplete + vocab select.
    // @todo test this.
    return FALSE;

    // Get the instance settings. There are three options for how this widget
    // will be displayed. Those are controlled in the instance settings
    // of the field.
    // Option 1:  relationship types are limited to a specific vocabulary.
    // Option 2:  relationship types are limited to a subset of one vocabulary.
    // Option 3:  relationship types are limited to a predefined set.
    $instance = $this->instance;
    $settings = '';
    $option1_vocabs = '';
    $option2_parent = '';
    $option2_vocab = '';
    $option3_rtypes = '';
    if (array_key_exists('relationships', $instance)) {
      $settings = $instance['settings']['relationships'];
      $option1_vocabs = $settings['option1_vocabs'];
      $option2_vocab = $settings['option2_vocab'];
      $option2_parent = $settings['option2_parent'];
      $option3_rtypes = $settings['relationship_types'];
    }

    // For testing if there are selected vocabs for option1 we'll copy the
    // contents in a special variable for later.
    $option1_test = $option1_vocabs;

    // Option 3: Custom list of Relationship Types
    $rtype_options = [];
    if ($option3_rtypes) {
      $rtypes = explode(PHP_EOL, $option3_rtypes);
      foreach ($rtypes AS $rtype) {
        // Ignore empty lines
        if (trim($rtype) == '') {
          continue;
        }
        $term = chado_get_cvterm(['name' => trim($rtype)]);
        // Try to get term with vocabulary specified
        if (!$term) {
          $tmp = explode('|', trim($rtype), 2);
          $cv = chado_get_cv(['name' => trim($tmp[0])]);
          $rtype = trim($tmp[1]);
          $term = chado_get_cvterm(['name' => $rtype, 'cv_id' => $cv->cv_id]);
        }
        $rtype_options[$term->cvterm_id] = $term->name;
      }
      return $rtype_options;
    }
    // Option 2: Child terms of a selected cvterm
    else {
      if ($option2_vocab) {
        $values = [
          'cv_id' => $option2_vocab,
          'name' => $option2_parent,
        ];
        $parent_term = chado_get_cvterm($values);

        // If the term wasn't found then see if it's a synonym.
        if (!$parent_term) {
          $values = [
            'synonym' => [
              'name' => trim($option2_parent),
            ],
          ];
          $synonym = chado_get_cvterm($values);
          if ($synonym && $synonym->cv_id->cv_id == $option2_vocab) {
            $parent_term = $synonym;
          }
        }
        // Get the child terms of the parent term found above.
        $sql = "
        SELECT subject_id,
          (SELECT name from {cvterm} where cvterm_id = subject_id) AS name
        FROM {cvtermpath}
        WHERE
          object_id = :parent_cvterm_id AND
          cv_id = :parent_cv_id
        ORDER BY name
       ";
        $args = [
          ':parent_cvterm_id' => $parent_term->cvterm_id,
          ':parent_cv_id' => $parent_term->cv_id->cv_id,
        ];
        $results = chado_query($sql, $args);
        while ($child = $results->fetchObject()) {
          $rtype_options[$child->subject_id] = $child->name;
        }
        return $rtype_options;
      }
      // Option 1: All terms of selected vocabularies
      else {
        if ($option1_test && array_pop($option1_test)) {
          $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cv_id IN (:cv_id) ORDER BY name";
          $results = chado_query($sql, [':cv_id' => $option1_vocabs]);
          while ($obj = $results->fetchObject()) {
            $rtype_options[$obj->cvterm_id] = $obj->name;
          }
          return $rtype_options;
        }
        // Default option:
        // Let the form deal with this by providing a type autocomplete?
        else {
          return FALSE;
        }
      }
    }
  }

}

function theme_sbo__relationship_instance_settings($variables) {
  $element = $variables['element'];
  $option1 = $element['option1'];
  $option1_vocabs = $element['option1_vocabs'];
  $option2 = $element['option2'];
  $option2_vocab = $element['option2_vocab'];
  $option2_parent = $element['option2_parent'];
  $option3 = $element['option3'];
  $rtype = $element['relationship_types'];
  $layout = drupal_render($option1);
  $layout .= drupal_render($option1_vocabs);
  $layout .=
    drupal_render($option2) .
    "<div class=\"chado-linker--relationship-instance-settings-option2\">" .
    "<div class=\"chado-linker--relationship-instance-settings-option2-item\">" .
    drupal_render($option2_vocab) .
    "</div>" .
    "<div class=\"chado-linker--relationship-instance-settings-option2-item\">" .
    drupal_render($option2_parent) .
    "</div>" .
    "</div>";
  $layout .= drupal_render($option3);
  $layout .= drupal_render($rtype);
  return $layout;
}

/**
 * An Ajax callback for the relationshp widget.
 */
function sbo__relationship_widget_form_ajax_callback(&$form, $form_state) {
  // Get the triggering element
  $form_element_name = $form_state['triggering_element']['#name'];
  preg_match('/(.+?)\[(.+?)\]\[(.+?)\]/', $form_element_name, $matches);
  $field = $matches[1];
  $lang = $matches[2];
  $delta = $matches[3];

  // Return the widget that triggered the AJAX call
  if (isset($form[$field][$lang][$delta])) {
    return $form[$field][$lang][$delta];
  }
  // Alternatively, return the default value widget for the widget setting form
  else {
    return $form['instance']['default_value_widget'][$field];
  }
}

/**
 * An Ajax callback for the relationshp instance setting form.
 */
function sbo__relationship_instance_settings_form_ajax_callback(&$form, &$form_state) {
  $acpath = $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_path'];
  $acpath .= $form_state['values']['instance']['settings']['relationships']['option2_vocab'] . '/';
  $urlval = $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_input']['#url_value'];
  $urlval .= $form_state['values']['instance']['settings']['relationships']['option2_vocab'];
  // Reset value if a different vocabulary is selected
  $form['instance']['settings']['relationships']['option2_parent']['#value'] = NULL;
  $form_state['values']['instance']['settings']['relationships']['option2_parent'] = NULL;
  $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_path'] = $acpath;
  $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_input']['#url_value'] = $urlval;
  return $form['instance']['settings']['relationships']['option2_parent'];
}
