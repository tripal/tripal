<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * TripalEntity content types (bundles) and their entities.
 *
 */

/**
 * @defgroup tripal_entities_api Tripal Entities
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with
 * TripalEntity content types (bundles) and their entities.
 *
 * Bundles (Content Types): Bundles are types of content in a Drupal site.
 * By default, Drupal provides the Basic Page and Article content types,
 * and Drupal allows a site developer to create new content types on-the-fly
 * using the administrative interface--no programming required.  Tripal also
 * provides several Content Type by default. During installation of Tripal the
 * Organism, Gene, Project, Analysis and other content types are created
 * automatically.  The site developer can then create new content types for
 * different biological data--again, without any programming required.
 *
 * In order to to assist with data exchange and use of common data formats,
 * Tripal Bundles are defined using a controlled vocabulary term (cvterm).
 * For example, a "Gene" Bundle is defined using the Sequence Ontology term for
 * gene whose term accession is: SO:0000704. This mapping allows Tripal to
 * compare content across Tripal sites, and expose data to computational tools
 * that understand these vocabularies. By default, Tripal uses Chado as its
 * primary data storage back-end.
 *
 * Entity: An entity is a discrete data record.  Entities are most commonly
 * seen as "pages" on a Drupal web site and are instances of a Bundle
 * (i.e content type). When data is published on a Tripal site such as
 * organisms, genes, germplasm, maps, etc., each record is represented by a
 * single entity with an entity ID as its only attribute. All other
 * information that the entity provides is made available via Fields.
 *
 * For more information please see:
 * http://tripal.info/tutorials/v3.x/developers-handbook/structure
 * @}
 *
 */

/**
 * @section
 * Hooks.
 */

/**
 * Allows a module to make changes to an entity object after creation.
 *
 * This function is added by Tripal to allow datastore backends to add
 * addition properties to the entity that they themselves will use later.
 *
 * @param $entity
 * @param $entity_type
 *
 * @ingroup tripal_entities_api
 */
function hook_entity_create(&$entity, $entity_type) {

}

/**
 * Allows a module to perform tasks after a TripalBundle object is created.
 *
 * @param $bundle
 *   The newly created TripalBundle object.
 * @param $storage_args
 *   Arguments passed to the storage backend for this bundle.  These arguments
 *   typically provide details for how to associate this bundle with records
 *   in the storage system.  Each storage system will have its own set of
 *   arguments that it will expect.
 *
 * @ingroup tripal_entities_api
 */
function hook_bundle_create(&$bundle, $storage_args) {

}

/**
 * Allows a module to perform tasks after fields are added to a TripalBundle.
 *
 * @param $bundle
 *   The newly created TripalBundle object.
 *
 * @ingroup tripal_entities_api
 */
function hook_bundle_postcreate(&$bundle) {

}

/**
 * Allows a module to add admin notifications to the associated tripal table
 * during the cron run.
 *
 * @ingroup tripal_entities_api
 */
function hook_tripal_cron_notification() {

}

/**
 * Allows a module to perform tasks before a TripalBundle object is deleted.
 *
 * @param $bundle
 *   The newly created TripalBundle object.
 *
 * @ingroup tripal_entities_api
 */
function hook_bundle_delete($bundle) {

}

/**
 * Implement this hook to define default formats for Tripal Content Types.
 *
 * @param TripalBundle $bundle
 *   A tripal content type entity with information to be used for determining
 *   the default title format.
 * @param array $available_tokens
 *   An array of available tokens for this particular tripal content type.
 *
 * @return array
 *   An array of potential formats. The lightest weighted format suggested by
 *   all modules will be chosen.
 *   Each array item should consist of a 'weight' and 'format'. See the hook
 *   implementation below for examples.
 *    - weight: an integer used to determine priority of suggestions.
 *        The smaller/lighter the number the higher the priority.
 *        Best practice is to use a weight less than 0 for extension modules.
 *        specifically, -2 is a good weight for calculated formats and -5 is a
 *        good weight for hard-coded formats specific to a given type.
 *    - format: a string including approved tokens used to determine the title
 *        on Tripal content pages.
 *
 * @ingroup tripal_entities_api
 */
function hook_tripal_default_title_format($bundle, $available_tokens) {
  $format = [];

  // If you want to suggest a default format for a particular vocabulary term:
  //---------------------------------------------------------------------------
  // Load the term associated with this Tripal Content type.
  $term = entity_load('TripalTerm', ['id' => $bundle->term_id]);
  $term = reset($term);

  // If it's the term you are interested in then suggest a format.
  if ($term->name == 'organism') {
    // To suggest a format, add an element to the array with a format & weight
    // key.
    $format[] = [
      // This is the format/pattern you suggest be used to determine the title
      // of organism pages.
      'format' => '[organism__genus] [organism__species]',
      // The weight/priority of your suggestion.
      'weight' => -5,
    ];
  }

  // Say you know that in your particular site, all 'names' are required
  // and you want to only use the human-readable name:
  //---------------------------------------------------------------------------
  $name_field = preg_grep('/__name]$/', array_keys($available_tokens));
  $name_field = reset($name_field);
  if (is_string($name_field)) {
    $format[] = [
      'format' => $name_field,
      'weight' => -2,
    ];
  }

  return $format;
}

/**
 * @section
 * Entity.
 */

/**
 * A replacement for the entity_load function of Drupal.
 *
 * This function should be used for loading of Tripal Entities. It provides
 * greater control to limit which fields are loaded with the entity. The
 * entity_load() function of Drupal will automatically attach all fields at
 * once but this may not be desired as some fields can be complex and large, and
 * the site developer may desire loading of fields via AJAX or the user of
 * web services may wish to specify the fields they want to include.
 *
 * @param $entity_type :
 *   The entity type to load, e.g. node or user.
 * @param $ids
 *   An array of entity IDs, or FALSE to load all entities.
 * @param $reset : Whether to reset the internal cache for the requested entity
 *   type. Defaults to FALSE.
 * @param $field_ids
 *   A list of numeric field IDs that should be loaded.  The
 *   TripalField named 'content_type' is always automatically added.
 * @param $cache
 *  When loading of entities they can be cached with Drupal for later
 *  faster loading. However, this can cause memory issues when running
 *  Tripal jobs that load lots of entities.  Caching of entities can
 *  be disabled to improve memory performance by setting this to FALSE.
 *
 * @return
 *   An array of entity objects indexed by their ids. When no results are
 *   found, an empty array is returned.
 *
 * @ingroup tripal_entities_api
 */
function tripal_load_entity($entity_type, $ids = FALSE, $reset = FALSE,
                            $field_ids = [], $cache = TRUE) {

  // The $conditions is deprecated in the funtion arguments of entity_load
  // so it was removed from the parameters of this function as well. But
  // the load() function of the entity controller still expects it so set it
  // to an empty array.
  $conditions = [];

  // If this isn't a TripalEntity then just load it the old fashioned way
  // although caching will not be used if it not specifically set to FALSE.
  if ($entity_type != 'TripalEntity') {
    return entity_load($entity_type, $ids, $conditions, $reset);
  }

  // Get the entity controller and clear the cache if requested (default).
  $ec = entity_get_controller($entity_type);
  if ($reset) {
    $ec->resetCache();
  }

  return $ec->load($ids, $conditions, $field_ids, $cache);
}

