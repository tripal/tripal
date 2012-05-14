<?php

/**
 * Register form elements
 */
function tripal_views_elements() {

   $type['file_upload_combo'] = array(
      '#input' => TRUE,
      '#process' => array('expand_file_upload_combo'),
      '#element_validate' => array('file_upload_combo_validate'),
   );
   return $type;
}
/**
 * 
 */
function expand_file_upload_combo ($element, $edit, $form_state, $complete_form) {

	if (empty($element['#value'])) {
    	$element['#value'] = array(
         'items' => '',
         'items_file' => '',
         'file_path' => '',
    	);
  	}
   //dpm($form_state);

	$element['#tree'] = TRUE;

	$parents = $element['#parents'];
   $parents[] = 'items';
   $element['items'] = array(
      '#type' => 'textarea',
		'#default_value' => $element['#value']['items'],
   ); 
	$parents = $element['#parents'];
   $parents[] = 'items_file';
   $element['items_file'] = array(
      '#type' => 'file',
      '#title' =>  'File upload',
		'#default_value' => $element['#value']['items_file'],
   ); 

	$parents = $element['#parents'];
   $parents[] = 'file_path';
   $element['file_path'] = array(
      '#type' => 'hidden',
		'#default_value' => $element['#value']['file_path'],
   );
   return $element;
}

/**
 * 
 */
function theme_file_upload_combo($element) {
  	return theme('form_element', $element, '<div class="container-inline">'. $element['#children'] .'</div>');
}

/**
 * 
 */
function file_upload_combo_validate($element,&$form) {
    $file = file_save_upload($element['#name'],array());
    if($file) {
       $form['values'][$element['#name']]['file_path'] = $file->filepath;
       // we need to add our file path to the $_GET element as if it were
       // submitted along with the rest of the form 
       $_GET[$element['#name']]['file_path'] = $file->filepath;
    }
}
