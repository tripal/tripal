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

  // Element for uploading large files.  This form element
  // accepts the following keys when using in a form:
  //   - #title:  The title that will appear above the element.
  //   - #description:  The description that will appear below the element.
  //   - #usage_type:  Required.  The type of file.  This will be stored in
  //     the 'type' column of the file_usage table.
  //   - #usage_id: Required. A unique numeric ID representing an entity, node
  //     or some other record identifier.  This can be any identifier that
  //     makes sense to the module that implements a form that uses this
  //     element.
  //   -#allowed_types:  An array of file extensions that are allowed for
  //     to be uploaded.
  //   -#cardinality:  A numeric value indicating the number of files the
  //     user can upload. Set to 0 for unlimited.
  $elements['html5_file'] = array(
    '#input' => 'TRUE',
    '#process' => array('tripal_html5_file_process'),
    '#element_validate' => array('tripal_html5_file_validate'),
    '#value_callback' => 'tripal_html5_file_value',
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
     '#description' => t('Specify the number of upstream bases to include in the sequence'),
     '#default_value' => $element['#value']['upstream'],
  );
  // add the downstream box
  $parents = $element['#parents'];
  $parents[] = 'downstream';
  $element['downstream'] = array(
     '#type' => 'textfield',
     '#prefix' => '<br>',
     '#title' => t('Get Downstream Bases'),
     '#description' => t('Specify the number of downstream bases to include in the seqeunce'),
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

/**
 *  The process function for the html5_file form element.
 */
function tripal_html5_file_process($element, $form_state, $complete_form) {

  $module = array_key_exists('#usage_module', $element) ? $element['#usage_module'] : 'tripal_core';
  $type = $element['#usage_id'] . '-' . $element['#usage_type'] . '-' . $module;
  $name = $element['#name'];
  $type_var_name = 'uploader_' . $element['#usage_id'] . '_' . $element['#usage_type'] . '_' . $module;
  $name = preg_replace('/[^\w]/', '_', $name);
  $allowed_types = array_key_exists('#allowed_types', $element) ? $element['#allowed_types'] : array();
  $cardinality = array_key_exists('#cardinality', $element) ? $element['#cardinality'] : 1;

  $headers = array(
    array('data' => 'File'),
    array('data' => 'Size', 'width' => '10%'),
    array('data' => 'Upload Progress', 'width' => '20%'),
    array('data' => 'Action', 'width' => '10%')
  );
  $rows = array();
  $table_vars = array(
    'header'      => $headers,
    'rows'        => $rows,
    'attributes'  => array(
      'class' => array('tripal-html5-file-upload-table'),
      'id' => 'tripal-html5-file-upload-table-' . $type
    ),
    'sticky'      => TRUE,
    'colgroups'   => array(),
    'empty'       => t('There are currently no files.'),
  );
  $element['html5_file_table_key'] = array(
    '#type' => 'hidden',
    '#value' => $type,
    '#attributes' => array(
      'class' => array('tripal-html5-file-upload-table-key')
    )
  );
  $element['html5_file_table'] = array(
    '#type' => 'item',
    '#title' => $element['#title'] ? $element['#title'] : 'File Upload',
    '#description' => $element['#description'],
    '#markup' => theme('table', $table_vars)
  );

  $element[$name] = array(
    '#type' => 'hidden',
    '#attributes' => array('id' => 'tripal-html5-upload-fid-' . $type),
    '#default_value' => $element['#value'],
  );
  $element['html5_file_submit'] = array(
    '#type'     => 'submit',
    '#value'    => 'Upload File',
    '#name' => 'tripal_html5_file_upload_submit-' . $type,
    // We don't want this button to submit as the file upload
    // is handled by the JavaScript code.
    '#attributes' => array(
      'id' => 'tripal-html5-file-upload-submit-' . $type,
      'onclick' => 'return (false);'
    )
  );

  $uploader_settings = array(
    'table_id' => '#tripal-html5-file-upload-table-' . $type,
    'submit_id' => '#tripal-html5-file-upload-submit-' . $type,
    'category' => array($element['#usage_id'] . '-' . $element['#usage_type']),
    'cardinality' => $cardinality,
    'target_id' => 'tripal-html5-upload-fid-' . $type,
    'module' => $module,
    'allowed_types' => $allowed_types,
  );

  drupal_add_js(array($type_var_name => $uploader_settings), 'setting');
  drupal_add_js(drupal_get_path ('module', 'tripal_core') . '/theme/js/TripalUploader.js');
  drupal_add_js(drupal_get_path ('module', 'tripal_core') . '/theme/js/TripalUploadFile.js');
  drupal_add_js(drupal_get_path ('module', 'tripal_core') . '/theme/js/tripal.file.js');

  return $element;
}
/**
 *
 */
function tripal_html5_file_validate($element, &$form_state) {
  $is_required = $element['#required'];
  $name = $element['#name'];
  $name = preg_replace('/[^\w]/', '_', $name);
  if (is_array($element['#value']) and $array_key_exists($name, $element['#value'])) {
    $fid = $element['#value'][$name];
  }

  // TODO: the fid should just be the $element['#value'] why isn't this
  // working given the tripal_html5_file_value function below.

  if ($is_required and !$fid) {
   form_error($element, t('A file must be provided.'));
  }
}



/**
 * Implements hook_handle_uplaoded_file().
 */
function tripal_core_handle_uploaded_file($filename, $filepath, $type) {

  global $user;

  // Split the type into a node ID and form_key
  list($id, $form_key) = explode('-', $type);


  // See if this file is already managed then add another entry fin the
  // usage table.
  $fid = db_select('file_managed', 'fm')
    ->fields('fm', array('fid'))
    ->condition('uri', $filepath)
    ->execute()
    ->fetchField();

  // Create a file object.
  if (!$fid) {
    $file = new stdClass();
    $file->uri = $filepath;
    $file->filename = $filename;
    $file->filemime = file_get_mimetype($filepath);
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;
    $file = file_save($file);
    $fid = $file->fid;
  }

  $file = file_load($fid);
  file_usage_add($file, 'tripal', $form_key, $id);
  return $fid;
}

/**
 * Implements hook_value() for the html5_file form element.
 */
function tripal_html5_file_value($element, $input = FALSE, &$form_state) {

  if ($input) {
    if (is_array($input)) {
      $name = $element['#name'];
      $name = preg_replace('/[^\w]/', '_', $name);
      return $input[$name];
    }
    else {
      return $input;
    }
  }
}