/**
 * Retrieves a TripalTerm entity that matches the given arguments.
 *
 * @param $values
 *   An associative array used to match a term.
 *   Valid keys may be:
 *        - vocabulary: Must always be used with accession to uniquely
 *                        identify a term.
 *        - accession: Must always be used with vocabulary to uniquely
 *                       identify a term.
 *        - term_id: Can be used alone to uniquely identify a term.
 *
 * @return
 *   A TripalTerm entity object or NULL if not found.
 *
 * @ingroup tripal_entities_api
 */
function tripal_load_term_entity($values) {
  $vocabulary = array_key_exists('vocabulary', $values) ? $values['vocabulary'] : '';
  $accession = array_key_exists('accession', $values) ? $values['accession'] : '';
  $term_id = array_key_exists('term_id', $values) ? $values['term_id'] : '';

  $term = NULL;

  if ($vocabulary and $accession) {
    $query = db_select('tripal_term', 'tt');
    $query->join('tripal_vocab', 'tv', 'tv.id = tt.vocab_id');
    $query->fields('tt', ['id'])
      ->fields('tv', ['vocabulary'])
      ->condition('tv.vocabulary', $vocabulary)
      ->condition('tt.accession', $accession);
    $term = $query->execute()->fetchObject();
  }
  else {
    if ($term_id) {
      $query = db_select('tripal_term', 'tt');
      $query->fields('tt', ['id'])
        ->condition('tt.id', $term_id);
      $term = $query->execute()->fetchObject();
    }
  }

  if ($term) {
    $entity = entity_load('TripalTerm', [$term->id]);
    return reset($entity);
  }
  return NULL;
}

/**
 * Retrieves a TripalVocab entity that maches the given arguments.
 *
 * @param $values
 *   An associative array used to match a vocabulary.
 *   The valid keys are:
 *      - vocab_id: integer id of the vocabulary.
 *      - vocabulary: string name of vocabulary.
 *
 * @return
 * A TripalVocab entity object or NULL if not found.
 *
 * @ingroup tripal_entities_api
 */
function tripal_load_vocab_entity($values) {
  $vocabulary = array_key_exists('vocabulary', $values) ? $values['vocabulary'] : '';
  $vocab_id = array_key_exists('vocab_id', $values) ? $values['vocab_id'] : '';
  $vocab = NULL;

  $query = db_select('tripal_vocab', 'tv')
    ->fields('tv');

  if ($vocabulary) {
    $query->condition('tv.vocabulary', $vocabulary);
  }
  if ($vocab_id) {
    $query->condition('tv.id', $vocab_id);
  }
  $vocab = $query->execute()->fetchObject();

  if ($vocab) {
    $entity = entity_load('TripalVocab', [$vocab->id]);
    return reset($entity);
  }
  return NULL;
}

/**
 * Retrieves a TripalBundle entity that matches the given arguments.
 *
 * @param $values
 *   An associative array used to match a bundle.  Valid keys may:
 *     - id: the numeric id of the bundle.
 *     - name:  the bundle name (e.g. 'bio_data_234')
 *     - label: the bundle label (e.g. 'Organism')
 *     - term_id: the term ID to which the bundle belongs
 *     - accession: the full acccession for the bundle (e.g. OBI:0100026)
 *
 * @return
 *   A TripalBundle entity object or NULL if not found.
 *
 * @ingroup tripal_entities_api
 */
function tripal_load_bundle_entity($values) {

  $query = db_select('tripal_bundle', 'tb');
  $query->fields('tb');
  if (array_key_exists('id', $values)) {
    $query->condition('tb.id', $values['id']);
  }
  if (array_key_exists('name', $values)) {
    $query->condition('tb.name', $values['name']);
  }
  if (array_key_exists('label', $values)) {
    $query->condition('tb.label', $values['label']);
  }
  if (array_key_exists('term_id', $values)) {
    $query->condition('tb.term_id', $values['term_id']);
  }
  if (array_key_exists('accession', $values)) {
    list($vocab, $accession) = explode(':', $values['accession'], 2);
    $term = tripal_load_term_entity([
      'vocabulary' => $vocab,
      'accession' => $accession,
    ]);
    if (!$term) {
      return NULL;
    }
    $query->condition('tb.term_id', $term->id);

  }
  $bundle = $query->execute()->fetchObject();

  if ($bundle) {
    $entity = entity_load_unchanged('TripalBundle', $bundle->id);
    return $entity;
  }
  return NULL;
}

/**
 * Allows a module to write to the admin notification table.
 *
 * @param $title
 *   A generic phrase indicating what the notification is for.
 * @param $details
 *   A human-readable sentence or two describing the issue.
 * @param $type
 *   A one word type indicating the type of notification. Tripal types include:
 *   Jobs, Fields.
 *   If no type is required please pass NULL.
 * @param $actions
 *   A serialized PHP associative array containing the link and URL for each
 *   action.
 *   If not type is required please pass NULL.
 * @param $submitter_id
 *   A unique ID provided by the submitter for checking to make sure that the
 *   notification is not added more than once.
 *
 * @ingroup tripal_entities_api
 */
function tripal_add_notification($title, $details, $type, $actions, $submitter_id) {
  $transaction = db_transaction();
  try {
    // Check the notification isn't already in the admin notification table.
    $dedup = db_select('tripal_admin_notfications', 'tan')
      ->fields('tan')
      ->condition('submitter_id', $submitter_id, '=')
      ->execute()->fetchAll();

    if (empty($dedup)) {
      $record = new stdClass;
      $record->details = $details;
      $record->title = $title;
      $record->submitter_id = $submitter_id;
      $record->actions = serialize($actions);
      $record->enabled = 1;
      $record->type = $type;
      $success = drupal_write_record('tripal_admin_notfications', $record);
    }
  } catch (Exception $e) {
    $transaction->rollback();
    watchdog('tripal_cron', 'Could not write notification to database.');
  }
}

/**
 * Creates a new Tripal Entity type (i.e. bundle).
 *
 * @param $args
 *   An array of arguments that must include the following keys:
 *     - vocabulary:  The abbreviated vocabulary for the vocabulary
 *       (e.g. RO, SO, PATO).
 *     - accession:  The unique term ID in the vocabulary $vocabulary
 *       (i.e. an accession).
 *     - term_name: A human-readable name for this term.  This will became
 *       the name that appears for the content type.  In practice, this
 *       should be the name of the term. (E.g. the name for SO:0000704 is gene).
 *     - label: An alternative bundle label.  This will be used instead of the
 *       term name, for cases where the vocabulary term is not human readable.
 *
 * @param $job
 *  The job ID if this is launched via a job.
 *
 * @return
 *   The bundle object or FALSE if failure.
 *
 * @ingroup tripal_entities_api
 */
