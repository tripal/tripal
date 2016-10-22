<?php

class chado_linker__cvterm extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Annotations';

  // The default description for this field.
  public static $default_description = 'This record can be annotated with terms
            from other vocabularies.';

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

    $headers = array('Term', 'Definition', 'Is Not', 'Reference');
    $rows = array();

    $chado_table = $this->field['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      if ($item['chado-' . $chado_table . '__cvterm_id']) {
        $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $item['chado-' . $chado_table . '__cvterm_id']));
        $dbxref = $cvterm->dbxref_id;

        // Build the accession.
        $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
        if ($dbxref->db_id->urlprefix) {
          $accession = l($accession, tripal_get_dbxref_url($dbxref), array('attributes' => array('target' => '_blank')));
        }

        // Build the publication reference.
        $pub_ref = '';
        $pub_id = $item['chado-' . $chado_table . '__pub_id'];
        if ($pub_id) {
          $pub = chado_generate_var('pub', array('pub_id' => $pub_id));
          $pub_ref = $pub->title;
        }
        $rows[] = array(
          $accession,
          $cvterm->definition,
          $item['chado-' . $chado_table . '__is_not'] ? 'Yes' : '',
          '',
        );
      }
    }

    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => "$chado_table-table-terms",
        'class' => 'tripal-data-table'
      ),
      'caption' => '',
      'sticky' => FALSE,
      'colgroups' => array(),
      'empty' => 'There are no annotations of this type',
    );

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
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

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    // If the field name is not properly formatted then we can't tell what
    // table and type this is.  So just return.
    if (count($matches) != 3) {
      return $widget;
    }
    $table_name = $matches[1];
    $cv_id = $matches[2];

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
    $cvterm_name = '';
    $cvterm_id = '';
    $is_not = '';
    $cvterm = NULL;

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $record_id = $items[$delta]['value'];
      $fkey_value = $items[$delta]['chado-' . $table_name . '__' . $fkey];
      $cvterm_name = $items[$delta]['cvterm__name'];
      $is_not = $items[$delta]['chado-' . $table_name . '__is_not'];
      $cvterm_id = $items[$delta]['chado-' . $table_name . '__cvterm_id'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      // See example in chado_linker_contact.inc
//       $record_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name);
//       $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
//       $is_not = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_not');
//       $cvterm_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--cvterm__name');
    }

    if ($cvterm_name) {
      $cvterm = chado_generate_var('cvterm', array('cv_id' => $cv_id, 'name' => $cvterm_name));
    }

    $schema = chado_get_schema('cvterm');
    $options = tripal_get_cv_select_options();

    $widget['#table_name'] = $chado_table;
    $widget['#fkey_field'] = $fkey;
    $widget['#element_validate'] = array('chado_linker__cvterm_widget_validate');
    $widget['#theme'] = 'chado_linker__cvterm_widget';
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $items[$delta]['value'],
    );

    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['cv__cv_id'] = array(
      '#type' => 'value',
      '#default_value' => $cv_id,
    );
    $widget['chado-' . $table_name . '__cvterm_id'] = array(
      '#type' => 'value',
      '#default_value' => $cvterm ? $cvterm->cvterm_id : '',
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );

    $widget['cvterm__name'] = array(
      '#type' => 'textfield',
      '#title' => t('Term Name'),
      '#default_value' => $cvterm_name,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['name']) ? $schema['fields']['name']['length'] : 255,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/' . $cv_id,
      '#ajax' => array(
        'callback' => "chado_linker__cvterm_widget_form_ajax_callback",
        'wrapper' => "$table_name-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );

    $widget['chado-' . $table_name . '__is_not'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Not'),
      '#default_value' => $is_not,
      '#required' => $element['#required'],
    );

    $widget['cvterm__definition'] = array(
      '#type' => 'item',
      '#markup' => '',
    );

  }

  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    // If the form ID is field_ui_field_edit_form, then the user is editing the
    // field's values in the manage fields form of Drupal.  We don't want
    // to validate it as if it were being used in a data entry form.
    if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
      return;
    }

    // Get the field values.
    //   $cvterm_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__cvterm_id');
    //   $cv_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '--cvterm__cv_id--cv__cv_id');
    //   $cvterm_name = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '--cvterm__name');
    //   $cvterm_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__cvterm_id');
    //   $pub_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__pub_id');

    // If the user provided a cv_id and a name then we want to set the
    // foreign key value to be the chado_record_idd
    if ($cvterm_name) {

      $fkey_value = $element['#entity']->chado_record_id;
      tripal_chado_set_field_form_values($field_name, $form_state, $fkey_value, $delta, $table_name . '__' . $fkey);

      // Get the cvterm ID. If one is not available because it's a newly added
      // record, then we need to retrieve it and set the form element.
      if (!$cvterm_id) {
        $cvterm = tripal_get_cvterm(array('cv_id' => $cv_id, 'name' => $cvterm_name));
        tripal_chado_set_field_form_values($field_name, $form_state, $cvterm->cvterm_id, $delta, $table_name . '__cvterm_id');
      }

      if (!$pub_id) {
        $pub = chado_generate_var('pub', array('uniquename' => 'null'));
        tripal_chado_set_field_form_values($field_name, $form_state, $pub->pub_id, $delta, $table_name . '__pub_id');
      }
    }
    else {
      // If the $cv_id and name are not set, then remove the linker FK value to the base table.
      tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $fkey);
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

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cv_id = $matches[2];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'cvterm_id' => '',
      // The pub column is present in the cell_line_cvterm, feature_cvterm,
      // library_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
      // stock_cvterm, and stock_relationship_cvterm.
      'chado-' . $field_table . '__' . 'pub_id' => '',
      // The is_not column is present in feature_cvterm and stock_cvterm tables.
      'chado-' . $field_table . '__' . 'is_not' => '',
      // The rank column is present in the cell_line_cvterm, expression_cvterm,
      // feature_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
      // and stock_cvterm tables.
      'chado-' . $field_table . '__' . 'rank' => '',
      // The cvterm_type_id is present in the expression_cvterm table.
      'cvterm_type_id' => '',
      // The following field are to help link the cvterm.
      'cv__cv_id' => '',
      'cvterm__name' => '',
    );

    // Get the annotations associated with this base record for this fields type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $record->$fkey_rcolumn,
      'cvterm_id' => array(
        'cv_id' => $cv_id,
      ),
    );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $fcvterms = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($fcvterms); $i++) {
      $linker = $fcvterms[$i];
      $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $linker->cvterm_id));
      $entity->{$field_name}['und'][$i] = array(
        'value' => $linker->$pkey,
        'chado-' . $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn,
        'chado-' . $field_table . '__' . 'cvterm_id' => $linker->cvterm_id,
        'chado-' . $field_table . '__' . 'pub_id' => property_exists($linker, 'pub_id') ? $linker->pub_id : '',
        'chado-' . $field_table . '__' . 'is_not' => property_exists($linker, 'is_not') ? $linker->is_not : '',
        'chado-' . $field_table . '__' . 'rank' => property_exists($linker, 'rank') ? $linker->rank : '',
        'chado-' . $field_table . '__' . 'cvterm_type_id' => property_exists($linker, 'cvterm_type_id') ? $linker->cvterm_type_id : '',
        'cv__cv_id' => $cvterm->cv_id->cv_id,
        'cvterm__name' => $cvterm->name,
      );
    }
  }
}

/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_linker__cvterm_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"annotation-cvterm-widget\">
        <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element['cv__cv_id']) . "
        </div>
        <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element['cvterm__name']) . "
        </div>
        <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element['chado-' . $table_name . '__is_not']) . "
        </div>
      </div>
    ";

  return $layout;
}

/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__cvterm_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];


  return $form[$field_name]['und'][$delta];
}