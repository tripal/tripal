<?php

/**
 * Form for adding a file quota for a user.
 **/
function tripal_admin_add_custom_form($form, &$form_state) {

  $username = '';
  $default_quota = variable_get('tripal_default_file_quota', pow(20, 6));
  $default_expiration = variable_get('tripal_default_file_expiration', '60');

  if (array_key_exists('values', $form_state)) {
    $username = $form_state['values']['username'];
    $default_quota = $form_state['values']['default_quota'];
    $default_expiration = $form_state['values']['default_expiration_date'];
  }


  // Textfield (ajax call based off of existing users) for users on the site
  $form['username'] = [
    '#type' => 'textfield',
    '#title' => 'User',
    '#autocomplete_path' => 'admin/tripal/files/quota/user/autocomplete',
    '#default_value' => $username,
  ];

  // Custom quota textfield (prepopulated with defualt value)
  $form['quota'] = [
    '#type' => 'textfield',
    '#title' => 'Custom User Quota',
    '#description' => 'Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).',
    '#default_value' => tripal_format_bytes($default_quota),
  ];

  // Custom exp date textfield (prepopulated with defualt value)
  $form['expiration'] = [
    '#type' => 'textfield',
    '#title' => 'Days to Expire',
    '#description' => 'The number of days that a user uploaded file can remain on the server before it is automatically removed.',
    '#default_value' => $default_expiration,
  ];

  // Submit button
  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Submit'),
  ];
  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l('Cancel', 'admin/tripal/files/quota'),
  ];

  return $form;

}

/**
 * Validates the tripal_admin_add_custom_form form.
 **/
function tripal_admin_add_custom_form_validate($form, &$form_state) {

  $username = $form_state['values']['username'];
  $quota = $form_state['values']['quota'];
  $expiration = $form_state['values']['expiration'];

  // Make sure the username is a valid user.
  $sql = "SELECT uid FROM {users} WHERE name = :name";
  $uid = db_query($sql, [':name' => $username])->fetchField();
  if (!$uid) {
    form_set_error('username', 'Cannot find this username');
  }

  // Does a quota already exist for this user? If so, then don't add it again
  $check = db_select('tripal_custom_quota', 'tgcq')
    ->fields('tgcq', ['uid'])
    ->condition('uid', $uid)
    ->execute()
    ->fetchField();
  if ($check) {
    form_set_error('username', 'The user "' . $username . '" already has a custom quota set.');
  }

  // Validate the quota string.
  if (!preg_match("/^\d+(MB|GB|TB)$/", $quota)) {
    form_set_error('quota', t('Please provide a quota size in the format indicated.'));
  }

  // Validate the expiration time.
  if (!preg_match("/^\d+$/", $expiration)) {
    form_set_error('expiration', t('Please providate a positive non-decimal numeric value for the days to expire'));
  }
}


/**
 * Submiter for the tripal_admin_add_custom_form form.
 **/
function tripal_admin_add_custom_form_submit($form, &$form_state) {

  $username = $form_state['values']['username'];
  $quota = $form_state['values']['quota'];
  $expiration = $form_state['values']['expiration'];

  // if the 2nd element of the qutoa string occupied by a valid suffix we need to check to see
  // what we have to multiply the value by (1024 for GB 1024^2 for TB because
  // we assume that the initial number is already in MB)
  $matches = [];
  $multiplier = 'MB';
  $size = $quota;
  if (preg_match("/^(\d+)(MB|GB|TB)$/", $quota, $matches)) {
    $multiplier = $matches[2];
    $size = $matches[1];
  }

  switch ($multiplier) {
    case 'GB':
      $size = (int) $quota * pow(10, 9);
      break;
    case 'TB':
      $size = (int) $quota * pow(10, 12);
      break;
    default:
      $size = (int) $quota * pow(10, 6);
      break;
  }

  // Get the UID of the given user.
  $sql = "SELECT uid FROM {users} WHERE name = :name";
  $uid = db_query($sql, [':name' => $username])->fetchField();

  // Stripaluota.
  tripal_set_user_quota($uid, $size, $expiration);

  // TODO: check to make sure that the quota was actually set, can we assume
  // it will always work?

  drupal_set_message(t('Custom quota set for the user: @username', ['@username' => $username]));
  drupal_goto('admin/tripal/files/quota');
}

/**
 * Edit an existing users' quota and/or expiration date
 **/
