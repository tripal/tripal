<?php


/**
 *
 */
function tripal_user_collections_page() {
  global $user;

  $headers = array('Name', 'Description', 'Create Date', 'Downloads Formats', 'Actions');
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
    $formatters = $collection->getDownloadFormatters();
    foreach ($formatters as $class_name => $label) {
      $outfile = $collection->getOutfilePath($class_name);
      $outfileURL = file_create_url($outfile);
      if (file_exists($outfile)) {
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
      drupal_set_message('There was a problem deleting the data collection please contact the site to report the error.', 'error');
    }
  }
  drupal_goto('user/' . $user->uid . '/data-collections');
}