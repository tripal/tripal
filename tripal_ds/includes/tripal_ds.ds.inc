<?php


/**
 * Sorts a multidimensional array into alphabetical order.
 *
 * @param $key .
 *
 * @return \Closure
 */
function tripal_ds_sort_array($key) {
  return function ($a, $b) use ($key) {
    return strnatcmp($a[$key], $b[$key]);
  };
}

/**
 * Sorts a multidimensional array of objects into alphabetical order.
 *
 * @param $key .
 *
 * @return \Closure
 */
function tripal_ds_sort_object($key) {
  return function ($a, $b) use ($key) {
    return strnatcmp($a->$key, $b->$key);
  };
}

/**
 * Builds the tripal_ds layout for all content types other than Publications.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $instances
 *
 * @return bool
 */
function _ds_layout_settings_info($bundle_name, $instances) {
  $region_right = [];
  $region_left = [];
  $prop_fields = [];
  $summary_fields = [];
  $unhandled_fields = [];
  $fields_with_regions = [];
  $i = 0;


  try {

    // Get the bundle and term objects.
    $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
    $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);

    // Get the list of fields and sort them by label.
    $sorted_instances = [];
    foreach ($instances as $key => $instance) {
      $sorted_instances[$i] = $instance;
      $i++;
    }
    usort($sorted_instances, tripal_ds_sort_array('label'));

    // Iterate through the fields of this bundle and put them into appropriate
    // field groups.
    foreach ($sorted_instances as $key => $instance) {
      $base_table = array_key_exists('base_table', $instance['settings']) ? $instance['settings']['base_table'] : '';
      $chado_table = array_key_exists('chado_table', $instance['settings']) ? $instance['settings']['chado_table'] : '';
      $instance_name = $instance['field_name'];

      // Add the bundle type to the summary fields.
      if ($instance_name == "rdfs__type") {
        array_push($summary_fields, $instance_name);
        continue;
      }

      // The tripal_chado module adds an image to the organism content
      // type so we want to make sure that image goes in the summary.
      // It is not a TripalField so it won't have a chado table.
      if ($instance_name == 'data__image' and $term->name == 'organism') {
        array_push($summary_fields, $instance_name);
        continue;
      }

      // For fields that have a data__sequence prefix we want to hide these
      // unless it's the data__sequence_record field which displays all of the
      // sequence data and should be in it's own fieldset.
      if (preg_match('/data__sequence/', $instance_name)) {
        if ($instance_name == 'data__sequence_record') {
          $instance['display']['default']['label'] = 'hidden';
          field_update_instance($instance);
          array_push($unhandled_fields, $instance);
        }
        continue;
      }

      // For the same reason, hide the CDS and protein fields
      if ($instance_name == 'so__cds' or $instance_name == 'data__protein_sequence'){
        continue;
      }

      // If this field holds a value from the prop table then add it to the
      // properties field group.
      if (preg_match('/_prop$/', $chado_table)) {
        array_push($prop_fields, $instance_name);
        continue;
      }

      // Put all fields from the base table in the summary and all other
      // fields that were not handled above into a different array for
      // handling later.
      if ($base_table == $chado_table) {
        array_push($summary_fields, $instance_name);
        continue;
      }
      else {
        // Update the display settings so that the title is hidden.
        $instance['display']['default']['label'] = 'hidden';
        field_update_instance($instance);
        array_push($unhandled_fields, $instance);
      }
    } // end looping over fields.

    // Create the summary and properties field groups.
    if (!empty($summary_fields)) {
      tripal_ds_add_summary_field_group($bundle_name, $summary_fields);
    }
    if (!empty($prop_fields)) {
      tripal_ds_add_prop_field_group($bundle_name, $prop_fields);
    }

    // All other fields get their own field group (i.e. Tripal Pane).
    foreach ($unhandled_fields as $key => $other_field) {
      $group_field_name = 'gp_' . $other_field['field_name'];

      // Need to truncate the names because of database field size restrictions,
      // updating fields here to ensure name consistency.
      $group_field_name = substr($group_field_name, 0, 27);

      // Add random numbers to ensure the field name is unique within the 32
      // character limit of the field.
      $group_field_name = $group_field_name . rand(0, 99999);

      // Now add a generic field group for this field.
      tripal_ds_add_generic_field_group($bundle_name, $other_field['label'], $group_field_name, $other_field['field_name']);
    }

    // Build one large multidimensional array of all field groups to sort
    // in alpha order to display fields in label alpha order.
    $right_fields = [];
    $all_field_groups = field_group_info_groups('TripalEntity', $bundle_name, NULL, TRUE);

    if (!empty($all_field_groups)) {
      if (is_array($all_field_groups)) {
        if (!isset($all_field_groups['default'])) {
          $all_field_groups['default'] = [];
        }
        foreach ($all_field_groups['default'] as $key => $field_name) {
          $right_fields[$key] = $field_name;
        }
        usort($right_fields, tripal_ds_sort_object('label'));
      }
    }

    // Now build the $region_right array and the fields array.
    $i = 0;
    if (empty($right_fields)) {
      foreach ($all_field_groups as $index => $field) {
        $region_right[$i] = $field['field_name'];
        $i++;
        tripal_ds_field_group_update_weight($field['field_name'], $bundle_name, $i);
      }
    }
    elseif (!empty($right_fields)) {
      foreach ($right_fields as $index => $field) {
        // Check if the child is already present which is a problem when groups
        // are nested within groups.
        if (in_array($field->group_name, $region_right)) {
          // Still need to check for children and add them.
          if (!empty($field->children)) {
            foreach ($field->children as $index => $child) {
              $region_right[$i] = $child;
              $i++;
            }
          }
        }
        else {
          $region_right[$i] = $field->group_name;
          if (!empty($field->children)) {
            foreach ($field->children as $index => $child) {
              $i++;
              $region_right[$i] = $child;
            }
          }
          $i++;
        }
        // Now update the weights of the field_groups.
        tripal_ds_field_group_update_weight($field->group_name, $bundle_name, $i);
      }
    }
    foreach ($region_right as $index => $field) {
      $fields_with_regions[$field] = 'right';
    }
    // Add blocks to $region_left and build the toc field that is placed within.
    _ds_fields_info_write($bundle_name);
    $region_left = ['toc'];
    $fields_with_regions += ['toc' => 'left'];

    // Build the ds layout.
    $record = new stdClass;
    $record->id = 'TripalEntity|' . $bundle_name . '|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = $bundle_name;
    $record->view_mode = 'default';
    $record->layout = 'tripal_ds_feature';
    $settings = [
      'regions' => [
        'left' =>
          $region_left,
        'right' =>
          $region_right,
      ],
      'fields' =>
        $fields_with_regions,
      'classes' => [],
      'wrappers' => [
        'left' => 'div',
        'right' => 'div',
      ],
      'layout_wrapper' => 'div',
      'layout_attributes' => '',
      'layout_attributes_merge' => 1,
      'layout_link_attribute' => '',
      'layout_link_custom' => '',
      'layout_disable_css' => 0,
    ];
    $record->settings = $settings;

    drupal_write_record('ds_layout_settings', $record);
    // Clear the Drupal cache.
    drupal_flush_all_caches();
  } catch (Exception $e) {
    watchdog_exception('tripal_ds', $e);
    return FALSE;
  }
  return TRUE;
}