function tripal_admin_edit_quota_form($form, &$form_state, $uid) {

  $quota = tripal_get_user_quota($uid);
  $default_quota = $quota->custom_quota;
  $default_expiration = $quota->custom_expiration;

  if (array_key_exists('values', $form_state)) {
    $default_quota = $form_state['values']['default_quota'];
    $default_expiration = $form_state['values']['default_expiration_date'];
  }

  $user = user_load($uid);

  $form['uid'] = [
    '#type' => 'value',
    '#value' => $uid,
  ];

  // Textfield (ajax call based off of existing users) for users on the site
  $form['username'] = [
    '#type' => 'item',
    '#title' => 'User',
    '#markup' => $user->name,
  ];

  // Custom quota textfield (prepopulated with defualt value)
  $form['quota'] = [
    '#type' => 'textfield',
    '#title' => 'Custom User Quota',
    '#description' => 'Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).',
    '#default_value' => tripal_format_bytes($default_quota),
  ];

  // Custom exp date textfield (prepopulated with defualt value)
  $form['expiration'] = [
    '#type' => 'textfield',
    '#title' => 'Days to Expire',
    '#description' => 'The number of days that a user uploaded file can remain on the server before it is automatically removed.',
    '#default_value' => $default_expiration,
  ];

  // Submit button
  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Submit'),
  ];
  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l('Cancel', 'admin/tripal/files/quota'),
  ];

  return $form;
}

/**
 * Same validate as the add user with the exception of no duplicate entry
 **/
function tripal_admin_edit_quota_form_validate($form, &$form_state) {

  $uid = $form_state['values']['uid'];
  $quota = $form_state['values']['quota'];
  $expiration = $form_state['values']['expiration'];

  // Validate the quota string.
  if (!preg_match("/^\d+(MB|GB|TB)$/", $quota)) {
    form_set_error('quota', t('Please provide a quota size in the format indicated.'));
  }

  // Validate the expiration time.
  if (!preg_match("/^\d+$/", $expiration)) {
    form_set_error('expiration', t('Please providate a positive non-decimal numeric value for the days to expire'));
  }
}

/**
 * Same submit as the quota overwrite function
 **/
function tripal_admin_edit_quota_form_submit($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  $quota = $form_state['values']['quota'];
  $expiration = $form_state['values']['expiration'];

  // if the 2nd element of the qutoa string occupied by a valid suffix we need to check to see
  // what we have to multiply the value by (1024 for GB 1024^2 for TB because
  // we assume that the initial number is already in MB)
  $matches = [];
  $multiplier = 'MB';
  $size = $quota;
  if (preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $quota, $matches)) {
    $multiplier = $matches[2];
    $size = $matches[1];
  }

  switch ($multiplier) {
    case 'GB':
      $size = (int) $quota * pow(10, 9);
      break;
    case 'TB':
      $size = (int) $quota * pow(10, 12);
      break;
    default:
      $size = (int) $quota * pow(10, 6);
      break;
  }

  // Set the user quota.
  tripal_remove_user_quota($uid);
  tripal_set_user_quota($uid, $size, $expiration);

  $user = user_load($uid);

  drupal_set_message(t('Custom quota set for the user: @username', ['@username' => $user->name]));
  drupal_goto('admin/tripal/files/quota');
}

/**
 * Implements the form for setting the default file settings.
 */
function tripal_admin_manage_files_form($form, &$form_state) {

  if (array_key_exists('values', $form_state)) {
    $upload_max = $form_state['values']['upload_max'];
  }
  else {
    $upload_max = tripal_format_bytes(variable_get('tripal_upload_max_size', 10000000000));
  }

  $form['php_defaults'] = [
    '#type' => 'item',
    '#title' => 'PHP Maximum Upload Size',
    '#description' => t('Your php.ini file is currently configured with this size as the maximum size allowed for a single file during upload. However, Tripal uses an HTML5 uploader that supports much larger file sizes.  It works by breaking the file into chunks and uploading each chunk separately. Therefore this becomes the maximum allowed size of a chunk.'),
    '#markup' => ini_get("upload_max_filesize"),
  ];

  $form['upload_max'] = [
    '#type' => 'textfield',
    '#title' => 'Maximum file size',
    '#description' => t('Set the maximum size that a file can have for upload. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).  No user will be allowed to upload a file larger than this when Tripal\'s file upload tool is used.'),
    '#default_value' => $upload_max,
  ];

  $form['update_defaults'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
  ];
  return $form;
}

