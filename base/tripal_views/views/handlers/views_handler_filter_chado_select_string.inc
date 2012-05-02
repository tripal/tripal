<?php

/**
 * Purpose: This Handler provides a generic select list for any chado field that is a string
 *  The select list includes all distinct values for that field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_chado_select_string extends views_handler_filter_string {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['values_form_type'] = array(
      '#type' => 'radios',
      '#title' => 'Filter Type',
      '#options' => array(
        'textfield' => 'Text Field',
        'select' => 'Drop-Down Box',
      ),
      '#default_value' => ($this->options['values_form_type']) ? $this->options['values_form_type'] : 'select',
    );
    
    $form['multiple'] = array(
      '#type' => 'checkbox',
      '#title' => t('Select Multiple'),
      '#description' => t('Allows more then one option to be selected.'),
      '#default_value' => (isset($this->options['multiple'])) ? $this->options['multiple'] : FALSE,
    );

    $form['optional'] = array(
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('Adds --Any-- to the available options.'),
      '#default_value' => (isset($this->options['optional'])) ? $this->options['optional'] : TRUE,
    );
    
  }
  
 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    if (preg_match('/textfield/', $this->options['values_form_type'])) {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => $this->options['label'],
        '#default_value' => $this->value,
      );    
    } else {
      // Get Options
      if ($this->options['optional']) {
        //$options['<select '.$this->table.'>'] = '--None--';
        $options['All'] = '--Any--';
      }
      $results = tripal_core_chado_select(
        $this->table,
        array($this->field),
        array(),
        array('order_by' => array($this->field => 'ASC'))
      );
      $max_length = 40;
      foreach ($results as $r) {
        if (strlen($r->{$this->field}) > $max_length) {
          $options[$r->{$this->field}] = substr($r->{$this->field},0,$max_length) . '...';
        } else {
          $options[$r->{$this->field}] = $r->{$this->field};    
        }
      }
      
      //Select List
      $form['value'] = array(
          '#type' => 'select',
          '#title' => $this->options['label'],
          '#options' => $options,
          '#default_value' => $this->value,
      );
      
      if ($this->options['multiple']) {
        $form['value']['#multiple'] = TRUE;
      }
    }
  }

 /**
  * Ensures the select list gets rendered when the filter is exposed
  */
  function exposed_form(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

    $value = $this->options['expose']['identifier'];
    $this->value_form($form, $form_state);
    $form[$value] = $form['value'];

    if (isset($form[$value]['#title']) && !empty($form[$value]['#type']) && $form[$value]['#type'] != 'checkbox') {
      unset($form[$value]['#title']);
    }

    $this->exposed_translate($form[$value], 'value');

    if (!empty($form['#type']) && ($form['#type'] == 'checkboxes' || ($form['#type'] == 'select' && !empty($form['#multiple'])))) {
      unset($form[$value]['#default_value']);
    }

    if (!empty($form['#type']) && $form['#type'] == 'select' && empty($form['#multiple'])) {
      $form[$value]['#default_value'] = 'All';
    }

    if ($value != 'value') {
      unset($form['value']);
    }

  }

 /**
  *
  */
  function query() {
  
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";
    $upper = $this->case_transform();

    if ($this->options['multiple']) {
      // Remove any if it's there
      unset($this->value['All']);
      
      if (sizeof($this->value)) {
        $holders = array();
        foreach ($this->value as $v) {
          if (preg_match('/^[\d\.]+$/',$v)) {
            $holders[] = '%d';
          } else {
            $holders[] = "'%s'";
          }
        }
        $where = "$field IN (".implode(", ",$holders).")";
        $this->query->add_where($this->options['group'], $where, $this->value);
      }
    } else {
  
      // Deal with All/Any as value
      if (preg_match('/All/', $this->value)) {
        // Don't do anything    
      } else {
        $info = $this->operators();
        if (!empty($info[$this->operator]['method'])) {
          $this->{$info[$this->operator]['method']}($field, $upper);
        }
      }
    }
  }
}
