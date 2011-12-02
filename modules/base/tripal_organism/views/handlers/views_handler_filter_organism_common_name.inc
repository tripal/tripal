<?php

class views_handler_filter_organism_common_name extends views_handler_filter_string {

  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    // Get Options
    $options['<select organism>'] = '<None>';
    if ($this->options['exposed']) {
      $options['All'] = '<Any>';
    }
    $results = tripal_core_chado_select(
      'organism',
      array('common_name','abbreviation'),
      array()
    );
    foreach ($results as $r) {
      $options[$r->common_name] = $r->common_name . ' ('.$r->abbreviation.')';    
    }
    
    //Select List
    $form['value'] = array(
        '#type' => 'select',
        '#title' => $this->options['label'],
        '#options' => $options,
        '#default_value' => $this->value,
    );
    

  }

 /**
  * Render our chunk of the exposed filter form when selecting
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
}