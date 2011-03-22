<?php

/**
 * Purpose: This handler provides a TRUE/FALSE or YES/NO select for chado fields
 *  of type boolean (includes both 0/1 and t/f booleans)
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_chado_boolean extends views_handler_filter_boolean_operator {
 /**
  * Checks if this field uses 0/1 or t/f
  */
  function init(&$view, $options) {
    parent::init($view, $options);
    
    $sql = 'SELECT '.$this->field.' FROM '.$this->table.' LIMIT 1';
    $previous_db = tripal_db_set_active('chado');
    $result = db_fetch_object(db_query($sql));
    tripal_db_set_active($previous_db);
    
    $this->boolean_type_tf = FALSE;
    if (preg_match('/[tf]/', $result->{$this->field})) {
      $this->boolean_type_tf = TRUE;
    }

  }

 /**
  * This function sets the options array for the select.
  * If we are using a t/f boolean then the options need to evaluate to either t or f
  * Otherwise, (0/1 boolean) the options evaluate to 0 or 1
  */
  function get_value_options() {

    if ($this->boolean_type_tf) {
      if (isset($this->definition['type'])) {
        if ($this->definition['type'] == 'yes-no') {
          $this->value_options = array('t' => t('Yes'), 'f' => t('No'));
        }
        if ($this->definition['type'] == 'on-off') {
          $this->value_options = array('t' => t('On'), 'f' => t('Off'));
        }
      }

      // Provide a fallback if the above didn't set anything.
      if (!isset($this->value_options)) {
        $this->value_options = array('t' => t('True'), 'f' => t('False'));
      }    
    } else { //end of t/f boolean
      if (isset($this->definition['type'])) {
        if ($this->definition['type'] == 'yes-no') {
          $this->value_options = array(1 => t('Yes'), 0 => t('No'));
        }
        if ($this->definition['type'] == 'on-off') {
          $this->value_options = array(1 => t('On'), 0 => t('Off'));
        }
      }

      // Provide a fallback if the above didn't set anything.
      if (!isset($this->value_options)) {
        $this->value_options = array(1 => t('True'), 0 => t('False'));
      }
    } //end of 0/1 boolean
  }

  function query() {
    $this->ensure_my_table();

    $where = "$this->table_alias.$this->real_field ";

    if ($this->boolean_type_tf) {
      if (preg_match('/f/',$this->value)) {
        $where .= "= 'f'";
      } else {
        $where .= "= 't'";
      }
      $this->query->add_where($this->options['group'], $where);    
    } else {
      if (empty($this->value)) {
        $where .= '= 0';
        if ($this->accept_null) {
          $where = '(' . $where . " OR $this->table_alias.$this->real_field IS NULL)";
        }
      } else {
        $where .= '<> 0';
      }
      $this->query->add_where($this->options['group'], $where);
    }
  }
}