/**
 * Builds the tripal_ds layout for Publications.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $instances
 *
 * @return bool
 */
function _ds_layout_pub_settings_info($bundle_name, $instances) {
  $region_right = [];
  $region_left = [];
  $properties = [];
  $all_fields = [];
  $instances_for_field_groups = [];
  $disabled_instances = [];
  try {
    // Add Abstract, Citation, DB Cross Reference, Properties.
    $all_fields['tpub__abstract'] = 'right';
    $all_fields['tpub__citation'] = 'right';
    $all_fields['sbo__database_cross_reference'] = 'right';
    $all_fields['schema__additional_type'] = 'right';
    $all_fields['tpub__doi'] = 'right';
    $all_fields['tpub__publication_date'] = 'right';
    $all_fields['sio__references'] = 'right';

    // Iterate through the fields of this bundle.
    foreach ($instances as $key => $instance) {
      $instance_name = $instance['field_name'];

      if ($instance_name == 'sbo__database_cross_reference'
        || $instance_name == 'sio__references') {
        array_push($instances_for_field_groups, $instance);
        // Update the display settings so that the title is hidden.
        $instance['display']['default']['label'] = 'hidden';
        field_update_instance($instance);
      }
      elseif ($instance_name == 'schema__additional_type' || $instance_name == 'tpub__doi'
        || $instance_name == 'tpub__publication_date' || $instance_name == 'tpub__abstract' ||
        $instance_name == 'tpub__citation') {
        array_push($properties, $instance_name);
      }
      else {
        array_push($disabled_instances, $instance_name);
      }

    }
    //Publication fields that are not going in the properties table.
    foreach ($instances_for_field_groups as $key => $other_field) {
      // Temporary field names.
      $temporary_field = [];
      $group_field_name = 'gp_' . $other_field['field_name'];

      // Need to truncate the names because of database field size restrictions,
      // updating fields here to ensure name consistency.
      $group_field_name = substr($group_field_name, 0, 27);

      // Add random numbers to ensure the field name is unique within the 32
      // character limit of the field.
      $group_field_name = $group_field_name . rand(0, 99999);

      // Build the field group.
      tripal_ds_add_generic_field_group($bundle_name, $other_field['label'], $group_field_name, $other_field['field_name']);

      // Update arrays.
      array_push($temporary_field, $group_field_name, $other_field['field_name']);
      $region_right = array_merge($region_right, $temporary_field);
      $all_fields += [$group_field_name => 'right',];
    }

    //Properties table fields.
    if (!empty($properties)) {
      _publication_prop_field_group_info($bundle_name, $properties);
      array_unshift($properties, 'group_summary_tripalpane', 'group_summary_table');
      $region_right = array_merge($region_right, $properties);
      $all_fields += [
        'group_summary_tripalpane' => 'right',
        'group_summary_table' => 'right',
      ];
    }
    if (!empty($all_fields)) {
      foreach ($disabled_instances as $disabled_field) {
        $all_fields += [$disabled_field => 'disabled'];
      }
    }

    // Add blocks to $region_left and build the toc field that is placed within.
    _ds_fields_info_write($bundle_name);
    $region_left += ['toc'];
    $all_fields += ['toc' => 'left'];
    // Build the ds layout.
    $record = new stdClass;
    $record->id = 'TripalEntity|' . $bundle_name . '|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = $bundle_name;
    $record->view_mode = 'default';
    $record->layout = 'tripal_ds_feature';
    $settings = [
      'regions' => [
        'left' =>
          $region_left,
        'right' =>
          $region_right,
      ],
      'fields' =>
        $all_fields,
      'classes' => [],
      'wrappers' => [
        'left' => 'div',
        'right' => 'div',
      ],
      'layout_wrapper' => 'div',
      'layout_attributes' => '',
      'layout_attributes_merge' => 1,
      'layout_link_attribute' => '',
      'layout_link_custom' => '',
      'layout_disable_css' => 0,
    ];
    $record->settings = $settings;
    drupal_write_record('ds_layout_settings', $record);
    // Clear the Drpual chace
    drupal_flush_all_caches();
  } catch (Exception $e) {
    watchdog_exception('tripal_ds', $e);
    return FALSE;
  }
  return TRUE;
}