/**
 * Validate the form's values: proper numbers and/or MB, GB, TB for quota field.
 **/
function tripal_admin_manage_files_form_validate($form, &$form_state) {
  $upload_max = $form_state['values']['upload_max'];

  // Validate the quota string.
  if (!preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $upload_max)) {
    form_set_error('upload_max', t('Please provide a maximum size in the format indicated.'));
  }
}

/**
 * Implements the submit function of the tripal_admin_manage_files_form.
 **/
function tripal_admin_manage_files_form_submit($form, &$form_state) {
  $upload_max = $form_state['values']['upload_max'];

  // if the 2nd element of the qutoa string occupied by a valid suffix we need to check to see
  // what we have to multiply the value by (1024 for GB 1024^2 for TB because
  // we assume that the initial number is already in MB)
  $matches = [];
  $multiplier = 'MB';
  $size = $upload_max;
  if (preg_match("/^(\d+(?:\.\d+)*)(MB|GB|TB)$/", $upload_max, $matches)) {
    $multiplier = $matches[2];
    $size = $matches[1];
  }

  switch ($multiplier) {
    case 'GB':
      $size = (int) ($size * pow(10, 9));
      break;
    case 'TB':
      $size = (int) ($size * pow(10, 12));
      break;
    default:
      $size = (int) ($size * pow(10, 6));
      break;
  }
  variable_set('tripal_upload_max_size', $size);

  drupal_set_message('Default settings have been set.');
}

/**
 * Provides the means of setting quotas and seeing server consumption.
 *
 * @return A table of the current users with custom quotas, fieldsets for
 *         adding new users to the custom quotas, and fieldsets for setting
 *         site wide quota and expiration date.
 */
function tripal_admin_manage_quota_form($form, &$form_state) {
  // Provide overall server consumption (and space remaining)
  $total_size = 0;

  $default_quota = variable_get('tripal_default_file_quota', pow(20, 6));
  $default_expiration = variable_get('tripal_default_file_expiration', '60');

  if (array_key_exists('values', $form_state)) {
    $default_quota = $form_state['values']['default_quota'];
    $default_expiration = $form_state['values']['default_expiration_date'];
  }

  // Query file usage table for the fids that the module uses
  // Iterate through all of the files managed by the tripal module
  // and calculate the total.
  $sql = "SELECT DISTINCT fid FROM {file_usage} WHERE module = 'tripal'";
  $fids = db_query($sql);
  while ($fid = $fids->fetchObject()) {
    $sql = "SELECT filesize FROM {file_managed} WHERE fid = :fid";
    $total_size += db_query($sql, [':fid' => $fid->fid])->fetchObject()->filesize;
  }
  $form['total_size'] = [
    '#type' => 'item',
    '#title' => t('Total Current Usage'),
    '#description' => t('The total amount of space consumed by user file uploads.'),
    '#markup' => tripal_format_bytes($total_size),
  ];

  // TODO: add a D3 chart showing the amount of storage used by each user.

  $form['default_quota'] = [
    '#type' => 'textfield',
    '#title' => 'Default System-Wide User Quota',
    '#description' => t('Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).'),
    '#default_value' => tripal_format_bytes($default_quota),
  ];

  $form['default_expiration_date'] = [
    '#type' => 'textfield',
    '#title' => 'Default System-Wide Expiration Date',
    '#description' => t('The number of days that a user uploaded file can remain on the server before it is automatically removed'),
    '#default_value' => $default_expiration,
  ];

  // Populate the table from the custom quota db table (users, quota, exp date).
  $header = [
    'uid' => t('UID'),
    'user' => t('Users'),
    'custom_quota' => t('Custom Quota'),
    'exp_date' => t('Expiration Date'),
    'actions' => t('Actions'),
  ];

  // API call to the gather the users that have a custom quota
  $rows = [];
  $query = "SELECT * FROM {tripal_custom_quota}";
  $data = db_query($query);
  while ($entry = $data->fetchObject()) {
    $user = user_load($entry->uid);
    $rows[] = [
      'uid' => $user->uid,
      'user' => $user->name,
      'custom_quota' => tripal_format_bytes($entry->custom_quota),
      'exp_date' => $entry->custom_expiration,
    ];
  }

  // Add the Actions links
  foreach ($rows as $key => $entry) {
    $rows[$key]['actions'] = l('Edit', 'admin/tripal/files/quota/edit/' . $entry['uid']) . ' | ' .
      l('Remove', 'admin/tripal/files/quota/remove/' . $entry['uid']);
  }

  $form['custom'] = [
    '#type' => 'fieldset',
    '#title' => 'Custom Settings',
    '#description' => t('The settings above apply to all users.  The following allows for custom user settings that override the defaults set above.'),
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
  ];

  $form['custom']['links'] = [
    '#type' => 'markup',
    '#markup' => '<br>' . l('Add Custom User Quota', 'admin/tripal/files/quota/add'),
  ];

  $form['custom']['custom_quotas'] = [
    '#type' => 'item',
    '#title' => t('Custom User Quotas'),

    '#markup' => theme_table([
      'header' => $header,
      'rows' => $rows,
      'attributes' => [],
      'caption' => '',
      'sticky' => TRUE,
      'empty' => 'There are no custom user quotas.',
      'colgroups' => [],
    ]),
  ];

  $form['update_defaults'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
  ];

  return $form;
}


