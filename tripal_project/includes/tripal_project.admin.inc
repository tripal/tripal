<?php
/**
 * @file
 * Administration of projects
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_project
 */
function tripal_project_admin_project_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Projects', 'admin/tripal/chado/tripal_project');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_project_admin_projects','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Project module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Projects View', 'admin/tripal/chado/tripal_project/views/projects/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}
/**
 * Project settings
 *
 * @ingroup tripal_project
 */
function tripal_project_admin($form, $form_state) {
  $form = array();

  // project URL PATHS
  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Project URL Path'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $options = array(
    'project'      => 'project:' . t('Chado table name'),
    'SID[id]'      => '[id]:' . t('The Chado project_id'),
    '[name]'       => '[name]:' . t('The project name'),
  );

  $form['url']['options'] = array(
    '#type' => 'item',
    '#title' => 'Available Tokens',
    '#markup' => '<ul><li>' . implode('</li><li>', $options) . '</li></ul>'
  );

  $form['url']['chado_project_url_string'] = array(
    '#title' => 'URL Syntax',
    '#type' => 'textfield',
    '#description' => t('You may rearrange elements in this text box to '.
      'customize the URLs.  The available tags are listed below. You can separate or '.
      'include any text between the tags. Click the "Set project URLs" button to '.
      'reset the URLs for all project pages.  Click the "Save Configuration" button to '.
      'simply save this setup. <b>Important</b>: be sure that whatever you choose will '.
      'always be unique even considering future data that may be added.  If you include '.
      'the Chado table name and id you are guaranteed to have a unique URL. For example project/[id]'),
    '#size' => 150,
    '#default_value' => variable_get('chado_project_url_string', '/project/[id]'),
  );

  $form['url']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Project URLs'),
  );

  return system_settings_form($form);
}

/**
 * Validate project settings
 *
 * @ingroup tripal_project
 */
function tripal_project_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  switch ($form_state['values']['op']) {
    case t('Set Project URLs') :
      variable_set('chado_project_url', $form_state['values']['chado_project_url_string']);
      tripal_add_job('Set Project URLs', 'tripal_project',
        'tripal_project_set_urls', $job_args, $user->uid);
      break;
  }
}
