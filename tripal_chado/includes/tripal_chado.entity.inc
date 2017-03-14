<?php


/**
 * Implements hook_entity_create().
 *
 * This hook is called when brand new entities are created, but
 * they are not loaded so the hook_entity_load() is not yet called. We
 * can use this hook to add properties to the entity before saving.
 */
function tripal_chado_entity_create(&$entity, $type) {
  if ($type == 'TripalEntity') {

    // Set some defaults on vars needed by this module.
    if (!property_exists($entity, 'chado_table')) {
      $entity->chado_table =  NULL;
      $entity->chado_column = NULL;

      // Add in the Chado table information for this entity type.
      $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
      if ($bundle->data_table) {
        $entity->chado_table = $bundle->data_table;
        $entity->chado_column = $bundle->type_column;
      }
    }
    if (!property_exists($entity, 'chado_record')) {
      $entity->chado_record = NULL;
      $entity->chado_record_id = NULL;
    }
  }
}
/**
 * Implements hook_entity_presave().
 */
function tripal_chado_entity_presave($entity, $type) {

}

/**
 * Implements hook_entity_postsave().
 */
function tripal_chado_entity_postsave($entity, $type) {

}

/**
 * Implements hook_entity_load().
 */
function tripal_chado_entity_load($entities, $type) {

  if ($type == 'TripalBundle') {
    foreach ($entities as $bundle) {
      // We want to add in the record ID to the entity.
      if (property_exists($bundle, 'id')) {
        $chado_bundle = db_select('chado_bundle', 'cb')
          ->fields('cb')
          ->condition('cb.bundle_id', $bundle->id)
          ->execute()
          ->fetchObject();
        if ($chado_bundle) {
          $bundle->data_table = $chado_bundle->data_table;
          $bundle->type_linker_table = $chado_bundle->type_linker_table;
          $bundle->type_column = $chado_bundle->type_column;
          $bundle->type_id = $chado_bundle->type_id;
        }
      }
    }
  }
  if ($type == 'TripalEntity') {
    foreach ($entities as $entity) {

      // We want to add in the record ID to the entity.
      if (property_exists($entity, 'id')) {
        // Set some defaults on vars needed by this module.
        $entity->chado_table = NULL;
        $entity->chado_column = NULL;
        $entity->chado_record = NULL;
        $entity->chado_record_id = NULL;

        // Add in the Chado table information for this entity type.
        $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
        if (!$bundle) {
          continue;
        }
        // TODO: this may need fixing. The chado_column may not always
        // be the type_id of the base table. Is it expected to be so here?
        $entity->chado_table = $bundle->data_table;
        $entity->chado_column = $bundle->type_column;

        $chado_entity_table = tripal_chado_get_bundle_entity_table($bundle);
        $chado_entity = db_select($chado_entity_table, 'ce')
          ->fields('ce')
          ->condition('ce.entity_id', $entity->id)
          ->execute()
          ->fetchObject();
        if ($chado_entity) {
          $schema = chado_get_schema($entity->chado_table);
          $record = chado_generate_var($entity->chado_table, array($schema['primary key'][0] => $chado_entity->record_id));
          $entity->chado_record = $record;
          $entity->chado_record_id = $chado_entity->record_id;
        }
      }
    }
  }
}

/**
 *
 * Implements hook_entity_insert().
 */
function tripal_chado_entity_insert($entity, $type) {

}

/**
 *
 * Implements hook_entity_update().
 */
function tripal_chado_entity_update($entity, $type) {

}

/**
 *
 * Implements hook_entity_delete().
 */
function tripal_chado_entity_delete($entity, $type) {

}

/**
 * Implements hook_tripal_default_title_format().
 *
 * Overrides the default titles.
 */
function tripal_chado_tripal_default_title_format($bundle, $available_tokens) {
  $format = array();

  $table = $bundle->data_table;

  if ($table == 'organism') {
    $format[] = array(
      'format' => '[taxrank__genus] [taxrank__species]',
      'weight' => -5
    );
  }
  if ($table == 'analysis') {
    $format[] = array(
      'format' => '[schema__name]',
      'weight' => -5
    );
  }
  if ($table == 'feature') {
    $format[] = array(
      'format' => '[schema__name]',
      'weight' => -5
    );
  }
  if ($table == 'featuremap') {
    $format[] = array(
      'format' => '[schema__name]',
      'weight' => -5
    );
  }
  if ($table == 'stock') {
    $format[] = array(
      'format' => '[stock__name]',
      'weight' => -5
    );
  }
  if ($table == 'pub') {
    $format[] = array(
      'format' => '[tpub__title]',
      'weight' => -5,
    );
  }
  return $format;
}

/**
 * Implements hook_entity_property_info_alter().
 *
 * This is being implemented to ensure chado fields are exposed for search api
 * indexing. All fields are available for index by default but the getter
 * function set by default is not actually capable of getting the value from
 * chado. Thus we change the getter function to one that can :-).
 */
