<?php
/**
 * Implements hook_ds_layout_settings_info().
 */
function _ds_layout_settings_info($bundle_name, $instances) {
    $region_right = array();
    $region_left = array();
    $region_top = array();
    $region_bottom = array();
    $prop_fields = array();
    $data_sequence_fields = array();
    $all_other_fields = array();
    $fields_with_regions = array();
    $i = 0;

    foreach ($instances as $key => $instance){
        $instance_name = $instance['field_name'];

        if($instance_name=="rdfs__type"){
            array_push($region_top, $instance_name);
            $fields_with_regions[$instance_name]= 'top';

        } else {
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

                    } elseif ($data_sequence !== FALSE) {
                        //data sequence section instances
                        array_push($data_sequence_fields, $instance_name);
                        $fields_with_regions[$instance_name] = 'right';

                    }else {
                        //overview section instances
                        array_push($region_top, $instance_name);
                        $fields_with_regions[$instance_name] = 'top';
                    }

                } elseif ($instance_base_chado != $instance_base_table){
                    if ($prop_table !== FALSE){
                        //Properties section instances
                        array_push($prop_fields, $instance_name);
                        $fields_with_regions[$instance_name]= 'right';

                    } elseif ($data_sequence !== FALSE){
                        //data sequence section instances
                        array_push($data_sequence_fields, $instance_name);
                        $fields_with_regions[$instance_name]= 'right';

                    } else {
                        //Linker section instances
                        //array_push($region_right, $instance_name);
                        array_push($all_other_fields, $instance);
                        $fields_with_regions[$instance_name]= 'right';

                    }
                }
            }
        }
        $i++;
    }

    //add the field sets to the region arrays
    if(!empty($region_top)){
        _overview_field_group_info($bundle_name, $region_top);

        //add the fields to the regions
        array_unshift($region_top, 'group_summary_tripalpane', 'group_summary', 'group_summary_table');
        $fields_with_regions += [ 'group_summary_tripalpane' =>'top', 'group_summary' => 'top', 'group_summary_table' => 'top' ];
    }
    if (!empty($prop_fields)){
        _prop_field_group_info($bundle_name, $prop_fields);
        array_unshift($prop_fields, 'group_prop_tripalpane', 'group_prop', 'group_prop_table');

        //add the fields to the regions
        $region_right = array_merge($prop_fields, $region_right);
        $fields_with_regions += [ 'group_prop_tripalpane' => 'right', 'group_prop' => 'right', 'group_prop_table' => 'right' ];
    }
    if (!empty($data_sequence_fields)){
        _data_sequence_field_group_info($bundle_name, $data_sequence_fields);
        array_unshift($data_sequence_fields, 'group_sequence_tripalpane', 'group_sequence', 'group_sequence_table');

        //add the fields to the regions
        $region_right = array_merge($data_sequence_fields, $region_right);
        $fields_with_regions += [ 'group_sequence_tripalpane' => 'right', 'group_sequence' => 'right', 'group_sequence_table' => 'right' ];
    }
    if (!empty($all_other_fields)){
        foreach ($all_other_fields as $key => $other_field) {
          //temporary fields
          $temporary_field = array();
          $group_field_name = 'group_'.$other_field['field_name'];
          $fieldset_field_name = 'fieldset_'.$other_field['field_name'];

          //need to truncate the names because of database field size restrictions, updating fields here to ensure name consistency
          $group_field_name = substr($group_field_name, 0, 15);
          $fieldset_field_name = substr($fieldset_field_name, 0, 15);

          //build the field group
          _additional_fields_field_group_info($bundle_name, $other_field['label'], $group_field_name, $fieldset_field_name, $other_field['field_name']);
          //update arrays
          array_push($temporary_field, $group_field_name, $fieldset_field_name, $other_field['field_name']);
          $region_right = array_merge($region_right, $temporary_field);
          $fields_with_regions+= [ $group_field_name => 'right', $fieldset_field_name => 'right' ];

        }

    }

    //build the ds layout
    $record = new stdClass;
    $record->id ='TripalEntity|' . $bundle_name . '|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = $bundle_name;
    $record->view_mode = 'default';
    $record->layout = 'tripal_ds_feature';
    $settings = array(
        'regions' => array(
            'top' =>
                $region_top,
            'left' =>
                $region_left,
            'right' =>
                $region_right,
            'bottom' =>
                $region_bottom,
        ),
        'fields' =>
            $fields_with_regions,
        'classes' => array(),
        'wrappers' => array(
            'top' => 'div',
            'left' => 'div',
            'right' => 'div',
            'bottom' => 'div',
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