<?php

/**
 * Purpose: This Handler provides a file upload field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_file_upload extends views_handler_filter {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

  }
  
 /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    // add the file upload form element and set the encoding type for the
    // form to be multipart/form-data
    $form['value'] = array(
      '#type' => 'file',
      '#title' => $this->options['expose']['label'],
    );
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['#method'] = 'POST';
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
    unset($form[$value]['#title']);

    $this->exposed_translate($form[$value], 'value');
  }

 /**
  *
  */
  function query() {
     $file = file_save_upload($this->field,array());
     $field = "$this->table.$this->real_field";

     if($file){
        $fh = fopen($file->filepath,'r');
        $holders = array();
        $values = array();
        while($line = fgets($fh)){
          $line = trim($line);
          if (preg_match('/^[\d\.]+$/',$v)) {
            $holders[] = '%d';
          } else {
            $holders[] = "'%s'";
          }
          // don't include duplicates
          $values[] = $line;
        }
        $where = "$field IN (".implode(", ",$holders).")";
        $this->query->add_where($this->options['group'], $where, $values);
     }
  }
}
