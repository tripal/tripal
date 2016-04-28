<?php

/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_gene__transcripts_info() {
  return array(
    'label' => t('Transcripts'),
    'description' => t('Transcripts of genes.'),
    'default_widget' => 'chado_gene__transcripts_widget',
    'default_formatter' => 'chado_gene__transcripts_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_gene__transcripts_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];

  // If the linker table does not exists or this is not a gene then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (!chado_table_exists($rel_table) || $bundle->label != 'gene') {
    return $field_info;
  }

  $schema = chado_get_schema($rel_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_name' => 'gene__transcripts',
    'field_type' => 'chado_gene__transcripts',
    'widget_type' => 'chado_gene__transcripts_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Transcripts',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      'chado_table' => $rel_table,
      'chado_column' => $pkey,
      'base_table' => $table_name,
      'semantic_web' => array(
        'type' => '',
        'ns' => '',
        'nsurl' => '',
      ),
    ),
  );

  return $field_info;

}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_gene__transcripts_widget_info() {
  return array(
    'label' => t('Transcripts Settings'),
    'field types' => array('chado_gene__transcripts')
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_gene__transcripts_formatter_info() {
  return array(
    'label' => t('Transcripts'),
    'field types' => array('chado_gene__transcripts'),
    'settings' => array(
    ),
  );
}

/**
 * Implements hook_formatter_settings_summary.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_summary() for each field
 * to specify.
 *
 */
function chado_gene__transcripts_formatter_settings_summary($field, $instance,
    $view_mode) {

}

/**
 * Provides a settings form for the formatter.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_form() for each field
 * to specify.
 */
function chado_gene__transcripts_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {



}

/**
 * Validation function for the chado_gene_featureloc_formatter_settings_form.
 */
function chado_gene__transcripts_formatter_settings_form_validate(&$form, &$form_state) {

  // Place here as an example for validating the settings form.
}

/**
 * TODO: because this field is meant to handle any xxxxx_transcripts table
 * then feature hard-coding needs to be replaced as it won't work for
 * stocks, etc.
 */
function chado_gene__transcripts_formatter(&$element, $entity_type, $entity,
    $field, $instance, $langcode, $items, $display) {

  // Get the settings
  $settings = $display['settings'];
  $record = $entity->chado_record;

  foreach ($items as $delta => $item) {

    $all_transcripts = key_exists('part of', $item['all_transcripts']['object'])  && 
    key_exists('mRNA', $item['all_transcripts']['object']['part of']) ? 
    $item['all_transcripts']['object']['part of']['mRNA'] : array();   
    
    $headers = array('Feature Name' ,'Unique Name', 'Location');
    $rows = array();

    foreach ($all_transcripts AS $transcript) {
      // link the feature to it's node
      $feature_name = $transcript->record->subject_id->name;
      $entity_id = $transcript->record->subject_id->entity_id;
      if ($entity_id) {
        $feature_name = l($feature_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $locations = $transcript->child_featurelocs;
      $loc = "";
      foreach ($locations AS $location) {
        $loc .= $location->srcfeature_name . ":" . $location->fmin . ".." . $location->fmax . "<br>";
      }
      $rows[] = array(
        array('data' => $feature_name, 'width' => '30%'),
        array('data' => $transcript->record->subject_id->uniquename, 'width' => '30%'),
        array('data' => $loc, 'width' => '30%'),
      );
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-transcripts-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => '',
    );

    $content = theme_table($table);
  }

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  $element[$delta] = array(
    '#type' => 'markup',
    '#markup' => $content,
  ); 
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 */
function chado_gene__transcripts_load($field, $entity, $base_table, $record) {
  $field_name = $field['field_name'];


  $entity->{$field_name}['und'][0]['all_transcripts'] = tripal_get_feature_relationships($record);
}

/**
 * Implements hook_ws_formatter().
 */
// function chado_gene__transcripts_ws_formatter(&$element, $entity_type, $entity,
//     $field, $instance, $items) {

//   foreach ($items as $delta => $item) {

//   }
// }

/**
 *  Implements hook_widget().
 */
function chado_gene__transcripts_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

}
/**
 * Callback function for validating the chado_gene_featureloc_widget.
 */
function chado_gene__transcripts_widget_validate($element, &$form_state) {

}