function tripal_chado_entity_property_info_alter(&$info) {

  // Get a list of fields with the chado storage backend.

  // Loop through all of the bundles.
  if (isset($info['TripalEntity']['bundles'])) {
    foreach ($info['TripalEntity']['bundles'] as $bundle_id => $bundle) {
      // Loop through each of the fields for a given bundle.
      foreach ($bundle['properties'] as $field_name => $field_info) {
        // If the field is a chado field, then change the callback.
        // @todo check this properly.
        if (preg_match('/(\w+)__(\w+)/', $field_name, $matches)) {
          $info['TripalEntity']['bundles'][$bundle_id]['properties'][$field_name]['getter callback'] =
          'tripal_chado_entity_property_get_value';
        }
      }
    }
  }

}

/**
 * Provides a way for the search api to grab the value of a chado field.
 *
 * @param $entity
 *   The fully-loaded entity object to be indexed.
 * @param $options
 *   Options that can be ued when retrieving the value.
 * @param $field_name
 *   The machine name of the field we want to retrieve.
 * @param $entity_type
 *   The type of entity (ie: TripalEntity).
 *
 * @return
 *   The rendered value of the field specified by $field_name.
 */
function tripal_chado_entity_property_get_value($entity, $options, $field_name, $entity_type) {

  $display = array(
    'type' => '',
    'label' => 'hidden',
  );

  $langcode = LANGUAGE_NONE;
  $items = field_get_items($entity_type, $entity, $field_name);
  if (count($items) == 1) {
    $render_array = field_view_value($entity_type, $entity, $field_name, $items[0], $display, $langcode);
  }
  // @todo: handle fields with multiple values.
  else {
    $render_array = field_view_value($entity_type, $entity, $field_name, $items[0], $display, $langcode);
    drupal_set_message('Tripal Chado currently only supports views integration ' .
      'for single value fields. The first value has been shown.', 'warning');
  }

  return drupal_render($render_array);
}

/**
 * Implements hook_entity_view().
 */
function tripal_chado_entity_view($entity, $type, $view_mode, $langcode) {

  // If this entity is a TripalEntity and is a full view, then
  // we want to support the legacy view, but only if the legacy
  // module is enabled (the functions exist).
  if ($type =='TripalEntity') {
    // Use the generic template to render the fields
    if ($view_mode == 'full') {

      // Get the Chado table for this data type.
      $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
      $chado_table = $bundle->data_table;
      $chado_field = $bundle->type_column;

      // Get the list of templates that should be used for entities and generatte
      // the key in the array for this entity type (using the chado table the
      // entity maps to).
      $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates', array());
      $legacy_template = 'legacy_template--chado_' . $chado_table;

      // If the site admin has indicated that thhis entity type should use
      // a legacy tmplate then prepare the entity and content to fake a
      // node.
      if (key_exists($legacy_template, $enabled_templates) && $enabled_templates[$legacy_template]) {
        // Remove the fields added by the chado_field_storage.
        $fields = field_info_fields();
        foreach($fields as $field) {
          if ($field['storage']['type'] == 'field_chado_storage' or
              $field['storage']['type'] == 'tripal_no_storage') {
                $field_name = $field['field_name'];
                if (property_exists($entity, $field_name)) {
                  $entity->$field_name = NULL;
                  unset($entity->content[$field_name]);
                }
              }
        }

        // Make the entity look like a node.
        $entity->type = 'chado_' . $chado_table;
        $entity->$chado_table = $entity->chado_record;

        // Add any node specific fields to the entity to fake the node.
        $node_schema = drupal_get_schema('node');
        foreach ($node_schema['fields'] as $field_name => $details) {
          if (!property_exists($entity, $field_name)) {
            $entity->$field_name = '';
            if (array_key_exists('default', $details)) {
              $entity->$field_name = $details['default'];
            }
          }
        }

        // Now call the module's node_view hook to add additional
        // content to our 'fake' entity node.
        $modules = module_list();
        foreach ($modules as $mname => $details) {
          $function_name = $mname . '_node_view';
          if (function_exists($function_name)) {
            $function_name($entity, $view_mode, $langcode);
          }
        }
      }
    }
  }
}

/**
 * Implements hook_entity_view_alter().
 *
 * This function is used to support legacy Tripal v2 templates
 * for use with Tripal v3 entities.
 */
function tripal_chado_entity_view_alter(&$build) {
  // For the legacy support, we need to make sure the TOC
  // is built.
  if ($build['#entity_type'] == 'TripalEntity') {
    $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates', array());
    $entity = $build['#entity'];
    $legacy_template = 'legacy_template--' . $entity->type;
    if (key_exists($legacy_template, $enabled_templates) && $enabled_templates[$legacy_template]) {
      $build['#entity']->nid = NULL;
      $build['#node'] = $build['#entity'];
      $modules = module_list();
      foreach ($modules as $mname => $details) {
        $function_name = $mname . '_node_view_alter';
        if (function_exists($function_name)) {
          $function_name($build);
        }
      }
    }
  }
}
