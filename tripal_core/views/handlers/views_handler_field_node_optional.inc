<?php
// $Id: views_handler_field_node.inc,v 1.7.2.2 2010/03/16 23:09:05 merlinofchaos Exp $
/**
 * @file
 * Contains the basic 'node' field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a node.
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
    parent::query();
    if (!preg_match('/'.$this->options[table].'/', $this->view->base_table)) {
      // chado_* ======================================================
      $drupal_chado_table_name = 'chado_'.$this->options[table];
      
      // add to table_queue--------------------------------------------
      $drupal_chado_table_join['table'] = $drupal_chado_table_name;
      $drupal_chado_table_join['num'] = 1;
      $drupal_chado_table_join['alias'] = $drupal_chado_table_name;
      $drupal_chado_table_join['relationship'] = $this->options[table];

      $drupal_chado_table_join['join'] = clone($this->query->table_queue[$this->options[table]]['join']);
      $drupal_chado_table_join['join']->table = $drupal_chado_table_name;
      $drupal_chado_table_join['join']->left_table = $this->options[table];
      
      $drupal_chado_table_join['join']->definition['table'] = $drupal_chado_table_name;
      $drupal_chado_table_join['join']->definition['left_table'] = $this->options[table];

      $this->query->table_queue[$drupal_chado_table_name] = $drupal_chado_table_join;

      // add to table--------------------------------------------------
      $this->query->tables[$this->view->base_table][$drupal_chado_table_name] = array(
        'count' => 1,
        'alias' => $drupal_chado_table_name,
      );
      
      // node ============================================================
      $drupal_node_table_alias = $this->options[table].'_node';
      
      // add to table_queue--------------------------------------------
      $drupal_node_table_join['table'] = 'node';
      $drupal_node_table_join['num'] = 1;
      $drupal_node_table_join['alias'] = $drupal_node_table_alias;
      $drupal_node_table_join['relationship'] = $this->options[table];

      $drupal_node_table_join['join'] = clone($this->query->table_queue['node']['join']);
      $drupal_node_table_join['join']->left_table = $drupal_chado_table_name;
      $drupal_node_table_join['join']->definition['left_table'] = $drupal_chado_table_name;
      
      $this->query->table_queue[$drupal_node_table_alias] = $drupal_node_table_join;

      // add to table--------------------------------------------------
      $this->query->tables[$this->view->base_table][$drupal_node_table_alias] = array(
        'count' => 1,
        'alias' => $drupal_node_table_alias,
      );
      
      // add field and set alias ===========================
      $field_alias = $this->options[table].'_node_nid';
      $this->query->add_field($drupal_node_table_alias, 'nid');
      $this->add_additional_fields();
      
      $this->aliases['nid'] = $field_alias;     
    } else {
      $this->aliases['nid'] = 'node_nid';
    }
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
