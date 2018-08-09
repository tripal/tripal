<?php
/**
 * A class the controller will use for instantiating the TripalEntity entity.
 */
class TripalEntity extends Entity {
  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'TripalEntity/' . $this->id);
  }

  /**
   * Permanently saves the entity.
   *
   * @param DatabaseTransaction $transaction
   *    (OPTIONAL) Allows you to pass in your current transaction to save nested transaction overhead.
   * @param boolean $clear_cached_fields
   *    (OPTIONAL) Clearing cached fields is NECESSARY. IF you choose to set this to false then YOU
   *    must clear the cache yourself using cache_clear_all('field:TripalEntity:[entity_id]', 'cache_field', TRUE).
   *    The only known reason to set this to FALSE is to clear the cache in bulk for perfomance reasons.
   *
   * @see entity_save()
   */
  public function save(DatabaseTransaction $transaction = NULL, $clear_cached_fields = TRUE) {
    return entity_get_controller($this->entityType)->save($this, $transaction, $clear_cached_fields);
  }

}
