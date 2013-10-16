<?php
/**
 * @file
 * @todo Add file header description
 */

/**
 * Purpose: Provide administration options for chado_stocks
 *
 * @return
 *   Form array (as described by the drupal form api)
 *
 * @ingroup tripal_stock
 */
function tripal_stock_admin() {
  $form = array();

  // before proceeding check to see if we have any
  // currently processing jobs.
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_stock')) {
    $active_jobs = TRUE;
  }
  if ($active_jobs) {
    $form['notice'] = array(
       '#type' => 'fieldset',
       '#title' => t('Stock Management Temporarily Unavailable'),
       '#collapsible' => FALSE,
       '#collapsed' => FALSE,
    );
    $form['notice']['message'] = array(
       '#value' => t("Currently, jobs are waiting or ".
          "are running. Managemment features have been hidden until these ".
          "jobs complete.  Please check back later once these jobs have ".
          "finished.  You can view the status of pending jobs in the Tripal ".
          "jobs page."),
    );
    return system_settings_form($form);
  }
  
  get_tripal_stock_admin_form_title_set($form);
  get_tripal_stock_admin_form_url_set($form);
  get_tripal_stock_admin_form_vocabulary_set($form);       
  get_tripal_stock_admin_form_cleanup_set($form);
  

  return system_settings_form($form);

}

/**
 * Implements hook_form_validate(): Validates user input
 *
 * @param $form
 *   An array describing the form that was rendered
 * @param $form_state
 *   An array describing the current state of the form including user input
 *
 * @ingroup tripal_stock
 */
function tripal_stock_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();
  
  switch ($form_state['values']['op']) {
    case  t('Set Controlled Vacabularies') :      
      variable_set('chado_stock_types_cv', $form_state['values']['stock_types_cv']);
      variable_set('chado_stock_prop_types_cv', $form_state['values']['stock_prop_types_cv']);
      variable_set('chado_stock_relationship_cv', $form_state['values']['stock_relationship_cv']);
      break;
      
    case t('Clean up orphaned stocks') :
      tripal_add_job('Cleanup orphaned stocks', 'tripal_stock',
         'tripal_stock_cleanup', $job_args, $user->uid);
      break;
    
    case t('Set Stock URLs') :
      variable_set('chado_stock_url', $form_state['values']['stock_url']);
      tripal_add_job('Set Stock URLs', 'tripal_stock',
        'tripal_stock_set_urls', $job_args, $user->uid);
      break;
  }
}

/**
 * 
 * @param $form
 */
