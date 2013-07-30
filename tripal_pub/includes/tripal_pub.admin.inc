<?php

/**
 * Administrative settings form
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync publications
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_pub')) {
    $active_jobs = TRUE;
  }

  // add the field set for syncing publications
  if (!$active_jobs) {
    get_tripal_pub_admin_form_select_search_list($form);
    get_tripal_pub_admin_form_importing_set($form);
    get_tripal_pub_admin_form_default_type($form);
    get_tripal_pub_admin_form_cleanup_set($form);
  }
  else {
    $form['notice'] = array(
     '#type' => 'fieldset',
     '#title' => t('Publication Management Temporarily Unavailable')
    );
    $form['notice']['message'] = array(
        '#value' => t('Currently, publication management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
    );
  }

  return system_settings_form($form);
}

/**
 * 
 * @param $form
 */
function get_tripal_pub_admin_form_default_type(&$form) {

  // get the list of publication types.  In the Tripal publication
  // ontologies these are all grouped under the term 'Publication Type'
  // we want the default to be 'Journal Article'
  $sql = "
    SELECT CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
    WHERE CV.name = 'tripal_pub' and CVTO.name = 'Publication Type' and 
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC 
  ";
  $results = chado_query($sql);
  $pub_types = array();
  while ($pub_type = db_fetch_object($results)) {
    $pub_types[$pub_type->cvterm_id] = $pub_type->name;
    if (strcmp($pub_type->name,"Journal Article") == 0) {
      $d_type_id = $pub_type->cvterm_id;
    }
  }
  
  // override the default by using the stored variable
  $d_type_id = variable_get('tripal_pub_default_type', $d_type_id);
  
  $form['default_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default Publication Type'),
  );
  $form['default_type']['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Publication Type'),
    '#options' => $pub_types,
    '#description' => t('Please set a default publiation type used for manual entry of a new publication.  This is useful in the event that someone is manually adding the same
      publication repetitively'),
    '#default_value' => $d_type_id
  );
}
/**
 *
 *
 * @ingroup tripal_pub
 */
function get_tripal_pub_admin_form_importing_set(&$form) {
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import Settings')
  );
  $form['import']['import_duplicate_check'] = array(
    '#type' => 'radios', 
    '#title' => t('Unique Constraint'),     
    '#options' => array(
      'title_year' => t('Title and Year'), 
      'title_year_media' => t('Title, Year, Media name (e.g. Journal Name, etc.)'), 
      'title_year_type' => t('Title, Year, Media type (e.g. Journal, Conference Proceedings, etc.'),
    ),
    '#description' => t('During import, Tripal will attempt to find duplicate publications. 
       There are several options for how to find a duplicate publication.  Choose the
       option that best suits your needs.'),
    '#default_value' => variable_get('tripal_pub_import_duplicate_check', 'title_year_media'),
  );
}

/**
 *
 *
 * @ingroup tripal_pub
 */
function get_tripal_pub_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
     '#type' => 'item',
     '#value' => t("With Drupal and chado residing in different databases ".
        "it is possible that nodes in Drupal and publications in Chado become ".
        "\"orphaned\".  This can occur if an pub node in Drupal is ".
        "deleted but the corresponding chado pub is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies."),
     '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned publications'),
    '#weight' => 2,
  );
}

/**
 *
 *
 * @ingroup tripal_pub
 */
function get_tripal_pub_admin_form_select_search_list(&$form) {
  $form['searching'] = array(
    '#type' => 'fieldset',
    '#title' => t('Searching Options')
  );
  // get publication properties list
  $properties = array();
  $properties[] = 'Any Field';
  $sql = "
    SELECT DISTINCT CVTS.cvterm_id, CVTS.name, CVTS.definition
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
    WHERE CV.name = 'tripal_pub' and 
      (CVTO.name = 'Publication Details' or CVTS.name = 'Publication Type') and 
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC 
  ";
  $prop_types = chado_query($sql);
  while ($prop = db_fetch_object($prop_types)) {
    $properties[$prop->cvterm_id] = $prop->name;
  }
  $form['searching']['allowed_search_fields'] = array(
     '#type'    => 'checkboxes',
     '#options' => $properties,
     '#description'   => t("Please select the publication details that users can search by in the publication search form.  If none are selected then all fields will be available to the user."),
     '#prefix'  => '<div style="scroll: auto; border:1px solid #CCCCCC;">',
     '#suffix'  => '</div>',
     '#default_value' => variable_get('tripal_pub_allowed_search_fields', array()),
  );
}

/**
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // set the allowed search fields
  $allowed_fields = $form_state['values']['allowed_search_fields'];
  foreach ($allowed_fields as $cvterm_id => $selected) {
    if (!$selected) {
      unset($allowed_fields[$cvterm_id]);
    }
  }
  variable_set('tripal_pub_allowed_search_fields', $allowed_fields);

  $import_duplicate_check = $form_state['values']['import_duplicate_check'];
  variable_set('tripal_pub_import_duplicate_check', $import_duplicate_check);
  
  $default_type = $form_state['values']['type_id'];
  variable_set('tripal_pub_default_type', $default_type);

  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned publications')) {
    tripal_add_job('Cleanup orphaned publications', 'tripal_pub',
       'tripal_pub_cleanup', $job_args, $user->uid);
  }
}

/**
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_pub
 */
function tripal_pub_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('pub', $job_id);

}

/**
 *
 */
function tripal_pub_set_pub_url($node, $pub_id) {

  $node_url = "node/$node->nid";
  $url_alias = "pub/$pub_id";

  // remove any previous alias
  db_query("DELETE FROM {url_alias} WHERE src = '%s'", $node_url);
  // add the new alias
  path_set_alias($node_url, $url_alias);

  return $url_alias;
}