function tripal_create_bundle($args, $job = NULL) {
  $vocabulary = $args['vocabulary'];
  $accession = $args['accession'];
  $term_name = $args['term_name'];
  $storage_args = $args['storage_args'];
  $label = $args['label'];

  $message_args = [
    'job' => $job,
    'print' => TRUE,
    'watchdog' => TRUE,
  ];

  //   tripal_report_error('tripal_entities', TRIPAL_INFO,
  //    "Creation of a content type is performed using a database transaction. " .
  //     "If it fails or is terminated prematurely then all insertions and " .
  //     "updates are rolled back and will not be found in the database",
  //     [], $message_args);

  $transaction = db_transaction();
  try {
    // First create the TripalVocab if it doesn't already exist.
    $vocab = tripal_load_vocab_entity(['vocabulary' => $vocabulary]);
    if (!$vocab) {
      $vocab = entity_get_controller('TripalVocab')->create(['vocabulary' => $vocabulary]);
      if ($vocab) {
        $vocab->save();
      }
      else {
        $transaction->rollback();
        tripal_report_error('tripal_entities', TRIPAL_ERROR,
          'Unable to create TripalVocab :vocab', [':vocab' => $vocabulary], $message_args);
        return FALSE;
      }
    }

    // Next create the TripalTerm if it doesn't already exist.
    $term = tripal_load_term_entity([
      'vocabulary' => $vocabulary,
      'accession' => $accession,
    ]);
    if (!$term) {
      $targs = [
        'vocab_id' => $vocab->id,
        'accession' => $accession,
        'name' => $term_name,
      ];
      $term = entity_get_controller('TripalTerm')->create($targs);
      if ($term) {
        $term = $term->save();
      }
      else {
        $transaction->rollback();
        tripal_report_error('tripal_entities', TRIPAL_ERROR,
          'Unable to create TripalTerm :term', [':term' => $term_name], $message_args);
        return FALSE;
      }
    }

    // If the bundle doesn't already exist, then add it.
    $bundle_name = 'bio_data_' . $term->id;
    $einfo = entity_get_info('TripalEntity');
    if (!in_array($bundle_name, array_keys($einfo['bundles']))) {
      // Make the label for the content type have capitalized words.  The
      // exception is 'mRNA' which we know should not be uppercased.
      if (!$label) {
        $label = ucwords(preg_replace('/_/', ' ', $term_name));
        if ($term_name == 'mRNA') {
          $label = $term_name;
        }
      }
      // Insert the bundle.
      db_insert('tripal_bundle')
        ->fields([
          'label' => $label,
          'type' => 'TripalEntity',
          'name' => $bundle_name,
          'term_id' => $term->id,
        ])
        ->execute();
    }

    $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
    if (!$bundle) {
      $transaction->rollback();
      tripal_report_error('tripal_entities', TRIPAL_ERROR,
        'Unable to create Tripal Bundle :name.', [':name' => $bundle_name], $message_args);
      return FALSE;
    }

    $modules = module_implements('bundle_create');
    foreach ($modules as $module) {
      $function = $module . '_bundle_create';
      if (function_exists($function)) {
        $function($bundle, $storage_args);
      }
    }

    // Clear the entity cache so that Drupal will read our
    // hook_entity_info() implementation.
    global $language;
    $langcode = $language->language;
    cache_clear_all("entity_info:$langcode", 'cache');
    variable_set('menu_rebuild_needed', TRUE);

    // Get the bundle object.
    $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
    if (!$bundle) {
      $transaction->rollback();
      tripal_report_error('tripal_entities', TRIPAL_ERROR,
        'Unable to load Tripal Bundle :name after cache clear.', [':name' => $bundle_name], $message_args);
      return FALSE;
    }

    // Set the bundle category
    $category = array_key_exists('category', $args) ? $args['category'] : 'Other';
    tripal_set_bundle_variable('bundle_category', $bundle->id, $category);

    // Attache the bundle fields.
    tripal_create_bundle_fields($bundle, $term);

    // Specifically commiting here since we have a fully featured bundle.
    // Post-create hook implementations assume we have a
    // created bundle so we don't want to rollback if a
    // custom implementation causes an exception.
    unset($transaction);

  } catch (Exception $e) {
    $transaction->rollback();
    $message_args['drupal_set_message'] = TRUE;
    tripal_report_error('tripal_entities', TRIPAL_ERROR,
      "Failed to create content type: %message.", ['%message' => $e->getMessage()], $message_args);
    return FALSE;
  }

  // Call any custom hook_bundle_postcreate() implementations.
  // This is done outside of the try/catch & transaction
  // since it occurs after creation and thus should not cause
  // a rollback of the creation on error.
  $modules = module_implements('bundle_postcreate');
  foreach ($modules as $module) {
    $function = $module . '_bundle_postcreate';
    if (function_exists($function)) {
      $function($bundle);
    }
  }

  // Set admin access for the new bundle.
  tripal_admin_access($bundle);

  // Report that we're done.
  tripal_report_error('tripal_entities', TRIPAL_INFO, "Done.", [], $message_args);

  return $bundle;
}

/**
 * Retrieves a list of the content types.
 *
 * @return
 *   An array of bundles. Each bundle is an object containing information
 *   about that bundle.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_content_types() {
  return db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->execute()
    ->fetchAll();
}

/**
 * Refreshes the bundle such that new fields added by modules will be found
 * during cron.
 *
 * @param $bundle_name
 *   The name of the bundle to refresh (e.g. bio_data_4).
 *
 * @ingroup tripal_entities_api
 */
