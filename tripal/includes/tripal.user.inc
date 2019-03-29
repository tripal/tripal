<?php


function tripal_user_get_files($uid) {
  $directory = [];

  // Get all of the files that have been uploaded by the user.
  $sql = "
     SELECT FM.fid, FM.filename, FM.uri, FM.uid, FM.filesize, TGEF.expiration_date
     FROM {file_managed} FM
       INNER JOIN {file_usage} FU on FM.fid = FU.fid and FM.uid = :user_id
       LEFT JOIN {tripal_expiration_files} TGEF on TGEF.fid = FU.fid
     WHERE FU.module = 'tripal'
     ORDER BY FM.uri
   ";
  $files = db_query($sql, [':user_id' => $uid]);
  $rows = [];
  While ($file = $files->fetchObject()) {

    // Don't list files that don't exist on the file system.
    if (!file_exists($file->uri)) {
      continue;
    }

    // Files have to be in the User's directory.
    if (!preg_match('/^public:\/\/tripal\/users\//', $file->uri)) {
      continue;
    }

    // If the expiration date is somehow not set, then set it.
    if (!$file->expiration_date) {
      $file->expiration_date = tripal_reset_file_expiration($file->fid);
    }

    // Organize the file into it's directory structure.
    $rpath = preg_replace('/^public:\/\/tripal\/users\/' . $uid . '\//', '', $file->uri);
    $paths = explode('/', $rpath);
    _tripal_user_build_files_dir($directory, $paths, $file);
  }

  return $directory;
}

/**
 * A recursive helper function for building the directory file tree.
 *
 * @param $directory
 *   The $directory array into which the file will be placed.
 * @param $paths
 *   An containing the directory path of the file. Each Directory
 *   is a separate element of the array.
 * @param $file
 *   The file object to add to the $directory object.
 */
function _tripal_user_build_files_dir(&$directory, $paths, $file) {
  $path = array_shift($paths);
  if (count($paths) == 0) {
    $directory[$path] = $file;
  }
  else {
    _tripal_user_build_files_dir($directory[$path], $paths, $file);
  }
}

/**
 * Generates an item list of the files.
 *
 * The results from this function can be passed to the theme_item_list function
 * and then themed as a file tree..
 *
 * @param $files_list
 *   The array as returned by tripal_user_get_files().
 *
 * @return
 *   An item list.
 */
function tripal_user_get_files_item_list($files_list, &$i = 0) {
  $items = [];

  // Add a header row
  if ($i == 0) {
    $items[] = [
      'data' => '<span><b>File</b></span>' .
        '<span class="file-expires"><b>Expires</b></span>' .
        '<span class="file-size"><b>Size</b></span>',
    ];
  }

  // Iterate through each file and recursively add it to our items array.
  foreach ($files_list as $filename => $file) {
    $i++;
    // If this is a folder then recurse.
    if (is_array($file)) {
      $items[] = [
        'data' => $filename,
        'children' => tripal_user_get_files_item_list($file, $i),
        'class' => [
          'tree-node-folder',
          'tree-node-closed',
          ($i % 2 == 0) ? 'even' : 'odd',
        ],
      ];
    }
    // If this is a file then give details for it.
    else {
      $datediff = $file->expiration_date - time();
      $dayleft = round($datediff / (60 * 60 * 24));
      if ($dayleft < 0) {
        $dayleft = 0;
      }
      $expiration = $file->expiration_date ? date('Y-m-d', $file->expiration_date) : '';
      $items[] = [
        'data' => '<span class="file-details"><span class="file-name">' . $filename . '</span>' .
          '<span class="file-expires">' . $dayleft . ' days</span>' .
          '<span class="file-size">' . tripal_format_bytes($file->filesize) . '</span></span>',
        'class' => ['tree-node-file', ($i % 2 == 0) ? 'even' : 'odd'],
        'fid' => $file->fid,
        'uid' => $file->uid,
      ];
    }
  }

  return $items;
}

/**
 * Gets the list of collections that have not yet generated files.
 *
 * @param $uid
 *   The ID of the user.
 */
