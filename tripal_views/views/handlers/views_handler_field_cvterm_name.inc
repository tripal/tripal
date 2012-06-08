<?php 
// $Id$

/**
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_cvterm_name extends views_handler_field {

  /**
   * Add join to the cvterm table and cvterm.name field to the query
   */
  function query() {
      
    // if this table isn't a base table
    // if it is this can be done by describing a join between base table and cvterm
    if (!preg_match('/^'.$this->view->base_table.'$/', $this->table)) {

      //Want to add join: LEFT JOIN cvterm nd_experimentprop_cvterm ON nd_experimentprop.type_id = nd_experimentprop_cvterm.cvterm_id
      //===============================================================      

      $this->query->ensure_table($this->table);
      
      // add to table_queue--------------------------------------------
      $cvterm_join['table'] = 'cvterm';
      $cvterm_join['num'] = 1;
      $cvterm_join['alias'] = $this->table . '_cvterm';
      $cvterm_join['relationship'] = $this->table;

      $cvterm_join['join'] = clone($this->query->table_queue[$this->table]['join']);
      $cvterm_join['join']->table = 'cvterm';
      $cvterm_join['join']->field = 'cvterm_id';
      $cvterm_join['join']->left_table = $this->table;
      $cvterm_join['join']->left_field = 'type_id';
      
      $cvterm_join['join']->definition['table'] = 'cvterm';
      $cvterm_join['join']->definition['field'] = 'cvterm_id';
      $cvterm_join['join']->definition['left_table'] = $this->table;
      $cvterm_join['join']->definition['left_field'] = 'type_id';

      $this->query->table_queue[$cvterm_join['alias']] = $cvterm_join;

      // add to table--------------------------------------------------
      $this->query->tables[$this->view->base_table][$this->table.'_cvterm'] = array(
        'count' => 1,
        'alias' => $this->table.'_cvterm',
      );    

      //Want to add field: nd_experimentprop_cvterm.name as nd_experimentprop_type
      //===============================================================
      $field_alias = $this->table.'_cvterm_name';
      $this->query->add_field($this->table.'_cvterm', 'name');
      $this->add_additional_fields();
      $this->aliases['name'] = $field_alias;  
      
    }
  }
  
  /**
   * Ensure that the type/cvterm name is rendered
   */
 function render($values) { 
   return $values->{$this->aliases['name']}; 
 }
  
}