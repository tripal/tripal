<?php
 
/**
 * Purpose: Provide a field that counts the number of records in the current table
 *   are connected to the base table. For example, this field could be used to count
 *   the number of features connected to a given organism -base table=organism, 
 *   current table=feature: for each record in the organism view count feature records where 
 *   feature.organism_id=organism_id of current organism record
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_chado_count extends views_handler_field {

  function init(&$view, $options) {
    parent::init($view, $options);
    
    // the table to query is required
    // check that it was provided in the field definition and if not warn
    if ($this->definition['table_to_query']) {
      $this->aliases['current_table'] = $this->definition['table_to_query'];
    } else {
      drupal_set_message("The field definition ( in hook_views_data() ) needs to specify the 'table_to_query' in order for this fields to work. Field:".$this->field." in the ".$this->table." table definition", 'error');
    }
    
    // set aliases
    $this->aliases['primary_id'] = $this->table . '_id';
    $this->aliases['foreign_key'] = $this->table . '_id';
  }
  
  //Needed to ensure that the name of this field is not added to the query
  function query() {
    $this->add_additional_fields();
  }
  
  function pre_render(&$values) {
    // Render nothing if the current table wasn't set in the field definition
    if(!$this->aliases['current_table']) {
      return '';
    }

    foreach ($values as $key => $record) {
      $primary_id = $record->{$this->aliases['primary_id']};
      
      //Select count from database
      $sql = 'SELECT count(*) as count FROM %s WHERE %s=%d';
      $previous_db = tripal_db_set_active('chado');
      $result = db_fetch_object(db_query(
        $sql, 
        $this->aliases['current_table'], 
        $this->aliases['foreign_key'],
        $primary_id
      ));
      tripal_db_set_active($previous_db);
      
      //Add to view results
      $this->view->result[$key]->{$this->field} = $result->count;
    }
  }
  
  function render($values) {
    return $values->{$this->field};
  }

}