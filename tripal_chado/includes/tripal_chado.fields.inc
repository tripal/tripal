<?php

/**
 * Implements hook_field_info().
 */
function tripal_chado_field_info() {
  $fields = array(
    'organism_id' => array(
      'label' => t('Organism'),
      'description' => t('A field for specifying an organism.'),
      'default_widget' => 'tripal_chado_organism_select_widget',
      'default_formatter' => 'tripal_chado_organism_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    'dbxref_id' => array(
      'label' => t('Cross reference'),
      'description' => t('This record can be cross referenced with a record in
          another online database. This field is intended for the most prominent
          reference.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'tripal_chado_dbxref_id_widget',
      'default_formatter' => 'tripal_chado_dbxref_id_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    'residues' => array(
      'label' => t('Residues'),
      'description' => t('A field for managing nucleotide and protein residues.'),
      'default_widget' => 'tripal_chado_residue_textarea_widget',
      'default_formatter' => 'tripal_chado_residues_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    'md5checksum' => array(
      'label' => t('MD5 checksum'),
      'description' => t('A field for generating MD5 checksum for a sequence.'),
      'default_widget' => 'tripal_chado_md5checksum_checkbox_widget',
      'default_formatter' => 'tripal_chado_md5checksum_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    'seqlen' => array(
      'label' => t('Sequence length'),
      'description' => t('A field for calculating the length of a sequence.'),
      'default_widget' => 'tripal_chado_seqlen_hidden_widget',
      'default_formatter' => 'tripal_chado_seqlen_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    // The field provides a widget for adding new properties
    // to an entity that is connected to a base table that has a prop table
    // in Chado.
    'kvproperty_adder' => array(
      'label' => t('Add a Property Type'),
      'description' => t('This record may have any number of properties. Use
          this field to first add the type.'),
      'default_widget' => 'tripal_chado_kvproperty_adder_widget',
      //'default_formatter' => 'tripal_chado_kvproperty_adder_formatter',
      'default_formatter' => 'hidden',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    // The field provides form elements for adding a property to an entity
    // that in turn gets stored in a prop table of Chado (e.g. featureprop,
    // stockprop, etc).
    'kvproperty' => array(
      'label' => t('Add a Property'),
      'description' => t('Add details about this property.'),
      'default_widget' => 'tripal_chado_kvproperty_widget',
      'default_formatter' => 'tripal_chado_kvproperty_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),

    // The field provides form elements for adding multiple dbxrefs to an
    // entity that in turn get stored in a [base]_dbxref table of Chado
    // (e.g. organism_dbxref, feature_dbxref).  This is different
    // from the dbxref_id field as that is specific to a dbxref_id in
    // a base table.
    'dbxref' => array(
      'label' => t('Cross references'),
      'description' => t('This record can be cross referenced with a record in
          another online database. This field is intended for one or more
          references.  At a minimum, the database and accession must be provided.'),
      'default_widget' => 'tripal_chado_dbxref_widget',
      'default_formatter' => 'tripal_chado_dbxref_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    // The field provides a widget for adding new vocabularies for cvterm
    // linker tables. This will allow cvterms to be grouped by vocabulary
    // ('category').
    'cvterm_class_adder' => array(
      'label' => t('Add an Annotation Type'),
      'description' => t('This record may have any number of types of
          annotations. Use this field to first add the type.'),
      'default_widget' => 'tripal_chado_cvterm_class_adder_widget',
      //'default_formatter' => 'tripal_chado_cvterm_class_adder_formatter',
      'default_formatter' => 'hidden',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
    // The field provides form elements for adding multiple cvterms to an
    // entity that in turn get stored in a [base]_cvterm table of Chado
    // (e.g. feature_cvterm).
    'cvterm' => array(
      'label' => t('Annotations'),
      'description' => t('This record can be annotated with terms
          from other vocabularies.'),
      'default_widget' => 'tripal_chado_cvterm_widget',
      'default_formatter' => 'tripal_chado_cvterm_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),

    'synonym' => array(
      'label' => t('Synonyms'),
      'description' => t('Adds an alternative name (synonym or alias) to this record.'),
      'default_widget' => 'tripal_chado_synonym_widget',
      'default_formatter' => 'tripal_chado_synonym_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),

    'pub' => array(
      'label' => t('Publications'),
      'description' => t('Associates a publication (e.g. journal article, conference proceedings, book chapter, etc.) with this record.'),
      'default_widget' => 'tripal_chado_pub_widget',
      'default_formatter' => 'tripal_chado_pub_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}

/**
 * Implements hook_field_formatter_settings_summary.
 */
function tripal_chado_field_formatter_settings_summary($field, $instance, $view_mode) {

  $summary = '';
  switch ($field['type']) {
    case 'organism_id':
      $summary = 'Organisms can be displayed in vaious ways.';
      break;
    default:
      $summary = '';
  }

  return $summary;
}

/**
 * Implements hook_field_formatter_settings_form.
 */
function tripal_chado_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $element = array();
  switch ($field['type']) {
    case 'organism_id':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/organism_id');
      module_load_include('inc', 'tripal_chado', 'includes/fields/organism_id');
      $element = tripal_chado_organism_select_formatter_form($field, $instance, $view_mode, $form, $form_state);
      break;
  }

  return $element;
}

/**
 * Implements hook_field_formatter_view().
 */
function tripal_chado_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $element = array();
  switch ($display['type']) {
    case 'tripal_chado_organism_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/organism_id');
      tripal_chado_organism_select_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_dbxref_id_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/dbxref_id');
      tripal_chado_dbxref_id_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_dbxref_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/dbxref');
      tripal_chado_dbxref_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_cvterm_class_adder_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/cvterm_class_adder');
      tripal_chado_cvterm_class_adder_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_cvterm_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/cvterm');
      tripal_chado_cvterm_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_md5checksum_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/md5checksum');
      tripal_chado_md5checksum_checkbox_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_residues_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/residues');
      tripal_chado_residues_textarea_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_seqlen_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/seqlen');
      tripal_chado_seqlen_hidden_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_kvproperty_adder_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/kvproperty_adder');
      tripal_chado_kvproperty_adder_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_kvproperty_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/kvproperty');
      tripal_chado_kvproperty_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_synonym_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/synonym');
      tripal_chado_synonym_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
    case 'tripal_chado_pub_formatter':
      module_load_include('inc', 'tripal_chado', 'includes/fields/pub');
      tripal_chado_pub_formatter($element, $entity_type, $entity, $field,
          $instance, $langcode, $items, $display);
      break;
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_chado_field_widget_form(&$form, &$form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  switch ($instance['widget']['type']) {
    case 'tripal_chado_organism_select_widget':
      // Make sure the include files get parsed now and for the form submits.
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/organism_id');
      module_load_include('inc', 'tripal_chado', 'includes/fields/organism_id');
      // Update the widget with the new field.
      tripal_chado_organism_select_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_dbxref_id_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/dbxref_id');
      module_load_include('inc', 'tripal_chado', 'includes/fields/dbxref_id');
      tripal_chado_dbxref_id_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_dbxref_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/dbxref');
      module_load_include('inc', 'tripal_chado', 'includes/fields/dbxref');
      tripal_chado_dbxref_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_cvterm_class_adder_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/cvterm_class_adder');
      module_load_include('inc', 'tripal_chado', 'includes/fields/cvterm_class_adder');
      tripal_chado_cvterm_class_adder_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_cvterm_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/cvterm');
      module_load_include('inc', 'tripal_chado', 'includes/fields/cvterm');
      tripal_chado_cvterm_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_md5checksum_checkbox_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/md5checksum');
      module_load_include('inc', 'tripal_chado', 'includes/fields/md5checksum');
      tripal_chado_md5checksum_checkbox_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_residues_textarea_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/residues');
      module_load_include('inc', 'tripal_chado', 'includes/fields/residues');
      tripal_chado_residues_textarea_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_seqlen_hidden_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/seqlen');
      module_load_include('inc', 'tripal_chado', 'includes/fields/seqlen');
      tripal_chado_seqlen_hidden_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_kvproperty_adder_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/kvproperty_adder');
      module_load_include('inc', 'tripal_chado', 'includes/fields/kvproperty_adder');
      tripal_chado_kvproperty_adder_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_kvproperty_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/kvproperty');
      module_load_include('inc', 'tripal_chado', 'includes/fields/kvproperty');
      tripal_chado_kvproperty_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_synonym_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/synonym');
      module_load_include('inc', 'tripal_chado', 'includes/fields/synonym');
      tripal_chado_synonym_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
    case 'tripal_chado_pub_widget':
      form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/pub');
      module_load_include('inc', 'tripal_chado', 'includes/fields/pub');
      tripal_chado_pub_widget($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      break;
  }
  return $widget;
}
/**
 * Implements hook_field_widget_info().
 */
function tripal_chado_field_widget_info() {
  return array(
    'tripal_chado_organism_select_widget' => array(
      'label' => t('Organism Select'),
      'field types' => array('organism_id')
    ),
    'tripal_chado_dbxref_id_widget' => array(
      'label' => t('Cross reference'),
      'field types' => array('dbxref_id'),
      'description' => t('This record can be cross referenced with a record in
        another online database. This field is intended for the most
        prominent reference.  At a minimum, the database and accession
        must be provided.'),
    ),
    'tripal_chado_dbxref_widget' => array(
      'label' => t('Cross references'),
      'field types' => array('dbxref'),
      'description' => t('This record can be cross referenced with a record
        in another online database. This field is intended for the most
        prominent reference.  At a minimum, the database and accession
        must be provided.'),
    ),
    'tripal_chado_cvterm_class_adder_widget' => array(
      'label' => t('Add an Annotation'),
      'field types' => array('cvterm_class_adder'),
    ),
    'tripal_chado_cvterm_widget' => array(
      'label' => t('Annotations'),
      'field types' => array('cvterm'),
      'description' => t('This record can be annotated with terms
          from other vocabularies.'),
    ),
    'tripal_chado_md5checksum_checkbox_widget' => array(
      'label' => t('MD5 Checksum Checkbox'),
      'field types' => array('md5checksum'),
    ),
    'tripal_chado_residues_textarea_widget' => array(
      'label' => t('Residues'),
      'field types' => array('residues'),
    ),
    'tripal_chado_seqlen_hidden_widget' => array(
      'label' => t('Sequence Length'),
      'field types' => array('seqlen'),
    ),
    'tripal_chado_kvproperty_adder_widget' => array(
      'label' => t('Add a Property'),
      'field types' => array('kvproperty_adder'),
    ),
    'tripal_chado_kvproperty_widget' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty'),
    ),
    'tripal_chado_synonym_widget' => array(
      'label' => t('Synonyms'),
      'field types' => array('synonym'),
    ),
    'tripal_chado_pub_widget' => array(
      'label' => t('Publications'),
      'field types' => array('pub'),
    ),
  );
}
/**
 * Returns the values of the field from the $form_state.
 */
function tripal_chado_get_field_form_values($field_name, $form_state, $delta = 0, $child = NULL) {
  $value = NULL;
  // The form_state must have the 'values' key. If not then just return.
  if (!array_key_exists('values', $form_state)) {
    return $value;
  }

  // If the field name is not in the form_state['values'] then return.
  if (!array_key_exists($field_name, $form_state['values'])) {
    return $value;
  }

  // Iterate through the values looking for the field_name provided.
  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    if (!array_key_exists($delta, $items)) {
      continue;
    }
    $item = $items[$delta];
    if ($child){
      if(array_key_exists($child, $item) and $item[$child] != '') {
        $value = $item[$child];
      }
    }
    else {
      $value = $item['value'];
    }
  }
  return $value;
}

/**
 * Sets the values of the field from the $form_state.
 */
function tripal_chado_set_field_form_values($field_name, &$form_state, $newvalue, $delta = 0, $child = NULL) {
  // The form_state must have the 'values' key. If not then just return.
  if (!array_key_exists('values', $form_state)) {
    return FALSE;
  }

  // If the field name is not in the form_state['values'] then reutrn.
  if (!array_key_exists($field_name, $form_state['values'])) {
    return FALSE;
  }

  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    if ($child) {
      $form_state['values'][$field_name][$langcode][$delta][$child] = $newvalue;
    }
    else {
      $form_state['values'][$field_name][$langcode][$delta]['value'] = $newvalue;
    }
  }
  return TRUE;
}

/**
 * Implements hook_field_widget_form_alter().
 */
function tripal_chado_field_widget_form_alter(&$element, &$form_state, $context) {

  if (array_key_exists('#field_name', $element)) {
    $field_name = $element['#field_name'];
    $matches = array();

    if (preg_match('/(.+?)__(.+?)$/', $field_name, $matches)) {
      $tablename = $matches[1];
      $colname = $matches[2];
      $schema = chado_get_schema($tablename);

      // The timelastmodified field exists in many Chado tables.  We want
      // the form element to update to the most recent time rather than the time
      // in the database.
      if ($colname == 'timelastmodified' and $schema['fields'][$colname]['type'] == 'datetime') {
        // We want the default value for the field to be the current time.
        $element['#default_value']['value'] = format_date(time(), 'custom', "Y-m-d H:i:s", 'UTC');
        $element['#date_items']['value'] = $element['#default_value']['value'];
      }
      // We want the date combo fieldset to be collaspible so we will
      // add our own theme_wrapper to replace the one added by the date
      // module.
      if (array_key_exists($colname, $schema['fields']) and $schema['fields'][$colname]['type'] == 'datetime') {
        $element['#theme_wrappers'] = array('tripal_chado_date_combo');
      }
    }
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function tripal_chado_field_formatter_info() {
  return array(
    'tripal_chado_organism_formatter' => array(
      'label' => t('Organism'),
      'field types' => array('organism_id')
    ),
    'tripal_chado_dbxref_id_formatter' => array(
      'label' => t('Cross reference'),
      'field types' => array('dbxref_id')
    ),
    'tripal_chado_dbxref_formatter' => array(
      'label' => t('Cross references'),
      'field types' => array('dbxref')
    ),
    'tripal_chado_cvterm_class_adder_formatter' => array(
      'label' => t('Add an Annotation'),
      'field types' => array('cvterm_class_adder')
    ),
    'tripal_chado_cvterm_formatter' => array(
      'label' => t('Annotations'),
      'field types' => array('cvterm')
    ),
    'tripal_chado_md5checksum_formatter' => array(
      'label' => t('MD5 checksum'),
      'field types' => array('md5checksum')
    ),
    'tripal_chado_residues_formatter' => array(
      'label' => t('Residues'),
      'field types' => array('residues')
    ),
    'tripal_chado_seqlen_formatter' => array(
      'label' => t('Sequence length'),
      'field types' => array('seqlen')
    ),
    'tripal_chado_kvproperty_adder_formatter' => array(
      'label' => t('Add a Property'),
      'field types' => array('kvproperty_adder')
    ),
    'tripal_chado_kvproperty_formatter' => array(
      'label' => t('Property'),
      'field types' => array('kvproperty')
    ),
    'tripal_chado_synonym_formatter' => array(
      'label' => t('Synonyms'),
      'field types' => array('synonym')
    ),
    'tripal_chado_pub_formatter' => array(
      'label' => t('Publications'),
      'field types' => array('pub')
    ),
  );
}

/**
 * Returns a $field_info array for a field based on a database column.
 *
 */
function tripal_chado_add_bundle_fields_base_fields_defaults($table_name, $schema, $column_name) {
  $details = $schema['fields'][$column_name];

  // Create an array with information about this field.
  $field = array(
    'field_type' => '',
    'widget_type' => '',
    'description' => '',
    'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
    'is_required' => 0,
    'storage' => 'field_chado_storage',
    'widget_settings' => array(
      'display_label' => 1
    ),
    'field_settings' => array(
      // The table in Chado where this field maps to.
      'chado_table' => $table_name,
      // The column in the Chado table that this field maps to.
      'chado_column' => $column_name,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textfield';
      $field['field_settings']['max_length'] = $details['length'];
      break;
    case 'varchar':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textfield';
      $field['field_settings']['max_length'] = $details['length'];
      break;
    case 'text':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textarea';
      $field['field_settings']['max_length'] = 17179869184;
      $field['field_settings']['text_processing'] = 1;
      $field['format'] = filter_default_format();
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field['field_type'] = 'number_integer';
      $field['widget_type'] = 'number';
      break;
    case 'float':
      $field['field_type'] = 'number_float';
      $field['widget_type'] = 'number';
      $field['field_settings']['precision'] = 10;
      $field['field_settings']['scale'] = 2;
      $field['field_settings']['decimal_separator'] = '.';
      break;
    case 'numeric':
      $field['field_type'] = 'number_decimal';
      $field['widget_type'] = 'number';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field['field_type'] = 'list_boolean';
      $field['widget_type'] = 'options_onoff';
      $field['field_settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
      break;
    case 'datetime':
      // Use the Drupal Date and Date API to create the field/widget
      $field['field_type'] = 'datetime';
      $field['widget_type'] = 'date_select';
      $field['widget_settings']['increment'] = 1;
      $field['widget_settings']['tz_handling'] = 'none';
      $field['widget_settings']['collapsible'] = TRUE;

      // TODO: Add settings so that the minutes increment by 1.
      // And turn off the timezone, as the Chado field doesn't support it.
      break;
  }

  // Set some default semantic web information
  if ($column_name == 'name') {
    $field['field_settings']['semantic_web']['type'] = 'name';
    $field['field_settings']['semantic_web']['ns'] = 'foaf';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://xmlns.com/foaf/0.1/';
  }
  if ($column_name == 'description' or $column_name == 'definition' or
      $column_name == 'comment') {
    $field['field_settings']['semantic_web']['type'] = 'description';
    $field['field_settings']['semantic_web']['ns'] = 'hydra';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://www.w3.org/ns/hydra/core#';
  }

  //
  // GENERIC COLUMNS
  //
  if ($field['field_settings']['chado_column'] =='organism_id') {
    $field['field_type'] = 'organism_id';
    $field['widget_type'] = 'tripal_chado_organism_select_widget';
    $field['label'] = 'Organism';
    $field['description'] = 'Select an organism.';
    $field['field_settings']['semantic_web']['type'] = 'organism';
    $field['field_settings']['semantic_web']['ns'] = 'local';
    $field['field_settings']['semantic_web']['nsurl'] = '';

  }
  elseif ($field['field_settings']['chado_column'] =='dbxref_id') {
    $field['field_type'] = 'dbxref_id';
    $field['widget_type'] = 'tripal_chado_primary_dbxref_widget';
    $field['label'] = 'Cross Reference';
    $field['description'] = 'This record can be cross referenced with a ' .
        'record in another online database. The primary reference is for the ' .
        'most prominent reference.  At a minimum, the database and accession ' .
        'must be provided.  To remove a set reference, change the database ' .
        'field to "Select a Database".';
  }
  elseif ($field['label'] == 'Timeaccessioned') {
    $field['label'] = 'Time Accessioned';
    $field['description'] = 'Please enter the time that this record was first added to the database.';
  }
  elseif ($field['label'] == 'Timelastmodified') {
    $field['label'] = 'Time Last Modified';
    $field['description'] = 'Please enter the time that this record was last modified. The default is the current time.';
  }
  //
  // ORGANISM TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'organism' and $field['field_settings']['chado_column'] == 'comment') {
    $field['label'] = 'Description';
  }
  //
  // FEATURE TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'uniquename') {
    $field['field_type'] = 'text';
    $field['widget_type'] = 'text_textfield';
    $field['field_settings']['text_processing'] = 0;
    $field['field_settings']['semantic_web']['type'] = 'name';
    $field['field_settings']['semantic_web']['ns'] = 'foaf';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://xmlns.com/foaf/0.1/';
  }
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'md5checksum') {
    $field['field_type'] = 'md5checksum';
    $field['widget_type'] = 'tripal_chado_md5checksum_checkbox_widget';
    $field['label'] = 'MD5 Checksum';
    $field['description'] = 'Generate an MD5 checksum for the sequence.';
  }
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'seqlen') {
    $field['field_type'] = 'seqlen';
    $field['widget_type'] = 'tripal_chado_seqlen_hidden_widget';
    $field['label'] = 'Seqlen';
    $field['description'] = 'The length of the residues.';
  }
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'residues') {
    $field['field_type'] = 'residues';
    $field['widget_type'] = 'tripal_chado_residues_textarea_widget';
    $field['label'] = 'Residues';
    $field['description'] = 'Please provide an IUPAC compatible residues for this feature. Spaces and new lines are allowed.';
  }
  //
  // ANALYSIS TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'program') {
    $field['field_settings']['semantic_web']['type'] = 'SoftwareApplication';
    $field['field_settings']['semantic_web']['ns'] = 'schema';
    $field['field_settings']['semantic_web']['nsurl'] = 'https://schema.org/';
    $field['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
    $field['label'] = 'Program, Pipeline, Workflow or Method Name.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourceuri') {
    $field['field_type'] = 'text';
    $field['widget_type'] = 'text_textfield';
    $field['field_settings']['text_processing'] = 0;
    $field['label'] = 'Source URL';
    $field['description'] = 'The URL where the original source data was derived.  Ideally, this should link to the page where more information about the source data can be found.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourcename') {
    $field['label'] = 'Source Name';
    $field['description'] = 'The name of the source data. This could be a file name, data set or a small description for how the data was collected. For long descriptions use the larger description field.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourceversion') {
    $field['label'] = 'Source Version';
    $field['description'] = 'If hte source data set has a version include it here.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'algorithm') {
    $field['label'] = 'Source Version';
    $field['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'programversion') {
    $field['label'] = 'Program Version';
    $field['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
  }
  //
  // PROJECT TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'project' and $field['field_settings']['chado_column'] == 'description') {
    $field['label'] = 'Short Description';
  }
  return $field;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_display_overview_form is used for formatting the display
 * or layout of fields attached to an entity and shown on the entity view page.
 *
 * This function removes the cvterm class and property adder field as those are
 * really not meant for users to show or manage.
 */
function tripal_chado_form_field_ui_display_overview_form_alter(&$form, &$form_state, $form_id) {
  // Remove the kvproperty_addr field as it isn't ever displayed. It's just used
  // on the add/edit form of an entity for adding new property fields.
  $fields_names = element_children($form['fields']);
  foreach ($fields_names as $field_name) {
    $field_info = field_info_field($field_name);
    if ($field_info['type'] == 'kvproperty_adder') {
      unset($form['fields'][$field_name]);
    }
    if ($field_info['type'] == 'cvterm_class_adder') {
      unset($form['fields'][$field_name]);
    }
  }
}
/**
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_overview_form is used for ordering and configuring the
 * fields attached to an entity.
 *
 * This function removes the property adder field as that is really not meant
 * for users to show or manage.
 */
function tripal_chado_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
  // Remove the kvproperty_addr field as it isn't ever displayed. It's just used
  // on the add/edit form of an entity for adding new property fields.
  $fields_names = element_children($form['fields']);
  foreach ($fields_names as $field_name) {
    $field_info = field_info_field($field_name);
    if ($field_info['type'] == 'kvproperty_adder') {
      unset($form['fields'][$field_name]);
    }
    if ($field_info['type'] == 'cvterm_class_adder') {
      unset($form['fields'][$field_name]);
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function tripal_chado_field_is_empty($item, $field) {

  // If there is no value field then the field is empty.
  if (!array_key_exists('value', $item)) {
    return TRUE;
  }

  // Iterate through all of the fields and if at least one has a value
  // the field is not empty.
  foreach ($item as $form_field_name => $value) {
    if (isset($value) and $value != NULL and $value != '') {
      return FALSE;
    }
  }

  // Otherwise, the field is empty.
  return TRUE;
}



/**
 * Implements hook_add_bundle_fields().
 */
function tripal_chado_add_bundle_fields($entity_type, $bundle, $term) {

  $bundle_name = $bundle->name;

  // This array will hold details that map the bundle to tables in Chado.
  $bundle_data = array();

  // Get the cvterm that corresponds to this TripalTerm object.
  $vocab = entity_load('TripalVocab', array($term->vocab_id));
  $vocab = reset($vocab);
  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $vocab->namespace,
      ),
      'accession' => $term->accession
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);

  // The organism table does not have a type_id so we won't ever find
  // a record for it in the tripal_cv_defaults table.
  if ($cvterm->name == 'organism') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'organism',
      'type_table' => 'organism',
      'field' =>  '',
    );
  }
  // The analysis table does not have a type_id so we won't ever find
  // a record for it in the tripalcv_defaults table.
  else if ($cvterm->name == 'analysis') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'analysis',
      'type_table' => 'analysis',
      'field' =>  '',
    );
  }
  else if ($cvterm->name == 'project') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'project',
      'type_table' => 'project',
      'field' =>  '',
    );
  }
  else {
    // TODO: WHAT TO DO IF A VOCABULARY IS USED AS A DEFAULT FOR MULTIPLE
    // TABLES.
    // Look to see if this vocabulary is used as a default for any table.
    $default = db_select('tripal_cv_defaults', 't')
      ->fields('t')
      ->condition('cv_id', $cvterm->cv_id->cv_id)
      ->execute()
      ->fetchObject();
    if ($default) {
      $bundle_data = array(
        'cv_id' => $cvterm->cv_id->cv_id,
        'cvterm_id' => $cvterm->cvterm_id,
        'data_table' => $default->table_name,
        'type_table' => $default->table_name,
        'field' =>  $default->field_name,
      );
    }
  }

  // Save the mapping information so that we can reuse it when we need to
  // look things up for later for an entity
  tripal_set_bundle_variable('chado_cvterm_id', $bundle->id, $bundle_data['cvterm_id']);
  tripal_set_bundle_variable('chado_table', $bundle->id, $bundle_data['data_table']);
  tripal_set_bundle_variable('chado_column', $bundle->id, $bundle_data['field']);

  //////////////////////////////////////////////////////////////////////////////
  // ADD FIELDS TO BUNDLE
  ////////////////////////////////////////////////////////////////////////////

  ////
  //
  // Base table fields.
  //
  // Adds the fields for the base table to the entity.  Adds fields
  // for all columns including FK fields.  Excludes primary key and the
  // type_id field (that's inherent in the bundle).
  tripal_chado_add_bundle_fields_base_fields($entity_type, $bundle_name, $bundle_data);

  ////
  //
  // Property table fields.
  //
  // Check to see if there are any property tables with FKs to this
  // base table. If so, add the fields for that type of table.
  $prop_table = $bundle_data['data_table'] . 'prop';
  if (chado_table_exists($prop_table)) {
    tripal_chado_add_bundle_fields_kvproperty_adder_field($entity_type, $bundle_name, $prop_table, $bundle_data['data_table']);
  }

  ////
  //
  // Dbxref table fields.
  //
  // Check to see if there are any dbxref tables with FKs to this
  // base table. If so, add the fields for that type of table.
  $dbxref_table = $bundle_data['data_table'] . '_dbxref';
  if (chado_table_exists($dbxref_table)) {
    tripal_chado_add_bundle_fields_dbxref_field($entity_type, $bundle_name, $dbxref_table, $bundle_data['data_table']);
  }

  ////
  //
  // Cvterm table fields.
  //
  // Check to see if there are any cvterm tables with FKs to this
  // base table. If so, add the fields for that type of table.
  $cvterm_table = $bundle_data['data_table'] . '_cvterm';
  if (chado_table_exists($cvterm_table)) {
    tripal_chado_add_bundle_fields_cvterm_class_adder_field($entity_type, $bundle_name, $cvterm_table, $bundle_data['data_table']);
  }


  ////
  //
  // Synonym table fields.
  //
  // Check to see if there are any synonym tables with FKs to this
  // base table. If so, add the fields for that type of table.
  $syn_table = $bundle_data['data_table'] . '_synonym';
  if (chado_table_exists($syn_table)) {
    tripal_chado_add_bundle_fields_synonym_field($entity_type, $bundle_name, $syn_table, $bundle_data['data_table']);
  }

  ////
  //
  // Pub table fields.
  //
  // Check to see if there are any pub tables with FKs to this
  // base table. If so, add the fields for that type of table.
  $pub_table = $bundle_data['data_table'] . '_pub';
  if (chado_table_exists($pub_table)) {
    tripal_chado_add_bundle_fields_pub_field($entity_type, $bundle_name, $pub_table, $bundle_data['data_table']);
  }
}

/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_dbxref_field($entity_type_name, $bundle_name, $dbxref_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $dbxref_table;
  $schema = chado_get_schema($dbxref_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'dbxref',
    'widget_type' => 'tripal_fields_dbxfref_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Cross References',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $dbxref_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  // If the base table has a 'dbxref_id' then change the label to
  // indicate these are secondary cross references.
  $schema = chado_get_schema($base_table);
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_info['label'] = 'Secondary Cross References';
  }
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_synonym_field($entity_type_name, $bundle_name, $syn_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $syn_table;
  $schema = chado_get_schema($syn_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'synonym',
    'widget_type' => 'tripal_fields_synonym_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Synonyms',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $syn_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_pub_field($entity_type_name, $bundle_name, $pub_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $pub_table;
  $schema = chado_get_schema($pub_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'pub',
    'widget_type' => 'tripal_fields_pub_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Publications',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $pub_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing properties that are stored in a prop table.
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_chado_add_bundle_fields_kvproperty_adder_field($entity_type_name, $bundle_name, $kv_table, $base_table) {
  $field_name = $kv_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'kvproperty_adder',
    'widget_type' => 'tripal_fields_kvproperty_adder_widget',
    'field_settings' => array(
      'base_table' => $base_table,
    ),
    'storage' => 'field_chado_storage',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Properties',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing properties that are stored in a prop table.
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_chado_add_bundle_fields_cvterm_class_adder_field($entity_type_name, $bundle_name, $cvterm_table, $base_table) {
  // First add a generic property field so that users can add new property types.
  $field_name = $cvterm_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'cvterm_class_adder',
    'widget_type' => 'tripal_fields_cvterm_class_adder_widget',
    'field_settings' => array(
      'base_table' => $base_table,
    ),
    'storage' => 'field_chado_storage',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Annotation Types',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for the base table to the entity.
 */
function tripal_chado_add_bundle_fields_base_fields($entity_type_name, $bundle_name, $bundle_data) {

  $table_name = $bundle_data['data_table'];
  $type_table = $bundle_data['type_table'];
  $type_field = $bundle_data['field'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    $field_name = $table_name . '__' . $column_name;

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    // Get the field defaults for this column.
    $field_info = tripal_chado_add_bundle_fields_base_fields_defaults($table_name, $schema, $column_name);


    // TODO: add in a call to drupal_alter to allow other modules to change
    // the field settings.

    // Determine if the field is required.
    if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
      $field_info['is_required'] = array_key_exists('default', $details) ? 0 : 1;
    }

    // If we don't have a field type then we don't need to create a field.
    if (!$field_info['field_type']) {
      // If we don't have a field type but it is required and doesn't have
      // a default value then we are in trouble.
      if ($field_info['is_required'] and !array_key_exists('default', $details)) {
        throw new Exception(t('The %table.%field type, %type, is not yet supported for Entity fields, but it is required,',
            array('%table' => $table_name, '%field' => $column_name, '%type' => $details['type'])));
      }
      continue;
    }

    // If this field is a foreign key field then we will have a custom field.
    $is_fk = FALSE;
    if (array_key_exists('foreign keys', $schema)) {
      foreach ($schema['foreign keys'] as $remote_table => $fk_details) {
        if (array_key_exists($column_name, $fk_details['columns'])) {
          $is_fk = TRUE;
        }
      }
    }

    // Add the field to the bundle.
    tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
  }
}