function tripal_tripal_cron_notification() {
  $num_created = 0;

  // Get all bundle names to cycle through.
  $all_bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb', ['name'])
    ->execute()->fetchAll();

  foreach ($all_bundles as $bundle_name) {
    // Get the bundle object.
    $bundle = tripal_load_bundle_entity(['name' => $bundle_name->name]);
    if (!$bundle) {
      tripal_report_error('tripal', TRIPAL_ERROR, "Unrecognized bundle name '%bundle'.",
        ['%bundle' => $bundle_name]);
      return FALSE;
    }
    // Allow modules to add fields to the new bundle.
    $modules = module_implements('bundle_fields_info');
    foreach ($modules as $module) {
      $function = $module . '_bundle_fields_info';
      $info = $function('TripalEntity', $bundle);
      foreach ($info as $field_name => $details) {

        // If the field already exists then skip it.
        $field = field_info_field($details['field_name']);
        if ($field) {
          continue;
        }

        // Create notification that new fields exist.
        $detail_info = ' Tripal has detected a new field ' . $details['field_name'] . ' for ' . $bundle->label . ' content type is available for import.';
        $title = 'New field available for import';
        $actions['Import'] = 'admin/import/field/' . $details['field_name'] . '/' . $bundle_name->name . '/' . $module . '/field';
        $type = 'Field';
        $submitter_id = $details['field_name'] . '-' . $bundle_name->name . '-' . $module;

        tripal_add_notification($title, $detail_info, $type, $actions, $submitter_id);
        $num_created++;
      }
    }

    // Allow modules to add instances to the new bundle.
    $modules = module_implements('bundle_instances_info');
    foreach ($modules as $module) {
      $function = $module . '_bundle_instances_info';
      $info = $function('TripalEntity', $bundle);
      foreach ($info as $field_name => $details) {

        // If the field is already attached to this bundle then skip it.
        $field = field_info_field($details['field_name']);
        if ($field and array_key_exists('bundles', $field) and
          array_key_exists('TripalEntity', $field['bundles']) and
          in_array($bundle->name, $field['bundles']['TripalEntity'])) {
          continue;
        }

        // Create notification that new fields exist.
        $detail_info = ' Tripal has detected a new field ' . $details['field_name'] . ' for ' . $bundle->label . ' content type is available for import.';
        $title = 'New field available for import';
        $actions['Import'] = 'admin/import/field/' . $details['field_name'] . '/' . $bundle->name . '/' . $module . '/instance';
        $type = 'Field';
        $submitter_id = $details['field_name'] . '-' . $bundle_name->name . '-' . $module;

        tripal_add_notification($title, $detail_info, $type, $actions, $submitter_id);
        $num_created++;
      }
    }
  }
}

/**
 * Retrieves information about a given content type.
 *
 * @param $bundle_name
 *   The name of a bundle.
 *
 * @return
 *   An object containing information about the bundle.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_content_type($bundle_name) {
  return db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->condition('tb.name', $bundle_name)
    ->execute()
    ->fetchAll();
}

/**
 * Refreshes the bundle such that new fields added by modules will be found.
 *
 * @param $bundle_name
 *   The name of the bundle to refresh (e.g. bio_data_4).
 * @param $term
 *   The term object for the bundle.
 *
 * @return
 *   The array of field instance names that were added.
 *
 * @ingroup tripal_entities_api
 */
function tripal_create_bundle_fields($bundle, $term) {

  $added = [];

  // Allow modules to add fields to the new bundle.
  $modules = module_implements('bundle_fields_info');
  $field_info = [];
  foreach ($modules as $module) {
    $function = $module . '_bundle_fields_info';
    $temp = $function('TripalEntity', $bundle);
    if (is_array($temp)) {
      // TODO: it would be good to check this array to make sure it follows
      // protocol.  It would help identify potential errors.
      $field_info = array_merge($field_info, $temp);
    }
  }

  // Allow modules to alter which fields should be attached to content
  // types they create.
  drupal_alter('bundle_fields_info', $field_info, $bundle, $term);

  // Iterate through all of the fields and create them.
  foreach ($field_info as $field_name => $details) {
    $field_type = $details['type'];

    // If the field already exists then skip it.
    $field = field_info_field($details['field_name']);
    if ($field) {
      continue;
    }

    // Create the field.
    $field = field_create_field($details);
    if (!$field) {
      tripal_set_message(t("Could not create new field: %field.",
        ['%field' => $details['field_name']]), TRIPAL_ERROR);
    }
  }

  // Allow modules to add instances to the new bundle.
  $modules = module_implements('bundle_instances_info');
  $instance_info = [];
  foreach ($modules as $module) {
    $function = $module . '_bundle_instances_info';
    $temp = $function('TripalEntity', $bundle);
    if (is_array($temp)) {
      // TODO: it would be good to check this array to make sure it follows
      // protocol.  It would help identify potential errors.
      $instance_info = array_merge($instance_info, $temp);
    }
  }

  // Allow modules to alter which fields should be attached to content
  // types they create.
  drupal_alter('bundle_instances_info', $instance_info, $bundle, $term);

  // Get the list of existing instances
  $existing_instances = field_info_instances('TripalEntity', $bundle->name);

  // Iterate through all of the field instances and create them.
  foreach ($instance_info as $instance_name => $details) {

    // Make sure the instance has a term. If not, report it and skip the field.
    if (!array_key_exists('term_vocabulary', $details['settings'])) {
      tripal_report_error('tripal_fields', TRIPAL_WARNING,
        'The field instance, !field, is missing the "term_vocabulary" setting. The field instance cannot be added. Please check the field settings.',
        ['!field' => $instance_name], ['drupal_set_message' => TRUE]);
      continue;
    }
    if (!array_key_exists('term_accession', $details['settings'])) {
      tripal_report_error('tripal_fields', TRIPAL_WARNING,
        'The field instance, !field, is missing the "term_accession" setting. The field instance cannot be added. Please check the field settings.',
        ['!field' => $instance_name], ['drupal_set_message' => TRUE]);
      continue;
    }

    // Make sure the term exists. If not, skip the field instance and
    // report an error.
    $field_term_id = $details['settings']['term_vocabulary'] . ':' . $details['settings']['term_accession'];
    $field_term = tripal_get_term_details($details['settings']['term_vocabulary'], $details['settings']['term_accession']);
    if (!$field_term) {
      tripal_report_error('tripal_fields', TRIPAL_WARNING,
        'The term, !term, for the field, !field, does not exist in the database. The  ' .
        'field instance cannot be added. Please make sure the term is correct and add it if necessary.',
        [
          '!term' => $field_term_id,
          '!field' => $instance_name,
        ],
        ['drupal_set_message' => TRUE]);
      continue;
    }

    // Make sure the term is not used for any other existing field instance.
    $skip = FALSE;
    foreach ($existing_instances as $existing_name => $existing_instance) {
      // If this instance term is the same as this exsiting term and the
      // instance name is not the same then we have a problem.
      $existing_term_id = $existing_instance['settings']['term_vocabulary'] . ':' . $existing_instance['settings']['term_accession'];
      $existing_field = field_info_field($existing_name);
      if ($existing_term_id == $field_term_id and $instance_name != $existing_name) {
        tripal_report_error('tripal_fields', TRIPAL_WARNING,
          'The field, !field, uses a term, !term, that is already in use on this content type. The ' .
          'field instance cannot be added.',
          [
            '!term' => $existing_term_id,
            '!field' => $instance_name,
          ],
          ['drupal_set_message' => TRUE]);
        $skip = TRUE;
      }

      // If the instance term is the same as this exsting term but the storage
      // types are different then we have a problem.
      $existing_storage = $existing_field['storage']['type'];
      $this_field_storage = $field_info[$details['field_name']]['storage']['type'];
      if ($existing_term_id == $field_term_id and $existing_storage != $this_field_storage) {
        tripal_report_error('tripal_fields', TRIPAL_WARNING,
          'The field, !field, provided by the storage type, !type, uses a term, !term, that is already in use on this content type and provided by another storage backend. The ' .
          'field instance cannot be added.  Perhaps, consider a different term and adjust the data in the database.',
          [
            '!term' => $existing_term_id,
            '!type' => $this_field_storage,
            '!field' => $instance_name,
          ],
          ['drupal_set_message' => TRUE]);
        $skip = TRUE;
      }
    }
    if ($skip) {
      continue;
    }

    // If the field is already attached to this bundle then skip it.
    if (array_key_exists($instance_name, $existing_instances)) {
      continue;
    }

    // Create the field instance.
    $instance = field_create_instance($details);
    $existing_instances[$instance_name] = $instance;
    $added[] = $instance_name;
  }
  return $added;
}

