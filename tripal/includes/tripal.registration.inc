<?php
/**
 * @file
 * Contains functions related to the display of Tripal registration
 * form in a Tripal website.
 */


/**
 * Provides the page for the Tripal registration page
 *
 */
function tripal_registration_form($form, &$form_state) {
  $form_data = unserialize(variable_get('tripal_site_registration', NULL));

  $form_state['details']['funding'] =
    isset($form_state['details']['funding']) ? $form_state['details']['funding'] : 1;

  $form['description'] = array(
    '#markup' =>  t('Registering your Tripal site is important. When you 
    register your site it gives the Tripal developers information that will 
    allow us to secure continuing funding. Registration also gives us insight
    into usage so we can build features the community needs. </br></br>'),
    );

  $form['disable_tripal_reporting'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable registration reminder'),
    '#default_value' => isset($form_data['values']['disable_tripal_reporting']) ? $form_data['values']['disable_tripal_reporting'] : NULL,
    '#description' => "If you do not want to register your site please check
      this box as it will stop the reminder notifications.",
    '#ajax' => array(
      'callback' => '_tripal_form_disable_reg_callback',
      'event' => 'click',
    ),
  );

  $form['details']['principal_investigator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Principal Investigator Contact Information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['details']['principal_investigator']['principal_investigator_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($form_data['values']['principal_investigator_name']) ? $form_data['values']['principal_investigator_name'] : NULL,
    '#required' => TRUE,
  );

  $form['details']['principal_investigator']['principal_investigator_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#default_value' => isset($form_data['values']['principal_investigator_email']) ? $form_data['values']['principal_investigator_email'] : NULL,
    '#required' => TRUE,
  );
  $form['details']['tripal_reg_site_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Administrator (if different from the principal investigator)'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($form_data['values']['tripal_reg_site_admin_name']) ? $form_data['values']['tripal_reg_site_admin_name'] : NULL,
    '#required' => FALSE,
  );

  $form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#default_value' => isset($form_data['values']['tripal_reg_site_admin_email']) ? $form_data['values']['tripal_reg_site_admin_email'] : NULL,
    '#required' => FALSE,
  );

  $form['details']['tripal_reg_site_description']= array(
    '#type' => 'textarea',
    '#title' => t('Description of the site'),
    '#default_value' => isset($form_data['values']['tripal_reg_site_description']) ? $form_data['values']['tripal_reg_site_description'] : NULL,
    '#required' => FALSE,
  );

  $form['details']['funding'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#prefix' => '<div id="funding">',
    '#suffix' => '</div>',
  );

  $count = count($form_data['values']['funding']);
  $form_state['details']['funding'] = $form_state['details']['funding'] + $count;
  for ($i = 1; $i <= $form_state['details']['funding']; $i++) {
    $form['details']['funding'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t("Funding Period $i"),
      '#tree' => TRUE,
    );

    $form['details']['funding'][$i]['tripal_reg_site_agency'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_agency']) ? $form_data['values']['funding'][$i]['tripal_reg_site_agency'] : NULL,
      '#title' => t('Agency'),
    );

    $form['details']['funding'][$i]['tripal_reg_site_grant'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_grant']) ? $form_data['values']['funding'][$i]['tripal_reg_site_grant'] : NULL,
      '#title' => t('Grant Number'),
    );

    $form['details']['funding'][$i]['funding_period'] = array(
      '#type' => 'fieldset',
      '#title' => t('Funding Period'),
      '#tree' => TRUE,
    );

    $form['details']['funding'][$i]['funding_period']['tripal_reg_site_start'] = array(
      '#type' => 'date_select',
      '#title' => t("Start"),
      '#default_value' => isset($form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_start']) ? $form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_start'] : date('Y', time()),
      '#date_year_range' => '-20:+20',
      '#date_format' => 'Y',
    );

    $form['details']['funding'][$i]['funding_period']['tripal_reg_site_end'] = array(
      '#type' => 'date_select',
      '#title' => t('End'),
      '#default_value' => isset($form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_end']) ? $form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_end'] : date('Y', time()),
      '#date_year_range' => '-20:+20',
      '#date_format' => 'Y',
    );
  }
  
  $form['details']['funding']['add_funding'] = array(
    '#type' => 'button',
    '#value' => t('Add additional funding sources'),
    '#href' => '',
    '#ajax' => array(
      'callback' => 'custom_registration_ajax_add_funding',
      'wrapper' => 'funding',
    ),
  );

  $purpose = array(0 => t('Production'), 1 => t('Development'));
  $form['details']['tripal_reg_site_purpose'] = array(
    '#type' => 'radios',
    '#title' => t('Purpose of site'),
    '#default_value' => isset($form_data['values']['tripal_reg_site_purpose']) ? $form_data['values']['tripal_reg_site_purpose'] : NULL,
    '#options' => $purpose,
    '#required' => TRUE,
  );

  $form['details']['tripal_reg_site_modules'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($form_data['values']['tripal_reg_site_modules']) ? $form_data['values']['tripal_reg_site_modules'] : NULL,
    '#title' => t('Provide Tripal (not other 3rd party modules) module usage information.'),
  );
  $form_state['details']['funding']++;

  // Provide a submit button.
  if (!empty($form_data)) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Update registeration information',
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Register Site',
    );
  }


  return $form;

}

