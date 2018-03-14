<?php


/**
 *
 */
function tripal_user_collections_page() {
  global $user;

  $headers = array('Name', 'Description', 'Create Date', 'Download Formats',
    'Generate File', 'Actions');
  $rows = array();

  $collections = db_select('tripal_collection', 'tc')
    ->fields('tc', array('collection_id'))
    ->condition('uid', $user->uid)
    ->orderBy('tc.collection_name')
    ->execute();

  while ($collection_id = $collections->fetchField()) {
    $collection = new TripalEntityCollection();
    $collection->load($collection_id);

    $downloads = array();
    $formatters = $collection->getFormatters();

    foreach ($formatters as $class_name => $label) {

      $outfile = $collection->getOutfilePath($class_name);

      if (file_exists($outfile)) {
        $outfileURL = file_create_url($outfile);
        $downloads[] = l($label, $outfileURL);
      }
      else {
        $downloads[] = 'Waiting on ' . $label . '...';
      }
    }
    $download_list = theme_item_list(array(
      'items' => $downloads,
      'title' => '',
      'type' => 'ul',
      'attributes' => array(),
    ));

    $rows[] = array(
      'data' => array(
        $collection->getName(),
        $collection->getDescription(),
        $collection->getCreateDate(),
        $download_list,
        l('Generate File for Download', 'user/' . $user->uid . '/data-collections/generate/' . $collection_id),
        l('View', 'user/' . $user->uid . '/data-collections/' . $collection_id . '/view') . ' ' .
        l('Delete', 'user/' . $user->uid . '/data-collections/' . $collection_id . '/delete'),
      ),
    );
  }

  $content['instructions'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .  t('Data collections allow you to store sets of data
       for download or later use by other tools on this site.  Typically data
       collections are created using search tools.  The following data
       collections are available to you.  Some files take time to generate
       before they can be downloaded.') . '</p>',
  );
  $content['files_table'] = array(
    '#type' => 'item',
    '#title' => 'Data Collections',
    '#markup' => theme_table(array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => TRUE,
      'empty' => t('You currently have no data collections.')
    )),
  );

  return $content;
}

/**
 * Renders a view of a data collection
 */
function tripal_user_collections_view_page($uid, $collection_id) {

  $user = user_load($uid);
  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l($user->name, 'user/' . $uid);
  $breadcrumb[] = l('Data Collections', 'user/' . $uid . '/data-collections');
  drupal_set_breadcrumb($breadcrumb);

  $collection = new TripalEntityCollection();
  $collection->load($collection_id);

  drupal_set_title('Data Collection: ' . $collection->getName());

  // Get the content types for this data collection.
  $cbundles = $collection->getBundles();
  $content_types = array();
  $field_list = array();
  $formatters_list = array();
  $num_entities = 0;

  foreach ($cbundles as $cbundle) {
    $eids = $collection->getEntityIDs($cbundle->bundle_name);
    $fields = $collection->getFieldIDs($cbundle->bundle_name);

    // Convert local field IDs to their names.
    if (!$cbundle->site_id) {
      $bundle = tripal_load_bundle_entity(array('name' => $cbundle->bundle_name));
      $content_types[] = $bundle->label;

      foreach ($fields as $field_id) {
        $field = field_info_field_by_id($field_id);
        $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle->name);
        $field_list[] = $instance['label'];

        $field_formatters = tripal_get_field_field_formatters($field, $instance);
        foreach ($field_formatters as $class_name => $label) {
          tripal_load_include_downloader_class($class_name);
          $formatters_list[] = $class_name::$label . ' (' . $class_name::$full_label . ')';
        }
      }
    }
    // Convert remote field IDs to their names.
    // TODO: add in retrieval of remote details.


    $num_entities += count($eids);
  }


  $contents['content_types'] = array(
    '#type' => 'item',
    '#title' => 'Content types',
    '#description' => t('The content types in this data collection'),
    '#markup' => join(', ', $content_types),
  );

  $contents['field_list'] = array(
    '#type' => 'item',
    '#title' => 'Fields',
    '#description' => t('The fields listed in this data collection'),
    '#markup' => join(', ', array_unique($field_list)),
  );
  $contents['formatters'] = array(
    '#type' => 'item',
    '#title' => 'Supported File Types',
    '#description' => t('The fields in your data collection appear to support the above file formats.  Please note that not all fields are compatible with all formats and only those that are compatible will appear in the format. Some file formats require certain fields and if all fields are not present in the collection they will not be complete.'),
    '#markup' => join(', ', array_unique($formatters_list)),
  );
  $contents['ecount'] = array(
    '#type' => 'item',
    '#title' => 'Records',
    '#description' => t('The number of records contained in this data collection.'),
    '#markup' => number_format($num_entities),
  );

  $contents['peek'] = array(
    '#type' => 'item',
    '#title' => 'Data View',
    '#markup' => 'Coming soon...',
  );


  return $contents;
}
/**
 * Provides the confirm form for deleting a data collection.
 */