/**
 * Updates an existing field and its attached instance to a bundle.
 *
 *
 * @param $field_name
 *   The name of the field.
 * @param $field_info
 *   An associative array containing the field information.  The following
 *   key/value pairs are supported:
 *     - field_type: a valid field type.  May be any of the Drupal default
 *       fields, one created by the tripal_chado module or another custom
 *   module.
 *     - widget_type: a valid widget type. May be any of the Drupal default
 *       fields, one created by the tripal_chado module or another custom
 *   module.
 *     - field_settings: an array of settings that are appropriate for the
 *       selected field type.
 *     - widget_settings: an array of settings that are appropriate for the
 *       selected widget type.
 *     - description:  a default description for this field.
 *     - label: a label used as a header for this field.
 *     - is_required: indicates if the field is required in the edit form.
 *     - cardinality: indicates the number of values this field can support.
 *       the default is 1 (meaning only one value). Use a value of
 *       FIELD_CARDINALITY_UNLIMITED for unlimited number of values.
 *     - default_value: A default value for the field.
 *     - format: A string indicating the format for the field. Must be
 *       specific to the field.
 * @param $entity_type_name
 *   The entity type name.
 * @param $bundle_name
 *   The bundle name.
 *
 * @return
 *   FALSE if the field could not be updated
 *
 * TODO: this function really shouldn't try to create an instance and
 * attach to a bundle  at the same time.
 *
 * @ingroup tripal_entities_api
 */
function tripal_update_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name) {

  $field = field_info_field($field_name);

  // If the field doesn't exists or is not attached to this bundle then
  // just return, there is nothing left to do.
  if (!$field or !array_key_exists('bundles', $field) or
    !array_key_exists($entity_type_name, $field['bundles']) or
    !in_array($bundle_name, $field['bundles'][$entity_type_name])) {
    return FALSE;
  }

  $field['field_name'] = $field_name;
  if (array_key_exists('field_type', $field_info)) {
    $field['cardinality'] = $field_info['cardinality'];
  }
  if (array_key_exists('locked', $field_info)) {
    $field['locked'] = $field_info['locked'];
  }
  if (array_key_exists('storage', $field_info)) {
    $field['storage']['type'] = $field_info['storage'];
  }
  if (array_key_exists('field_settings', $field_info)) {
    $field['settings'] = $field_info['field_settings'];
  }

  field_update_field($field);

  $field_instance['field_name'] = $field_name;
  $field_instance['entity_type'] = $entity_type_name;
  $field_instance['bundle'] = $bundle_name;
  if (array_key_exists('label', $field_info)) {
    $field['label'] = $field_info['label'];
  }
  if (array_key_exists('description', $field_info)) {
    $field['description'] = $field_info['description'];
  }
  if (array_key_exists('widget', $field_info)) {
    if (array_key_exists('widget_type', $field_info['widget'])) {
      $field['widget']['type'] = $field_info['widget_type'];
    }
    if (array_key_exists('widget_settings', $field_info['widget'])) {
      $field['widget']['settings'] = $field_info['widget_settings'];
    }
  }
  if (array_key_exists('required', $field_info)) {
    $field['required'] = $field_info['is_required'];
  }
  if (array_key_exists('settings', $field_info)) {
    $field['settings'] = $field_info['field_settings'];
  }
  if (array_key_exists('default_value', $field_info)) {
    $field['default_value'] = $field_info['default_value'];
  }
  if (array_key_exists('format', $field_info)) {
    $field['format'] = $field_info['format'];
  }
  field_update_instance($field_instance);
}


/**
 * @section
 * Bundle Variables.
 */

/**
 * Fetch the value for a given tripal variable.
 *
 * @param string $variable_name
 *   The name of the variable as in tripal_variables.name.
 * @param int $bundle_id
 *   The unique identfier for the bundle you want the value for.
 *
 * @return text
 *   The value of the specified variable for the specified bundle.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_bundle_variable($variable_name, $bundle_id, $default = FALSE) {

  $variable = tripal_get_variable($variable_name);

  // Warn if we can't find the tripal_variable.
  if (!$variable) {
    return $default;
  }

  // Select the value for this variable.
  $value = db_select('tripal_bundle_variables', 'var')
    ->fields('var', ['value'])
    ->condition('var.bundle_id', $bundle_id)
    ->condition('var.variable_id', $variable->variable_id)
    ->execute()
    ->fetchField();

  // Warn if the value appears to be empty.
  if (!$value) {
    return $default;
  }

  return $value;
}

/**
 * Save the value of a tripal variable for a given bundle.
 *
 * @param string $variable_name
 *   The name of the variable as in tripal_variables.name.
 * @param int $bundle_id
 *   The unique identfier for the bundle you want the value for.
 * @param $text $value
 *   The value of the variable for the given bundle.
 *
 * @ingroup tripal_entities_api
 */
function tripal_set_bundle_variable($variable_name, $bundle_id, $value) {
  $variable = tripal_get_variable($variable_name);

  // And then we need to write the new format to the tripal_bundle_variables
  // table.
  $record = [
    'bundle_id' => $bundle_id,
    'variable_id' => $variable->variable_id,
    'value' => $value,
  ];

  // Check whether there is already a format saved.
  $bundle_variable_id = db_select('tripal_bundle_variables', 'var')
    ->fields('var', ['bundle_variable_id'])
    ->condition('var.bundle_id', $record['bundle_id'])
    ->condition('var.variable_id', $record['variable_id'])
    ->execute()
    ->fetchField();
  if ($bundle_variable_id) {
    $record['bundle_variable_id'] = $bundle_variable_id;
    return drupal_write_record('tripal_bundle_variables', $record, 'bundle_variable_id');
  }
  else {
    return drupal_write_record('tripal_bundle_variables', $record);
  }

}


/**
 * @section
 * Title & URL Formats.
 */

/**
 * Get Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $bundle
 *   The Entity object for the Tripal Bundle the title format is for.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_title_format($bundle) {

  // Get the existing title format if it exists.
  $title_format = tripal_get_bundle_variable('title_format', $bundle->id);

  // If there isn't yet a title format for this bundle/type then we should
  // determine the default.
  if (!$title_format) {
    $title_format = tripal_get_default_title_format($bundle);
    tripal_save_title_format($bundle, $title_format);
  }

  return $title_format;
}

/**
 * Save Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 * @param string $format
 *   The pattern to be used when generating entity titles for the above type.
 *
 * @ingroup tripal_entities_api
 */
function tripal_save_title_format($entity, $format) {

  return tripal_set_bundle_variable('title_format', $entity->id, $format);
}

