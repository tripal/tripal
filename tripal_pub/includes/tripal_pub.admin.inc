<?php
/**
 * @file
 * Administration of publications
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin_pub_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Publications', 'admin/tripal/chado/tripal_pub');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_pub_admin_publications','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Publications module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Publications View', 'admin/tripal/chado/tripal_pub/views/pubs/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_pub
 */
function tripal_pub_admin() {
  $form = array();

  // -----------------------------------------
  // add in the fields for selecting which fields are used when search for pubs
  $form['searching'] = array(
    '#type' => 'fieldset',
    '#title' => t('Searching Options'),
    '#description' => t("The list of checkboxes below indicate which fields a user
      can search with when using the publication search tool.  Check the fields that you want
      to allow users to search with.  Click the 'Save configuration' button below to save changes."),
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
  while ($prop = $prop_types->fetchObject()) {
    $properties[$prop->cvterm_id] = $prop->name;
  }
  $form['searching']['allowed_search_fields'] = array(
    '#type'    => 'checkboxes',
    '#options' => $properties,
    '#prefix'  => '<div style="scroll: auto; border:1px solid #CCCCCC;">',
    '#suffix'  => '</div>',
    '#default_value' => variable_get('tripal_pub_allowed_search_fields', array()),
  );

  // -----------------------------------------
  // add the field set for syncing publications
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import Settings'),
    '#description' => t('During import, Tripal will attempt to find duplicate publications,
       and will not try to insert a publication that already exists in the database.  It can
       find duplicates using the title, year, series name (e.g. Journal Name) and media type
       (e.g. Journal Article etc.).
       There are several options for how to find a duplicate publication.  Choose the
       option that best suits your needs.'),
  );
  $form['import']['import_duplicate_check'] = array(
    '#type' => 'radios',
    '#title' => t('Unique Constraint'),
    '#options' => array(
      'title_year' => t('Title and Year'),
      'title_year_media' => t('Title, Year, Media name (e.g. Journal Name, etc.)'),
      'title_year_type' => t('Title, Year, Media type (e.g. Journal, Conference Proceedings, etc.'),
    ),
    '#default_value' => variable_get('tripal_pub_import_duplicate_check', 'title_year_media'),
  );

  // -----------------------------------------
  // get the list of publication types.  In the Tripal publication
  // ontologies these are all grouped under the term 'Publication Type'
  // we want the default to be 'Journal Article'
  $sql = "
    SELECT
      CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id  = CVTO.cvterm_id
      INNER JOIN {cv}          ON CVTO.cv_id      = CV.cv_id
    WHERE CV.name = 'tripal_pub' AND CVTO.name = 'Publication Type' AND
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC
  ";
  $results = chado_query($sql);
  $pub_types = array();
  while ($pub_type = $results->fetchObject()) {
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
    '#description' => t('Please set a default publiation type used for manual entry of a new
      publication.  This is useful in the event that someone is manually adding the same
      publication type repetitively'),
    '#default_value' => $d_type_id
  );

  return system_settings_form($form);
}

/**
 * Validate the admin settings form
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

}

/**
 * Set the URL for a publication
 *
 * @param $node
 *   The publication node from pub_load().
 * @param $pub_id
 *   The chado pub_id of the publication to set the url for
 *
 * @return
 *   The url alias set
 *
 * @ingroup tripal_pub
 */
function tripal_pub_set_pub_url($node, $pub_id) {

  $node_url = "node/$node->nid";
  $url_alias = "pub/$pub_id";

  // remove any previous alias
  db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => $node_url));
  // add the new alias
  $path_alias = array("source" => $node_url, "alias" => $url_alias);
  path_save($path_alias);

  return $url_alias;
}
