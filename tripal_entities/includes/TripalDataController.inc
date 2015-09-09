<?php
/**
 * TripalDataController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class TripalDataController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Create a Tripal data entity - we first set up the values that are specific
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
    // Add values that are specific to our entity
    $values += array(
      'id' => '',
      'bundle' => '',
      'title' => '',
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for deleteMultiple().
   */
  public function delete($entity) {
    $this->deleteMultiple(array($entity));
  }

  /**
   * Delete one or more tripal_entities entities.
   *
   * Deletion is unfortunately not supported in the base
   * DrupalDefaultEntityController class.
   *
   * @param array $entities
   *   An array of entity IDs or a single numeric ID.
   */
  public function deleteMultiple($entities) {
    $ids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {
          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, $entity->type);
          field_attach_delete($entity->type, $entity);
          $ids[] = $entity->id;
        }
        db_delete('tripal_data')
          ->condition('id', $ids, 'IN')
          ->execute();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('entity_example', $e);
        throw $e;
      }
    }
  }

  /**
   * Saves the custom fields using drupal_write_record().
   */
  public function save($entity) {
    global $user;
    $pkeys = array();

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
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if ($invocation == 'entity_insert') {
      field_attach_insert($entity->entity_type, $entity);
    }
    else {
      field_attach_update($entity->entity_type, $entity);
    }

    // Write out the entity record.
    $tablename = 'feature';
    $schema = chado_get_schema($tablename);
    $pkey_field = $schema['primary key'][0];
    $record = array(
      'cvterm_id' => $entity->cvterm_id,
      'type'      => $entity->entity_type,
      'bundle'    => $entity->bundle,
      'tablename' => $tablename,
      'record_id' => $entity->record_id,
      'title'     => 'title',
      'uid'       => $user->uid,
      'created'   => $entity->created,
      'changed'   => time(),
    );
    if ($invocation == 'entity_update') {
      $record['id'] = $entity->id;
    }
    $success = drupal_write_record('tripal_data', $record, $pkeys);
    if ($success == SAVED_NEW) {
      $entity->id = $record['id'];
    }

    return $entity;
  }
}