/**
 * Determine the default title format to use for an entity.
 *
 * @param TripalBundle $bundle
 *   The Entity object for the Tripal Bundle that the title format is for.
 *
 * @return string
 *   A default title format.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_default_title_format($bundle) {
  $format = '';

  // Retrieve all available tokens.
  $tokens = tripal_get_entity_tokens($bundle);

  // A) Check to see if more informed modules have suggested a title for this
  //    type. Invoke hook_tripal_default_title_format() to get all suggestions
  //    from other modules.
  $suggestions = module_invoke_all('tripal_default_title_format', $bundle, $tokens);
  if ($suggestions) {
    // Use the suggestion with the lightest weight.
    $lightest_key = NULL;
    foreach ($suggestions as $k => $s) {
      if ($lightest_key === NULL) {
        $lightest_key = $k;
      }
      if ($s['weight'] < $lightest_key) {
        $lightest_key = $k;
      }
    }
    $format = $suggestions[$lightest_key]['format'];
    return $format;
  }

  // B) Generate our own ugly title by simply comma-separating all the
  //    required fields.
  if (!$format) {
    $tmp = [];

    // Check which tokens are required fields and join them into a default
    // format.
    foreach ($tokens as $token) {
      if ($token['required']) {
        $tmp[] = $token['token'];
      }
    }
    $format = implode(', ', $tmp);
    return $format;
  }

  return $format;
}


/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param TripalBundle $bundle
 *    The bundle entity for which you want tokens.
 *
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_entity_tokens($bundle, $options = []) {
  $tokens = [];

  // Set default options.
  $options['required only'] = (isset($options['required only'])) ? $options['required only'] : FALSE;
  $options['include id'] = (isset($options['include id'])) ? $options['include id'] : TRUE;

  if ($options['include id']) {
    $token = '[TripalBundle__bundle_id]';
    $tokens[$token] = [
      'label' => 'Bundle ID',
      'description' => 'The unique identifier for this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalEntity__entity_id]';
    $tokens[$token] = [
      'label' => 'Content/Entity ID',
      'description' => 'The unique identifier for an individual piece of Tripal Content.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];
  }

  $instances = field_info_instances('TripalEntity', $bundle->name);
  foreach ($instances as $instance_name => $instance) {

    if (!$instance['required'] and $options['required only']) {
      continue;
    }

    $use_field = FALSE;

    // Iterate through the TripalEntity fields and see if they have
    // sub-elements, if so, add those as tokens too.
    $field_name = $instance['field_name'];
    if ($instance['entity_type'] == 'TripalEntity') {
      if (tripal_load_include_field_class($field_name)) {
        $field = field_info_field($field_name);
        $field_obj = new $field_name($field, $instance);
        $element_info = $field_obj->elementInfo();
        $term_id = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];
        if ($element_info and
          array_key_exists($term_id, $element_info) and
          array_key_exists('elements', $element_info[$term_id]) and count($element_info[$term_id]['elements']) > 0) {
          $elements = $element_info[$term_id]['elements'];
          _tripal_get_entity_tokens_for_elements($instance, $field_name, $elements, $tokens, $options);
        }
        else {
          $use_field = TRUE;
        }
      }
      else {
        $use_field = TRUE;
      }
    }
    else {
      $use_field = TRUE;
    }

    // If we have no elements to add then just add the field as is.
    if ($use_field) {
      // Build the token from the field information.
      $token = '[' . $instance['field_name'] . ']';
      $tokens[$token] = [
        'label' => $instance['label'],
        'description' => $instance['description'],
        'token' => $token,
        'field_name' => $instance['field_name'],
        'required' => $instance['required'],
      ];
    }
  }

  return $tokens;
}

/**
 * A recursive helper function to get tokens for element sub fields.
 *
 * @param $instance
 *   A original field instance object.
 * @param $parent
 *   The name of the parent. The first time this is called outside of
 *   recursion this should be the field name.
 * @param $elements
 *   The array of elements to process.
 * @param $tokens
 *   The array of tokens to be added to.
 */
function _tripal_get_entity_tokens_for_elements($instance, $parent, $elements, &$tokens, $options) {

  // Iterate through all of the elements and add tokens for each one.
  foreach ($elements as $child_term_id => $details) {

    // We don't need to add the entity element.
    if ($child_term_id == 'entity') {
      continue;
    }

    // Skip elements that aren't required.
    $required = array_key_exists('required', $details) ? $details['required'] : FALSE;
    if (!$required and $options['required only']) {
      continue;
    }
    $token = '[' . $parent . ',' . $child_term_id . ']';
    $label = $child_term_id;
    if (array_key_exists('name', $details)) {
      $label = $details['name'];
    }
    elseif (preg_match('/:/', $child_term_id)) {
      list($vocabulary, $accession) = explode(':', $child_term_id);
      $term = tripal_get_term_details($vocabulary, $accession);
      $label = $term['name'];
    }

    // Add the token!
    $tokens[$token] = [
      'label' => $label,
      'description' => array_key_exists('description', $details) ? $details['description'] : '',
      'token' => $token,
      'field_name' => $instance['field_name'],
      'required' => $required,
    ];

    // Recurse to include sub elements
    if (array_key_exists('elements', $details)) {
      _tripal_get_entity_tokens_for_elements($instance, $parent . ',' . $child_term_id,
        $details['elements'], $tokens, $options);
    }
  }
}

/**
 * Replace all Tripal Tokens in a given string.
 *
 * NOTE: If there is no value for a token then the token is removed.
 *
 * @param string $string
 *   The string containing tokens.
 * @param TripalEntity $entity
 *   The entity with field values used to find values of tokens.
 * @param TripalBundle $bundle_entity
 *   The bundle enitity containing special values sometimes needed for token
 *   replacement.
 *
 * @return
 *   The string will all tokens replaced with values.
 *
 * @ingroup tripal_entities_api
 */