function tripal_user_collections_delete_form($form, &$form_state,  $uid, $collection_id) {
  $form_state['collection_id'] = $collection_id;
  $form['#submit'][] = 'tripal_user_collections_delete_form_submit';

  $collection  = new TripalEntityCollection();
  $collection->load($collection_id);

  $form = confirm_form($form,
      t('Click the delete button below to confirm deletion of the collection titled: %title',
          array('%title' => $collection->getName())), 'user/' . $uid . '/data-collections',
      '<p>' .t('This action cannot be undone.') .'</p>', t('Delete'), t('Cancel'), 'confirm');

  return $form;
}
/**
 * Deletes a user's collection.
 *
 * @param $collection_id
 *   The ID of the collection to delete.
 */
function tripal_user_collections_delete_form_submit($form, &$form_state) {
  global $user;
  $collection_id = $form_state['collection_id'];
  $collection  = new TripalEntityCollection();
  $collection->load($collection_id);

  if ($collection->getUserID() == $user->uid) {
    try {
      $collection->delete();
      drupal_set_message('The data collection has been deleted.');
    }
    catch (Exception $e) {
      drupal_set_message(t('There was a problem deleting the data collection ' .
          'please contact the site to report the error: !message', array('!message', $e->getMessage())), 'error');
    }
  }
  drupal_goto('user/' . $user->uid . '/data-collections');
}

function tripal_user_collections_generate_file_form($form, &$form_state,  $uid, $collection_id) {
  $form_state['collection_id'] = $collection_id;
  $form['#submit'][] = 'tripal_user_collections_generate_file_form_submit';

  $collection  = new TripalEntityCollection();
  $collection->load($collection_id);
  $form = confirm_form($form,
      t('Confirm creation of files for the data collection named: "%title"',
          array('%title' => $collection->getName())), 'user/' . $uid . '/data-collections',
      '<p>' .t('It may take some time for the file(s) to be generated.  An email will be sent when files are ready.') .'</p>', t('Generate File'), t('Cancel'), 'confirm');

  return $form;
}
/**
 * Deletes a user's collection.
 *
 * @param $collection_id
 *   The ID of the collection to delete.
 */
function tripal_user_collections_generate_file_form_submit($form, &$form_state) {
  global $user;
  $collection_id = $form_state['collection_id'];
  $collection  = new TripalEntityCollection();
  $collection->load($collection_id);

  if ($collection->getUserID() == $user->uid) {
    try {
      // Add the job to write the collection download files.
      $args = array($collection_id);
      tripal_add_job('Create data collection files for ' . $user->name, 'tripal',
      'tripal_create_collection_files', $args, $user->uid, 10, array());
    }
    catch (Exception $e) {
      drupal_set_message('There was a problem creating the file for the data collection please contact the site to report the error.', 'error');
    }
  }

  drupal_goto('user/' . $user->uid . '/data-collections');
}