/**
 * Validate the form's values: proper numbers and/or MB, GB, TB for quota field.
 **/
function tripal_admin_manage_quota_form_validate($form, &$form_state) {
  $quota = $form_state['values']['default_quota'];
  $expiration = $form_state['values']['default_expiration_date'];

  // Validate the quota string.
  if (!preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $quota)) {
    form_set_error('default_quota', t('Please provide a quota size in the format indicated.'));
  }

  // Validate the expiration time.
  if (!preg_match("/^\d+$/", $expiration)) {
    form_set_error('default_expiration', t('Please providate a positive non-decimal numeric value for the days to expire'));
  }

}

/**
 * Write to the two drupal variables the site wide defualt quota and exp date.
 **/
function tripal_admin_manage_quota_form_submit($form, &$form_state) {
  $quota = $form_state['values']['default_quota'];
  $expiration = $form_state['values']['default_expiration_date'];

  // if the 2nd element of the qutoa string occupied by a valid suffix we need to check to see
  // what we have to multiply the value by (1024 for GB 1024^2 for TB because
  // we assume that the initial number is already in MB)
  $matches = [];
  $multiplier = 'MB';
  $size = $quota;
  if (preg_match("/^(\d+(?:\.\d+)*)(MB|GB|TB)$/", $quota, $matches)) {
    $multiplier = $matches[2];
    $size = $matches[1];
  }

  switch ($multiplier) {
    case 'GB':
      $size = (int) ($size * pow(10, 9));
      break;
    case 'TB':
      $size = (int) ($size * pow(10, 12));
      break;
    default:
      $size = (int) ($size * pow(10, 6));
      break;
  }

  // Grab the quota value and exp_date to write to the drupal variables
  variable_set('tripal_default_file_quota', $size);
  variable_set('tripal_default_file_expiration', $expiration);

  drupal_set_message('Default quota settings have been set.');
}

/**
 * API call to remove a user from the custom quota table
 *
 * @return Boolean value to confirm removal from the table.
 **/
function tripal_admin_remove_quota_form($form, &$form_state, $uid) {
  $form = [];

  $form['uid'] = [
    '#type' => 'value',
    '#value' => $uid,
  ];

  $user = user_load($uid);
  if (!$user) {
    drupal_set_message('There was a problem finding this user\'s account. Could not remove the quota', 'error');
    drupal_goto('admin/tripal/files/quota');
    return $form;
  }

  return confirm_form($form,
    t('Confirm removal of the custom quota for the user: "' . $user->name . '"?'),
    'admin/tripal/files/quota',
    t('Removal of the custom quota will enforce default quotas for the user. If the user\'s current usage exceeds the defaults the user must then remove files before more may be uploaded.')
  );
}

/**
 * Implements submit hook for the tripal_admin_remove_quota_form form.
 */
function tripal_admin_remove_quota_form_submit($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  tripal_remove_user_quota($uid);

  $user = user_load($uid);
  drupal_set_message('The custom quota for user, "' . $user->name . '", has been removed.');

  drupal_goto('admin/tripal/files/quota');
}

/**
 * Provides contents for the File Usgae page.
 */
function tripal_admin_file_usage_page() {
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('User File Management', 'admin/tripal/files');
  drupal_set_breadcrumb($breadcrumb);

  $content = [
    [
      '#type' => 'markup',
      '#markup' => 'Usage reports coming in the future...',
    ],
  ];
  return $content;
}