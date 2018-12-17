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
  dpm($form_state);
  $form_state['storage']['participants'] =
    isset($form_state['storage']['details']['funding']) ? $form_state['storage']['details']['funding'] : 0;

  $form['description'] = array(
    '#markup' =>  t('Registering your Tripal site is important. When you 
    register your site it gives the Tripal developers information that will 
    allow us to secure continuing funding. Registration also gives us insight
    into usage so we can build features the community needs. </br></br>'),
    );

  $form['disable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable registration reminder'),
    '#default_value' => FALSE,
    '#description' => "If you do not want to register your site please check
      this box as it will stop the reminder notifications.",
  );

  $form['details']['principal_investigator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Principal Investigator Contact Information'),
    '#collapsible' => true,
    '#collapsed' => false,
  );

  $form['details']['principal_investigator']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
  );

  $form['details']['principal_investigator']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#required' => TRUE,
  );
  $form['details']['site_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Administrator (if different from the principal investigator)'),
    '#collapsible' => true,
    '#collapsed' => false,
  );

  $form['details']['site_admin']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => FALSE,
  );

  $form['details']['site_admin']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#required' => FALSE,
  );

  $form['details']['description']= array(
    '#type' => 'textarea',
    '#title' => t('Description of the site'),
    '#required' => FALSE,
  );

  $form['details']['funding'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#prefix' => '<div id="funding">',
    '#suffix' => '</div>',
  );
 
    for ($i = 1; $i <= $form_state['storage']['details']['funding']; $i++) {
      $form['details']['funding'][$i] = array(
        '#type' => 'fieldset',
        '#tree' => true,
      );

      $form['details']['funding'][$i]['agency'] = array(
        '#type' => 'textfield',
        '#title' => t('Agency'),
      );

      $form['details']['funding'][$i]['grant'] = array(
        '#type' => 'textfield',
        '#title' => t('Grant Number'),
      );

      $form['details']['funding'][$i]['funding_period'] = array(
        '#type' => 'textfield',
        '#title' => t('Funding Period'),
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
  $form['details']['purpose'] = array(
    '#type' => 'radios',
    '#title' => t('Purpose of site'),
    '#default_value' => isset($node->purpose) ? $node->purpose : 1,
    '#options' => $purpose,
    '#required' => TRUE,
  );

  $form['details']['modules'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide Tripal (not other 3rd party modules) module usage information.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form_state['storage']['details']['funding']++;
  return $form;

}
function custom_registration_ajax_add_funding($form, $form_state) {
  return $form['details']['funding'];
}


