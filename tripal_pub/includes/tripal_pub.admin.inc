<?php
/**
 * Implementation of tripal_pub_form().
 *
 *  This form takes the following information:A Publication Title,Volume title,Volume,Series Name,
 *  Issue,Publication Year,Pages where the Article is located, Miniref,Type-Id, if the article is Obsolete,
 *  Publishing company,Pubplication Place and a Uniquename for the the instance. It then puts the
 *  infromation into the Chado_project database table based on its 'pub_id'.
 *
 *
 *  @return $form
 *    An array of menu items '$form'
 *
 */
function tripal_pub_configuration_form() {

  $cv_options = tripal_cv_get_cv_options();

  //Creating Fieldset for multiple fields in form
  $form['node_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create/Edit Publication Settings'),
  );

  $form['node_form']['tripal_pub_types_cv'] = array(
    '#type' => 'select',
    '#title' => t('Controlled Vocabularies'),
    '#options' => $cv_options,
    '#default_value' => variable_get('tripal_pub_types_cv', 0),
    '#description' => 'Set the controlled vocabulary to pull publication type options from. Terms in this vocabulary will be available is the Publication Type select box on both the create and edit pages.',
  );

  $form['pubmed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create Nodes via PubMed Search'),
  );

  $form['pubmed']['description'] = array(
    '#type' => 'item',
    '#value' => 'Publication nodes are created based on the results of a PubMed publication search using '
      .'the keywords entered below. No content is created until the sync is clicked below and the registered tripal job is '
      .'run. This script attempts to only load new publications (ones which don\'t already have nodes from a previous search) '
      .'by comparing the pub_id thus if a publication is added manually which also appears in the pubmed search it will likely '
      .'get added twice.'
  );

  //define form elements for the node's title and body.
  $form['pubmed']['unique_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Search Keywords'),
    '#description' => t('Specific search terms. Must be seperated by a single space.'),
    '#required' => FALSE,
    '#default_value' => variable_get('unique_name', NULL)
  );

  //define form elements for the node's title and body.
  /**
  $form['set']['time_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Time Search Interval (Minutes)'),
    '#description'=>t(' The “Search Interval” set here determines when a drupal cron job should
    schedule a tripal job. As such, in reality the time until publications are sync’d is “Search Interval”
    + time remaining until drupal cron is run + time between drupal cron run and next tripal jobs run'),
    '#required' => FALSE,
    '#default_value' => variable_get('time_interval', NULL)
  );
  */

  $form['pubmed']['sync_info'] = array(
      '#type' => 'submit',
      '#title' => t('Sync Publications Jobs'),
      '#value' => t('Sync'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10,
    '#value' => t('Save Configuration')
  );

  return $form;

}


/*
 * Pub Configuration-Form
 * This form submit uses variable_set to set the vocabularies that are used, as well as the
 * unique_name and time interval that was entered by the user. If the user selects the option
 * to Sync Publicatin Jobs, the 'tripal_add_job' function is called, and a tripal job
 * will be added.
 *
 * @param $form
 *    -The submitted form containing the user entered infromation
 * @param $form_state
 *    -Is the state of the form: i.e what button was pressed, what infromation was entered,etc.
 *    The key is the 'values'
 */
function tripal_pub_configuration_form_submit($form, $form_state) {

    global $user;    //needed to make the current users details available so access of user id is available

    if ($form_state['values']['op'] == t('Save Configuration')) {

        variable_set('tripal_pub_types_cv', $form_state['values']['tripal_pub_types_cv']);
        variable_set('unique_name', $form_state['values']['unique_name'] );

    }

    //adding a tripal job if the user selects to Sync the Publications
    if ($form_state['values']['op'] == t('Sync')) {

        variable_set('unique_name', $form_state['values']['unique_name'] );
        $job_args = array($form_state['values']['unique_name']);
        $job_id = tripal_add_job('Search & Load PubMed Publications', 'tripal_pub', 'tripal_pub_search_load_pubmed_publications', $job_args, $user->uid);

    }

}