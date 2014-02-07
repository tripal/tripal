<?php
/**
 * @file
 * Form elements used Various places in Tripal
 */

/**
 * Register form elements.
 *
 * @ingroup tripal_core
 */
function tripal_core_element_info() {
  $elements = array();

  $elements['file_upload_combo'] = array(
    '#input' => TRUE,
    '#process' => array('expand_file_upload_combo'),
    '#value_callback' =>'file_upload_combo_value_callback',
    '#theme' => 'theme_file_upload_combo',
    '#theme_wrappers' => array('form_element'),
  );

  $elements['sequence_combo'] = array(
    '#input' => TRUE,
    '#process' => array('expand_sequence_combo'),
    '#value_callback' => 'sequence_combo_value_callback',
    '#theme' => 'theme_sequence_combo',
    '#theme_wrappers' => array('form_element'),
    '#tree' => TRUE,
  );

  return $elements;
}

/**
 * Upload File and keep track of previously uploaded files.
 *
 * @ingroup tripal_core
 */
function expand_file_upload_combo($element, $form_state, $complete_form) {

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
    '#default_value' => (isset($element['#value']['items'])) ? $element['#value']['items'] : '',
  );

  // add file upload element
  $parents = $element['#parents'];
  $parents[] = 'items_file';
  $element['items_file'] = array(
    '#type' => 'file',
    '#title' =>  'File upload',
    '#default_value' => (isset($element['#value']['items_file'])) ? $element['#value']['items_file'] : '',
  );

  // add hidden elelment
  $parents = $element['#parents'];
  $parents[] = 'file_path';
  $element['file_path'] = array(
    '#type' => 'hidden',
    '#default_value' => (isset($element['#value']['file_path'])) ? $element['#value']['file_path'] : '',
  );

  return $element;
}

/**
 * Theme the file upload combo form element.
 *
 * @ingroup tripal_core
 */
function theme_file_upload_combo($variables) {
  $element = $variables['element'];
  $output = '';

  $output .= drupal_render($element['items']);
  $output .= " "; // This space forces our fields to have a little room in between.
  $output .= drupal_render($element['items_file']);
  $output .= " "; // This space forces our fields to have a little room in between.
  $output .= drupal_render($element['file_path']);

  return $output;
}

/**
 * Validate all content passed into the file upload combo form element.
 *
 * @ingroup tripal_core
 */
function file_upload_combo_value_callback($element, $input = FALSE, &$form_state) {
  $values = array();

  if ($input == FALSE) {
    if (!empty($element['#default_value'])) {
      return $element['#default_value'];
    }
    else {
      return;
    }
  }

  // get the items in the textbox
  $items =  $input['items'];
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
    $file_path = $file->uri;

    $input['file_path'] = $file_path;
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
  $input['items_array'] = $values;
  return $input;
}

/**
 * Retrieve Sequence bases form element.
 *
 * @ingroup tripal_core
 */
function expand_sequence_combo($element, $form_state, $complete_form) {

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
 * D7 @todo: test/fix this callback.
 *
 * @ingroup tripal_core
 */
function sequence_combo_value_callback($element, $input = FALSE, &$form_state) {
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
 * Theme the file sequence form element.
 *
 * @ingroup tripal_core
 */
function theme_sequence_combo($variables) {
  $element = $variables['element'];
  $output = '';

  $output .= drupal_render($element['upstream']);
  $output .= " "; // This space forces our fields to have a little room in between.
  $output .= drupal_render($element['downstream']);

  return $output;
}
