<?php
/**
 * A class the controller will use for instantiating the TripalEntity entity.
 */
class TripalEntity extends Entity {
  public function __construct($values = array(), $entity_type) {
    parent::__construct($values, $entity_type);

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
      $this->chado_table = $details->data_table;
      $this->chado_field = $details->field;

      // Add in the record.
      $schema = chado_get_schema($this->chado_table);
      $pkey = $schema['primary key'][0];
      $this->chado_record_id = $details->record_id;
      $this->chado_record = chado_generate_var($this->chado_table, array($pkey =>+ $details->record_id));
    }
    // If we do not have an ID then we need to do a few queries to get
    // information about the chado table this entity maps to.
    else {
      // Use the cvterm_id to look up the base table for this term.  We find
      // the base table by looking in the term_usage table for the mapping.
      $sel_values = array(
        'term_id' => array(
          'cvterm_id' => $this->cvterm_id,
        ),
      );
      $term_usage = chado_generate_var('tripal_term_usage', $sel_values, array('return_array' => 1));

      // For each table that uses this term, insert the field recursively
      foreach ($term_usage as $usage) {
        $this->chado_table = $usage->data_table;
        $this->chado_field = $usage->field;
      }
    }
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'BioData/' . $this->id);
  }

}