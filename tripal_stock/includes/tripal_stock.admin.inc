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

  variable_set('chado_stock_types_cv', $form_state['values']['stock_types_cv']);
  variable_set('chado_stock_prop_types_cv', $form_state['values']['stock_prop_types_cv']);
  variable_set('chado_stock_relationship_cv', $form_state['values']['stock_relationship_cv']);  
  variable_set('chado_stock_url_string', $form_state['values']['chado_stock_url_string']);
  
  switch ($form_state['values']['op']) {
    case  t('Set Controlled Vacabularies') :      
      break;
      
    case t('Clean up orphaned stocks') :
      tripal_add_job('Cleanup orphaned stocks', 'tripal_stock',
         'tripal_stock_cleanup', $job_args, $user->uid);
      break;
    
    case t('Set Stock URLs') :
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

  $options = array(
    'SID[id]'      => '[id]:' . t('The Chado stock_id'),
    'stock'        => 'stock:' . t('Chado table name'),
    '[genus]'      => '[genus]:' . t('Genus to which the stock belongs'),
    '[species]'    => '[species]:' . t('Species to which the stock belongs'),
    '[type]'       => '[type]:' . t('The type of stock'),
    '[uniquename]' => '[uniquename]:' . t('The stock unique name'),
    '[name]'       => '[name]:' . t('The stock name'),
    'reset'        => t('Reset'),
  );
  

  $form['url']['chado_stock_url_string'] = array(
    '#title' => 'URL Syntax',
    '#type' => 'textfield',
    '#description' => t('You may rearrange elements in this text box to
      customize the URLs.  The available tags include: [id],
      [uniquename]. [name], [species], [genus], [type]. You can separate or
      include any text between the tags. Click the "Set Stock URLs" button to 
      reset the URLs for all stock pages.  Click the "Save Configuration" button to
      simply save this setup. <b>Important</b>: be sure that whatever you choose will always be unique even considering
      future data that may be added.  If you include the Chado table name, genus, species, type 
      and uniquename you are guaranteed to have a unique URL. For example stock/[genus]/[species]/[type]/[uniquename]'),
    '#size' => 150,
    '#default_value' => variable_get('chado_stock_url_string', '/stock/[genus]/[species]/[type]/[uniquename]'), 
  );
  
  $form['url']['chado_stock_url'] = array(
    '#title'         => t('URL components'),
    '#type'          => 'checkboxes',
    '#required'      => FALSE,
    '#options'       => $options,
    '#description'   => t('Click the item above to make it appear in the URL Syntax box'),
    '#attributes'    => array(
      'onclick' => '
        box = $(\'#edit-chado-stock-url-string\');
        if (this.value == \'reset\') {
          box.val(\'\');
        }
        else {        
          box.val(box.val() + "/" + this.value);          
        }
        this.checked = false;
      ',
    ),
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