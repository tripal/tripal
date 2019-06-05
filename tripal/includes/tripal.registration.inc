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

  $form['description'] = [
    '#title' => 'Why Register your Site?',
    '#type' => 'item',
    '#markup' => t('Registering your site is important for continued improvements to the software.  You may opt-in by providing answers to the
        following questions or opt-out by checking the box below. If you opt-in, your site will periodically
        connect to the http://tripal.info website and provide updated registration details. If you opt-out, no
        registration information is communictated. You can opt-out at any time.  If you want previously submitted information
        deleted from the tripal.info database please email !admin.', ['!admin' => l('admin@tripal.info', 'mailto:admin@tripal.info')])
  ];

  $form['usage_details'] = [
      '#title' => 'How will this data be used?',
      '#type' => 'item',
      '#markup' => t('Tripal is open-source, freely-available, but
        dependent on external funding. When you register your site, it provides important details that can
        be used to justify continued support from funding agencies. The information provided will not be shared publically.
        Information about the Tripal modules installed on your site will be used to help justify continued development to
        funding agencies.  Registration details may be shared with members of Tripal\'s Project Management Committee (PMC) and
        Tripal\'s Steering Committee (TSC) and Tripal extension module usage may be shared with developers
        of the the extension modules to aid in their funding requests.'),
  ];

  $form['disable_tripal_reporting'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not register this site (opt-out).'),
    '#default_value' => isset($form_data['values']['disable_tripal_reporting']) ? $form_data['values']['disable_tripal_reporting'] : NULL,
    '#description' => "If you do not want to register your site please check
      this box as it will stop the reminder notifications.  You can return later and register at any time.",
    '#ajax' => array(
      'callback' => '_tripal_form_disable_reg_callback',
      'event' => 'click',
    ),
  );

  $purpose = array(0 => t('Production'), 1 => t('Development'), 2 => t('Experimental'));
  $form['details']['tripal_reg_site_purpose'] = array(
      '#type' => 'radios',
      '#title' => t('Site Status'),
      '#default_value' => isset($form_data['values']['tripal_reg_site_purpose']) ? $form_data['values']['tripal_reg_site_purpose'] : NULL,
      '#options' => $purpose,
      '#required' => FALSE,
      '#description' => t('Please register your site regardless if it is experimental (to explore tripal), 
       for development of a future site (or updates to an existing site), or a site currently 
       in production. For funding, it is important to know how many sites are active for each category.  If your site changes 
       status, such as from development to production, please remember to return and update the purpose.')
  );

  $form['details']['tripal_reg_site_modules'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($form_data['values']['tripal_reg_site_modules']) ? $form_data['values']['tripal_reg_site_modules'] : 1,
      '#title' => t('Report your installed Tripal Extensions.'),
      '#description' => t('When checked, any Tripal extension modules that you have installed will be reported with your site\'s registration information.')
  );

  $form['details']['tripal_reg_site_description']= array(
      '#type' => 'textarea',
      '#title' => t('Description of the site'),
      '#default_value' => isset($form_data['values']['tripal_reg_site_description']) ? $form_data['values']['tripal_reg_site_description'] : NULL,
      '#required' => FALSE,
      '#description' => t('Please provide a brief description of this site.  Consider including
     details such as its purpose, the primary data types your site provides, and the
     research community your site serves.')
  );

  $form['details']['principal_investigator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Principal Investigator Contact Information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Please provide the name and email of this site\'s principal
     investigator (PI). If the name and email are provided then the PI agrees to
     receive periodic communication from either the Tripal Advisory Committee (TAC) or
     Project Management Committee (PMC) for the purposes of engaging with the larger
     Tripal user community. The PI will NOT be automatically subscribed to mailing lists.')
  );

  $form['details']['principal_investigator']['principal_investigator_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($form_data['values']['principal_investigator_name']) ? $form_data['values']['principal_investigator_name'] : NULL,
    '#required' => FALSE,
  );

  $form['details']['principal_investigator']['principal_investigator_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#default_value' => isset($form_data['values']['principal_investigator_email']) ? $form_data['values']['principal_investigator_email'] : NULL,
    '#required' => FALSE,
  );
  $form['details']['tripal_reg_site_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Manager (if different from the principal investigator)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Please provide the name and email of this site\'s manager if
     different from the PI. Sometimes, site managers desire involvement in community
     activites as well as the PI.  If the name and email are provided then the site manager agrees to
     receive periodic communication from either the Tripal Advisory Committee (TAC) or
     Project Management Committee (PMC) for the purposes of engaging with the larger
     Tripal user community. The site manager will NOT be automatically subscribed to mailing lists.')
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

  $form['details']['funding'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#prefix' => '<div id="funding">',
    '#suffix' => '</div>',
  );

  $count = count($form_data['values']['funding']);
  if ($form_state['details']['funding'] < $count) {
    $form_state['details']['funding'] = $form_state['details']['funding'] + $count;
  }
  else {
    $form_state['details']['funding'] = $form_state['details']['funding'] + 1;
  }

  for ($i = 1; $i <= $form_state['details']['funding']; $i++) {
    if ($i === $form_state['details']['funding']) {
      $form['details']['funding'][$i] = array(
        '#type' => 'fieldset',
        '#title' => t("Funding Source $i"),
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => t('When requesting funds for additional Tripal development,
       it is important to report the breadth of of funding sources for Tripal sites.
       Please consider sharing this information by providing the granting
       agency, and funding periods.')
      );
    }
    else {
      $form['details']['funding'][$i] = array(
        '#type' => 'fieldset',
        '#title' => t("Funding Source $i"),
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('When requesting funds for additional Tripal development,
       it is important to report the breadth of of funding sources for Tripal sites.
       Please consider sharing this information by providing the granting
       agency, and funding periods.')
      );
    }
    $form['details']['funding'][$i]['tripal_reg_site_agency'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_agency']) ? $form_data['values']['funding'][$i]['tripal_reg_site_agency'] : NULL,
      '#title' => t('Funding Agency'),
    );

    $form['details']['funding'][$i]['tripal_reg_site_grant'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_grant']) ? $form_data['values']['funding'][$i]['tripal_reg_site_grant'] : NULL,
      '#title' => t('Grant Number'),
    );
    $form['details']['funding'][$i]['tripal_reg_site_amount'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_amount']) ? $form_data['values']['funding'][$i]['tripal_reg_site_amount'] : NULL,
      '#title' => t('Funding Amount'),
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

  // Provide a submit button.
  if (!empty($form_data)) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Update registration information',
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
  if ($form_state['values']['disable_tripal_reporting'] != TRUE) {
    if (!empty($mail_pi) && !valid_email_address($mail_pi)) {
      form_set_error('[details][principal_investigator][principal_investigator_email]', t('The email address for the principal investigator appears to be invalid.'));
    }
    if(!empty($mail_sa) && !valid_email_address($mail_sa)) {
      form_set_error('[details][site_admin][tripal_reg_site_admin_email]', t("The email address for the site administrator appears to be invalid."));
    }
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
  variable_set('disable_tripal_reporting', TRUE);

  //Check for empty funding periods and remove them.
  $j = 1;
  foreach ($form_state['values']['funding'] as $funding_source) {
    if (!empty($funding_source['tripal_reg_site_agency']) && !empty($funding_source['tripal_reg_site_grant'])) {
      $form_state['values']['fundings'][$j] = $funding_source;
      $j++;
    }
  }
  $form_state['values']['funding'] = $form_state['values']['fundings'] ?? [];
  $registration = serialize($form_state);
  variable_set('tripal_site_registration', $registration);

  //Now send the updated info to the Tripal Site.
  // Only register with tripal.info if the user has not opt'd out.
  if ($form_state['values']['disable_tripal_reporting'] == FALSE) {
    tripal_registration_remote_submit($registration);
    drupal_set_message(t('Registration sent to tripal.info'), 'status');
    drupal_set_message(t('Thank you for registering. You can update your details at any time.'), 'status');
  }
  else {
    drupal_set_message(t('You are not registered with tripal.info. You can 
            change this at any time by unchecking the opt out checkbox and 
            submitting the form.'), 'status');
  }


}

function tripal_registration_remote_submit($data) {
  global $base_url;
  $endpoint = 'http://tripal.info/registration/content/50619jdi8ciayjhygidf';
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
    $outgoing_data['funding_period'][$i]['funding_amount']  = $funding_source['tripal_reg_site_amount'];
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

  variable_set('tripal_site_registration_last_update', time());

  return $result;
}
