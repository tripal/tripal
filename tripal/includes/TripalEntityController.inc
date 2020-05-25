<?php

/**
 * TripalEntityController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class TripalEntityController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);

  }

  /**
   * Create a Tripal data entity
   *
   * We first set up the values that are specific
   * to our data schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the entity.
   *
   * @return
   *   An object with all default fields initialized.
   */
  public function create(array $values = []) {

    // Add some items to the values array passed to the constructor
    global $user;

    $values['uid'] = $user->uid;
    $values['created'] = time();
    $values['changed'] = time();
    $values['title'] = '';
    $values['type'] = 'TripalEntity';
    $values['nid'] = '';
    $values['status'] = 1;

    // Call the parent constructor.
    $entity = parent::create($values);

    // Allow modules to make additions to the entity when it's created.
    $modules = module_implements('entity_create');
    foreach ($modules as $module) {
      $function = $module . '_entity_create';
      if (isset($values['bundle_object'])) {
        $function($entity, $values['type'], $values['bundle_object']);
      }
      else {
        $function($entity, $values['type']);
      }
    }
    return $entity;

  }

  /**
   * Provides an unpublish function.
   *
   * @param array $ids
   *    An array of the ids denoting which entities to delete.
   * @param DatabaseTransaction $transaction
   *    Optionally a DatabaseTransaction object to use. Allows overrides to
   *   pass in their transaction object.
   */
  public function unpublish($ids, $transaction = NULL) {

    if (!$transaction) {
      $transaction = db_transaction();
    }

    try {
      // First load the entity.
      $entities = entity_load('TripalEntity', $ids);

      // Then properly delete each one.
      foreach ($entities as $entity) {

        // Invoke hook_entity_delete().
        module_invoke_all('entity_unpublish', $entity, $entity->type);

        // Delete any field data for this entity.
        field_attach_delete('TripalEntity', $entity);

        // Delete the entity record from our base table.
        db_delete('tripal_entity')
          ->condition('id', $entity->id)
          ->execute();
      }
    } catch (Exception $e) {
      if ($transaction) {
        $transaction->rollback();
      }
      watchdog_exception('tripal', $e);
      throw $e;
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Overrides EntityAPIController::delete().
   *
   * @param array $ids
   *    An array of the ids denoting which entities to delete.
   * @param DatabaseTransaction $transaction
   *    Optionally a DatabaseTransaction object to use. Allows overrides to
   *   pass in their transaction object.
   */
  public function delete($ids, $transaction = NULL) {
    global $user;

    // Next delete the entities
    if (!$transaction) {
      $transaction = db_transaction();
    }

    try {

      // First load the entity.
      $entities = entity_load('TripalEntity', $ids);
      $this->unpublish($ids, $transaction);

      // Then properly delete each one.
      foreach ($entities as $entity) {
        module_invoke_all('entity_delete', $entity, $entity->type);
      }

    } catch (Exception $e) {
      if ($transaction) {
        $transaction->rollback();
      }
      watchdog_exception('tripal', $e);
      throw $e;
      return FALSE;
    }

    // Now remove any orphaned entities
    tripal_add_job('Delete Orphaned Entities', 'tripal_chado',
        'tripal_unpublish_orphans', [0], $user->uid, 10, [], TRUE);

    return TRUE;
  }

  /**
   * Sets the title for an entity.
   *
   * @param $entity
   *   The entity whose title should be changed.
   * @param $title
   *   The title to use. It can contain tokens the correspond to field values.
   *   Token should be be compatible with those returned by
   *   tripal_get_entity_tokens().
   * @param $cache
   *   This array is used to store objects you want to cache for performance
   *   reasons, as well as, cache related options. The following are supported:
   *   - TripalBundle $bundle
   *       The bundle for the current entity.
   */
  public function setTitle($entity, $title = NULL, $cache = []) {

    if (isset($cache['bundle'])) {
      $bundle = $cache['bundle'];
    }
    else {
      $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
    }

    // If no title was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$title) {
      $title = tripal_get_title_format($bundle);
    }
    $title = tripal_replace_entity_tokens($title, $entity, $bundle);

    if ($title) {
      db_update('tripal_entity')
        ->fields([
          'title' => $title,
        ])
        ->condition('id', $entity->id)
        ->execute();
    }
  }

  /**
   * Sets the URL alias for an entity.
   *
   * @param $entity
   *   The entity whose URL alias should be changed.
   * @param $alias
   *   The alias to use. It can contain tokens the correspond to field values.
   *   Token should be be compatible with those returned by
   *   tripal_get_entity_tokens().
   * @param $cache
   *   This array is used to store objects you want to cache for performance
   *   reasons, as well as, cache related options. The following are supported:
   *   - TripalBundle $bundle
   *       The bundle for the current entity.
   *   - TripalTerm $term
   *       The term for the current entity.
   */
  public function setAlias($entity, $alias = NULL, $cache = []) {
    $source_url = "bio_data/$entity->id";

    // If no alias was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$alias) {

      // Load the TripalBundle entity for this TripalEntity (if it's not cached).
      // First get the format for the url alias based on the bundle of the entity.
      // Then replace all the tokens with values from the entity fields.
      if (isset($cache['bundle'])) {
        $bundle_entity = $cache['bundle'];
      }
      else {
        $bundle_entity = tripal_load_bundle_entity(['name' => $entity->bundle]);
      }
      $alias = tripal_get_bundle_variable('url_format', $bundle_entity->id);
      $alias = tripal_replace_entity_tokens($alias, $entity, $bundle_entity);
    }

    // If there is still no alias supplied then we should generate one using
    // the term name and entity id.
    if (!$alias) {

      // Load the term for this TripalEntity. Set a default based on the term
      // name and entity id. Then replace all the tokens with values from
      // the entity fields.
      $term = (isset($cache['term'])) ? $cache['term'] : entity_load('TripalTerm', ['id' => $entity->term_id]);
      $term = reset($term);
      $alias = str_replace(' ', '', $term->name) . '/[TripalEntity__entity_id]';
      $alias = tripal_replace_entity_tokens($alias, $entity, $bundle_entity);
    }

    // Check if the passed alias has tokens. Load the TripalBundle entity for
    // this TripalEntity. Then replace all the tokens with values from the
    // entity fields.
    if ($alias && (preg_match_all("/\[[^\]]*\]/", $alias, $bundle_tokens))) {
      if (isset($cache['bundle'])) {
        $bundle_entity = $cache['bundle'];
      }
      else {
        $bundle_entity = tripal_load_bundle_entity(['name' => $entity->bundle]);
      }
      $alias = tripal_replace_entity_tokens($alias, $entity, $bundle_entity);
    }

    // Make sure the alias doesn't contain spaces.
    //$alias = preg_replace('/\s+/','-',$alias);
    // Or any non alpha numeric characters.
    //$alias = preg_replace('/[^a-zA-Z0-9\-\/]/','',$alias);
    //$alias = preg_replace('/_/','-',$alias);

    if ($alias) {
      // Determine if this alias has already been used.
      $sql = '
        SELECT count(*) as num_alias
        FROM {url_alias}
        WHERE alias=:alias
      ';
      $num_aliases = db_query($sql, [':alias' => $alias])->fetchField();

      // Either there isn't an alias yet so we just create one.
      // OR an Alias already exists but we would like to add a new one.
      if ($num_aliases == 0) {
        // First delete any previous alias' for this entity.
        // Then save the new one.

        // @performance: Look into this further.
        // @spficklin publishing an entity can be very slow if there are lots of
        // entries in the url_alias table, due to this type of
        // SQL statement that gets called in drupal_path_alias_whitelist_rebuild():
        // SELECT DISTINCT SUBSTRING_INDEX(source, '/', 1) AS path FROM url_alias.
        // Perhaps we should write our own SQL to avoid this issue.
        // @lacey: drupal_path_alias_whitelist_rebuild() isn't getting called for me during publish.
        $values = [
          'source' => $source_url,
          'alias' => $alias,
          'language' => 'und',
        ];

        // Now check if an entry with the source url for this entity already
        // exists. This is an issue when updating existing url aliases. To avoid
        // creating 404s existing aliases need to be updated and a redirect
        // created to handle the old alias.
        $existing_aliases = db_select('url_alias', 'ua')
          ->fields('ua')
          ->condition('source', $source_url, '=')
          ->execute()->fetchAll();
        $num_aliases = count($existing_aliases);
        if ($num_aliases) {
          // For each existing entry create a redirect.
          foreach ($existing_aliases as $ea) {
            $path = [
              'source' => $ea->source,
              'alias' => $alias,
              'pid' => $ea->pid,
              'original' => [
                'alias' => $ea->alias,
                'pid' => $ea->pid,
                'language' => $ea->language,
              ],
            ];
            module_load_include('module', 'redirect', 'redirect');
            redirect_path_update($path);

            //After redirects created now update the url_aliases table.
            db_update('url_alias')
              ->fields([
                'alias' => $alias,
              ])
              ->condition('source', $source_url, '=')
              ->condition('pid', $ea->pid, '=')
              ->execute();
          }
        }
        else {
          drupal_write_record('url_alias', $values);
        }
      }
      // If there is only one alias matching then it might just be that we
      // already assigned this alias to this entity in a previous save.
      elseif ($num_aliases == 1) {

        if (isset($cache['bundle'])) {
          $bundle_entity = $cache['bundle'];
        }
        else {
          $bundle_entity = tripal_load_bundle_entity(['name' => $entity->bundle]);
        }

        // Check to see if the single alias is for the same entity and if not
        // warn the admin that the alias is already used (ie: not unique?)
        $sql = "
          SELECT count(*) as num_alias
          FROM {url_alias}
          WHERE alias=:alias AND source=:source
        ";
        $replace = [':alias' => $alias, ':source' => $source_url];
        $same_alias = db_query($sql, $replace)->fetchField();
        if (!$same_alias) {
          $msg = 'The URL alias, %alias, already exists for another page. ' .
            'Please ensure the pattern supplied on the <a href="!link" ' .
            'target="_blank">%type Edit Page</a> under URL Path options is ' .
            'unique.';
          $msg_var = [
            '%alias' => $alias,
            '!link' => url("admin/structure/bio_data/manage/$entity->bundle"),
            '%type' => $bundle_entity->label,
          ];
          tripal_report_error('trpentity', TRIPAL_WARNING, $msg, $msg_var);
          drupal_set_message(t($msg, $msg_var), 'warning');
        }
      }
      // If there are more then one alias' matching what we generated then there's
      // a real problem and we need to warn the administrator.
      else {
        if (isset($cache['bundle'])) {
          $bundle_entity = $cache['bundle'];
        }
        else {
          $bundle_entity = tripal_load_bundle_entity(['name' => $entity->bundle]);
        }

        $aliases = db_query('SELECT source FROM {url_alias} WHERE alias=:alias',
          [':alias' => $alias])->fetchAll();
        $pages = [];
        foreach ($aliases as $a) {
          $pages[] = $a->source;
        }

        $msg = 'The URL alias, %alias, already exists for multiple pages! ' .
          'Please ensure the pattern supplied on the <a href="!link" ' .
          'target="_blank">%type Edit Page</a> under URL Path options is ' .
          'unique.';
        $msg_var = [
          '%alias' => $alias,
          '!link' => url("admin/structure/bio_data/manage/$entity->bundle"),
          '%type' => $bundle_entity->label,
        ];
        drupal_set_message(t($msg, $msg_var), 'error');

        $msg .= ' This url alias has already been used for the following pages: %pages.
          You can manually delete alias\' using a combination of path_load() and path_delete().';
        $msg_var['%pages'] = implode(', ', $pages);
        tripal_report_error('trpentity', TRIPAL_ERROR, $msg, $msg_var);
      }
    }
  }

  /**
   * Saves a new entity.
   *
   * @param $entity
   *   A TripalEntity object to save.
   * @param $cache
   *   This array is used to store objects you want to cache for performance
   *   reasons, as well as, cache related options. The following are supported:
   *   - boolean $clear_cached_fields
   *       Clearing cached fields is NECESSARY. IF you choose to set this to
   *   false then YOU must clear the cache yourself using
   *   cache_clear_all('field:TripalEntity:[entity_id]', 'cache_field', TRUE).
   *   The only known reason to set this to FALSE is to clear the cache in bulk
   *   for perfomance reasons.
   *   - TripalBundle $bundle
   *       The bundle for the current entity.
   *   - TripalTerm $term
   *       The term for the current entity.
   *
   * @return
   *   The saved entity object with updated properties.
   */
  public function save($entity, $cache = []) {
    global $user;
    $pkeys = [];

    if (!isset($cache['clear_cached_fields'])) {
      $cache['clear_cached_fields'] = TRUE;
    }

    $changed_date = time();
    $create_date = $changed_date;
    if (property_exists($entity, 'created')) {
      if (!is_numeric($entity->created)) {
        $temp = new DateTime($entity->created);
        $create_date = $temp->getTimestamp();
      }
    }

    $status = 1;
    if (property_exists($entity, 'status')) {
      if ($entity->status === 0 or $entity->status === 1) {
        $status = $entity->status;
      }
    }

    $transaction = db_transaction();
    try {
      // If our entity has no id, then we need to give it a
      // time of creation.
      if (empty($entity->id)) {
        $entity->created = $create_date;
        $invocation = 'entity_insert';
      }
      else {
        $invocation = 'entity_update';
        $pkeys = ['id'];
      }
      if (property_exists($entity, 'publish') and $entity->publish == TRUE) {
        $invocation = 'entity_publish';
      }

      // Invoke hook_entity_presave().
      module_invoke_all('entity_presave', $entity, $entity->type);

      // Write out the entity record.
      $record = [
        'term_id' => $entity->term_id,
        'type' => $entity->type,
        'bundle' => $entity->bundle,
        'title' => $entity->title,
        'uid' => $entity->uid,
        'created' => $create_date,
        'changed' => $changed_date,
        'status' => $status,
      ];
      if (property_exists($entity, 'nid') and $entity->nid) {
        $record['nid'] = $entity->nid;
      }
      if ($invocation == 'entity_update') {
        $record['id'] = $entity->id;
      }
      $success = drupal_write_record('tripal_entity', $record, $pkeys);
      if ($success == SAVED_NEW) {
        $entity->id = $record['id'];
      }

      // Now we need to either insert or update the fields which are
      // attached to this entity. We use the same primary_keys logic
      // to determine whether to update or insert, and which hook we
      // need to invoke.  We do not attach fields when publishing an entity.
      // This is because a field may have default values and if so, those fields
      // will be attached and the storage backend may then try to insert
      // fields which should not be inserted because they already exist.
      if ($invocation == 'entity_insert') {
        field_attach_insert('TripalEntity', $entity);
      }
      if ($invocation == 'entity_update') {
        field_attach_update('TripalEntity', $entity);
      }

      // Set the title for this entity.
      $this->setTitle($entity, NULL, $cache);

      // Set the path/url alias for this entity.
      $this->setAlias($entity, NULL, $cache);

      // Invoke either hook_entity_update() or hook_entity_insert().
      module_invoke_all('entity_postsave', $entity, $entity->type);
      module_invoke_all($invocation, $entity, $entity->type);

      // Clear any cache entries for this entity so it can be reloaded using
      // the values that were just saved.
      // Also, we don't need to clear cached fields when publishing because we
      // didn't attach any (see above).
      if ($cache['clear_cached_fields'] AND ($invocation != 'entity_publish')) {
        $cid = 'field:TripalEntity:' . $entity->id;
        cache_clear_all($cid, 'cache_field', TRUE);
      }

      return $entity;
    } catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('tripal', $e);
      drupal_set_message("Could not save the TripalEntity: " . $e->getMessage(), "error");
      return FALSE;
    }
  }

  /**
   * Override the load function.
   *
   * A TripalEntity may have a large number of fields attached which may
   * slow down the loading of pages and web services.  Therefore, we only
   * want to attach fields that are needed.
   *
   * @param $ids
   *  The list of entity IDs to load.
   * @param $conditions
   *  The list of key/value filters for querying the entity.
   * @param $field_ids
   *  The list of numeric field IDs for fields that should be attached.
   * @param $cache
   *  When loading of entities they can be cached with Drupal for later
   *  faster loading. However, this can cause memory issues when running
   *  Tripal jobs that load lots of entities.  Caching of entities can
   *  be disabled to improve memory performance by setting this to FALSE.
   */
  public function load($ids = [], $conditions = [], $field_ids = [], $cache = TRUE) {

    $entities = [];

    // Revisions are not statically cached, and require a different query to
    // other conditions, so separate the revision id into its own variable.
    if ($this->revisionKey && isset($conditions[$this->revisionKey])) {
      $revision_id = $conditions[$this->revisionKey];
      unset($conditions[$this->revisionKey]);
    }
    else {
      $revision_id = FALSE;
    }

    // Create a new variable which is either a prepared version of the $ids
    // array for later comparison with the entity cache, or FALSE if no $ids
    // were passed. The $ids array is reduced as items are loaded from cache,
    // and we need to know if it's empty for this reason to avoid querying the
    // database when all requested entities are loaded from cache.
    $passed_ids = !empty($ids) ? array_flip($ids) : FALSE;

    // Try to load entities from the static cache.
    if ($this->cache && !$revision_id) {
      $entities = $this->cacheGet($ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      if ($passed_ids) {
        $ids = array_keys(array_diff_key($passed_ids, $entities));
      }
    }

    // Support the entitycache module if activated.
    if (!empty($this->entityInfo['entity cache']) && !$revision_id && $ids && !$conditions) {
      $cached_entities = EntityCacheControllerHelper::entityCacheGet($this, $ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      $ids = array_diff($ids, array_keys($cached_entities));
      $entities += $cached_entities;

      // Add loaded entities to the static cache if we are not loading a
      // revision.
      if ($this->cache && !empty($cached_entities) && !$revision_id) {
        $this->cacheSet($cached_entities);
      }
    }

    // Load any remaining entities from the database. This is the case if $ids
    // is set to FALSE (so we load all entities), if there are any ids left to
    // load or if loading a revision.
    if (!($this->cacheComplete && $ids === FALSE && !$conditions) && ($ids === FALSE || $ids || $revision_id)) {
      $queried_entities = [];
      foreach ($this->query($ids, $conditions, $revision_id) as $record) {
        // Skip entities already retrieved from cache.
        if (isset($entities[$record->{$this->idKey}])) {
          continue;
        }

        // For DB-based entities take care of serialized columns.
        if (!empty($this->entityInfo['base table'])) {
          $schema = drupal_get_schema($this->entityInfo['base table']);

          foreach ($schema['fields'] as $field => $info) {
            if (!empty($info['serialize']) && isset($record->$field)) {
              $record->$field = unserialize($record->$field);
              // Support automatic merging of 'data' fields into the entity.
              if (!empty($info['merge']) && is_array($record->$field)) {
                foreach ($record->$field as $key => $value) {
                  $record->$key = $value;
                }
                unset($record->$field);
              }
            }
          }
        }

        $queried_entities[$record->{$this->idKey}] = $record;
      }
    }

    // Pass all entities loaded from the database through $this->attachLoad(),
    // which attaches fields (if supported by the entity type) and calls the
    // entity type specific load callback, for example hook_node_load().
    if (!empty($queried_entities)) {
      $this->attachLoad($queried_entities, $revision_id, $field_ids);
      $entities += $queried_entities;
    }

    // Entity cache module support: Add entities to the entity cache if we are
    // not loading a revision.
    if (!empty($this->entityInfo['entity cache']) && !empty($queried_entities) && !$revision_id) {
      EntityCacheControllerHelper::entityCacheSet($this, $queried_entities);
    }

    if ($this->cache and $cache) {
      // Add entities to the cache if we are not loading a revision.
      if (!empty($queried_entities) && !$revision_id) {
        $this->cacheSet($queried_entities);

        // Remember if we have cached all entities now.
        if (!$conditions && $ids === FALSE) {
          $this->cacheComplete = TRUE;
        }
      }
    }
    // Ensure that the returned array is ordered the same as the original
    // $ids array if this was passed in and remove any invalid ids.
    if ($passed_ids && $passed_ids = array_intersect_key($passed_ids, $entities)) {
      foreach ($passed_ids as $id => $value) {
        $passed_ids[$id] = $entities[$id];
      }
      $entities = $passed_ids;
    }
    return $entities;
  }

  /**
   * Override the attachLoad function.
   *
   * A TripalEntity may have a large number of fields attached which may
   * slow down the loading of pages and web services.  Therefore, we only
   * want to attach fields that are needed.
   *
   * @param $queried_entities
   *   The list of queried
   * @param $revision_id
   *   ID of the revision that was loaded, or FALSE if the most current
   *   revision was loaded.
   * @param $field_ids
   */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE,
                                $field_ids = []) {

    // Attach fields.
    if ($this->entityInfo['fieldable']) {
      if ($revision_id) {
        $function = 'field_attach_load_revision';
      }
      else {
        $function = 'field_attach_load';
      }
      foreach ($queried_entities as $id => $entity) {
        $info = entity_get_info($queried_entities[$id]->type);
        $field_cache = array_key_exists('field cache', $info) ? $info['field cache'] : FALSE;
        $bundle_name = $queried_entities[$id]->bundle;
        $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

        // Iterate through the field instances and find those that are set to
        // 'auto_attach' and which are attached to this bundle. Add all
        // fields that don't need auto attach to the field_ids array.
        $instances = field_info_instances('TripalEntity', $bundle_name);
        foreach ($instances as $instance) {
          $field_name = $instance['field_name'];
          $field = field_info_field($field_name);
          $field_id = $field['id'];

          // Add this field to the entity with default value.
          if (!isset($queried_entities[$id]->{$field_name})) {
            $queried_entities[$id]->{$field_name} = [];
          }

          // Options used for the field_attach_load function.
          $options = [];
          $options['field_id'] = $field['id'];

          // The cache ID for the entity.  We must manually set the cache
          // because the field_attach_load won't do it for us.
          $cfid = "field:TripalEntity:$id:$field_name";

          // Check if the field is cached. if so, then don't reload.
          if ($field_cache) {
            $cache_data = cache_get($cfid, 'cache_field');
            if (!empty($cache_data)) {
              $queried_entities[$id]->{$field_name} = $cache_data->data;
              $queried_entities[$id]->{$field_name}['#processed'] = TRUE;
              continue;
            }
          }

          // If a list of field_ids is provided then we specifically want
          // to only load the  fields specified.
          if (count($field_ids) > 0) {
            if (in_array($field_id, $field_ids)) {
              $function($this->entityType, [$id => $queried_entities[$id]],
                FIELD_LOAD_CURRENT, $options);
              // Cache the field.
              if ($field_cache) {
                cache_set($cfid, $queried_entities[$id]->{$field_name}, 'cache_field');
              }
              $queried_entities[$id]->{$field_name}['#processed'] = TRUE;
            }
          }
          // If we don't have a list of fields then load them all, but only
          // if the instance is a TripalField and it is set to not auto
          // attach then we will ignore it. It can only be set by providing
          // the id in the $field_id array handled previously.
          else {

            // Does the field instance have an 'auto_attach' setting?
            $auto_attach = FALSE;
            if (array_key_exists('settings', $instance) and
              array_key_exists('auto_attach', $instance['settings']) and
              $instance['settings']['auto_attach'] == TRUE) {
              $auto_attach = TRUE;
            }

            // Do not load fields that are not auto attached.  Instead set
            // their value to an empty string and set the #processed key to
            // FALSE.
            if (!$auto_attach) {
              // Add an empty value. This will allow the tripal_entity_view()
              // hook to add the necessary prefixes to the field for ajax
              // loading.
              $queried_entities[$id]->{$field_name}['und'][0]['value'] = '';
              $queried_entities[$id]->{$field_name}['#processed'] = FALSE;
            }
            else {
              $function($this->entityType, [$id => $queried_entities[$id]],
                FIELD_LOAD_CURRENT, $options);
              // Cache the field.
              if ($field_cache) {
                if (property_exists($queried_entities[$id], $field_name)) {
                  cache_set($cfid, $queried_entities[$id]->{$field_name}, 'cache_field');
                }
              }
              $queried_entities[$id]->{$field_name}['#processed'] = TRUE;
            }
          }
        }
      }
    }

    // Call hook_entity_load().
    foreach (module_implements('entity_load') as $module) {
      $function = $module . '_entity_load';
      $function($queried_entities, $this->entityType);
    }

    // Call hook_TYPE_load(). The first argument for hook_TYPE_load() are
    // always the queried entities, followed by additional arguments set in
    // $this->hookLoadArguments.
    $args = array_merge([$queried_entities], $this->hookLoadArguments);
    foreach (module_implements($this->entityInfo['load hook']) as $module) {
      call_user_func_array($module . '_' . $this->entityInfo['load hook'], $args);
    }
  }
}

