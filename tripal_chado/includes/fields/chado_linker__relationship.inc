<?php

/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_linker__relationship_info() {
  return array(
    'label' => t('Relationships'),
    'description' => t('Relationships between features.'),
    'default_widget' => 'chado_linker__relationship_widget',
    'default_formatter' => 'chado_linker__relationship_formatter',
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
function chado_linker__relationship_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];

  // If the linker table does not exists then we don't want to add attach.
  $rel_table = $table_name . '_relationship';
  if (!chado_table_exists($rel_table)) {
    return $field_info;
  }

  $schema = chado_get_schema($rel_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_name' => $table_name . '__relationship',
    'field_type' => 'chado_linker__relationship',
    'widget_type' => 'chado_linker__relationship_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => 'A generic field for displaying relationships between data types',
    'label' => 'Relationships',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      'chado_table' => $rel_table,
      'chado_column' => $pkey,
      'base_table' => $table_name,
      'semantic_web' => array(
        'name' => '',
        'accession' => '',
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
function chado_linker__relationship_widget_info() {
  return array(
    'label' => t('Relationship Settings'),
    'field types' => array('chado_linker__relationship')
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
function chado_linker__relationship_formatter_info() {
  return array(
    'label' => t('Relationships'),
    'field types' => array('chado_linker__relationship'),
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
function chado_linker__relationship_formatter_settings_summary($field, $instance,
    $view_mode) {

}

/**
 * Provides a settings form for the formatter.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_form() for each field
 * to specify.
 */
function chado_linker__relationship_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {



}

/**
 * Validation function for the chado_linker_featureloc_formatter_settings_form.
 */
function chado_linker__relationship_formatter_settings_form_validate(&$form, &$form_state) {

  // Place here as an example for validating the settings form.
}

/**
 * TODO: because this field is meant to handle any xxxxx_relationship table
 * then feature hard-coding needs to be replaced as it won't work for
 * stocks, etc.
 */
function chado_linker__relationship_formatter(&$element, $entity_type, $entity,
    $field, $instance, $langcode, $items, $display) {

  // Get the settings
  $settings = $display['settings'];
  $record = $entity->chado_record;

  foreach ($items as $delta => $item) {

    $all_relationships = $item['all_relationships'];
    $object_rels = $all_relationships['object'];
    $subject_rels = $all_relationships['subject'];
    $content = '';

    if (count($object_rels) > 0 or count($subject_rels) > 0) {

      // first add in the subject relationships.
      foreach ($subject_rels as $rel_type => $rels){
        foreach ($rels as $obj_type => $objects){

          // Make the verb in the sentence make sense in English.
          switch ($rel_type) {
            case 'integral part of':
            case 'instance of':
              $verb = 'is an';
              break;
            case 'proper part of':
            case 'transformation of':
            case 'genome of':
            case 'part of':
            case 'position of':
            case 'sequence of':
            case 'variant of':
              $verb = 'is a';
              break;
            case 'derives from':
            case 'connects on':
            case 'contains':
            case 'finishes':
            case 'guides':
            case 'has origin':
            case 'has part':
            case 'has quality':
            case 'is consecutive sequence of':
            case 'maximally overlaps':
            case 'overlaps':
            case 'starts':
              $verb = '';
              break;
            default:
              $verb = 'is';
          }

          // the $headers array is an array of fields to use as the colum headers.
          // additional documentation can be found here
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $headers = array('Feature Name' ,'Unique Name', 'Species', 'Type');

          // the $rows array contains an array of rows where each row is an array
          // of values for each column of the table in that row.  Additional documentation
          // can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $rows = array();

          foreach ($objects as $object){
            // link the feature to it's node
            $feature_name = $object->record->object_id->name;
            if (property_exists($object->record, 'nid')) {
              $feature_name = l($feature_name, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank")));
            }
            // link the organism to it's node
            $organism = $object->record->object_id->organism_id;
            $organism_name = $organism->genus ." " . $organism->species;
            if (property_exists($organism, 'nid')) {
              $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
            }
            $rows[] = array(
              array('data' => $feature_name, 'width' => '30%'),
              array('data' => $object->record->object_id->uniquename, 'width' => '30%'),
              array('data' => $organism_name, 'width' => '30%'),
              array('data' => $object->record->object_id->type_id->name, 'width' => '10%'),
            );
           }
           // the $table array contains the headers and rows array as well as other
           // options for controlling the display of the table.  Additional
           // documentation can be found here:
           // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
           $table = array(
             'header' => $headers,
             'rows' => $rows,
             'attributes' => array(
               'id' => 'tripal_feature-table-relationship-object',
               'class' => 'tripal-data-table'
             ),
             'sticky' => FALSE,
             'caption' => "This " . $record->type_id->name . " $verb  $rel_type the following  <b>$obj_type</b> feature(s):",
             'colgroups' => array(),
             'empty' => '',
           );

           // once we have our table array structure defined, we call Drupal's theme_table()
           // function to generate the table.
           $content .= theme_table($table);
         }
      }

      // second add in the object relationships.
      foreach ($object_rels as $rel_type => $rels){
        foreach ($rels as $subject_type => $subjects){

          // Make the verb in the sentence make sense in English.
          switch ($rel_type) {
            case 'integral part of':
            case 'instance of':
              $verb = 'are an';
              break;
            case 'proper part of':
            case 'transformation of':
            case 'genome of':
            case 'part of':
            case 'position of':
            case 'sequence of':
            case 'variant of':
              $verb = 'are a';
              break;
            case 'derives from':
            case 'connects on':
            case 'contains':
            case 'finishes':
            case 'guides':
            case 'has origin':
            case 'has part':
            case 'has quality':
            case 'is consecutive sequence of':
            case 'maximally overlaps':
            case 'overlaps':
            case 'starts':
              $verb = '';
              break;
            default:
              $verb = 'are';
          }

          // the $headers array is an array of fields to use as the colum headers.
          // additional documentation can be found here
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $headers = array('Feature Name' ,'Unique Name', 'Species', 'Type');

          // the $rows array contains an array of rows where each row is an array
          // of values for each column of the table in that row.  Additional documentation
          // can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $rows = array();

          foreach ($subjects as $subject){
            // link the feature to it's node
            $feature_name = $subject->record->subject_id->name;
            if (property_exists($subject->record, 'nid')) {
              $feature_name = l($feature_name, "node/" . $subject->record->nid, array('attributes' => array('target' => "_blank")));
            }
            // link the organism to it's node
            $organism = $subject->record->subject_id->organism_id;
            $organism_name = $organism->genus ." " . $organism->species;
            if (property_exists($organism, 'nid')) {
              $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
            }
            $rows[] = array(
              array('data' => $feature_name, 'width' => '30%'),
              array('data' =>$subject->record->subject_id->uniquename, 'width' => '30%'),
              array('data' =>$organism_name, 'width' => '30%'),
              array('data' =>$subject->record->subject_id->type_id->name, 'width' => '10%'),
            );
           }
           // the $table array contains the headers and rows array as well as other
           // options for controlling the display of the table.  Additional
           // documentation can be found here:
           // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
           $table = array(
             'header' => $headers,
             'rows' => $rows,
             'attributes' => array(
               'id' => 'tripal_feature-table-relationship-subject',
               'class' => 'tripal-data-table'
             ),
             'sticky' => FALSE,
             'caption' => "The following <b>" . $subjects[0]->record->subject_id->type_id->name . " </b> feature(s) $verb $rel_type this " . $record->type_id->name . ":",
             'colgroups' => array(),
             'empty' => '',
           );

           // once we have our table array structure defined, we call Drupal's theme_table()
           // function to generate the table.
           $content .= theme_table($table);
         }
      }
    }

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 */
function chado_linker__relationship_load($field, $entity, $base_table, $record) {
  $field_name = $field['field_name'];


  $entity->{$field_name}['und'][0]['all_relationships'] = tripal_get_feature_relationships($record);
}


/**
 * Implements hook_ws_formatter().
 */
// function chado_linker__relationship_ws_formatter(&$element, $entity_type, $entity,
//     $field, $instance, $items) {

//   foreach ($items as $delta => $item) {

//   }
// }

/**
 *  Implements hook_widget().
 */
function chado_linker__relationship_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

}
/**
 * Callback function for validating the chado_linker_featureloc_widget.
 */
function chado_linker__relationship_widget_validate($element, &$form_state) {

}