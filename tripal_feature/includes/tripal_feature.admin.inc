<?php
/**
 * @file
 * Administration of features
 */

/**
 * Launchpad for feature administration.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin_feature_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Features', 'admin/tripal/chado/tripal_feature');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_feature_admin_features','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Feature module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Features View', 'admin/tripal/chado/tripal_feature/views/features/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Feature Settings page
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin() {

  // FEATURE PAGE TITLES
  $form['title'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature Page Titles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['title']['desc'] = array(
    '#markup' => t(
      'Each synced feature must have a unique page title, however, features '.
      'may have the same name if they are of different types or from '.
      'different organisms.  Therefore, we must be sure that the '.
      'page titles can uniquely identify the feature being viewed.  Select '.
      'an option below that will uniquely identify all features on your site.'),
  );
  $options = array(
    'feature_unique_name'  => 'Feature unique name',
    'feature_name'         => 'Feature name',
    'unique_constraint'    => 'Feature Name, uniquename, type and species',
  );
  $form['title']['chado_feature_title'] = array(
    '#title'         => t('Feature Page Titles'),
    '#type'          => 'radios',
    '#description'   => t('Choose a title type  from the list above that is '.
      'guaranteed to be unique for all features.  If in doubt it is safest to '.
      'choose the last option as that guarantees uniqueness. Click the '.
      '\'Save Configuration\' button at the bottom to save your selection.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_feature_title', 'unique_constraint'),
  );
    
  // FEATURE URL PATHS
  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature URL Path'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $options = array(
    'SID[id]'      => '[id]:' . t('The Chado feature_id'),
    'feature'      => 'feature:' . t('Chado table name'),
    '[genus]'      => '[genus]:' . t('Genus to which the feature belongs'),
    '[species]'    => '[species]:' . t('Species to which the feature belongs'),
    '[type]'       => '[type]:' . t('The type of feature'),
    '[uniquename]' => '[uniquename]:' . t('The feature unique name'),
    '[name]'       => '[name]:' . t('The feature name'),
    'reset'        => t('Reset'),
  );
  
  $form['url']['chado_feature_url_string'] = array(
    '#title' => 'URL Syntax',
    '#type' => 'textfield',
    '#description' => t('You may rearrange elements in this text box to '.
      'customize the URLs.  The available tags include: [id], '.
      '[uniquename]. [name], [species], [genus], [type]. You can separate or '.
      'include any text between the tags. Click the "Set Feature URLs" button to '.
      'reset the URLs for all feature pages.  Click the "Save Configuration" button to '.
      'simply save this setup. <b>Important</b>: be sure that whatever you choose will always be unique even considering '.
      'future data that may be added.  If you include the Chado table name, genus, species, type '.
      'and uniquename you are guaranteed to have a unique URL. For example feature/[genus]/[species]/[type]/[uniquename]'), 
    '#size' => 150,
    '#default_value' => variable_get('chado_feature_url_string', '/feature/[genus]/[species]/[type]/[uniquename]'),
  );
  
  $form['url']['chado_feature_url'] = array(
    '#title'         => t('URL components'),
    '#type'          => 'checkboxes',
    '#required'      => FALSE,
    '#options'       => $options,
    '#description'   => t('Click the item above to make it appear in the URL Syntax box'),
    '#attributes'    => array(
      'onclick' => '
      box = $(\'#edit-chado-feature-url-string\');
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
    '#value' => t('Set Feature URLs'),
  );
    
    
    
  // FEATURE BROWSER
  $form['browser'] = array(
     '#type' => 'fieldset',
     '#title' => t('Feature Browser'),
     '#collapsible' => TRUE,
     '#collapsed' => FALSE,
  );
  $form['browser']['browser_desc'] = array(
     '#markup' => t('A feature browser can be added to an organism page to allow users to quickly ' .
        'access a feature.  This will most likely not be the ideal mechanism for accessing feature ' .
        'information, especially for large sites, but it will alow users exploring the site (such ' .
        'as students) to better understand the data types available on the site.'),

  );
  $form['browser']['feature_types'] = array(
     '#title'       => t('Feature Types'),
     '#type'        => 'textarea',
     '#description' => t("Enter the Sequence Ontology (SO) terms for the feature types that " .
                         "will be shown in the feature browser."),
     '#default_value' => variable_get('chado_browser_feature_types', 'gene mRNA'),
  );


  $form['browser']['set_browse_button'] = array(
     '#type' => 'submit',
     '#value' => t('Set Browser'),
     '#weight' => 2,
  );

  // FEATURE SUMMARY REPORT
  $form['summary'] = array(
     '#type' => 'fieldset',
     '#title' => t('Feature Summary Report'),
     '#collapsible' => TRUE,
     '#collapsed' => FALSE,
  );
  $form['summary']['feature_mapping'] = array(
     '#title' => 'Map feature types',
     '#description' => t('You may specify which Sequence Ontology (SO) terms to show in the ' .
        'feature summary report by listing them in the following text area.   Enter one per line. ' .
        'If left blank, all SO terms for all features will be shown in the report. Only those terms ' .
        'listed below will be shown in the report. Terms will appear in the report in the same order listed. To rename a ' .
        'SO term to be more human readable form, use an \'=\' sign after the SO term (e.g. \'polypeptide = Protein\')'),
     '#type' => 'textarea',
     '#rows' => 15,
     '#default_value' => variable_get('tripal_feature_summary_report_mapping', ''),
  );
  $form['summary']['set_summary_button'] = array(
     '#type' => 'submit',
     '#value' => t('Set Summary'),
     '#weight' => 2,
  );
  
  return system_settings_form($form);
}

/**
 * Validate the feature settings forms
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  variable_set('chado_browser_feature_types', $form_state['values']['feature_types']);

  switch ($form_state['values']['op']) {

    case t('Set Summary') :
      variable_set('tripal_feature_summary_report_mapping', $form_state['values']['feature_mapping']);
      break;

    case t('Set Feature URLs') :
      variable_set('chado_feature_url', $form_state['values']['feature_url']);
      tripal_add_job('Set Feature URLs', 'tripal_feature',
        'tripal_feature_set_urls', $job_args, $user->uid);
      break;
  }

}
