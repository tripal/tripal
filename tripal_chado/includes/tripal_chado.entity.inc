<?php

/**
 * Implements hook_entity_create().
 *
 * This hook is called when brand new entities are created, but
 * they are not loaded so the hook_entity_load() is not yet called. We
 * can use this hook to add properties to the entity before saving.
 *
 * @param $entity
 *   The entity being created.
 * @param $type
 *   The type of entity being created.
 * @param $bundle (OPTIONAL)
 *   The bundle object for the current entity.
 */
function tripal_chado_entity_create(&$entity, $type, $bundle = NULL) {
  if ($type == 'TripalEntity') {

    // Set some defaults on vars needed by this module.
    if (!property_exists($entity, 'chado_table')) {
      $entity->chado_table = NULL;
      $entity->chado_column = NULL;
      $entity->chado_linker = NULL;
      $entity->chado_type_id = NULL;
      $entity->chado_type_value = NULL;
      $entity->chado_base_type_id = NULL;

      // Add in the Chado table information for this entity type.
      if (!$bundle) {
        $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
      }
      if ($bundle->data_table) {
        $entity->chado_table = $bundle->data_table;
        $entity->chado_column = $bundle->type_column;
        $entity->chado_linker = $bundle->type_linker_table;
        $entity->chado_type_id = $bundle->type_id;
        $entity->chado_type_value = $bundle->type_value;
        $entity->chado_base_type_id = $bundle->base_type_id;
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
          $bundle->type_value = $chado_bundle->type_value;
          $bundle->base_type_id = $chado_bundle->base_type_id;
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
        $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
        if (!$bundle) {
          continue;
        }
        $entity->chado_table = $bundle->data_table;
        $entity->chado_column = $bundle->type_column;

        $chado_entity_table = chado_get_bundle_entity_table($bundle);
        $chado_entity = db_select($chado_entity_table, 'ce')
          ->fields('ce')
          ->condition('ce.entity_id', $entity->id)
          ->execute()
          ->fetchObject();
        if ($chado_entity) {
          $schema = chado_get_schema($entity->chado_table);
          $record = chado_generate_var($entity->chado_table,
            [$schema['primary key'][0] => $chado_entity->record_id]);
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
  if ($type !== 'TripalEntity') {
    return;
  }

  // Remove the base record for this entity from the Chado tables.
  $bundle = $entity->bundle;
  if (db_table_exists("chado_{$bundle}")) {
    $chado_table = $entity->chado_table;
    $table_def = chado_get_schema($chado_table);
    $pkey_field = $table_def['primary key'][0];
    $match = [
        $pkey_field => $entity->chado_record_id,
    ];

    // Here we need to set the active database to Chado
    // because records will cascade delete. If one of the tables that
    // gets items removed is the featureloc then there will be
    // errors about undefined PLSQL functions.
    $previous_db = chado_set_active('chado');
    try {
      chado_delete_record($entity->chado_table, $match);
      chado_set_active($previous_db);
    } catch (Exception $e) {
      chado_set_active($previous_db);
      throw $e;
    }
  }
}

/**
 *
 * Implements hook_entity_unpublish().
 *
 * This is not a Drupal hook, but one added by Tripal.
 */
function tripal_chado_entity_unpublish($entity, $type) {
  if ($type !== 'TripalEntity') {
    return;
  }

  // Remove the entity from the corresponding chado_bio_data_x table
  $bundle = $entity->bundle;
  if (db_table_exists("chado_{$bundle}")) {
    db_delete("chado_{$bundle}")->condition('entity_id', $entity->id)->execute();
  }
}

/**
 * Implements hook_tripal_default_title_format().
 *
 * Overrides the default titles.
 */
function tripal_chado_tripal_default_title_format($bundle, $available_tokens) {
  $format = [];

  $table = $bundle->data_table;

  if ($table == 'organism') {
    if (chado_get_version() <= '1.2') {
      $format[] = [
        'format' => '[taxrank__genus] [taxrank__species]',
        'weight' => -5,
      ];
    }
    else {
      $format[] = [
        'format' => '[taxrank__genus] [taxrank__species] [taxrank__infraspecific_taxon,TAXRANK:0000045]',
        'weight' => -5,
      ];
    }
  }
  if ($table == 'arraydesign') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'assay') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'biomaterial') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'analysis') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'feature') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'featuremap') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'stock') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'pub') {
    $format[] = [
      'format' => '[tpub__title]',
      'weight' => -5,
    ];
  }
  if ($table == 'cvterm') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'project') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'contact') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'phylotree') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'library') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  if ($table == 'protocol') {
    $format[] = [
      'format' => '[schema__name]',
      'weight' => -5,
    ];
  }
  return $format;
}

/**
 * Implements hook_entity_view().
 */
function tripal_chado_entity_view($entity, $type, $view_mode, $langcode) {

  // If this entity is a TripalEntity and is a full view, then
  // we want to support the legacy view, but only if the legacy
  // module is enabled (the functions exist).
  if ($type == 'TripalEntity') {
    // Use the generic template to render the fields
    if ($view_mode == 'full') {

      // Get the Chado table for this data type.
      $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
      $chado_table = $bundle->data_table;
      $chado_field = $bundle->type_column;

      // Get the list of templates that should be used for entities and generatte
      // the key in the array for this entity type (using the chado table the
      // entity maps to).
      $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates',
        []);
      $legacy_template = 'legacy_template--chado_' . $chado_table;

      // If the site admin has indicated that this entity type should use
      // a legacy tmplate then prepare the entity and content to fake a
      // node.
      if (key_exists($legacy_template,
          $enabled_templates) && $enabled_templates[$legacy_template]) {
        // Remove the fields added by the chado_field_storage.
        $fields = field_info_fields();
        foreach ($fields as $field) {
          if ($field['storage']['type'] == 'field_chado_storage' or $field['storage']['type'] == 'tripal_no_storage') {
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
    $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates',
      []);
    $entity = $build['#entity'];
    $legacy_template = 'legacy_template--' . $entity->type;
    if (key_exists($legacy_template,
        $enabled_templates) && $enabled_templates[$legacy_template]) {
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