function tripal_user_files_get_pending_collections_table($uid) {

  $collections = db_select('tripal_collection', 'tc')
    ->fields('tc', ['collection_id'])
    ->condition('uid', $uid)
    ->orderBy('tc.collection_name')
    ->execute();

  $headers = ['Name', 'Download Formats', 'Actions'];
  $rows = [];

  while ($collection_id = $collections->fetchField()) {
    $collection = new TripalEntityCollection();
    $collection->load($collection_id);

    $downloads = [];
    $formatters = $collection->getFormatters();
    $formatter_labels = [];

    $status = 'complete';
    foreach ($formatters as $class_name => $label) {
      $formatter_labels[] = $label;

      $outfile = $collection->getOutfilePath($class_name);

      if (file_exists($outfile)) {
        continue;
      }
      else {
        $status = 'pending';
      }
    }

    if ($status == 'pending') {
      $rows[] = [
        'data' => [
          $collection->getName(),
          implode(', ', $formatters),
          l('Create Files', 'user/' . $uid . '/data-collections/generate/' . $collection_id) . ' | ' .
          l('View', 'user/' . $uid . '/data-collections/' . $collection_id . '/view') . ' | ' .
          l('Delete', 'user/' . $uid . '/data-collections/' . $collection_id . '/delete'),
        ],
      ];
    }
  }
  return theme_table([
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'caption' => '',
    'colgroups' => [],
    'sticky' => TRUE,
    'empty' => t('You currently have no pending data collections.'),
  ]);
}

/**
 * Provides the page with a list of files uploaded by the user.
 *
 * @param $uid
 *   The user ID.
 *
 * @return
 *   A Drupal render array.
 */
