<?php

/**
 * @file
 * Form elements used for tripal views
 */

/**
 * Register form elements
 */
function tripal_core_elements() {

  $type['file_upload_combo'] = array(
    '#input'            => TRUE,
    '#process'          => array('expand_file_upload_combo'),
    '#element_validate' => array('file_upload_combo_validate'),
  );
  
  $type['sequence_combo'] = array(
    '#input' => TRUE,
    '#process' => array('expand_sequence_combo'),
    '#element_validate' => array('sequence_combo_validate'),
  );

  return $type;
}

/**
 * Upload File and keep track of previously uploaded files
 * Form element description
 */
function expand_file_upload_combo($element, $edit, $form_state, $complete_form) {

  // set the default values for each field
  if (empty($element['#value'])) {
    $element['#value'] = array(
      'items' => '',
      'items_file' => '',
      'file_path' => '',
    );
  }

  $element['#tree'] = TRUE;

  // add items text area element
  $parents = $element['#parents'];
  $parents[] = 'items';
  $element['items'] = array(
    '#type' => 'textarea',
    '#default_value' => $element['#value']['items'],
  );
  
  // add file upload element
  $parents = $element['#parents'];
  $parents[] = 'items_file';
  $element['items_file'] = array(
    '#type' => 'file',
    '#title' =>  'File upload',
    '#default_value' => $element['#value']['items_file'],
  );

  // add hidden elelment
  $parents = $element['#parents'];
  $parents[] = 'file_path';
  $element['file_path'] = array(
    '#type' => 'hidden',
    '#default_value' => $element['#value']['file_path'],
  );

  return $element;
}


/**
 * Theme the file upload combo form element
 */
function theme_file_upload_combo($element) {
  return theme('form_element', $element, '<div class="container-inline">' . $element['#children'] . '</div>');
}


/**
 * Validate all content passed into the file upload combo form element
 */
function file_upload_combo_validate($element, &$form) {
  
  $values = array();
  
  // get the items in the textbox
  $items =  $form['values'][$element['#name']]['items'];
  if ($items) {
    // split on new line or comma
    $vals  = preg_split("/[\n,]+/", $items);
    // iterate through the values and trim surrounding space
    foreach ($vals as $i => $value) {
      $values[] = trim($value);
    }
  }

  // merge any items from the file upload
  $file = file_save_upload($element['#name'], array());
  if ($file) {
    $file_path = $file->filepath;
    
    $form['values'][$element['#name']]['file_path'] = $file_path;
    // we need to add our file path to the $_GET element as if it were
    // submitted along with the rest of the form
    $_GET[$element['#name']]['file_path'] = $file_path;

    $fh = fopen($file_path, 'r');
    while ($line = fgets($fh)) {
      $items = trim($line);

      // split on new line or comma
      $vals  = preg_split("/[\n,]+/", $items);
      // iterate through the values and trim surrounding space
      foreach ($vals as $i => $value) {
        $values[] = trim($value);
      }
    }
    fclose($fh);       
  }
  
  // add a new 'items_array' element that contains the array of 
  // submitted items from both the textbox and the input file
  $form['values'][$element['#name']]['items_array'] = $values;
}
/**
 * Form element description
 */
function expand_sequence_combo($element, $edit, $form_state, $complete_form) {
 
  // set the default values for each field
  if (empty($element['#value'])) {
    $element['#value'] = array(
      'upstream' => '',
      'downstream' => '',
    );
  }
  
  $element['#tree'] = TRUE;

  // add the upstream box
  $parents = $element['#parents'];
  $parents[] = 'upstream';
  $element['upstream'] = array(
     '#type' => 'textfield',
     '#title' => t('Get Upstream Bases'),
     '#description' => t('Specify the number of upstream bases to include in the sequnce'),
     '#default_value' => $element['#value']['upstream'],
  );
  // add the downstream box
  $parents = $element['#parents'];
  $parents[] = 'downstream';
  $element['downstream'] = array(
     '#type' => 'textfield',
     '#prefix' => '<br>',
     '#title' => t('Get Downstream Bases'),
     '#description' => t('Specify the number of downstream bases to include in the sequnce'),
     '#default_value' => $element['#value']['downstream'],     
  );
  return $element;
}
/**
 * Validate all content passed into the sequence combo form element
 */
function sequence_combo_validate($element, &$form) {
  $upstream = $form['values'][$element['#name']]['upstream'];
  $downstream = $form['values'][$element['#name']]['downstream'];
  

  if ($upstream < 0) {
    form_set_error($element['#name'], 'Please provide a positive number for upstream bases');
  }
  if ($upstream and !preg_match('/^\d+$/', $upstream)) {
    form_set_error($element['#name'], 'Please provide a decimal number for upstream bases');
  }

  if ($downstream < 0) {
    form_set_error($element['#name'], 'Please provide a positive number for downstream bases');
  }
  if ($downstream and !preg_match('/^\d+$/', $downstream)) {
    form_set_error($element['#name'], 'Please provide a decimal number for downstream bases');
  }
}

/**
 * Theme the file sequence form element
 */
function theme_sequence_combo($element) {
  return theme('form_element', $element, '<div class="container-inline">' . $element['#children'] . '</div>');
}
