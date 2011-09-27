<?php

/**
 * Field handler to provide simple renderer that allows linking to a node.
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_node_optional extends views_handler_field_node {

  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
    if (module_exists('translation')) {
      $this->additional_fields['language'] = array('table' => 'node', 'field' => 'language');
    }
  }
  
  /**
   * Add chado_* and *_node alias'd left joins to the table
   */
  function query() {
    
    // Check what we have (ie: current table? node table? )
    $chado_table = 'chado_' . $this->table;
    foreach ($this->query->table_queue as $table_def) {
      //check is $this->table
      if ($table_def['table'] == $this->table) {
        $this_table_alias = $table_def['alias'];
      }
      
      // check is node joined to #this->table
      if ($table_def['table'] == $chado_table) {
        $node_table_alias = $table_def['alias'];
      }
    }
    
    // First: Add the main field-----------------------------------------------
    if (!$this_table_alias) {
      $this_table_alias = $this->ensure_my_table();
    }
    $field_alias = $this->query->add_field($this_table_alias, $this->real_field); 
    $this->aliases[ $this->real_field ] = $field_alias;
    $this->field_alias = $field_alias;
    
    // Second: Add nid field and Joins if necessary----------------------------
    // Add node join if needed
    if (!$node_table_alias) {
      $def['table'] = $chado_table;
      $def['field'] = $this->table . '_id';
      $def['left_table'] = $this_table_alias;
      $def['left_field'] = $this->table . '_id';
      
      $join = new views_join();
      $join->definition = $def;
      $join->construct();
      $join->adjusted = TRUE;
    
      $node_table_alias = $this->query->add_relationship($def['table'], $join, $def['table']);
    }
    
    // Finally Add Field
    $field_alias = $this->query->add_field($node_table_alias, 'nid'); 
    $this->aliases['nid'] = $field_alias;   

  }

  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_node']) && $data !== NULL && $data !== '') {  
      if (!empty($values->{$this->aliases['nid']})) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = "node/" . $values->{$this->aliases['nid']};
        if (isset($this->aliases['language'])) {
          $languages = language_list();
          if (isset($languages[$values->{$this->aliases['language']}])) {
            $this->options['alter']['language'] = $languages[$values->{$this->aliases['language']}];
          }
          else {
            unset($this->options['alter']['language']);
          }
        }
      } else {
        $this->options['alter']['make_link'] = FALSE;
        $this->options['alter']['path'] = "";        
      }
    }
    return $data;
  }
}