function tripal_user_files_page($uid) {

  drupal_add_css(drupal_get_path('module', 'tripal') . '/theme/css/tripal_user_files.css');
  drupal_add_js(drupal_get_path('module', 'tripal') . '/theme/js/tripal.user_files.js', 'file');

  $user_files = tripal_user_get_files($uid);
  $items = tripal_user_get_files_item_list($user_files);
  $theme_files = theme_item_list([
    'items' => $items,
    'title' => '',
    'type' => 'ul',
    'attributes' => [
      'id' => 'tripal-user-file-tree',
    ],
  ]);

  $data_collections = tripal_user_files_get_pending_collections_table($uid);

  // Get the user quota settings.
  $quota = tripal_get_user_quota($uid);
  $usage = tripal_get_user_usage($uid);

  $content = [
    'page_title' => [
      '#type' => 'markup',
      '#markup' => '<h2>Your Files</h2>',
    ],
    'page_description' => [
      '#type' => 'markup',
      '#markup' => '<p>' . t('Each user is allowed to consume a limited amount of space for files. This page provides details about your current usage, your limits and files in your account.') . '</p>',
    ],
    'usage' => [
      '#type' => 'item',
      '#title' => 'Current Usage',
      '#markup' => tripal_format_bytes($usage),
      '#description' => t('The total number of bytes you currently consume.'),
    ],
    'quota' => [
      '#type' => 'item',
      '#title' => 'Current Quota',
      '#markup' => tripal_format_bytes($quota->custom_quota),
      '#description' => t('The maximum number of bytes of files you can upload.'),
    ],
    'expiration' => [
      '#type' => 'item',
      '#title' => 'Current Days to Expire',
      '#markup' => $quota->custom_expiration,
      '#description' => t('The number of days a file will remain on the server before deletion. The expiration of date of a file can be renewed by selecting the file name and then selecting the "Renew" link in the file details table.'),
    ],
    'data_collections' => [
      '#type' => 'item',
      '#title' => 'Pending Data Collections',
      '#markup' => $data_collections,
      '#description' => t('Data collections allow you to store custom sets of data
       for use on this site.  Typically data collections are created using search
       tools.  The above data collections are waiting to be generated. You must
       generate the files before you can use them.'),
    ],
    'file_list' => [
      '#type' => 'item',
      '#title' => 'File Browser',
      '#markup' => $theme_files,
    ],
    'file_details' => [
      '#type' => 'item',
      '#title' => 'File Details',
      '#markup' => '<div id="tripal-user-file-details">Click a file above for details.</div>',
    ],
  ];


  if ($usage < $quota->custom_quota) {
    drupal_set_message('Your file usage is currently below the file quota limit.');
  }
  else {
    drupal_set_message('Your file usage is currently over your file quota limit. Please remove some files before uploading more', 'warning');
  }

  return $content;
}

/**
 * User action to renew the expiration of a file.
 *
 * Adds the current time and the expiration date (either from default or if
 * the user has a custom expiration date) to tripal_expiration_files
 * table.
 *
 **/
function tripal_renew_file($fid) {
  $file = file_load($fid);
  $success = tripal_reset_file_expiration($fid);

  if ($success) {
    drupal_set_message('Successfully updated expiration date.');
  }
  drupal_goto('user/' . $file->uid . '/files/');
}

/**
 * Downloads a file.
 *
 * @param $fid
 *   The File ID of the file to be downloaded.
 */
function tripal_download_file($fid) {
  $file = file_load($fid);
  if (file_exists($file->uri)) {
    $headers = [];
    $headers['Content-Type'] = $file->filemime;
    $headers['Content-Disposition'] = 'attachment; filename=' . $file->filename;
    $headers['Content-Length'] = $file->filesize;
    file_transfer($file->uri, $headers);
  }
  else {
    drupal_set_message('Can not download. The file no longer exists on the server.', 'error');
    drupal_goto('user/' . $file->uid . '/files/');
  }
}

/**
 * Provides a confirmation form for deleting an uploaded file.
 */
function tripal_delete_file_form($form, $form_state, $uid, $fid) {
  $form = [];

  $file = file_load($fid);

  $form['uid'] = [
    '#type' => 'value',
    '#value' => $uid,
  ];
  $form['fid'] = [
    '#type' => 'value',
    '#value' => $fid,
  ];

  return confirm_form($form,
    t('Confirm deletion of the file named "' . $file->filename . '"?'),
    'user/' . $uid . '/files/'
  );
}

/**
 * Implements a form submit for deleting an uploaded file.
 */
function tripal_delete_file_form_submit($form, &$form_state) {
  $fid = $form_state['values']['fid'];
  $uid = $form_state['values']['uid'];
  $file = file_load($fid);

  // Remove the file from the file_usage table for all entries that link
  // to the tripal module.
  file_usage_delete($file, 'tripal', NULL, NULL, 0);

  // Get any remaining usage for other modules
  $file_usage = file_usage_list($file);

  // If this file is still used by the tripal module then something
  // didn't work right.
  if (in_array('tripal', $file_usage)) {
    drupal_set_message('The file could not be removed.  Please contact the site administrator.', 'error');
  }

  // If there is no other usage of this file from other modules then delete it.
  if (count(array_keys($file_usage)) == 0) {
    if (file_unmanaged_delete($file->uri)) {

      // Also remove the md5 checksum.
      if (file_exists(file_unmanaged_delete($file->uri . '.md5'))) {
        file_unmanaged_delete($file->uri . '.md5');
      }
      drupal_set_message('The file has been fully removed.');
    }
    else {
      drupal_set_message('The file has removed from this list and does not count against your quota, but other components of this site rely on this file. Thus it has not been fully removed.');
    }
  }
  drupal_goto('user/' . $file->uid . '/files/');
}

/**
 * Provides details about a file.
 */
function tripal_view_file($uid, $fid) {
  $file = file_load($fid);
  $usage = file_usage_list($file);

  // Check to see if this is a data collection.
  $collection = NULL;
  $collection_ctypes = [];
  $collection_field_list = [];
  $collection_formatters = [];
  $collection_entities = 0;
  if (array_key_exists('tripal', $usage)) {
    if (array_key_exists('data_collection', $usage['tripal'])) {
      $collection_id = array_keys($usage['tripal']['data_collection'])[0];
      $collection = new TripalEntityCollection();
      $collection->load($collection_id);

      // Get the content types for this data collection.
      $cbundles = $collection->getBundles();
      foreach ($cbundles as $cbundle) {
        $eids = $collection->getEntityIDs($cbundle->bundle_name);
        $fields = $collection->getFieldIDs($cbundle->bundle_name);

        // Convert local field IDs to their names.
        if (!$cbundle->site_id) {
          $bundle = tripal_load_bundle_entity(['name' => $cbundle->bundle_name]);
          $collection_ctypes[] = $bundle->label;

          foreach ($fields as $field_id) {
            $field = field_info_field_by_id($field_id);
            $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle->name);
            $collection_field_list[] = $instance['label'];

            $field_formatters = tripal_get_field_field_formatters($field, $instance);
            foreach ($field_formatters as $class_name => $label) {
              tripal_load_include_downloader_class($class_name);
              $collection_formatters[] = $class_name::$label . ' (' . $class_name::$full_label . ')';
            }
          }
        }
        // Convert remote field IDs to their names.
        // TODO: add in retrieval of remote details.


        $collection_entities += count($eids);
      }

    }
  }

  $headers = [];
  $rows = [];

  $actions = l('Delete', "user/$uid/files/$file->fid/delete") . '<br>' .
    l('Download', "user/$uid/files/$file->fid/download") . '<br>' .
    l('Renew', "user/$uid/files/$file->fid/renew");

  // Name row
  $rows[] = [
    [
      'data' => 'File Name',
      'header' => TRUE,
      'width' => '20%',
    ],
    $file->filename,
  ];

  $date_uploaded = date('Y-m-d H:i:s', $file->timestamp);
  $rows[] = [
    [
      'data' => 'Create Date',
      'header' => TRUE,
      'width' => '20%',
    ],
    $date_uploaded,
  ];

  $expiration_date = db_select('tripal_expiration_files', 'tgef')
    ->fields('tgef', ['expiration_date'])
    ->condition('fid', $fid)
    ->execute()
    ->fetchField();
  $expiration = $expiration_date ? date('Y-m-d H:i:s', $expiration_date) : '';
  $rows[] = [
    [
      'data' => 'Expiration Date',
      'header' => TRUE,
      'width' => '20%',
    ],
    $expiration,
  ];

  $md5_file = $file->uri . '.md5';
  if (file_exists($md5_file)) {
    $rows[] = [
      [
        'data' => 'MD5',
        'header' => TRUE,
        'width' => '20%',
      ],
      file_get_contents($md5_file),
    ];
  }
  $rows[] = [
    [
      'data' => 'Actions',
      'header' => TRUE,
      'width' => '20%',
    ],
    $actions,
  ];

  $file_content = theme_table([
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ]);


  $collection_content = '';
  if ($collection) {
    $rows = [];
    $rows[] = [
      [
        'data' => 'Description',
        'header' => TRUE,
        'width' => '20%',
      ],
      $collection->getDescription(),
    ];
    $rows[] = [
      [
        'data' => 'Content types',
        'header' => TRUE,
        'width' => '20%',
      ],
      join(', ', $collection_ctypes),
    ];

    $rows[] = [
      [
        'data' => 'Fields',
        'header' => TRUE,
        'width' => '20%',
      ],
      join(', ', array_unique($collection_field_list)),
    ];
    $rows[] = [
      [
        'data' => 'Supported File Types',
        'header' => TRUE,
        'width' => '20%',
      ],
      join(', ', array_unique($collection_formatters)),
    ];
    $rows[] = [
      [
        'data' => 'Records',
        'header' => TRUE,
        'width' => '20%',
      ],
      number_format($collection_entities),
    ];
    $rows[] = [
      [
        'data' => 'Actions',
        'header' => TRUE,
        'width' => '20%',
      ],
      l('Delete', 'user/' . $uid . '/data-collections/' . $collection->getCollectionID() . '/delete'),
    ];

    $collection_content = '<b>Collection Details</b>' . theme_table([
        'header' => $headers,
        'rows' => $rows,
        'attributes' => [],
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => [],
        'empty' => '',
      ]);
  }

  drupal_json_output($file_content . $collection_content);
}


