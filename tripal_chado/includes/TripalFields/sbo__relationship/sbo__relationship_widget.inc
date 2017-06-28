<?php

class sbo__relationship_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Relationship';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__relationship');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // TODO: make this widget deal better with the various relationship
    // tables. See the load function as it does a better job of this.
    return;

    // Get the field settings.
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK column that links to the base table.
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

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
    $option3_rtypes  = '';
    if (array_key_exists('relationships', $instance)) {
      $settings = $instance['settings']['relationships'];
      $option1_vocabs = $settings['option1_vocabs'];
      $option2_vocab  = $settings['option2_vocab'];
      $option2_parent = $settings['option2_parent'];
      $option3_rtypes = $settings['relationship_types'];
    }

    // For testing if there are selected vocabs for option1 we'll copy the
    // contents in a special variable for later.
    $option1_test = $option1_vocabs;

    // Get the field defaults.
    $record_id = '';
    $subject_id = '';
    $object_id = '';
    $type_id = '';
    $value = '';
    $rank = '';
    $subject_uniquename = '';
    $object_uniquename = '';
    $type = '';

    // 'nd_reagent_relationship' and 'project_relationship' have different column names from
    // subject_id/object_id. Do a pattern matching to get the column names.
    $subject_id_key = 'subject_id';
    $object_id_key = 'object_id';
    foreach ($schema['foreign keys'][$base_table]['columns'] AS $lcolum => $rcolum) {
      if (preg_match('/^subject_.*id/', $lcolum)) {
        $subject_id_key = $lcolum;
      }
      else if (preg_match('/^object_.*id/', $lcolum)) {
        $object_id_key = $lcolum;
      }
    }

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      // Check for element values that correspond to fields in the Chado table.
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $subject_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $subject_id_key, $subject_id);
      $object_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $object_id_key, $object_id);
      $type_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__type_id', $type_id);
      // Not all Chado tables have a value and rank.  So we'll only get
      // those if applicable.
      if (array_key_exists('value', $schema['fields'])) {
        $value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__value', $value);
      }
      if (array_key_exists('rank', $schema['fields'])) {
        $rank = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__rank', $rank);
      }
      // Get element values added to help support insert/updates.
      $object_uniquename = tripal_get_field_item_keyval($items, $delta, 'object_name', $object_uniquename);
      $subject_uniquename = tripal_get_field_item_keyval($items, $delta, 'subject_name', $subject_uniquename);
      $type = tripal_get_field_item_keyval($items, $delta, 'type_name', $type);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
        array_key_exists($field_name, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $subject_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $subject_id_key];
      $object_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $object_id_key];
      $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];
      if (array_key_exists('value', $schema['fields'])) {
        $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
      }
      if (array_key_exists('rank', $schema['fields'])) {
        $rank = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'];
      }
      $object_uniquename = $form_state['values'][$field_name]['und'][$delta]['object_name'];
      $subject_uniquename = $form_state['values'][$field_name]['und'][$delta]['subject_name'];
      $type = $form_state['values'][$field_name]['und'][$delta]['type_name'];
    }
   $widget['#table_name'] = $field_table;

    $widget['#fkeys'] = $schema['foreign keys'];
    $widget['#base_table'] = $base_table;
    $widget['#chado_record_id'] = isset($form['#entity']) ? $form['#entity']->chado_record_id : '';
    //$widget['#element_validate'] = array('sbo__relationship_validate');
    $widget['#prefix'] =  "<span id='$field_table-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['chado-' . $field_table . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $field_table . '__' . $subject_id_key] = array(
      '#type' => 'value',
      '#default_value' => $subject_id,
    );
    $widget['chado-' . $field_table . '__type_id'] = array(
      '#type' => 'value',
      '#default_value' => $type_id,
    );
    $widget['chado-' . $field_table . '__' . $object_id_key] = array(
      '#type' => 'value',
      '#default_value' => $object_id,
    );
    if (array_key_exists('value', $schema['fields'])) {
      $widget['chado-' . $field_table . '__value'] = array(
        '#type' => 'value',
        '#default_value' => $value,
      );
    }
    if (array_key_exists('rank', $schema['fields'])) {
      $widget['chado-' . $field_table . '__rank'] = array(
        '#type' => 'value',
        '#default_value' => $rank,
      );
    }
    $widget['subject_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Subject'),
      '#default_value' => $subject_uniquename,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists($subject_id_key, $schema['fields']) && array_key_exists('length', $schema['fields'][$subject_id_key]) ? $schema['fields'][$subject_id_key]['length'] : 255,
      '#size' => 35,
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/$base_table",
    );

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
    // Option 3: Custom list of Relationship Types
    $rtype_options = array();
    $rtype_options[] = 'Select a Type';
    if ($option3_rtypes) {
      $rtypes = explode(PHP_EOL, $option3_rtypes);
      foreach($rtypes AS $rtype) {
        // Ignore empty lines
        if (trim($rtype) == '') {
          continue;
        }
        $term = tripal_get_cvterm(array('name' => trim($rtype)));
        // Try to get term with vocabulary specified
        if (!$term) {
          $tmp = explode('|', trim($rtype), 2);
          $cv = tripal_get_cv(array('name' => trim($tmp[0])));
          $rtype = trim($tmp[1]);
          $term = tripal_get_cvterm(array('name' => $rtype, 'cv_id' => $cv->cv_id));
        }
        $rtype_options[$term->cvterm_id] = $term->name;
      }
      $widget['type_id'] = array(
        '#type' => 'select',
        '#title' => t('Relationship Type'),
        '#options' => $rtype_options,
        '#default_value' => $default_type_id,
      );
      if ($type_id && !key_exists($type_id, $rtype_options)) {
        form_set_error($this->field['field_name'] . '[' . $langcode . '][' . $delta . '][type_id]', 'Illegal option detected for Relationship Type. Please contact site administrator to fix the problem');
      }
    }
    // Option 2: Child terms of a selected cvterm
    else if ($option2_vocab) {
      $values = array(
        'cv_id' => $option2_vocab,
        'name' => $option2_parent
      );
      $parent_term = tripal_get_cvterm($values);

      // If the term wasn't found then see if it's a synonym.
      if(!$parent_term) {
        $values = array(
          'synonym' => array(
            'name' => trim($option2_parent),
          )
        );
        $synonym = tripal_get_cvterm($values);
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
      $args = array(
        ':parent_cvterm_id' => $parent_term->cvterm_id,
        ':parent_cv_id' => $parent_term->cv_id->cv_id
      );
      $results = chado_query($sql, $args);
      while($child = $results->fetchObject()) {
        $rtype_options[$child->subject_id] = $child->name;
      }
      $widget['type_id'] = array(
        '#type' => 'select',
        '#title' => t('Relationship Type'),
        '#options' => $rtype_options,
        '#default_value' => $default_type_id,
      );
      if ($type_id && !key_exists($type_id, $rtype_options)) {
        form_set_error($this->field['field_name'] . '[' . $langcode . '][' . $delta . '][type_id]', 'Illegal option detected for Relationship Type. Please contact site administrator to fix the problem');
      }
    }
    // Option 1: All terms of selected vocabularies
    else if ($option1_test && array_pop($option1_test)) {
      $sql = "SELECT cvterm_id, name FROM {cvterm} WHERE cv_id IN (:cv_id) ORDER BY name";
      $results = chado_query($sql, array(':cv_id' => $option1_vocabs));
      while ($obj = $results->fetchObject()) {
        $rtype_options[$obj->cvterm_id] = $obj->name;
      }
      $widget['type_id'] = array(
        '#type' => 'select',
        '#title' => t('Relationship Type'),
        '#options' => $rtype_options,
        '#default_value' => $default_type_id,
      );
      if ($type_id && !key_exists($type_id, $rtype_options)) {
        form_set_error($this->field['field_name'] . '[' . $langcode . '][' . $delta . '][type_id]', 'Illegal option detected for Relationship Type. Please contact site administrator to fix the problem');
      }
    }
    // Default option:
    else {
      // Set up available cvterms for selection
      $vocs = array(0 => 'Select a vocabulary');
      $vocs = tripal_get_cv_select_options();
      $cv_id = isset($form_state['values'][$field_name]['und'][0]['vocabulary']) ? $form_state['values'][$field_name]['und'][0]['vocabulary'] : 0;
      // Try getting the cv_id from cvterm for existing records
      if (!$cv_id && $type_id) {
        $cvterm = tripal_get_cvterm(array('cvterm_id' => $type_id));
        if (isset($cvterm->cv_id->cv_id)) {
          $cv_id = $cvterm->cv_id->cv_id;
          $default_term = $cvterm->name;
        }
      }
      if (!$cv_id) {
        $cv_id = $default_voc;
      }
      $widget['vocabulary'] = array(
        '#type' => 'select',
        '#title' => t('Vocabulary'),
        '#options' => $vocs,
        '#required' => $element['#required'],
        '#default_value' => $cv_id,
        '#ajax' => array(
          'callback' => "sbo__relationship_widget_form_ajax_callback",
          'wrapper' => "$field_table-$delta",
          'effect' => 'fade',
          'method' => 'replace'
        ),
      );
      if ($cv_id) {
        $options = array();
        $widget['type_name'] = array(
          '#type' => 'textfield',
          '#title' => t('Relationship Type'),
          '#size' => 15,
          '#default_value' => $default_term,
          '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv_id"
        );
      }
    }

    $widget['object_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Object'),
      '#default_value' => $object_uniquename,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists($object_id_key, $schema['fields']) && array_key_exists('length', $schema['fields'][$object_id_key]) ? $schema['fields'][$object_id_key]['length'] : 255,
      '#size' => 35,
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/$base_table",
    );
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
    $chado_record_id = array_key_exists('#entity', $element)? $element['#entity']->chado_record_id : NULL;

    $schema = chado_get_schema($field_table);
    $fkeys = $schema['foreign keys'];

    // 'nd_reagent_relationship' and 'project_relationship' have different column names from
    // subject_id/object_id. Do a pattern matching to get the column names.
    $subject_id_key = 'subject_id';
    $object_id_key = 'object_id';
    foreach ($schema['foreign keys'][$base_table]['columns'] AS $lcolum => $rcolum) {
      if (preg_match('/^subject_.*id/', $lcolum)) {
        $subject_id_key = $lcolum;
      }
      else if (preg_match('/^object_.*id/', $lcolum)) {
        $object_id_key = $lcolum;
      }
    }

    $voc_id = array_key_exists('vocabulary', $form_state['values'][$field_name][$langcode][$delta]) ? $form_state['values'][$field_name][$langcode][$delta]['vocabulary'] : '';
    $type_name = array_key_exists('type_name', $form_state['values'][$field_name][$langcode][$delta]) ? $form_state['values'][$field_name][$langcode][$delta]['type_name'] : '';
    $subject_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] : '';
    $object_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key]: '';
    $type_id = $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'];

    $subject_name = $form_state['values'][$field_name][$langcode][$delta]['subject_name'];
    $object_name = $form_state['values'][$field_name][$langcode][$delta]['object_name'];

    // If the row is empty then skip this one, there's nothing to validate.
    if (!($type_id or !$type_name) and !$subject_name and !$object_name) {
      return;
    }
    else if ($type_name && $voc_id) {
      $val = array(
        'cv_id' => $voc_id,
        'name' => $type_name
      );
      $cvterm = chado_generate_var('cvterm', $val);
      $type_id = $cvterm->cvterm_id;
    }

    // Do not proceed if subject ID or object ID does not exist
    if (!key_exists($subject_id_key, $fkeys[$base_table]['columns']) ||
        !key_exists($object_id_key, $fkeys[$base_table]['columns'])) {
      return;
    }
    // Get the subject ID.
    $subject_id = '';
    $fkey_rcolumn = $fkeys[$base_table]['columns'][$subject_id_key];
    $matches = array();
    if (preg_match('/\[id: (\d+)\]/', $subject_name, $matches)) {
      $subject_id =  $matches[1];
    }
    else {
      $values = array('uniquename' => $subject_name);
      $subject = chado_select_record($base_table, array($fkey_rcolumn), $values);
      if(count($subject) > 0) {
        $subject_id = $subject[0]->$fkey_rcolumn;
      }
    }

    // Get the object ID.
    $object_id = '';
    $fkey_rcolumn = $fkeys[$base_table]['columns'][$object_id_key];
    $matches = array();
    if (preg_match('/\[id: (\d+)\]/', $object_name, $matches)) {
      $object_id = $matches[1];
    }
    else {
      $values = array('uniquename' => $object_name);
      $object = chado_select_record($base_table, array($fkey_rcolumn), $values);
      if (count($object) > 0) {
        $object_id = $object[0]->$fkey_rcolumn;
      }
    }

    if ($subject_id && $object_id && $type_id) {
      // Set the IDs according to the values that were determined above.
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = $subject_id;
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = $object_id;
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = $type_id;
      if (array_key_exists('rank', $schema['fields'])) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = $form_state['values'][$field_name][$langcode][$delta]['_weight'];
      }
    }
    else {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $subject_id_key] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__value'] = '';
      if (array_key_exists('rank', $schema['fields'])) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__rank'] = '';
      }
    }
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

}

function theme_sbo__relationship_instance_settings ($variables) {
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
  $acpath  = $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_path'];
  $acpath .=  $form_state['values']['instance']['settings']['relationships']['option2_vocab'] . '/';
  $urlval  = $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_input']['#url_value'];
  $urlval .=  $form_state['values']['instance']['settings']['relationships']['option2_vocab'];
  // Reset value if a different vocabulary is selected
  $form['instance']['settings']['relationships']['option2_parent']['#value'] = NULL;
  $form_state['values']['instance']['settings']['relationships']['option2_parent'] = NULL;
  $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_path'] = $acpath;
  $form['instance']['settings']['relationships']['option2_parent']['#autocomplete_input']['#url_value'] = $urlval;
  return $form['instance']['settings']['relationships']['option2_parent'];
}
