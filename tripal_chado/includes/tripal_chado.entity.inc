<?php


/**
 * Implements hook_entity_create().
 *
 * This hook is called when brand new entities are created, but
 * they are not loaded so the hook_entity_load() is not yet called.
 */
function tripal_chado_entity_create(&$entity, $type) {
  if ($type == 'TripalEntity') {

    // Set some defaults on vars needed by this module.
    $entity->chado_table = NULL;
    $entity->chado_column = NULL;
    $entity->chado_record = NULL;
    $entity->chado_record_id = NULL;

    // Add in the Chado table information for this entity type.
    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $chado_table = tripal_get_bundle_variable('chado_table', $bundle->id);
    $chado_column = tripal_get_bundle_variable('chado_column', $bundle->id);
    if ($chado_table) {
      $entity->chado_table = $chado_table;
      $entity->chado_column = $chado_column;
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
        $chado_table = tripal_get_bundle_variable('chado_table', $bundle->id);
        $chado_column = tripal_get_bundle_variable('chado_column', $bundle->id);
        if ($chado_table) {
          $entity->chado_table = $chado_table;
          $entity->chado_column = $chado_column;
        }

        $chado_entity = db_select('chado_entity' ,'ce')
          ->fields('ce')
          ->condition('ce.entity_id', $entity->id)
          ->execute()
          ->fetchObject();
        $schema = chado_get_schema($chado_table);
        $record = chado_generate_var($chado_table, array($schema['primary key'][0] => $chado_entity->record_id));
        $entity->chado_record = $record;
        $entity->chado_record_id = $chado_entity->record_id;
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
  $record = db_select('chado_entity', 'ce')
    ->fields('ce', array('chado_entity_id', 'data_table', 'record_id'))
    ->condition('entity_id', $entity->id)
    ->execute()
    ->fetchObject();

  if ($record && property_exists($record, 'chado_entity_id')) {
    // Delete the corresponding record in Chado
    $table = $record->data_table;
    $record_id = $record->record_id;
    chado_delete_record($table, array($table . '_id' => $record_id));

    //Delete the record in the public.chado_entity table
    $sql = "DELETE FROM {chado_entity} WHERE chado_entity_id = :id";
    db_query($sql, array(':id' => $record->chado_entity_id));
  }
}

/**
 * Determines whether the given user has access to a tripal data entity.
 *
 * TODO: I'm not sure this function should be at this level. I think all
 * access controls should be handled by the tripal_entity module and that
 * storage backends should just attach data as requested.
 *
 * @param $op
 *   The operation being performed. One of 'view', 'update', 'create', 'delete'
 *   or just 'edit' (being the same as 'create' or 'update').
 * @param $entity
 *   Optionally a tripal data entity or a tripal data type to check access for.
 *   If nothing is given, access for all types is determined.
 * @param $account
 *   The user to check for. Leave it to NULL to check for the global user.
 * @return boolean
 *   Whether access is allowed or not.
 */
function tripal_chado_entity_access($op, $entity = NULL, $account = NULL) {
  if (user_access('administer tripal data', $account)) {
    return TRUE;
  }
  if (isset($entity) && $type_name = $entity->type) {
    $op = ($op == 'view') ? 'view' : 'edit';
    if (user_access("$op any $type_name data", $account)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Implements hook_tripal_default_title_format().
 */
function tripal_chado_tripal_default_title_format($entity, $available_tokens) {
  $format = array();

  // Load the term associated with this Tripal Content type.
  $term = entity_load('TripalTerm', array('id' => $entity->term_id));
  $term = reset($term);

  // Load the bundle
  $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
  $bundle_id = $bundle->id;
  $table = tripal_get_bundle_variable('chado_table', $bundle_id);
  $column = tripal_get_bundle_variable('chado_column', $bundle_id);
  $cvterm_id = tripal_get_bundle_variable('chado_cvterm_id', $bundle_id);

  // For organism titles  we want the genus and species with no comma separation.
  if ($table == 'organism') {
    $format[] = array(
      'format' => '[organism.genus] [organism.species]',
      'weight' => -5
    );
  }
  if ($table == 'analysis') {
    $format[] = array(
      'format' => '[analysis__name]',
      'weight' => -5
    );
  }
  if ($table == 'feature') {
    $format[] = array(
      'format' => '[feature__name]',
      'weight' => -5
    );
  }
  if ($table == 'stock') {
    $format[] = array(
      'format' => '[stock__name]',
      'weight' => -5
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
    drupal_set_message('Tripal Chado currently only supports views integration for single value fields. The first value has been shown.', 'warning');
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
      $chado_table = tripal_get_bundle_variable('chado_table', $bundle->id);
      $chado_field = tripal_get_bundle_variable('chado_column', $bundle->id);

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