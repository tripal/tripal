<?php


/**
 * Renders a view of a data collection
 */
function tripal_user_collections_view_page($uid, $collection_id) {

  $user = user_load($uid);
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l($user->name, 'user/' . $uid);
  $breadcrumb[] = l('Files', 'user/' . $uid . '/files');
  drupal_set_breadcrumb($breadcrumb);

  $collection = new TripalEntityCollection();
  $collection->load($collection_id);

  drupal_set_title('Data Collection: ' . $collection->getName());

  // Get the content types for this data collection.
  $cbundles = $collection->getBundles();
  $content_types = [];
  $field_list = [];
  $formatters_list = [];
  $num_entities = 0;

  foreach ($cbundles as $cbundle) {
    $eids = $collection->getEntityIDs($cbundle->bundle_name);
    $fields = $collection->getFieldIDs($cbundle->bundle_name);

    // Convert local field IDs to their names.
    if (!$cbundle->site_id) {
      $bundle = tripal_load_bundle_entity(['name' => $cbundle->bundle_name]);
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


  $contents['content_types'] = [
    '#type' => 'item',
    '#title' => 'Content types',
    '#description' => t('The content types in this data collection'),
    '#markup' => join(', ', $content_types),
  ];

  $contents['field_list'] = [
    '#type' => 'item',
    '#title' => 'Fields',
    '#description' => t('The fields listed in this data collection'),
    '#markup' => join(', ', array_unique($field_list)),
  ];
  $contents['formatters'] = [
    '#type' => 'item',
    '#title' => 'Supported File Types',
    '#description' => t('The fields in your data collection appear to support the above file formats.  Please note that not all fields are compatible with all formats and only those that are compatible will appear in the format. Some file formats require certain fields and if all fields are not present in the collection they will not be complete.'),
    '#markup' => join(', ', array_unique($formatters_list)),
  ];
  $contents['ecount'] = [
    '#type' => 'item',
    '#title' => 'Records',
    '#description' => t('The number of records contained in this data collection.'),
    '#markup' => number_format($num_entities),
  ];

  return $contents;
}

/**
 * Provides the confirm form for deleting a data collection.
 */
function tripal_user_collections_delete_form($form, &$form_state, $uid, $collection_id) {
  $form_state['collection_id'] = $collection_id;
  $form['#submit'][] = 'tripal_user_collections_delete_form_submit';

  $collection = new TripalEntityCollection();
  $collection->load($collection_id);

  $form = confirm_form($form,
    t('Click the delete button below to confirm deletion of the collection titled: %title',
      ['%title' => $collection->getName()]), 'user/' . $uid . '/files',
    '<p>' . t('This action cannot be undone.') . '</p>', t('Delete'), t('Cancel'), 'confirm');

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
  $collection = new TripalEntityCollection();
  $collection->load($collection_id);

  if ($collection->getUserID() == $user->uid) {
    try {
      $collection->delete();
      drupal_set_message('The data collection has been deleted.');
    } catch (Exception $e) {
      drupal_set_message(t('There was a problem deleting the data collection ' .
        'please contact the site to report the error: !message',
        ['!message' => $e->getMessage()]), 'error');
    }
  }
  drupal_goto('user/' . $user->uid . '/files');
}

function tripal_user_collections_generate_file_form($form, &$form_state, $uid, $collection_id) {
  $form_state['collection_id'] = $collection_id;
  $form['#submit'][] = 'tripal_user_collections_generate_file_form_submit';

  $collection = new TripalEntityCollection();
  $collection->load($collection_id);
  $form = confirm_form($form,
    t('Confirm creation of files for the data collection named: "%title"',
      ['%title' => $collection->getName()]), 'user/' . $uid . '/files',
    '<p>' . t('It may take some time for the file(s) to be generated.  An email will be sent when files are ready.') . '</p>', t('Generate File'), t('Cancel'), 'confirm');

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
  $collection = new TripalEntityCollection();
  $collection->load($collection_id);

  if ($collection->getUserID() == $user->uid) {
    try {
      // Add the job to write the collection download files.
      $args = [$collection_id];
      tripal_add_job('Create data collection files for ' . $user->name, 'tripal',
        'tripal_create_collection_files', $args, $user->uid, 10, []);
    } catch (Exception $e) {
      drupal_set_message('There was a problem creating the file for the data collection please contact the site to report the error.', 'error');
    }
  }

  drupal_goto('user/' . $user->uid . '/files');
}
