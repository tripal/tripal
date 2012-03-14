<?php

class chado_views_handler_field extends views_handler_field {

  function init (&$view, $options) {
    include_once('chado_wrapper_functions.inc');
    parent::init($view,$options);
  }
  
  /**
   * Defines the defaults for the options form
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['type'] = array('default' => 'separator');
    $options['separator'] = array('default' => ', ');

    return $options;
  }
  
  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['type'] = array(
      '#type' => 'radios',
      '#title' => t('Display type'),
      '#options' => array(
        'ul' => t('Unordered list'),
        'ol' => t('Ordered list'),
        'separator' => t('Simple separator'),
      ),
      '#default_value' => $this->options['type'],
    );

    $form['separator'] = array(
      '#type' => 'textfield',
      '#title' => t('Separator'),
      '#default_value' => $this->options['separator'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('radio:options[type]' => array('separator')),
    );
  }
  
  /**
   * Determines whether the current field is aggregated or not
   * Note: The parent::query() takes care of adding the field to the query, etc.
   */
  function query () {
    parent::query();
    
    $table = $this->query->get_table_info($this->table);
    if (preg_match('/aggregator/',$table['join']->definition['handler'])) {
      $this->aggregated = TRUE;
    } else {
      $this->aggregated = FALSE;
    }
  }
  
  /**
   * Splits the aggregated values up for use in rendering
   */
  function pre_render (&$values) {
    
    // further check the results to see if this field is a postgresql array
    if (!$this->aggregated) {
      if (preg_match('/^{.*}$/',$values[0]->{$this->field_alias})) {
        $this->aggregated = TRUE;
      }
    }
    
    // Split Aggregated Results
    if ($this->aggregated) {
      foreach($values as $k => $v) {
        $values[$k]->{$this->field_alias} = chado_wrapper_split_array_agg_results($this, $v->{$this->field_alias});
      }
    }
    
  }
  
  /**
   * Render the field.
   *
   * Note: Checks to see if we have an array or simple field. If we have an array, then
   *   split it up and render each part using the parent render functionality.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {

    // If it's aggregated (an array), then render each part 
    // using the parent render functionality
    if ($this->aggregated) {
      $items = array();
      
      $parts = $values->{$this->field_alias};
      foreach ($parts as $p) {
        $v[ $this->field_alias ] = $p;
        $val = (object) $v;
        $items[] = parent::render($val);
        unset($v, $val);
      }
      
      if ($this->options['type'] == 'separator') {
        return implode(check_plain($this->options['separator']), $items);
      }
      else {
        return theme('item_list', $items, NULL, $this->options['type']);
      }
    
    // Otherwise it is not aggragated
    // Just render like the default handler would
    } else {
      return parent::render($values);
    }
    
  }
  
}