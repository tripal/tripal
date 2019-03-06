<?php

/**
 * A class the controller will use for instantiating the TripalEntity entity.
 */
class TripalEntity extends Entity {

  public function __construct($values = [], $entity_type) {
    parent::__construct($values, $entity_type);
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return ['path' => 'TripalEntity/' . $this->id];
  }

  /**
   * Permanently saves the entity.
   *
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
   * @see entity_save()
   */
  public function save($cache = []) {
    return entity_get_controller($this->entityType)->save($this, $cache);
  }

}