function get_tripal_stock_admin_form_title_set(&$form) {

  $form['title'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock Page Titles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['title']['desc'] = array(
    '#type'  => 'markup',
    '#value' => t('Each synced stock must have a unique page title, however, stocks
      may have the same name if they are of different types or from different 
      organisms.  Therefore, we must be sure that the page titles can uniquely 
      identify the stock being viewed.  Select an option below that will 
      uniquely identify all stocks on your site.'),
  );
  $options = array(
    'stock_unique_name'  => 'Only stock unique name',
    'stock_name'         => 'Only stock name',
    'unique_constraint'  => 'Includes stock name, uniquename, type and species',
  );
  $form['title']['chado_stock_title'] = array(
    '#title'         => t('Stock Page Titles'),
    '#type'          => 'radios',
    '#description'   => t('Choose a title type  from the list above that is 
      guaranteed to be unique for all stocks If in doubt it is safest to choose 
      the last option as that guarantees uniqueness. Click the 
      \'Save Configuration\' button at the bottom to save your selection.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_stock_title', 'unique_constraint'),
  );
}
/**
 * 
 * @param  $form
 */
function get_tripal_stock_admin_form_url_set(&$form) {

  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock URL Path'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['url']['desc'] = array(
    '#type'        => 'markup',
    '#value' => t('Each synced stock will have a unique URL which consists of 
      the site domain followed by a unique identifer: for example 
      http://my-tripal-site.org/SID1034, where the element just after the final 
      slash is the unique identifier for the stock.'),
  );
  $options = array(
    'internal ID'               => 'Internal ID (Uses the Chado stock_id. Please set the ID Prefix below)',
    'stock_unique_name'         => 'Stock unique name',
    'stock_name'                => 'Stock name',
    'genus_species_uqname'      => 'Genus + species + unique name (e.g. http://your.site.url/[genus]/[genus]_[species]/[unique name]',
    'genus_species_name'        => 'Genus + species + name (e.g. http://your.site.url/[genus]/[genus]_[species]/[name]',
    'genus_species_type_uname'  => 'Genus + species + type + unique name (e.g. http://your.site.url/[genus]/[genus]_[species]/[type]/[unique name]',
  );
  
  $form['url']['chado_stock_url'] = array(
    '#title'         => t('Unique Identifier'),
    '#type'          => 'radios',
    '#description'   => t('Choose an identifier type from the list above that is 
      guaranteed to be unique for all stocks. If in doubt it is safest to choose the 
      internal ID. Click the \'Save Configuration\' button at the bottom to save 
      your selection Click the \'Set Stock URLs\' button to submit a job to reset 
      the URLs for all synced stocks.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_stock_url', 'internal ID'),
  );

  $form['url']['chado_stock_accession_prefix'] = array(
    '#title'       => t('ID Prefix'),
    '#type'        => t('textfield'),
    '#description' => t("If you choose an Internal ID above you must also enter 
      an ID prefix. This prefix will be prepended to the internal ID number 
      (e.g. ID38294). if you chose to use the stock name or unique name then 
      this prfix is not used"),
    '#required'    => TRUE,
    '#default_value' => variable_get('chado_stock_accession_prefix', 'SID'),
  );

  $form['url']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Stock URLs'),
  );
}
/**
 * 
 * @param $form
 */
function get_tripal_stock_admin_form_vocabulary_set(&$form) {

  $form['set_cv'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set Stock Controlled Vocabularies'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['set_cv']['message'] = array(
       '#value' => t("This setting allows you to set which chado controlled vocabularies (cv)"
              ." are used. Cvs are used to control user input for the type of stock,"
        ." any properties they enter for a stock & the types of relationships"
        ." between stocks. Only cvs already loaded into chado can be selected here.")
  );

  // get the list of CVs for the next form element
  $sql = "SELECT * FROM {cv} ORDER BY name";
  $results = chado_query($sql);
  $cv_options = array();
  while ($r = db_fetch_object($results)) {
    $cv_options[$r->cv_id] = $r->name;
  }

  $form['set_cv']['stock_types_cv'] = array(
   '#type' => 'select',
   '#title' => t('Controlled Vocabulary governing Stock Types'),
   '#options' => $cv_options,
   '#default_value' => variable_get('chado_stock_types_cv', 0)
  );

  $form['set_cv']['stock_prop_types_cv'] = array(
   '#type' => 'select',
   '#title' => t('Controlled Vocabulary governing Types of Stock Properties'),
   '#description' => t("This cv must contain a cvterm entry where name='synonym'."),
   '#options' => $cv_options,
   '#default_value' => variable_get('chado_stock_prop_types_cv', 0)
  );

  $form['set_cv']['stock_relationship_cv'] = array(
   '#type' => 'select',
   '#title' => t('Controlled Vocabulary governing Types of Relationsips between Stocks'),
   '#options' => $cv_options,
   '#default_value' => variable_get('chado_stock_relationship_cv', 0)
  );

  $form['set_cv']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Controlled Vacabularies')
  );
}
/**
 *
 *
 * @ingroup tripal_stock
 */
function get_tripal_stock_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['cleanup']['description'] = array(
     '#type' => 'item',
     '#value' => t("With Drupal and Chado residing in different databases ".
        "it is possible that nodes in Drupal and stocks in Chado become ".
        "\"orphaned\".  This can occur if an stock node in Drupal is ".
        "deleted but the corresponding chado stock is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies."),
     '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned stocks'),
    '#weight' => 2,
  );
}
/**
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_stock
 */
function tripal_stock_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('stock', $job_id);

}