function tripal_replace_entity_tokens($string, &$entity, $bundle_entity = NULL) {
  // Determine which tokens were used in the format string
  $used_tokens = [];
  if (preg_match_all('/\[.*?\]/', $string, $matches)) {
    $used_tokens = $matches[0];
  }

  // If there are no tokens then just return the string.
  if (count($used_tokens) == 0) {
    return $string;
  }

  // If the fields are not loaded for the entity then we want to load them
  // but we won't do a field_attach_load() as that will load all of the
  // fields. For syncing (publishing) of content loading all fields for
  // all synced entities causes extreme slowness, so we'll only attach
  // the necessary fields for replacing tokens.
  $attach_fields = [];

  foreach ($used_tokens as $token) {
    $token = preg_replace('/[\[\]]/', '', $token);
    $elements = explode(',', $token);
    $field_name = array_shift($elements);
    //$field_name = str_replace(array('.','[',']'), array('__','',''), $field_name);
    if (!property_exists($entity, $field_name) or empty($entity->{$field_name})) {
      $field = field_info_field($field_name);
      $storage = $field['storage'];
      $attach_fields[$storage['type']]['storage'] = $storage;
      $attach_fields[$storage['type']]['fields'][] = $field;
    }
  }

  // If we have any fields that need attaching, then do so now.
  if (count(array_keys($attach_fields)) > 0) {
    foreach ($attach_fields as $storage_type => $details) {
      $field_ids = [];
      $storage = $details['storage'];
      $fields = $details['fields'];
      foreach ($fields as $field) {
        $field_ids[$field['id']] = [$entity->id];
      }
      $entities = [$entity->id => $entity];
      module_invoke($storage['module'], 'field_storage_load', 'TripalEntity',
        $entities, FIELD_LOAD_CURRENT, $field_ids, []);
    }
  }

  // Now that all necessary fields are attached process the tokens.
  foreach ($used_tokens as $token) {
    $token = preg_replace('/[\[\]]/', '', $token);
    $elements = explode(',', $token);
    $field_name = array_shift($elements);
    $value = '';

    if (property_exists($entity, $field_name)) {
      $value = '';
      // Note: there is a memory leak in field_get_items() so we can't use it
      // here or bulk publishing will slowly erode memory.
      // $field_value = field_get_items('TripalEntity', $entity, $field_name);
      if (array_key_exists('und', $entity->{$field_name}) and
        array_key_exists(0, $entity->{$field_name}['und'])) {
        $value = $entity->{$field_name}['und'][0]['value'];
        // If the value is an array it means we have sub elements and we can
        // descend through the array to look for matching value.
        if (is_array($value) and count($elements) > 0) {
          $value = _tripal_replace_entity_tokens_for_elements($elements, $value);
        }
      }
    }
    // The TripalBundle__bundle_id is a special token for substituting the
    // bundle id.
    elseif ($field_name === 'TripalBundle__bundle_id') {
      // Load the bundle entity if we weren't given it.
      if (!$bundle_entity) {
        $bundle_entity = tripal_load_bundle_entity(['name' => $entity->bundle]);
      }
      // This token should be the id of the TripalBundle.
      $value = $bundle_entity->id;
    }
    // The TripalBundle__bundle_id is a special token for substituting the
    // entty id.
    elseif ($field_name === 'TripalEntity__entity_id') {
      // This token should be the id of the TripalEntity.
      $value = $entity->id;
    }

    // We can't support tokens that have multiple elements (i.e. in an array).
    if (is_array($value)) {
      $string = str_replace('[' . $token . ']', '', $string);
    }
    else {
      $string = str_replace('[' . $token . ']', $value, $string);
    }
  }

  return $string;
}

/**
 * A helper function for tripal_replace_entity_tokens to get token values.
 *
 * This helper function is used when the tokens are from subelements.
 *
 * @param $entity
 */
function _tripal_replace_entity_tokens_for_elements($elements, $values) {
  $term_id = array_shift($elements);
  $value = $values[$term_id];
  if (count($elements) == 0) {
    return $value;
  }
  else {
    _tripal_replace_entity_tokens_for_elements($elements, $value);
  }
}

/**
 * Formats the tokens for display.
 *
 * @param array $tokens
 *   A list of tokens generated via tripal_get_entity_tokens().
 *
 * @return
 *   Rendered output describing the available tokens.
 *
 * @ingroup tripal_entities_api
 */
function theme_token_list($tokens) {

  $header = ['Token', 'Name', 'Description'];
  $rows = [];
  foreach ($tokens as $details) {
    $rows[] = [
      $details['token'],
      $details['label'],
      $details['description'],
    ];
  }

  return theme('table', ['header' => $header, 'rows' => $rows]);
}

/**
 * Define the entity label callback.  This will return the title.
 *
 * @param $entity
 *
 * @return mixed
 *
 * @ingroup tripal_entities_api
 */
function tripal_entity_label($entity) {
  if (property_exists($entity, 'title')) {
    return $entity->title;
  }
  return NULL;
}

/**
 * Retrieves details, including attached fields, for a given bundle.
 *
 * @param $bundle_name
 *   The name of the bundle (e.g. bio_data_xx)
 *
 * @return
 *   An array containing the name, label, controlled vocabulary details
 *   and a list of fields attached to the bundle.  Returns FALSE
 *   if the bundle does not exist.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_bundle_details($bundle_name) {
  global $user;

  $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
  if (!$bundle) {
    return FALSE;
  }
  $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
  $vocab = $term->vocab;
  $instances = field_info_instances('TripalEntity', $bundle->name);

  $details = [
    'name' => $bundle->name,
    'label' => $bundle->label,
    'term' => [
      'accession' => $vocab->vocabulary . ':' . $term->accession,
      'name' => $term->name,
      'definition' => $term->definition,
      'url' => $term->url,
    ],
    'fields' => [],
  ];

  // Iterate through each feild and provide a discription of it and
  // it's sub elements.
  foreach ($instances as $instance) {
    // Skip deleted fields.
    if ($instance['deleted']) {
      continue;
    }

    $field_name = $instance['field_name'];
    $field = field_info_field($field_name);

    $field_class = $field['type'];
    $term_vocab = $instance['settings']['term_vocabulary'];
    $term_accession = $instance['settings']['term_accession'];
    $field_term = tripal_get_term_details($term_vocab, $term_accession);
    $field_details = [
      'name' => $field_name,
      'label' => $instance['label'],
      'term' => [
        'accession' => $term_vocab . ":" . $term_accession,
        'name' => $field_term['name'],
        'definition' => $field_term['definition'],
        'url' => $field_term['url'],
      ],
      // These items can be overridden by the element_info array that
      // is present in a TripalField instance.  Here we set defaults.
      'required' => $instance['required'] ? TRUE : FALSE,
      'type' => 'xs:string',
      'readonly' => TRUE,
      // The cardinatlity value always comes from the field.
      'cardinality' => $field['cardinality'],
    ];

    if (tripal_load_include_field_class($field_class)) {
      $field_obj = new $field_class($field, $instance);
      $element_info = $field_obj->elementInfo();
      $element_info = $element_info[$term_vocab . ':' . $term_accession];

      // If the element info for this field sets required, type and readonly
      // attributes then set those.
      $field_details['required'] = array_key_exists('required', $element_info) ? $element_info['required'] : FALSE;
      $field_details['type'] = array_key_exists('type', $element_info) ? $element_info['type'] : 'xs:string';
      $field_details['readonly'] = array_key_exists('readonly', $element_info) ? $element_info['readonly'] : TRUE;
      $field_details['label'] = array_key_exists('label', $element_info) ? $element_info['label'] : $field_details['label'];
      $field_details['help'] = array_key_exists('help', $element_info) ? $element_info['help'] : '';

      // If this field is an 'xs:complexType' then it will have sub elements.
      // we need to add those as well.
      if (array_key_exists('elements', $element_info) and is_array($element_info['elements'])) {
        _tripal_get_bundle_field_element_details($element_info['elements'], $field_details);
      }
      $details['fields'][] = $field_details;
    }

  }
  return $details;
}

/**
 * A recursive helper function for the tripal_get_bundle_details.
 *
 * @param $elementInfo
 *
 * @ingroup tripal_entities_api
 */
