<?php
/**
 * A class the controller will use for instantiating the TripalEntity entity.
 */
class TripalEntity extends Entity {
  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);
    $bundle_id = $this->bundle;
    $bundle = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->condition('bundle', $bundle_id)
      ->execute()
      ->fetchObject();

    $data = unserialize($bundle->data);
    $this->chado_table = $data['data_table'];
    $this->chado_field = $data['field'];

    // If we have an ID then this entity has been saved and it will
    // also therefore have a chado_entity record.  We want to
    // load this record so it is always part of the entity object.
    if (property_exists($this, 'id') and $this->id) {
      $details = db_select('chado_entity', 'ce')
        ->fields('ce')
        ->condition('entity_id', $this->id)
        ->execute()
        ->fetchObject();

      // Add the chado entity details to the entity in case it's needed
      // downstream (e.g. in field widget construction).
      $this->chado_entity_id = $details->chado_entity_id;

      // Add in the record.
      $schema = chado_get_schema($this->chado_table);
      $pkey = $schema['primary key'][0];
      $this->chado_record_id = $details->record_id;
      $this->chado_record = chado_generate_var($this->chado_table, array($pkey =>+ $details->record_id));
    }
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'TripalEntity/' . $this->id);
  }

}