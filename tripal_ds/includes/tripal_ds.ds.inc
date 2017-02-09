<?php

/**
 * Implements hook_ds_layout_settings_info().
 */
function _ds_layout_settings_info($bundle_name, $instances) {
    $region_right = array(); 
    $region_left = array();
    $prop_fields = array(); 
    $summary_fields = array();
    $data_sequence_fields = array(); 
    $all_other_fields = array();
    $fields_with_regions = array();
    $i = 0;
    foreach ($instances as $key => $instance){
        $instance_name = $instance['field_name'];
        if($instance_name=="rdfs__type"){
            array_push($summary_fields, $instance_name);
            $fields_with_regions[$instance_name]= 'right';

        } 
        else {
            //TODO: How do we handle non-chado dbs, placement of fields within tripal panes might need to be done in a hook. 
            $instance_base_table = $instance['settings']['base_table'];
            $instance_base_chado = $instance['settings']['chado_table'];
            $prop_table = strpos($instance_base_chado, 'prop');
            $data_sequence = strpos($instance_name, 'data__sequence');

            if ($instance_base_chado && $instance_base_table){

                if ($instance_base_chado == $instance_base_table){
                    if ($prop_table !== FALSE){
                        //Properties section instances
                        array_push($prop_fields, $instance_name);
                        $fields_with_regions[$instance_name]= 'right';

                    } 
                    elseif ($data_sequence !== FALSE) {
                        //data sequence section instances
                        array_push($data_sequence_fields, $instance_name);
                        $fields_with_regions[$instance_name] = 'right';

                    } 
                    else {
                        //overview section instances
                        array_push($summary_fields, $instance_name);
                        $fields_with_regions[$instance_name] = 'right';
                    }

                } 
                elseif ($instance_base_chado != $instance_base_table){
                    if ($prop_table !== FALSE){
                        //Properties section instances
                        array_push($prop_fields, $instance_name);
                        $fields_with_regions[$instance_name]= 'right';

                    } 
                    elseif ($data_sequence !== FALSE){
                        //data sequence section instances
                        array_push($data_sequence_fields, $instance_name);
                        $fields_with_regions[$instance_name]= 'right';

                    } 
                    else {
                        //Linker section instances
                        array_push($all_other_fields, $instance);
                        $fields_with_regions[$instance_name]= 'right';

                        //update the display settings so that the title is hidden
                        $instance['display']['default']['label'] = 'hidden';
                        field_update_instance($instance);
                    }
                }
            }
        }
        $i++;
    }

    //consolidate the field sets
    if(!empty($summary_fields)){
        _summary_field_group_info($bundle_name, $summary_fields);

        //add the fields to the regions
        array_unshift($summary_fields, 'group_summary_tripalpane', 'group_summary', 'group_summary_table');
        $fields_with_regions += [ 'group_summary_tripalpane' =>'right', 'group_summary' => 'right', 'group_summary_table' => 'right' ];
        $region_right = array_merge($summary_fields, $region_right);

    }
    if (!empty($prop_fields)){
        _prop_field_group_info($bundle_name, $prop_fields);
        array_unshift($prop_fields, 'group_prop_tripalpane', 'group_prop', 'group_prop_table');

        //add the fields to the regions
        $region_right = array_merge($region_right, $prop_fields);
        $fields_with_regions += [ 'group_prop_tripalpane' => 'right', 'group_prop' => 'right', 'group_prop_table' => 'right' ];
    }
    if (!empty($data_sequence_fields)){
        _data_sequence_field_group_info($bundle_name, $data_sequence_fields);
        array_unshift($data_sequence_fields, 'group_sequence_tripalpane', 'group_sequence', 'group_sequence_table');

        //add the fields to the regions
        $region_right = array_merge($region_right, $data_sequence_fields);
        $fields_with_regions += [ 'group_sequence_tripalpane' => 'right', 'group_sequence' => 'right', 'group_sequence_table' => 'right' ];
    }
    if (!empty($all_other_fields)){
        foreach ($all_other_fields as $key => $other_field) {
          //temporary field names
          $temporary_field = array();
          $group_field_name = 'gp_'.$other_field['field_name'];
          $fieldset_field_name = 'ft_'.$other_field['field_name'];

          //need to truncate the names because of database field size restrictions, updating fields here to ensure name consistency
          $group_field_name = substr($group_field_name, 0, 27);
          $fieldset_field_name = substr($fieldset_field_name, 0, 27);

          //add randomm numbers to ensure the field name is unique within the 32 character limit of the field
          $group_field_name = $group_field_name.rand(0, 99999);
          $fieldset_field_name = $fieldset_field_name.rand(0, 99999);

          //build the field group
          _additional_fields_field_group_info($bundle_name, $other_field['label'], $group_field_name, $fieldset_field_name, $other_field['field_name']);
          //update arrays
          array_push($temporary_field, $group_field_name, $fieldset_field_name, $other_field['field_name']);
          $region_right = array_merge($region_right, $temporary_field);
          $fields_with_regions += [ $group_field_name => 'right', $fieldset_field_name => 'right' ];
        }
    }
    //add blocks to $region_left
    $position = count($region_right);
    $region_left[$position] = 'toc';
    $fields_with_regions += [ 'toc' => 'left'];
    //build the ds layout
    $record = new stdClass;
    $record->id ='TripalEntity|' . $bundle_name . '|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = $bundle_name;
    $record->view_mode = 'default';
    $record->layout = 'tripal_ds_feature';
    $settings = array(
        'regions' => array(
            'left' => 
                $region_left,
            'right' =>
                $region_right,
        ),
        'fields' =>
            $fields_with_regions,
        'classes' => array(),
        'wrappers' => array(
            'left' => 'div',
            'right' => 'div',
        ),
        'layout_wrapper' => 'div',
        'layout_attributes' => '',
        'layout_attributes_merge' => 1,
        'layout_link_attribute' => '',
        'layout_link_custom' => '',
        'layout_disable_css' => 0,
    );
    $record->settings = $settings;
    drupal_write_record('ds_layout_settings', $record);
}


/*
 * Implements hook_ds_fields_info().
*/
function tripal_ds_ds_fields_info($entity_type) {
  $fields = array();

  $fields['toc'] = array(
    'title' => t('Table of Contents'),
    'field_type' => DS_FIELD_TYPE_FUNCTION,
    'function' => 'tripal_ds_toc_block',
  );
  return array('TripalEntity' => $fields);
}

function tripal_ds_toc_block($entity_type) {
    $bundle_name = $entity_type['bundle'];
    $toc = views_embed_view('tripal_content_type_toc', 'block', $bundle_name);
    return $toc;
}

