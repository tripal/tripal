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
  public function create(array $values = array()) {

    // Add some items to the values array passed to the constructor
    global $user;
    $values['uid'] = $user->uid;
    $values['created'] = time();
    $values['changed'] = time();
    $values['title'] = '';
    $values['type'] = 'TripalEntity';

    // Call the parent constructor.
    $entity = parent::create($values);

    // Allow modules to make additions to the entity when it's created.
    $modules = module_implements('entity_create');
    foreach ($modules as $module) {
      $function = $module . '_entity_create';
      $function($entity, $values['type']);
    }

    return $entity;

  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for deleteMultiple().
   */
  public function delete($entity) {
    $transaction = db_transaction();
    try {
      // Invoke hook_entity_delete().
      module_invoke_all('entity_delete', $entity, $entity->type);
      field_attach_delete('TripalEntity', $entity);

      db_delete('tripal_entity')
        ->condition('id', $entity->id)
        ->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('tripal_entities', $e);
      throw $e;
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Sets the title for an entity.
   *
   * @param $entity
   * @param $title
   */
  public function setTitle($entity, $title = NULL) {

    // If no title was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$title) {

      // First get the format for the title based on the bundle of the entity.
      $bundle_entity = tripal_load_bundle_entity($entity->bundle);
      $title = tripal_get_title_format($bundle_entity);

      // Determine which tokens were used in the format string
      if (preg_match_all('/\[\w+\]/', $title, $matches)) {
        $used_tokens = $matches[0];

        foreach($used_tokens as $token) {
          $field = str_replace(array('.','[',']'),array('__','',''),$token);

          $value = '';
          if (isset($entity->{$field})) {

            // Render the value from the field.
            // @TODO: Handle the case where thefield is empty... currently returns error.
            $field_value = field_get_items('TripalEntity', $entity, $field);
            $field_render_arr = field_view_value('TripalEntity', $entity, $field, $field_value[0]);
            $value = render($field_render_arr);
          }
          $title = str_replace($token, $value, $title);
        }
      }
    }

    // As long as we were able to determine a title, we should update it ;-).
    if ($title) {
      db_update('tripal_entity')
        ->fields(array(
          'title' => $title
        ))
        ->condition('id', $entity->id)
        ->execute();
    }
  }

  /**
   * Sets the URL alias for an entity.
   */
  public function setAlias($entity, $alias = NULL) {
    $source_url = "bio-data/$entity->id";
    
    // If no alias was supplied then we should try to generate one using the
    // default format set by admins.
    if (!$alias) {
    
      // First get the format for the url alias based on the bundle of the entity.
      $bundle_entity = tripal_bundle_load($entity->bundle);
      $alias = tripal_get_bundle_variable('url_format', $bundle_entity->id);

      // Determine which tokens were used in the format string
      if (preg_match_all('/\[\w+\]/', $alias, $matches)) {
        $used_tokens = $matches[0];

        foreach($used_tokens as $token) {
          $field = str_replace(array('.','[',']'),array('__','',''),$token);

          $value = '';
          if (isset($entity->{$field})) {

            // Render the value from the field.
            // @TODO: Handle the case where thefield is empty... currently returns error.
            $field_value = field_get_items('TripalEntity', $entity, $field);
            $field_render_arr = field_view_value('TripalEntity', $entity, $field, $field_value[0]);
            $value = render($field_render_arr);
          }
          $alias = str_replace($token, trim($value), $alias);
        }
      }
    }

    // Make sure the alias doesn't contain spaces.
    $alias = preg_replace('/\s+/','-',$alias);
    // Or any non alpha numeric characters.
    $alias = preg_replace('/[^a-zA-Z0-9\-\/]/','',$alias);
    $alias = preg_replace('/_/','-',$alias);
    
    if ($alias) {
           
      // Determine if this alias has already been used.
      $num_aliases = db_query('SELECT count(*) as num_alias FROM {url_alias} WHERE alias=:alias',
        array(':alias' => $alias))->fetchField();
      
      // Either there isn't an alias yet so we just create one.
      // OR an Alias already exists but we would like to add a new one.
      if ($num_aliases == 0) {
      
        // First delete any previous alias' for this entity.
        path_delete(array('source' => $source_url));
        
        // Then save the new one.
        $path = array('source' => $source_url, 'alias' => $alias);
        path_save($path);
      }
      // If there is only one alias matching then it might just be that we already
      // assigned this alias to this entity in a previous save.
      elseif ($num_aliases == 1) {
      
        $bundle_entity = tripal_bundle_load($entity->bundle);
        
        // Checking to see if the single alias is for the same entity and if not
        // warning the admin that the alias is already used (ie: not unique?)
        $same_alias = db_query('SELECT count(*) as num_alias FROM {url_alias} WHERE alias=:alias AND source=:source',
          array(':alias' => $alias, ':source' => $source_url))->fetchField();
        if (!$same_alias) {

          $msg = 'The URL alias, %alias, already exists for another page. Please ensure the pattern 
            supplied on the <a href="!link" target="_blank">%type Edit Page</a> under URL Path options is unique.';
          $msg_var = array(
              '%alias' => $alias, 
              '!link' => url("admin/structure/bio-data/manage/$entity->bundle"),
              '%type' => $bundle_entity->label
            );
          tripal_report_error(
            'trpentity',
            TRIPAL_WARNING,
            $msg,
            $msg_var
          );
          drupal_set_message(t($msg, $msg_var), 'warning');
          
        }
      }
      // If there are more then one alias' matching what we generated then there's 
      // a real problem and we need to warn the administrator.
      else {
        $bundle_entity = tripal_bundle_load($entity->bundle);
        
        $aliases = db_query('SELECT source FROM {url_alias} WHERE alias=:alias',
          array(':alias' => $alias))->fetchAll();
        $pages = array();
        foreach($aliases as $a) {
          $pages[] = $a->source;
        }

        $msg = 'The URL alias, %alias, already exists for multiple pages! Please ensure the pattern 
          supplied on the <a href="!link" target="_blank">%type Edit Page</a> under URL Path options is unique.';
        $msg_var = array(
            '%alias' => $alias, 
            '!link' => url("admin/structure/bio-data/manage/$entity->bundle"),
            '%type' => $bundle_entity->label
          );
        drupal_set_message(t($msg, $msg_var), 'error');
        
        $msg .= ' This url alias has already been used for the following pages: %pages. 
          You can manually delete alias\' using a combination of path_load() and path_delete().';
        $msg_var['%pages'] = implode(', ', $pages);
        tripal_report_error(
          'trpentity',
          TRIPAL_ERROR,
          $msg,
          $msg_var
        );
          
      }
    }
  }
  
  /**
   * Saves the custom fields using drupal_write_record().
   */
  public function save($entity) {
    global $user;
    $pkeys = array();

    $transaction  = db_transaction();
    try {
      // If our entity has no id, then we need to give it a
      // time of creation.
      if (empty($entity->id)) {
        $entity->created = time();
        $invocation = 'entity_insert';
      }
      else {
        $invocation = 'entity_update';
        $pkeys = array('id');
      }

      // Invoke hook_entity_presave().
      module_invoke_all('entity_presave', $entity, $entity->type);

      // Write out the entity record.
      $record = array(
        'term_id'   => $entity->term_id,
        'type'      => $entity->type,
        'bundle'    => $entity->bundle,
        'title'     => $entity->title,
        'uid'       => $user->uid,
        'created'   => $entity->created,
        'changed'   => time(),
      );
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
      // need to invoke.
      if ($invocation == 'entity_insert') {
        field_attach_insert('TripalEntity', $entity);
      }
      else {
        field_attach_update('TripalEntity', $entity);
      }
      
      // Set the title for this entity.
      $this->setTitle($entity);
      
      // Set the path/url alias for this entity.
      $this->setAlias($entity);

      // Invoke either hook_entity_update() or hook_entity_insert().
      module_invoke_all('entity_postsave', $entity, $entity->type);
      module_invoke_all($invocation, $entity, $entity->type);

      return $entity;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('tripal_core', $e);
      drupal_set_message("Could not save the entity: " . $e->getMessage(), "error");
      return FALSE;
    }
  }
}

