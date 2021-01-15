<?php

class so__transcript_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Transcript';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__transcript'];
    
  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    $_entity = $entity;
    
    // if no transcript (mRNA) fields are selected in the transcript fields edit page, use a table to list the mRNA name, id, type and location only
    $headers = ['Transcript Name', 'Identifier', 'Type', 'Location'];
    $rows = [];
    
    $mRNA_fieldset = [];
    $fields_exist = FALSE; 
    foreach ($items as $delta => $item) {

      if (!$item['value']) {
        continue;
      }
      
      $transcript = $item['value'];

      // Get the field values
      $feature_name = $transcript['schema:name'];
      $feature_uname = $transcript['data:0842'];
      $loc = $transcript['SO:0000735'];
      $type = $transcript['rdfs:type'];
      
      // Add a link if there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
      
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        
        $transcript_name = $feature_name;
        // the transcript id is the identifier for the mRNA transcript
        $transcript_id = $entity_id; 
        $mRNA_fieldset[$transcript_id] = [
          '#type' => 'fieldset',
          '#title' => $transcript_name,
          '#description' => '',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#attributes' => ['class' => ['collapsible', 'collapsed']],
        ];
        drupal_add_library('system', 'drupal.collapse');
        $mRNA_fieldset[$transcript_id]['table'] = [];
        
        $transcript_fields_header = [];
        $transcript_field_rows = [];
        $single_cardinality_fields_exist = FALSE;
        
        // mRNA expanded fields
        // obtain each field for the mRNA as specified in the transcripts field edit configuration
        $fields = $this->instance['settings']['transcript_fields'];
        $entity = TripalEntity_load($entity_id);
        foreach ($fields as $field_name => $field_value) {

          if (array_key_exists('show', $field_value) and ($field_value['show'] != 1)) {
            // this field is not selected for display in the transcripts field edit configuration page  
            continue;
          }
          $fields_exist = TRUE;
          
          // Load the field info.
          $field_info = field_info_field($field_name);
          $field_instance = field_info_instance('TripalEntity', $field_name, $entity->bundle); // get title and description
          $field_items = field_get_items('TripalEntity', $entity, $field_name);
          $field_element = field_view_field('TripalEntity', $entity, $field_name, $field_instance['display']['default']);
          $field_element['#label_display'] = 'hidden';
          
          // use $_entity parameter for gene to obtain the 'hide empty' gene field setting
          $bundle = tripal_load_bundle_entity(['name'=> $_entity->bundle]);
          $hide_empty = tripal_get_bundle_variable('hide_empty_field', $bundle->id);
          $cardinality = array_key_exists('cardinality', $field_info) ? $field_info['cardinality'] : 1; 
          $field_is_empty = $this->isFieldEmpty($field_items);
          
          // do not show this field if empty and specified to be hidden from the gene edit page
          if ($field_is_empty and $hide_empty) {
            continue;
          }
          
          // check field cardinality 
          if ($cardinality == 1) {
            // add field to a special transcripts table, where fields have cardinality of 1
            $single_cardinality_fields_exist = TRUE;
            $transcript_field_rows[] = [
              [
                'data' => $field_instance['label'], 
                'header' => TRUE,
              ],
              [
                'data' => drupal_render($field_element),
              ]  
            ];
          }
          else {
            // render the fields with cardinality not 1 individually after the special transcripts table.  
            $mRNA_fieldset[$transcript_id][$field_name] = [
              '#type' => 'item',
              '#title' => $field_instance['label'],
              '#description' => $field_instance['description'],  
              '#markup' => drupal_render($field_element),
            ];
          }
        }
        
        $feature_name = l($feature_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
      
        if ($single_cardinality_fields_exist) {
          // display fields of single cardinality in a special transcripts table
          $transcript_fields_table = [
            'header' => $transcript_fields_header,
            'rows' => $transcript_field_rows,
            'attributes' => [
              'id' => 'tripal_feature-table-transcript-fields-object',
              'class' => 'tripal-data-table',
            ],
            'sticky' => FALSE,
            'caption' => "",
            'colgroups' => [],
            'empty' => 'This feature has no single cardinality transcript fields',
          ];
          
          $mRNA_fieldset[$transcript_id]['table'] = [
            '#type' => 'markup',
            '#markup' => theme_table($transcript_fields_table),
          ];
        }
      }
      
      $mRNA_fieldset[$transcript_id]['mRNA_link'] = [
        '#type' => 'markup',
        '#markup' => "For further details go to ".$feature_name,
      ];
      $rows[] = [$feature_name, $feature_uname, $type, $loc];
    }
    
    $table = [
      'header' => $headers,
        'rows' => $rows,
        'attributes' => [
        'id' => 'tripal_feature-table-transcripts-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'This feature has no transcripts',
    ];
       
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      if ($fields_exist) {
        $content = $mRNA_fieldset;
        $element[0] = $content;
      }
      else {
        $content = theme_table($table);
        $element[0] = [
          '#type' => 'markup',
          '#markup' => $content,
        ];
      }
    }
  }
  
  /**
   * Checks if the field is empty. 
   *
   * @param $field_items
   *   The TripalEntity field items, holds the value for the field.
   *
   * @return bool 
   *   TRUE if field is empty, otherwise FALSE.
   */
  public function isFieldEmpty($field_items) {
      
    $field_is_empty = FALSE;

    // handle when field_items is an array with single of multiple values
    if (is_array($field_items)) {
      if (sizeof($field_items) == 1) {
        if (sizeof($field_items[0]) == 1) {
          if (is_string($field_items[0]['value'])) {
            if (strlen($field_items[0]['value']) == 0) {
              // the value is of zero length  
              $field_is_empty = TRUE;
            }
          }
        }
        else {
          $all_empty = TRUE;
          foreach ($field_items[0] as $key=>$val) {
            if ((is_string($val) and (strlen($val) != 0)) or (!is_string($val))) {
              // the value is a string of non-zero length, or not a string at all  
              $all_empty = FALSE;
            }
          }
          if ($all_empty) {
            $field_is_empty = TRUE;
          }
        }
      }
    }
    elseif ($field_items == null) {
      $field_is_empty = TRUE;
    }
    
    return $field_is_empty;
    
  }
  
}