/**
 * Implements hook_ds_fields_info().
 * Creates the Table of Contents field.
 *
 * $param $entity_type
 */
function tripal_ds_ds_fields_info($entity_type) {
  $fields = [];

  $fields['toc'] = [
    'title' => t('Table of Contents'),
    'field_type' => DS_FIELD_TYPE_FUNCTION,
    'function' => 'tripal_ds_toc_block',
  ];
  return ['TripalEntity' => $fields];
}

/**
 * Adds the content the to Table of Contents block.
 *
 * @param $entity_type
 *
 * @return Object
 */
function tripal_ds_toc_block($entity_type) {
  $bundle_name = $entity_type['bundle'];
  $toc = views_embed_view('tripal_content_type_toc', 'block', $bundle_name);
  return $toc;
}

/**
 * Creates the field_group for the Table of Contents.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 */
function _ds_fields_info_write($bundle_name) {
  $fields = new stdClass;
  $fields->id = 'TripalEntity|' . $bundle_name . '|default';
  $fields->entity_type = 'TripalEntity';
  $fields->bundle = $bundle_name;
  $fields->view_mode = 'default';
  $fields->settings = [
    'toc' => [
      'weight' => 0,
      'label' => 'hidden',
      'format' => 'default',
    ],
  ];
  drupal_write_record('ds_field_settings', $fields);
}
