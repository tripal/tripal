<?php
/**
 * ChadoDataController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class ChadoDataController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Create a Chado data entity - we first set up the values that are specific
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
      'entity_id' => '',
      'title' => '',
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Overriding the buildContent function to add entity specific fields
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content = parent::buildContent($entity, $view_mode, $langcode, $content);

    return $content;
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
    $entity_ids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {
          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, 'chado_data');
          field_attach_delete('chado_data', $entity);
          $entity_ids[] = $entity->entity_id;
        }
        db_delete('chado_data')
        ->condition('entity_id', $entity_ids, 'IN')
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

    // If our entity has no entity_id, then we need to give it a
    // time of creation.
    if (empty($entity->entity_id)) {
      $entity->created = time();
      $invocation = 'entity_insert';
    }
    else {
      $invocation = 'entity_update';
    }
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if ($invocation == 'entity_insert') {
      field_attach_insert('chado_data', $entity);
    }
    else {
      field_attach_update('chado_data', $entity);
    }

    // Invoke hook_entity_presave().
    module_invoke_all('entity_presave', $entity, 'chado_data');

    // Write out the entity record.
    $tablename = 'feature';
    $type_field = 'type_id';
    $schema = chado_get_schema($tablename);
    $pkey_field = $schema['primary key'][0];
    $record = array(
      'cvterm_id' => $entity->cvterm_id,
      'type'      => $entity->type,
      'tablename' => $tablename,
      'record_id' => $entity->record_id,
      'title'     => 'title',
      'uid'       => $user->uid,
      'created'   => $entity->created,
      'changed'   => time(),
    );
    $success = drupal_write_record('chado_data', $record);
    if ($success == SAVED_NEW) {
      $entity->entity_id = $record['entity_id'];
    }

    // Invoke either hook_entity_update() or hook_entity_insert().
    module_invoke_all($invocation, $entity, 'chado_data');

    return $entity;
  }
}