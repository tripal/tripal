<?php

/**
 * Purpose: This Handler provides a file upload field.
 *
 * @ingroup views_filter_handlers
 * @ingroup tripal_core
 */
class views_handler_filter_file_upload extends views_handler_filter {

  function init(&$view,$options){
     parent::init($view,$options);

  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

  }
  
  /**
  * Defines the value field in both the views filter options form
  *   and the exposed form
  */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    // we'll provide a single text area for this field.
    // in the exposed_form function we'll add in the file upload button
    $form['value'] = array(
       '#type' => 'textarea',
       '#title' => $this->options['expose']['label'],
       '#default_value' => $this->value,
       '#multiple' => FALSE,
       '#description' => t('Provide search values for ' . $this->options['expose']['label'] . 
         '. Please place each search item on a separate line or separated by commas.'),
    ); 
  }

  /** 
   *  Validates the input form
   */
  function exposed_validate(&$form, &$form_state) {
    //dpm($form_state);

  }
  /**
  * Ensures the select list gets rendered when the filter is exposed
  */
  function exposed_form(&$form, &$form_state) {

    if (empty($this->options['exposed'])) {
      return;
    }

    // rebuild the form elements 
    $value = $this->options['expose']['identifier'];
    $this->value_form($form, $form_state);
    $form[$value] = $form['value'];
    unset($form[$value]['#title']);
    $form[$this->options['field'] . '_upload'] = array(
      '#type' => 'file',
      '#title' => '',
      '#description' => t('Upload a file to provide search values for ' . $this->options['expose']['label'] . 
         '. Please place each search item on a separate line.'),
    );
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['#method'] = 'POST';
    $this->exposed_translate($form[$value], 'value');
  }

 /**
  *
  */
  function query() {
     $field = "$this->table.$this->real_field";
     $holders = array();
     $values = array();

     // get the file upload info if one has been provided
     $file = file_save_upload($this->field.'_upload',array());
     if($file){
        $fh = fopen($file->filepath,'r');

        while($line = fgets($fh)){
          $line = trim($line);
          $values[] = $line;
        }
     } 
     // if a file upload has not been provided then use the value in the textarea
     else {
        $items = $this->value[0];
        $items = preg_replace("/\s+,/",",",$items);
        $items = preg_replace("/\s+\n/","\n",$items);
        $items = preg_replace("/,\n/","\n",$items);
        $values = preg_split("/[\n,]+/",$items);

     }
     for($i = 0 ; $i < count($values); $i++){
        $values[$i] = trim($values[$i]);
        if (preg_match('/^[\d\.]+$/',$values[$i])) {
           $holders[] = '%d';
        } else {
           $holders[] = "'%s'";
        }
     }
     // if we have any values supplied then update the where clause for
     // the views query
     if(count($holders) > 0){
        $where = "$field IN (".implode(", ",$holders).")";
        $this->query->add_where($this->options['group'], $where, $values);
     }
  }
}