function _tripal_get_bundle_field_element_details($elements, &$field_details) {
  $field_details['elements'] = [];
  foreach ($elements as $element_key => $element_info) {
    // Handle the entity element differnetly.
    if ($element_key == 'entity') {
      continue;
    }
    list($term_vocab, $term_accession) = explode(':', $element_key);
    $term = tripal_get_term_details($term_vocab, $term_accession);

    $element_details = [
      'name' => $element_info['name'],
      'label' => array_key_exists('label', $element_info) ? $element_info['label'] : ucfirst(preg_replace('/_/', ' ', $term['name'])),
      'help' => array_key_exists('help', $element_info) ? $element_info['help'] : '',
      'term' => [
        'accession' => $term_vocab . ':' . $term_accession,
        'name' => $term['name'],
        'definition' => $term['definition'],
        'url' => $term['url'],
      ],
      'required' => array_key_exists('required', $element_info) ? $element_info['required'] : FALSE,
      'type' => array_key_exists('type', $element_info) ? $element_info['type'] : 'xs:string',
      'readonly' => array_key_exists('readonly', $element_info) ? $element_info['readonly'] : TRUE,
    ];
    if (array_key_exists('elements', $element_info) and is_array($element_info['elements'])) {
      _tripal_get_bundle_field_element_details($element_info['elements'], $element_details);
    }
    $field_details['elements'][] = $element_details;
  }
}

/**
 * Is this completed? It doesn't look right and I can't find it used anywhere
 * in the existing code.
 *
 * @param $bundle_name
 *   The name of the bundle (e.g. bio_data_xx)
 * @param unknown $values
 *
 * @throws Exception
 *
 *
 */
function tripal_insert_entity($bundle_name, $values) {
  global $user;

  $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

  // Get the fields associated with this content type.
  $instances = field_info_instances('TripalEntity', $bundle->name);

  foreach ($instances as $instance) {
    $field_name = $instance['field_name'];
    $field = field_info_field($field_name);
    $field_type = $field['type'];
    $field_settings = $field['settings'];
    $instance_settings = $instance['settings'];
    $field_name = $field['field_name'];
    $vocabulary = $instance['settings']['term_vocabulary'];
    $accession = $instance['settings']['term_accession'];
    $field_accession = $vocabulary . ':' . $accession;
    $field_term = tripal_get_term_details($vocabulary, $accession);
    $field_key = $field_term['name'];
    $field_key = strtolower(preg_replace('/ /', '_', $key));

    // There are three ways that a field value can be specified. Those
    // are as the controlled vocabulary accession (e.g. GO:0000134), sa
    // the field name or as the field key which is the term name with
    // spaces replaced with underscores.
    // First make sure that required fields are present.
    if ($instance['required'] == TRUE) {
      if (!array_key_exists($field_key, $values) and
        !array_key_exists($field_accession, $values) and
        !array_key_exists($field_name, $values)) {
        throw new Exception(t('Cannot insert the record. Missing the required field "%missing".',
          ['%missing' => $field_name]));
      }
    }
  }

  // Make sure that all required fields are present.

  // TODO: make sure the user has permission to do this.
  $ec = entity_get_controller('TripalEntity');
  $entity = $ec->create([
    'bundle' => $bundle_name,
    'term_id' => $bundle->term_id,
  ]);
  $entity = $entity->save();
}

/**
 * Are we keeping this?
 *
 * @param $bundle_name
 * @param $values
 *
 *
 */
function tripal_update_entity($bundle_name, $values) {


}


/**
 * Removes orphaned entities.
 *
 * An orphaned entity can occur if the module that created the entity
 * unknowingly lost its underlying record in its data store.  Such a case
 * could happen if someone directly removed the record from the data store
 * outside of the module's control. This function allows each module
 * to report if any orphans are missing for a given bundle type.
 *
 * @param integer $bundle_id
 *   The numeric ID of the bundle.
 * @param TripalJob $job
 *   (Optional). If this function is executed via the Tripal Jobs system then
 *   this argument is provided.
 */
function tripal_unpublish_orphans(int $bundle_id, TripalJob $job = NULL) {
  $bundlec = entity_get_controller('TripalBundle');
  $ids = $bundlec->deleteOrphans($bundle_id, $job);
}

/**
 * A hook for modules to delete details for orphaned entities.
 *
 * This hook is called by the TripalBundleController.  Modules that create
 * entities should use this hook to clean up entities that are orphaned. The
 * list of $ids passed should be entities who are already known to
 * be orphaned. These IDs are found by the TripalBundleController using the
 * results from the hook_bundle_find_orphans() function.
 *
 * An implementation of this hook should not try to clean up the entity itself,
 * but rather it should only clean up its own records used to manage the
 * relationship between the entity and the underlying data that the
 * module provides.
 *
 * An orphaned entity can occur if the module that created the entity
 * unknowingly lost its underlying record in its data store.  Such a case
 * could happen if someone directly removed the record from the data store
 * outside of the module's control. This function allows each module
 * to report if any orphans are missing for a given bundle type.
 *
 * @param TripalBundle $bundle
 *   A TripalBundle object for the bundle whose entities are orphaned.
 * @param array $ids
 *   A list of entity IDs known to be orphaned.
 * @param TripalJob $job
 *   An optional Tripal Job object. This is provided when this function is
 *   called using the Tripal Jobs system.  Implementors of this hook can
 *   use the addItemsHandled() function to indicate how many entities were
 *   cleaned up.
 *
 * @return integer
 *   The number of entitites that were cleaned up.
 */
function hook_bundle_delete_orphans(TripalBundle $bundle, array $ids, TripalJob $job = NULL) {

  // See the tripal_chado_bundle_delete_orphans() function for an example.

}

/**
 * A hook for modules to report on oprhaned entities.
 *
 * An orphaned entity can occur if the module that created the entity
 * unknowingly lost its underlying record in its data store.  Such a case
 * could happen if someone directly removed the record from the data store
 * outside of the module's control. This function allows each module
 * to report if any orphans are missing for a given bundle type.
 *
 * @param TripalBunldle $bundle
 *   A TripalBundle object for the bundle that should be checked for
 *   orphaned entities.
 * @param bool $count
 *   TRUE if the function should return only the number of orphaned entities.
 *   FALSE if the function should return the list of orphned entities.
 * @param integer $offset
 *   For paging of entities set this to the offset within the total count.
 * @param integer $limit
 *   For paging of entities set this to the total number to return.
 *
 * @return array|bool
 *  If $count == FALSE then an array of all entity IDs that are orphaned. If
 *  $count == TRUE then a single integer count value is returned.
 */
function hook_bundle_find_orphans(TripalBundle $bundle, $count = FALSE,
                                  $offset = 0, $limit = 10) {

  // See the tripal_chado_bundle_find_orphans() function for an example.

}