function custom_registration_ajax_add_funding($form, $form_state) {
  return $form['details']['funding'];
}

function _tripal_form_disable_reg_callback($form, &$form_state) {
  variable_set('disable_tripal_reporting', TRUE);
}

/**
 * Implements validation from the Form API.
 * 
 * @param $form
 *   A structured array containing the elements and properties of the form.
 * @param $form_state
 *   An array that stores information about the form's current state 
 *   during processing.
 */
function tripal_registration_form_validate($form, &$form_state){
  $mail_pi = $form_state['values']['principal_investigator_email'];
  $mail_sa = $form_state['values']['tripal_reg_site_admin_email'];
  if (!valid_email_address($mail_pi)) {
    form_set_error('[details][principal_investigator][principal_investigator_email]', t('The email address for the principal investigator appears to be invalid.'));
  }
  if (!valid_email_address($mail_sa) && !empty($mail_sa)) {
    form_set_error('[details][site_admin][tripal_reg_site_admin_email]', t("The email address for the site administrator appears to be invalid."));
  }
}

/**
 * Implements submit from the Form API.
 * 
 * @param $form
 *   A structured array containing the elements and properties of the form.
 * @param $form_state
 *   An array that stores information about the form's current state 
 *   during processing.
 */
function tripal_registration_form_submit($form, &$form_state) {
  //Check for empty funding periods and remove them.
  $j = 1;
  foreach ($form_state['values']['funding'] as $funding_source) {
    if (!empty($funding_source['tripal_reg_site_agency']) && !empty($funding_source['tripal_reg_site_grant'])) {
      $form_state['values']['fundings'][$j] = $funding_source;
      $j++;
    }
  }
  $form_state['values']['funding'] = $form_state['values']['fundings'];
  $registration = serialize($form_state);
  variable_set('tripal_site_registration', $registration);
  //Now send the updated info to the Tripal Site.
  tripal_registration_remote_submit($registration);
}

function tripal_registration_remote_submit($data) {
  global $base_url;
  $endpoint = 'http://localhost/tripal30';
  //Are we getting the modules?
  $form_data = unserialize($data);
  if ($form_data['values']['tripal_reg_site_modules']) {
    // Get current list of modules.
    $files = system_rebuild_module_data();

    // Remove hidden modules from display list.
    $visible_files = $files;
    foreach ($visible_files as $filename => $file) {
      if (!empty($file->info['hidden'])) {
        unset($visible_files[$filename]);
      }
    }

    // Iterate through each of the modules.
    $tripal_modules = [];
    foreach ($visible_files as $filename => $module) {
      if ($module->info['package'] == 'Tripal Extensions') {
        $module_name = $module->info['name'];
        if (!in_array($module->info, $tripal_modules)) {
          $tripal_modules[$module_name]['info'] = $module->info;
          $tripal_modules[$module_name]['status'] = $module->status;
        }
      }
    }
  }

  //Clean up form data
  $outgoing_data['pi_name'] = $form_data['values']['principal_investigator_name'];
  $outgoing_data['pi_email'] = $form_data['values']['principal_investigator_email'];
  $outgoing_data['sa_name'] = $form_data['values']['tripal_reg_site_admin_name'];
  $outgoing_data['sa_email'] = $form_data['values']['tripal_reg_site_admin_email'];
  $outgoing_data['description'] = $form_data['values']['tripal_reg_site_description'];

  $i = 0;
  foreach ($form_data['values']['funding'] as $funding_source){
    $outgoing_data['funding_period'][$i]['funding_agency'] = $funding_source['tripal_reg_site_agency'];
    $outgoing_data['funding_period'][$i]['funding_grant']  = $funding_source['tripal_reg_site_grant'];
    $outgoing_data['funding_period'][$i]['funding_start']  = $funding_source['funding_period']['tripal_reg_site_start'];
    $outgoing_data['funding_period'][$i]['funding_end']  = $funding_source['funding_period']['tripal_reg_site_end'];
    $i++;
  }

  $outgoing_data['type'] = $form_data['values']['tripal_reg_site_purpose'];

  //Build the info to send out.
  $outgoing_data['tripal_modules'] = $tripal_modules;
  $outgoing_data['site_name'] = variable_get('site_name', 'Default');
  $outgoing_data['site_url'] = $base_url;

  //Send
  $result = drupal_http_request($endpoint, array(
    'method' => 'POST',
    'headers' => array('Content-Type' => 'application/json', 'Accept' => 'application/json'),
    'data' => json_encode($outgoing_data),
  ));

}
