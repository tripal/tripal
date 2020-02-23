<?php

/**
 * @file
 * Contains API functions to set titles and paths for all chado nodes
 *
 * TITLES
 * ====================================
 * There are three steps to implement the ability to set custom titles for the
 *   node type:
 * 1) Add the 'chado_node_api' elements to the hook_node_info function().
 *   These values define the name of the base table and how to refer to nodes
 *   in singular and plural. There are additional paramaters that can be added
 *   to the 'chado_node_api' for syncing nodes (see documentation for the
 *   chado_node_sync_form() function for additional options.
 * @code
 * function modulename_node_info() {
 * return array(
 * 'chado_example' => array(
 * 'name' => t('example'),
 * 'base' => 'chado_example',
 * 'description' => t('A Chado example is a collection of material that can be
 *   sampled and have experiments performed on it.'),
 * 'has_title' => TRUE,
 * 'locked' => TRUE,
 *
 * // this is what differs from the regular Drupal-documented hook_node_info()
 * 'chado_node_api' => array(
 * 'base_table' => 'example',            // the name of the chado base table
 * 'hook_prefix' => 'chado_example',     // usually the name of the node type
 * 'record_type_title' => array(
 * 'singular' => t('Example'),         // Singular human-readable title
 * 'plural' => t('Examples')           // Plural human-readable title
 * ),
 * )
 * ),
 * );
 * }
 * @endcode
 *
 * 2) Add the "Set Page Titles" Form to the admin settings form
 * @code
 * // If the module is using the "Chado Node: Title & Path API" to allow custom
 *   titles
 * // for your node type then you need to add the configuration form for this
 *   functionality.
 * $details = array(
 * 'module' => 'tripal_example',        // the name of the MODULE implementing
 *   the content type
 * 'content_type' => 'chado_example',   // the name of the content type
 * // An array of options to use under "Page Titles"
 * // the key should be the token and the value should be the human-readable
 *   option
 * 'options' => array(
 * '[example.name]' => 'Germplasm Name Only',
 * '[example.uniquename]' => 'Germplasm Unique Name Only',
 * // there should always be one options matching the unique constraint.
 * // If you have a more human-readable constraint, then that is preferrable.
 * // See the tripal feature module for a good example of this.
 * '[example.example_id]' => 'Unique Contraint: The Chado ID for Examples'
 * ),
 * // the token indicating the unique constraint in the options array
 * 'unique_option' => '[example.example_id]'
 * );
 * // This call adds the configuration form to your current form
 * // This sub-form handles it's own validation & submit
 * chado_add_admin_form_set_title($form, $form_state, $details);
 * @endcode
 *
 * 3) Use chado_get_node_title($node) where ever you want the title for your
 *   node. This should be done in hook_load(), hook_node_insert(),
 *   hook_node_update(). The reason you set the title in the node_action hooks,
 *   which act on all nodes, is because at that point you have the generic
 *   loaded node.
 * @code
 * function tripal_example_load($nodes) {
 *
 * foreach ($nodes as $nid => $node) {
 *
 * // Add all of the custom content for your node type.
 * // See tripal_example.chado_node.api: chado_example_load()
 *
 * // Now get the title
 * $node->title = chado_get_node_title($node);
 *
 * $nodes[$nid] = $node;
 * }
 * }
 * @endcode
 *
 * Optionally define a default for a specific content type by implementing a
 *   function of the name
 * [content type]_chado_node_default_title_format() that returns a string
 *   describing the default format.
 * @code
 * function chado_example_chado_node_default_title_format() {
 * return '[example.example_id]';
 * }
 * @endcode
 * If you don't implement this then a default format based on the unique
 *   constraint for the base table of the content type will be generated.
 *
 * NODE URL/PATHS
 * ====================================
 * There are three steps to implement the ability to set custom URLs for the
 *   node type:
 * 1) Add the 'chado_node_api' elements to the hook_node_info function().
 *   These values define the name of the base table and how to refer to nodes
 *   in singular and plural. There are additional paramaters that can be added
 *   to the 'chado_node_api' for syncing nodes (see documentation for the
 *   chado_node_sync_form() function for additional options.
 * @code
 * function modulename_node_info() {
 * return array(
 * 'chado_example' => array(
 * 'name' => t('example'),
 * 'base' => 'chado_example',
 * 'description' => t('A Chado example is a collection of material that can be
 *   sampled and have experiments performed on it.'),
 * 'has_title' => TRUE,
 * 'locked' => TRUE,
 *
 * // this is what differs from the regular Drupal-documented hook_node_info()
 * 'chado_node_api' => array(
 * 'base_table' => 'example',            // the name of the chado base table
 * 'hook_prefix' => 'chado_example',     // usually the name of the node type
 * 'record_type_title' => array(
 * 'singular' => t('Example'),         // Singular human-readable title
 * 'plural' => t('Examples')           // Plural human-readable title
 * ),
 * )
 * ),
 * );
 * }
 * @endcode
 *
 * 2) Add the "Set Page URLs" Form to the admin settings form
 * @code
 * // If the module is using the "Chado Node: Title & Path API" to allow custom
 *   URLs
 * // for your node type then you need to add the configuration form for this
 *   functionality.
 * $details = array(
 * 'module' => 'tripal_example',        // the name of the MODULE implementing
 *   the content type
 * 'content_type' => 'chado_example',   // the name of the content type
 * // An array of options to use under "Page URLs"
 * // the key should be the token and the value should be the human-readable
 *   option
 * 'options' => array(
 * '/example/[example.type_id>cvterm.name]/[example.example_id]' => 'Examples
 *   separated by Type',
 * // there should always be one options matching the unique constraint.
 * // If you have a more human-readable constraint, then that is preferrable.
 * // See the tripal feature module for a good example of this.
 * '/example/[example.example_id]' => 'Unique Contraint: The Chado ID for
 *   Examples'
 * ),
 * );
 * // This call adds the configuration form to your current form
 * // This sub-form handles it's own validation & submit
 * chado_add_admin_form_set_url($form, $form_state, $details);
 * @endcode
 *
 * 3) Use chado_set_node_url($node) where ever you want to reset the URL of the
 *    node. This should be done in hook_node_insert(), hook_node_update(). The
 *   reason you set the title in the node_action hooks, which act on all nodes,
 *   is because at that point you have the generic loaded node.
 * @code
 * function tripal_example_node_insert($node) {
 *
 * // Set the URL path after inserting.
 * switch ($node->type) {
 * case 'chado_example':
 *
 * // Now use the API to set the path.
 * chado_set_node_url($node);
 *
 * break;
 * }
 * }
 * @endcode
 *
 * Optionally define a default for a specific content type by implementing a
 *   function of the name
 * [content type]_chado_node_default_url_format() that returns a string
 *   describing the default format.
 * @code
 * function chado_example_chado_node_default_url_format() {
 * return '/example/[example.example_id]';
 * }
 * @endcode
 * If you don't implement this then a default format based on the unique
 *   constraint for the base table of the content type will be generated.
 */

/**
 * @section
 * Set Titles
 */

/**
 * Get the title of a node based on the Title Format set in the admin
 * section of the module. If the format has not yet been set than
 * the the unique constrain and name fields will be used to generate
 * a default format
 *
 * @param $node
 *   The node object
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_get_node_title($node) {
  $content_type = $node->type;

  // Get the tokens and format
  $tokens = []; // this will be set by chado_node_get_title_format
  $title = chado_node_get_title_format($content_type, $tokens);

  // Determine which tokens were used in the format string
  if (preg_match_all('/\[[^]]+\]/', $title, $used_tokens)) {

    // Get the value for each token used
    foreach ($used_tokens[0] as $token) {
      $token_info = $tokens[$token];
      if (!empty($token_info)) {
        $value = chado_get_token_value($token_info, $node);
        $title = str_replace($token, $value, $title);
      }
    }
  }
  else {
    return $title;
  }

  return $title;
}

/**
 * Generic "Set Node Title" sub-form for setting the title of any chado node
 *
 * @param $form
 *   The Drupal form array into which the property form elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $details
 *   An array defining details used by this form.
 *   Required keys that are always required:
 *     -module: the name of the module implementing the node. For example, for
 *   features the module is tripal_feature.
 *     -options: an array of quick-choice options to supply to the user. The
 *   key should be the token and the value should be a human-readable
 *   description of the option
 *     -content_type: the name of the content type. Defaults to module name.
 *   Optional keys include:
 *     -fieldset_title: the title to use for the fieldset. Defaults to "Set
 *   Page Title".
 *     -default_option: the default format to use which matches one of those in
 *   $details['options']
 *     -custom_tokens: an array of custom tokens that follow the same format as
 *   those generated by chado_node_generate_tokens().
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_add_admin_form_set_title(&$form, &$form_state, $details) {

  // Get Node Info
  if (isset($details['module'])) {
    $node_info = call_user_func($details['module'] . '_node_info');
    $chado_node_api = $node_info[$details['content_type']]['chado_node_api'];
  }
  else {
    tripal_report_error(
      'chado_node_api',
      TRIPAL_ERROR,
      "Set Titles API: When calling chado_add_admin_form_set_title, you \$details array must include 'module' => [name of your module] in order to pull out all the information provided in your implementation of hook_node_info"
    );
  }

  // Defaults
  $details['fieldset_title'] = (isset($details['fieldset_title'])) ? $details['fieldset_title'] : 'Set Page Titles';
  $details['additional_instructions'] = (isset($details['additional_instructions'])) ? $details['additional_instructions'] : '';
  $details['custom_tokens'] = (isset($details['custom_tokens'])) ? $details['custom_tokens'] : [];
  $details['content_type'] = (isset($details['content_type'])) ? $details['content_type'] : $details['module'];

  $tokens = [];
  if (empty($tokens)) {
    $tokens = chado_node_generate_tokens($chado_node_api['base_table']);
  }
  $tokens = array_merge($tokens, $details['custom_tokens']);
  $token_list = chado_node_format_tokens($tokens);
  $details['default_option'] = (isset($details['default_option'])) ? $details['default_option'] : chado_node_get_title_format($details['content_type'], $tokens);

  // FORM PROPER
  $msg = t(
    'Each synced %singular must have a unique page title, however, %plural may have the
      same name if they are of different types or from different organisms. Therefore,
      we must be sure that the page titles can uniquely identify the %singular being viewed.
      Select an option below that will uniquely identify all %plural on your site.'
    . $details['additional_instructions'],
    [
      '%singular' => $chado_node_api['record_type_title']['singular'],
      '%plural' => $chado_node_api['record_type_title']['plural'],
    ]
  );
  $form['set_titles'] = [
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => $msg,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => "<div id='set_titles-fieldset'>",
    '#suffix' => '</div>',
  ];

  $form['set_titles']['content_type'] = [
    '#type' => 'hidden',
    '#value' => $node_info[$details['content_type']]['base'],
  ];

  $details['options']['custom'] = 'Custom: See the text field below.';
  $form['set_titles']['title_option'] = [
    '#title' => t('%singular Page Titles', ['%singular' => $chado_node_api['record_type_title']['singular']]),
    '#type' => 'radios',
    '#description' => t("Choose a title type from the list above that is
      guaranteed to be unique for all %plural. If in doubt it is safest to choose
      the 'Unique Constaint' option as that guarantees uniqueness.",
      ['%plural' => $chado_node_api['record_type_title']['plural']]),
    '#required' => FALSE,
    '#options' => $details['options'],
    '#default_value' => (isset($details['options'][$details['default_option']])) ? $details['default_option'] : 'custom',
  ];

  $form['set_titles']['title_format_variable'] = [
    '#type' => 'hidden',
    '#value' => $details['module'] . '_title_format',
  ];

  $form['set_titles']['custom_title'] = [
    '#type' => 'textarea',
    '#title' => 'Custom Page Title',
    '#description' => 'You may rearrange elements in this text box to customize the page
      titles. The available tokens are listed below. You can separate or include any text
      between the tokens. <strong>Important: be sure that whatever you choose
      will always be unique even considering future data that may be added. If in doubt,
      please select the unique constraint title option above.</strong>',
    '#default_value' => $details['default_option'],
    '#rows' => 1,
  ];

  $form['set_titles']['token_display'] = [
    '#type' => 'fieldset',
    '#title' => 'Available Tokens',
    '#description' => 'Copy the token and paste it into the "Custom Page Title" text field above.',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $form['set_titles']['token_display']['content'] = [
    '#type' => 'item',
    '#markup' => $token_list,
  ];

  $form['set_titles']['tokens'] = [
    '#type' => 'hidden',
    '#value' => serialize($tokens),
  ];

  $form['set_titles']['submit'] = [
    '#type' => 'submit',
    '#value' => 'Set Titles',
    '#validate' => ['chado_add_admin_form_set_title_form_validate'],
    '#submit' => ['chado_add_admin_form_set_title_form_submit'],
  ];

}

/**
 * Implements hook_form_validate().
 * VALIDATE: validate the format.
 */
function chado_add_admin_form_set_title_form_validate($form, $form_state) {

  // Ensure that all tokens used in the format are in the tokens list
  if (preg_match_all('/\[[^]]+\]/', $form_state['values']['custom_title'], $used_tokens)) {
    $token_list = unserialize($form_state['values']['tokens']);
    foreach ($used_tokens[0] as $token) {
      if (!array_key_exists($token, $token_list)) {
        form_set_error('custom_title', 'All tokens used must be in the "Available Tokens" list. Please make sure not to use [ or ] unless it\'s denoting a token');
      }
    }
  }

}

/**
 * Implements hook_form_submit().
 * SUBMIT: Actually add the format specified by chado_add_admin_form_set_title()
 */
function chado_add_admin_form_set_title_form_submit($form, $form_state) {

  if ($form_state['values']['title_option'] == 'custom') {
    $format = $form_state['values']['custom_title'];
  }
  else {
    $format = $form_state['values']['title_option'];
  }

  chado_node_add_token_format('title', $form_state['values']['content_type'], $format, $form_state['values']['tokens']);
}

/**
 * Get the title format for a specific content type
 *
 * If the title format has not yet been set then the following will be done
 *  1) Check to see if there is a legacy title format set (features & stocks)
 *  2) Check if there is a defined default for this content type
 *  3) Create a format using any name fields and the unique constraint for the
 *     base table associated with this content type
 *
 * Define a default for a specific content type by implementing a function of
 * the name
 * [content type]_chado_node_default_title_format() that returns a string
 * describing the default format.
 *
 * @param $content_type
 *   The name of the content (node) type you are interested in (ie:
 *   chado_feature)
 * @param $tokens
 *   An array, passed by reference that is filled to include the tokens for
 *   this
 *   node type.  Each token is an array with the following keys:
 *    -table: the name of the chado table
 *    -field: the name of the field in the above table
 *    -token: the token string (ie: [stock.stock_id])
 *    -description: a very short description of the token (displayed when
 *   tokens are listed)
 *    -location: the location of the value in a chado node variable with each
 *   level separated by an arrow (->) symbol. For example, the location for
 *   $node->feature->type_id->name is feature>type_id>name
 *
 * @return
 *   A string containing tokens describing the default format for the title of
 *   nodes of the specified content type.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_title_format($content_type, &$tokens, $base_table = NULL) {
  $format_record_format = $format = '';
  $format_record_tokens = '';

  // Is there a title format set?
  $format_record = chado_node_get_token_format('title', $content_type, ['return_record' => TRUE]);
  if (!empty($format_record)) {
    $format_record_format = $format = $format_record->format;
    $format_record_tokens = $tokens = $format_record->tokens;
  }

  // All three options below need the tokens to be generated so do that now
  if (empty($format)) {
    if (empty($base_table)) {
      $base_table = chado_node_get_base_table($content_type);
    }
    $tokens = chado_node_generate_tokens($base_table);
  }

  // 1) Check for legacy format
  if (empty($format)) {
    $format = chado_node_get_legacy_title_default($content_type);
  }

  // 2) Module-defined default format
  if (empty($format)) {
    $hook = $content_type . '_chado_node_default_title_format';
    if (function_exists($hook)) {
      $format = call_user_func($hook);
    }
  }

  // 3) Create unique constraint format
  if (empty($format)) {
    if (empty($base_table)) {
      $base_table = chado_node_get_base_table($content_type);
    }
    $format = chado_node_get_unique_constraint_format($base_table);
  }

  // Add the format to table so we can use it later
  // (optimization: to speed up bulk updates, don't update the record if
  // the format and tokens are unchanged)
  if ($format != $format_record_format || $tokens != $format_record_tokens) {
    chado_node_add_token_format('title', $content_type, $format, $tokens);
  }

  return $format;
}

/**
 * Handles legacy title options
 *
 * Features & Stocks already had custom functionality to handle title
 * setting before this API was created. That has since been removed but
 * but to remain backwards compatible this function checks for those
 * old settings and translates them into new defaults.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_legacy_title_default($content_type) {
  if ($content_type == 'chado_feature') {
    $legacy_option = variable_get('chado_feature_title', 'unique_constraint');
    switch ($legacy_option) {
      case 'feature_unique_name':
        $default_title_format = '[feature.uniquename]';
        break;
      case 'feature_name':
        $default_title_format = '[feature.name]';
        break;
      case 'unique_constraint':
        $default_title_format = '[feature.name], [feature.uniquename] ([feature.type_id>cvterm.name]) [feature.organism_id>organism.genus] [feature.organism_id>organism.species]';
        break;
    }
    return $default_title_format;
  }
  elseif ($content_type == 'chado_stock') {
    $legacy_option = variable_get('chado_stock_title', 'unique_constraint');
    switch ($legacy_option) {
      case 'stock_unique_name':
        $default_title_format = '[stock.uniquename]';
        break;
      case 'stock_name':
        $default_title_format = '[stock.name]';
        break;
      case 'unique_constraint':
        $default_title_format = '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]';
        break;
    }
    return $default_title_format;
  }
  else {
    return FALSE;
  }
}

/**
 * @section
 * Set Paths
 */

/**
 * Get the url of a node based on the url Format set in the admin
 * section of the module. If the format has not yet been set than
 * the the unique constrain and name fields will be used to generate
 * a default format
 *
 * @param $node
 *   The node object
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_get_node_url($node) {
  $content_type = $node->type;

  // Get the tokens and format
  $tokens = []; // this will be set by chado_node_get_url_format
  $url = chado_node_get_url_format($content_type, $tokens);

  // Determine which tokens were used in the format string
  if (preg_match_all('/\[[^]]+\]/', $url, $used_tokens)) {

    // Get the value for each token used
    foreach ($used_tokens[0] as $token) {
      $token_info = $tokens[$token];
      if (!empty($token_info)) {
        $value = chado_get_token_value($token_info, $node);
        if (is_string($value)) {
          $url = str_replace($token, $value, $url);
        }
        else {
          tripal_report_error('chado_node_api', TRIPAL_ERROR,
            'Unable to replace %token. The value in the node should be a string but is instead: \'%value\'',
            ['%token' => $token, '%value' => print_r($value, TRUE)]
          );
        }
      }
    }
  }
  else {
    return $url;
  }

  return $url;
}

/**
 * Set the URL for a given node.
 *
 * Note: This makes the old URL completely invalid which breaks bookmarks.
 * Furthermore, if someone attempts to go to an old URL they will get a white
 * screen PDO error which is not very user friendly ;-)
 *
 * @todo handle re-directing for old URLs or at least ensure a page not found
 *   error is displayed.
 *
 * @param $node
 *   The node to set the URL for.  The node object must have at a
 *   minimum the 'nid' and 'type' properties, as well as the
 *   chado object (e.g. 'organism' for chado_organism node type, and
 *   'feature' for chado_feature node type, etc.).
 *
 * @return
 *   The URL alias that was set.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_set_node_url($node) {

  // Get the global variable determining the language of the current content.
  global $language_content;

  // First we need to get the URL alias.
  $url_alias = chado_get_node_url($node);

  // And remove any forward slashes since those wreak havok.
  $url_alias = preg_replace('/^\//', '', $url_alias);

  // Use the node to determine the source/original URL.
  $source_url = 'node/' . $node->nid;

  // Ensure that there are no spaces and other non-friendly characters are
  // sanitized.
  $url_alias = preg_replace('/\s+/', '-', $url_alias);

  // Only bother setting the alias if there is one.
  if (!empty($url_alias) AND $source_url != $url_alias) {

    // First we need to check if the alias already exists.
    $path = path_load(['source' => $source_url, 'alias' => $url_alias]);

    // Etierh there isn't an alias yet so we just create one.
    if (empty($path)) {
      $path = [
        'source' => $source_url,
        'alias' => $url_alias,
      ];
      path_save($path);
    }
    // Or an Alias already exists but we would like to add a new one.
    elseif ($path['alias'] != $url_alias) {
      $path = [
        'source' => $source_url,
        'alias' => $url_alias,
      ];
      path_save($path);
    }
    // If the Alias already exists we want to double check that it's for the
    // current node. Otherwise we should warn the administrator that the path
    // format they chose isn't unique.
    else {

      $num_aliases = db_query('SELECT count(*) as num_alias FROM {url_alias} WHERE alias=:alias AND source!=:source',
        [':alias' => $url_alias, ':source' => $source_url])->fetchField();

      // If there is an alias with a different source than warn the admin.
      if ($num_aliases > 0) {
        tripal_report_error('chado_node_api', TRIPAL_WARNING,
          'URL Alias: The URL format for %content-type is not unique. Specifically, %alias was almost added multiple times.',
          [
            '%content-type' => $node->type,
            '%alias' => $url_alias,
          ]
        );
      }
    }

  }

  return $url_alias;
}

/**
 * Tripal Job for updating all node URLs for a particular node type.
 *
 * @param $content_type
 *   The machine name of the node type to update URLs for.
 * @param $job_id
 *   The ID of the tripal job calling this function.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_update_existing_node_urls($content_type, $job_id = 0) {
  $transaction = db_transaction();

  print "\nNOTE: Setting of URLs is performed using a database transaction. \n" .
    "If the load fails or is terminated prematurely then the entire set of \n" .
    "new URLs will be rolled back and no changes will be made.\n\n";

  try {
    // Get the number of records we need to set URLs for.
    $num_nodes = db_query('SELECT count(*) FROM {' . $content_type . '}')->fetchField();

    // Calculate the interval at which we will print an update on the screen.
    $num_set = 0;
    $num_per_interval = 100;

    if ($num_nodes > 0) {

      print "There are $num_nodes nodes to update URLs for. You will be \n" .
        "updated on the progress every 100 nodes.\n\n";

      // Get the list of nodes of this particular content type.
      $query = new EntityFieldQuery();
      $result = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', $content_type)
        ->execute();

      if (isset($result['node'])) {
        $nids = array_keys($result['node']);

        foreach ($nids as $nid) {

          // Load the current node. Normally here we would use the node_load()
          // function that is part of the Drupal API.  However, this seems
          // to have memory leaks that have not yet been identified, so
          // we'll create the object by hand:
          $node = new stdClass();
          $node->nid = $nid;
          $node->type = $content_type;
          $table = preg_replace('/chado_/', '', $content_type);
          $id = chado_get_id_from_nid($table, $nid);
          $node->$table = chado_generate_var($table, [$table . "_id" => $id]);

          // Now set the URL for the current node.
          $alias = chado_set_node_url($node);

          // update the job status every 1% nodes
          if ($num_set % $num_per_interval == 0) {
            $percent = ($num_set / $num_nodes) * 100;

            // Update the user on progress.
            $percent = sprintf("%.2f", $percent);
            print "Setting URLs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";

            // Update the tripal job.
            if ($job_id) {
              tripal_set_job_progress($job_id, intval($percent));
            }
          }
          $num_set++;
        }
      }
      $percent = ($num_set / $num_nodes) * 100;
      tripal_set_job_progress($job_id, intval($percent));
      $percent = sprintf("%.2f", $percent);
      print "Setting URLs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      print "\nDone. Set " . number_format($num_set) . " URLs\n";
    }
    else {
      print "\nThere are no $content_type nodes to update. If you know there\n"
        . "are records of this type in chado, try sync'ing them to Drupal.\n";
    }
  } catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('chado_node_api', $e);
    tripal_report_error(
      'chado_node_api',
      TRIPAL_WARNING,
      'Unable to update URLs for the :content-type Node Type. Specifically, the last URL attempted was NID=:nid and Alias=:alias',
      [
        ':content-type' => $content_type,
        ':nid' => $node->nid,
        ':alias' => $alias,
      ]
    );
  }
}

/**
 * Generic "Set Node URL" sub-form for setting the url of any chado node
 *
 * @param $form
 *   The Drupal form array into which the property form elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $details
 *   An array defining details used by this form.
 *   Required keys that are always required:
 *     -module: the name of the module implementing the node. For example, for
 *   features the module is tripal_feature.
 *     -options: an array of quick-choice options to supply to the user. The
 *   key should be the token and the value should be a human-readable
 *   description of the option Optional keys include:
 *     -content_type: the name of the content type. Defaults to module name.
 *     -fieldset_title: the title to use for the fieldset. Defaults to "Set
 *   Page url".
 *     -default_option: the default format to use which matches one of those in
 *   $details['options']
 *     -custom_tokens: an array of custom tokens that follow the same format as
 *   those generated by chado_node_generate_tokens().
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_add_admin_form_set_url(&$form, &$form_state, $details) {

  // Get Node Info
  if (isset($details['module'])) {
    $node_info = call_user_func($details['module'] . '_node_info');
    $chado_node_api = $node_info[$details['content_type']]['chado_node_api'];
  }
  else {
    tripal_report_error(
      'chado_node_api',
      TRIPAL_ERROR,
      "Set URL API: When calling chado_add_admin_form_set_url, you \$details array must include 'module' => [name of your module] in order to pull out all the information provided in your implementation of hook_node_info"
    );
  }

  // Defaults
  $details['fieldset_title'] = (isset($details['fieldset_title'])) ? $details['fieldset_title'] : 'Set Page URLs';
  $details['additional_instructions'] = (isset($details['additional_instructions'])) ? $details['additional_instructions'] : '';
  $details['custom_tokens'] = (isset($details['custom_tokens'])) ? $details['custom_tokens'] : [];
  $details['content_type'] = (isset($details['content_type'])) ? $details['content_type'] : $details['module'];

  $tokens = [];
  if (empty($tokens)) {
    $tokens = chado_node_generate_tokens($chado_node_api['base_table']);
  }
  $tokens = array_merge($tokens, $details['custom_tokens']);
  $token_list = chado_node_format_tokens($tokens);
  $current_format = chado_node_get_url_format($details['content_type'], $tokens);
  $details['default_option'] = (isset($details['default_option'])) ? $details['default_option'] : $current_format;

  // FORM PROPER
  $msg = t(
    'Each synced %singular must have a unique page URL, however, %plural may have the
      same name if they are of different types or from different organisms. Therefore,
      we must be sure that the page URLs can uniquely identify the %singular being viewed.
      Select an option below that will uniquely identify all %plural on your site.'
    . $details['additional_instructions'],
    [
      '%singular' => $chado_node_api['record_type_title']['singular'],
      '%plural' => $chado_node_api['record_type_title']['plural'],
    ]
  );
  $form['set_url'] = [
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => $msg,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => "<div id='set_url-fieldset'>",
    '#suffix' => '</div>',
  ];

  $form['set_url']['content_type'] = [
    '#type' => 'hidden',
    '#value' => $node_info[$details['content_type']]['base'],
  ];

  $details['options']['custom'] = 'Custom: See the text field below.';

  $form['set_url']['url_option'] = [
    '#title' => t('%singular Page URL', ['%singular' => $chado_node_api['record_type_title']['singular']]),
    '#type' => 'radios',
    '#description' => t("Choose a URL type from the list above that is
      guaranteed to be unique for all %plural. If in doubt it is safest to choose
      the 'Unique Constaint' option as that guarantees uniqueness.",
      ['%plural' => $chado_node_api['record_type_title']['plural']]),
    '#required' => FALSE,
    '#options' => $details['options'],
    '#default_value' => (isset($details['options'][$details['default_option']])) ? $details['default_option'] : 'custom',
  ];

  $form['set_url']['url_format_variable'] = [
    '#type' => 'hidden',
    '#value' => $details['module'] . '_url_format',
  ];

  $form['set_url']['custom_url'] = [
    '#type' => 'textarea',
    '#title' => 'Custom Page URL',
    '#description' => 'You may rearrange elements in this text box to customize the page
      URLs. The available tokens are listed below. You can separate or include any text
      between the tokens. <strong>Important: be sure that whatever you choose
      will always be unique even considering future data that may be added. If in doubt,
      please select the unique constraint title option above.</strong>',
    '#default_value' => $current_format,
    '#rows' => 1,
  ];

  $form['set_url']['token_display'] = [
    '#type' => 'fieldset',
    '#title' => 'Available Tokens',
    '#description' => 'Copy the token and paste it into the "Custom Page URL" text field above.',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $form['set_url']['token_display']['content'] = [
    '#type' => 'item',
    '#markup' => $token_list,
  ];

  $form['set_url']['tokens'] = [
    '#type' => 'hidden',
    '#value' => serialize($tokens),
  ];

  $form['set_url']['submit'] = [
    '#type' => 'submit',
    '#value' => 'Set URLs',
    '#validate' => ['chado_add_admin_form_set_url_form_validate'],
    '#submit' => ['chado_add_admin_form_set_url_form_submit'],
  ];

}

/**
 * Implements hook_form_validate().
 * VALIDATE: validate the format.
 */
function chado_add_admin_form_set_url_form_validate($form, $form_state) {

  // Ensure that all tokens used in the format are in the tokens list
  if (preg_match_all('/\[[^]]+\]/', $form_state['values']['custom_url'], $used_tokens)) {
    $token_list = unserialize($form_state['values']['tokens']);
    foreach ($used_tokens[0] as $token) {
      if (!array_key_exists($token, $token_list)) {
        form_set_error('custom_url', 'All tokens used must be in the "Available Tokens" list. Please make sure not to use [ or ] unless it\'s denoting a token');
      }
    }
  }

}

/**
 * Implements hook_form_submit().
 * SUBMIT: Actually add the format specified by chado_add_admin_form_set_title()
 */
function chado_add_admin_form_set_url_form_submit($form, $form_state) {

  if ($form_state['values']['url_option'] == 'custom') {
    $format = $form_state['values']['custom_url'];
  }
  else {
    $format = $form_state['values']['url_option'];
  }

  // Add the format to the table for all new nodes.
  chado_node_add_token_format(
    'url',
    $form_state['values']['content_type'],
    $format,
    $form_state['values']['tokens']
  );

  // Add a job to Update all existing nodes.
  global $user;
  tripal_add_job(
    "Set " . $form_state['values']['content_type'] . " URLs for all existing nodes.",
    $form_state['values']['content_type'],
    'chado_update_existing_node_urls',
    ['content_type' => $form_state['values']['content_type']],
    $user->uid
  );
}

/**
 * Get the url format for a specific content type
 *
 * If the url format has not yet been set then the following will be done
 *  1) Check to see if there is a legacy url format set (features & stocks)
 *  2) Check if there is a defined default for this content type
 *  3) Create a format using any name fields and the unique constraint for the
 *     base table associated with this content type
 *
 * Define a default for a specific content type by implementing a function of
 * the name
 * [content type]_chado_node_default_url_format() that returns a string
 * describing the default format.
 *
 * @param $content_type
 *   The name of the content (node) type you are interested in (ie:
 *   chado_feature)
 * @param $tokens
 *   An array, passed by reference that is filled to include the tokens for
 *   this
 *   node type.  Each token is an array with the following keys:
 *    -table: the name of the chado table
 *    -field: the name of the field in the above table
 *    -token: the token string (ie: [stock.stock_id])
 *    -description: a very short description of the token (displayed when
 *   tokens are listed)
 *    -location: the location of the value in a chado node variable with each
 *   level separated by an arrow (->) symbol. For example, the location for
 *   $node->feature->type_id->name is feature>type_id>name
 *
 * @return
 *   A string containing tokens describing the default format for the url of
 *   nodes of the specified content type.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_url_format($content_type, &$tokens, $base_table = NULL) {
  $format_record_format = $format = '';
  $format_record_tokens = '';

  // Is there a url format set?
  $format_record = chado_node_get_token_format('url', $content_type, ['return_record' => TRUE]);
  if (!empty($format_record)) {
    $format_record_format = $format = $format_record->format;
    $format_record_tokens = $tokens = $format_record->tokens;
  }

  // All three options below need the tokens to be generated so do that now
  if (empty($format)) {
    if (empty($base_table)) {
      $base_table = chado_node_get_base_table($content_type);
    }
    if (empty($tokens)) {
      $tokens = chado_node_generate_tokens($base_table);
    }
  }

  // 1) Check for legacy format
  if (empty($format)) {
    $format = chado_node_get_legacy_url_default($content_type);
  }

  // 2) Module-defined default format
  if (empty($format)) {
    $hook = $content_type . '_chado_node_default_url_format';
    if (function_exists($hook)) {
      $format = call_user_func($hook);
    }
  }

  // 3) Create unique constraint format
  if (empty($format)) {
    if (empty($base_table)) {
      $base_table = chado_node_get_base_table($content_type);
    }
    $format = chado_node_get_unique_constraint_format($base_table, 'url');
  }

  // Add the format to table so we can use it later
  // (optimization: to speed up bulk updates, don't update the record if
  // the format and tokens are unchanged)
  if ($format != $format_record_format || $tokens != $format_record_tokens) {
    chado_node_add_token_format('url', $content_type, $format, $tokens);
  }

  return $format;
}

/**
 * Handles legacy URL options
 *
 * Features, Projects & Stocks already had custom functionality to handle URL
 * setting before this API was created. That has since been removed but
 * to remain backwards compatible this function checks for those
 * old settings and translates them into new defaults.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_legacy_url_default($content_type) {

  if ($content_type == 'chado_feature') {
    $legacy_format = variable_get('chado_feature_url_string', NULL);
    $legacy_tokens = [
      '[id]' => '[feature.feature_id]',
      '[genus]' => '[feature.organism_id>organism.genus]',
      '[species]' => '[feature.organism_id>organism.species]',
      '[type]' => '[feature.type_id>cvterm.name]',
      '[uniquename]' => '[feature.uniquename]',
      '[name]' => '[feature.name]',
    ];
    if ($legacy_format) {
      return str_replace(array_keys($legacy_tokens), $legacy_tokens, $legacy_format);
    }
  }
  elseif ($content_type == 'chado_stock') {
    $legacy_format = variable_get('chado_stock_url_string', NULL);
    $legacy_tokens = [
      '[id]' => '[stock.stock_id]',
      '[genus]' => '[stock.organism_id>organism.genus]',
      '[species]' => '[stock.organism_id>organism.species]',
      '[type]' => '[stock.type_id>cvterm.name]',
      '[uniquename]' => '[stock.uniquename]',
      '[name]' => '[stock.name]',
    ];
    if ($legacy_format) {
      return str_replace(array_keys($legacy_tokens), $legacy_tokens, $legacy_format);
    }
  }
  elseif ($content_type == 'chado_project') {
    $legacy_format = variable_get('chado_project_url_string', NULL);
    $legacy_tokens = [
      '[id]' => '[project.project_id]',
      '[name]' => '[project.name]',
    ];
    if ($legacy_format) {
      return str_replace(array_keys($legacy_tokens), $legacy_tokens, $legacy_format);
    }
  }

  return FALSE;
}

/**
 * @section
 * Tokens
 */

/**
 * Save a format to be used by chado_get_node_title() or chado_get_node_url()
 *
 * @param $application
 *   What the format is to be applied to. For example 'title' for generating
 *   node titles and 'path' for generating node paths
 * @param $content_type
 *   The name of the content type this format applies to (ie: $node->type)
 * @param $format
 *   A string including tokens used to generate the title/path (which is based
 *   on $application)
 * @param $tokens
 *   An array of tokens generated by chado_node_generate_tokens(). This is
 *   saved to ensure the tokens that are available when the format is created
 *   are still available when it's used
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_add_token_format($application, $content_type, $format, $tokens) {

  if (is_array($tokens)) {
    $tokens = serialize($tokens);
  }

  $record = [
    'content_type' => $content_type,
    'application' => $application,
    'format' => $format,
    'tokens' => $tokens,
  ];

  // Check if it already exists
  $id = db_query('SELECT tripal_format_id FROM {tripal_token_formats} WHERE content_type=:type AND application=:application', [
    ':type' => $record['content_type'],
    ':application' => $record['application'],
  ])->fetchField();
  if ($id) {
    drupal_write_record('tripal_token_formats', $record, [
      'content_type',
      'application',
    ]);
  }
  else {
    drupal_write_record('tripal_token_formats', $record);
  }

}

/**
 * Get the format for the given application of a given content type (ie: the
 * feature title)
 *
 * @param $application
 *   What the format is to be applied to. For example 'title' for generating
 *   node titles and 'path' for generating node paths
 * @param $content_type
 *   The name of the content type this format applies to (ie: $node->type)
 * @param $options
 *   An array of any of the following options:
 *    - return_record: if TRUE this will return the entire record rather
 *      than just the format string
 *
 * @return
 *   A string specifying the format
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_token_format($application, $content_type, $options = []) {

  $format_record = db_select('tripal_token_formats', 't')
    ->fields('t')
    ->condition('content_type', $content_type, '=')
    ->condition('application', $application, '=')
    ->execute()
    ->fetchObject();

  if (is_object($format_record)) {
    if (isset($options['return_record'])) {
      $format_record->tokens = unserialize($format_record->tokens);
      return $format_record;
    }
    else {
      return $format_record->format;
    }
  }
  else {
    return FALSE;
  }
}

/**
 * Generate the unique constraint for a given base table using the
 * Chado Schema API definition
 *
 * @param $base_table
 *   The base table to generate the unique constraint format for
 * @param $format_type
 *   The type of format to return. This should be one of 'title' or 'url'.
 *
 * @return
 *   A format string including tokens describing the unique constraint
 *   including all name fields
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_unique_constraint_format($base_table, $format_type = 'title') {

  $table_descrip = chado_get_schema($base_table);

  // Find the name/uniquename from the base table
  $names = [];
  foreach ($table_descrip['fields'] as $field_name => $field) {
    if (preg_match('/name/', $field_name)) {
      $names[$field_name] = "[$base_table.$field_name]";
    }

  }
  uksort($names, 'tripal_sort_key_length_asc');


  // Get tokens to match the unique key
  $tokens = [];
  foreach ($table_descrip['unique keys'] as $keyset) {
    foreach ($keyset as $key) {
      if (isset($names[$key])) {
        // Do not add it into the tokens if it's already in the names
        // since we don't want it repeated 2X
      }
      elseif ($key == 'type_id') {
        $tokens[$key] = "[$base_table.type_id>cvterm.name]";
      }
      elseif ($key == 'organism_id') {
        $tokens[$key] = "[$base_table.organism_id>organism.abbreviation]";
      }
      else {
        $tokens[$key] = "[$base_table.$key]";
      }
    }
  }

  if ($format_type == 'title') {
    $format = implode(', ', $names) . ' (' . implode(', ', $tokens) . ')';
  }
  elseif ($format_type == 'url') {
    // We don't want to put more than one name in the URL. Thus we are
    // arbitrarily grabbing the longest name token since it it likely the
    // uniquename.
    $format = implode('/', $tokens) . '/' . array_pop($names);
  }
  else {
    $format = FALSE;
    tripal_report_error(
      'tripal_node_api',
      TRIPAL_ERROR,
      'Unable to determine the format for the unique constraint since the format type (%format-type) is not supported (only "title" and "url" are at this time).',
      ['%format-type' => $format_type]
    );
  }

  return $format;
}

/**
 * Generate tokens for a particular base table
 *
 * @param $base_table
 *   The name of the chado table you would like to generate tokens for
 * @param $token_prefix
 *   RECURSIVE ARG: Used to determine the generic token based on previous
 *   interations. For example, when adding cvterm fields to a feature token,
 *   the token_prefix is "feature.type_id" so that resulting tokens can be
 *   "feature.type_id>cvterm.*" (ie: [feature.type_id>cvterm.name] )
 * @param $location_prefix
 *   RECURSIVE ARG: Used to keep track of the location of the value based on
 *   previous interations. For example, when adding cvterm fields to a feature
 *   token, the location_prefix is "feature > type_id" so that resulting tokens
 *   can be "feature > type_id > *" (ie: feature > type_id > name)
 *
 * @return
 *   An array of available tokens where the key is the table.field and the
 *   value is an array with the following keys:
 *    -table: the name of the chado table
 *    -field: the name of the field in the above table
 *    -token: the token string (ie: [stock.stock_id])
 *    -description: a very short description of the token (displayed when
 *   tokens are listed)
 *    -location: the location of the value in a chado node variable with each
 *   level separated by an arrow (->) symbol. For example, the location for
 *   $node->feature->type_id->name is feature>type_id>name
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_generate_tokens($base_table, $token_prefix = FALSE, $location_prefix = FALSE) {

  $tokens = [];
  $table_descrip = chado_get_schema($base_table);
  foreach ($table_descrip['fields'] as $field_name => $field_details) {

    if (empty($token_prefix)) {
      $token = '[' . $base_table . '.' . $field_name . ']';
      $location = implode(' > ', [$base_table, $field_name]);
    }
    else {
      $token = '[' . $token_prefix . '>' . $base_table . '.' . $field_name . ']';
      $location = $location_prefix . ' > ' . $field_name;
    }


    $tokens[$token] = [
      'name' => ucwords(str_replace('_', ' ', $base_table)) . ': ' . ucwords(str_replace('_', ' ', $field_name)),
      'table' => $base_table,
      'field' => $field_name,
      'token' => $token,
      'description' => array_key_exists('description', $field_details) ? $field_details['description'] : '',
      'location' => $location,
    ];

    if (!array_key_exists('description', $field_details) or preg_match('/TODO/', $field_details['description'])) {
      $tokens[$token]['description'] = 'The ' . $field_name . ' field of the ' . $base_table . ' table.';
    }
  }

  // RECURSION:
  // Follow the foreign key relationships recursively
  if (array_key_exists('foreign keys', $table_descrip)) {
    foreach ($table_descrip['foreign keys'] as $table => $details) {
      foreach ($details['columns'] as $left_field => $right_field) {
        if (empty($token_prefix)) {
          $sub_token_prefix = $base_table . '.' . $left_field;
          $sub_location_prefix = implode(' > ', [$base_table, $left_field]);
        }
        else {
          $sub_token_prefix = $token_prefix . '>' . $base_table . ':' . $left_field;
          $sub_location_prefix = $location_prefix . ' > ' . implode(' > ', [
              $base_table,
              $left_field,
            ]);
        }

        $sub_tokens = chado_node_generate_tokens($table, $sub_token_prefix, $sub_location_prefix);
        if (is_array($sub_tokens)) {
          $tokens = array_merge($tokens, $sub_tokens);
        }
      }
    }
  }

  return $tokens;
}

/**
 * Retrieve the value of the token from the node based on the
 * $token_info['location']
 *
 * @param $token_info
 *   An array of information about the token including:
 *    -table: the name of the chado table
 *    -field: the name of the field in the above table
 *    -token: the token string (ie: [stock.stock_id])
 *    -description: a very short description of the token (displayed when
 *   tokens are listed)
 *    -location: the location of the value in a chado node variable with each
 *   level separated by an arrow (>) symbol. For example, the location for
 *   $node->feature->type_id->name is feature>type_id>name
 * @param $node
 *   The node to get the value of the token
 *
 * @return
 *   The value of the token
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_get_token_value($token_info, $node, $options = []) {

  $token = $token_info['token'];
  $table = $token_info['table'];
  $var = $node;

  $supress_errors = (isset($options['supress_errors'])) ? $options['supress_errors'] : FALSE;

  // Iterate through each portion of the location string. An example string
  // might be:  stock > type_id > name.
  $location = explode('>', $token_info['location']);
  foreach ($location as $index) {
    $index = trim($index);

    // if $var is an object then it is the $node object or a table
    // that has been expanded.
    if (is_object($var)) {
      // check to see if the index is a member of the object. If so,
      // then reset the $var to this value.
      if (property_exists($var, $index)) {
        $var = $var->$index;
      }
      else {
        if (!$supress_errors) {
          tripal_report_error('chado_node_api', TRIPAL_WARNING,
            'Tokens: Unable to determine the value of %token. Things went awry when trying ' .
            'to access \'%index\' for the following: \'%var\'.',
            [
              '%token' => $token,
              '%index' => $index,
              '%var' => print_r($var, TRUE),
            ]
          );
        }
        return '';
      }
    }
    // if the $var is an array then there are multiple instances of the same
    // table in a FK relationship (e.g. relationship tables)
    elseif (is_array($var)) {
      $var = $var[$index];
    }
    else {
      if (!$supress_errors) {
        tripal_report_error('chado_node_api', TRIPAL_WARNING,
          'Tokens: Unable to determine the value of %token. Things went awry when trying ' .
          'to access \'%index\' for the following: \'%var\'.',
          [
            '%token' => $token,
            '%index' => $index,
            '%var' => print_r($var, TRUE),
          ]
        );
      }
      return '';
    }
  }
  return $var;
}

/**
 * Format a set of tokens for consistent display
 *
 * @param $tokens
 *   An array of tokens from chado_node_generate_tokens()
 *
 * @return
 *   HTML displaying the token list
 *
 */
function chado_node_format_tokens($tokens) {

  $header = [
    'name' => 'Name',
    'token' => 'Token',
    'description' => 'Description',
  ];
  $rows = [];

  usort($tokens, 'chado_sort_tokens_by_location');
  foreach ($tokens as $token) {
    $rows[] = [
      'name' => $token['name'],
      'token' => $token['token'],
      'description' => $token['description'],
    ];
  }

  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_tokens',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  return theme_table($table);
}

/**
 * This sorts tokens first by depth (ie: stock.* is before stock.*>subtable.*)
 * and then alphabetically within a level (ie: stock.name comes before
 * stock.type_id)
 *
 * This is a usort callback and shouldn't be called directly. To use:
 *    usort($tokens, 'chado_sort_tokens_by_location');
 */
function chado_sort_tokens_by_location($tokenA, $tokenB) {

  // First check if they're the same
  if ($tokenA['location'] == $tokenB['location']) {
    return 0;
  }

  // Then check if there's a difference in depth
  // For example, "stock > type_id" comes before "stock > type_id > name"
  $tokenA_depth = substr_count($tokenA['location'], '>');
  $tokenB_depth = substr_count($tokenB['location'], '>');
  if ($tokenA_depth != $tokenB_depth) {
    return ($tokenA_depth < $tokenB_depth) ? -1 : 1;
  }

  // If the depth is equal then just use alphabetical basic string compare
  return ($tokenA['location'] < $tokenB['location']) ? -1 : 1;
}

/**
 * Sorts an associative array by key length where sorter keys will be first
 *
 * This is a uksort callback and shouldn't be called directly. To use;
 *    uksort($arr, 'tripal_sort_key_length_asc');
 */
function tripal_sort_key_length_asc($a, $b) {
  if (strlen($a) == strlen($b)) {
    return 0;
  }
  elseif (strlen($a) > strlen($b)) {
    return 1;
  }
  else {
    return -1;
  }
}

/**
 * Generate a Readable but not necessarily unique format based on a given
 * primary key token.
 *
 * For example, given the token [feature.type_id>cvterm.cvterm_id] you don't
 * want the actual id indexed but instead would want the term name,
 * [feature.type_id>cvterm.name]
 *
 */
function chado_node_get_readable_format($token) {

  // First, lets break down the token into it's parts.
  // 1. Remove containing brackets.
  $parts = str_replace(['[', ']'], '', $token);
  // 2. Break into table clauses.
  $parts = explode('>', $parts);
  // 3. Break each table clause into table & field.
  foreach ($parts as $k => $v) {
    $parts[$k] = explode('.', $v);
    if (sizeof($parts[$k]) == 1) {
      $parts[$k] = explode(':', $v);
    }
  }
  $last_k = $k;

  // Now, we want to find readable fields for the last table specified in the token.
  // (ie: for cvterm in [feature.type_id>cvterm.cvterm_id])
  $table = $parts[$last_k][0];
  $format = [];
  if ($table == 'organism') {
    $format[] = preg_replace('/(\w+)\]$/', 'genus]', $token);
    $format[] = preg_replace('/(\w+)\]$/', 'species]', $token);
    $format[] = preg_replace('/(\w+)\]$/', 'common_name]', $token);
    $format = $format[0] . ' ' . $format[1] . ' (' . $format[2] . ')';
  }
  elseif ($table == 'dbxref') {
    $format[] = preg_replace('/(\w+)\]$/', 'accession]', $token);
    $format[] = preg_replace('/(\w+)\]$/', 'db_id>db.name]', $token);
    $format = $format[0] . ' (' . $format[1] . ')';
  }
  else {
    $schema = chado_get_schema($table);
    foreach ($schema['fields'] as $field_name => $details) {
      if (preg_match('/name/', $field_name)) {
        $format[] = preg_replace('/(\w+)\]$/', $field_name . ']', $token);
      }
    }
    $format = implode(', ', $format);
  }
  if (empty($format)) {
    return FALSE;
  }
  return $format;
}

/**
 * Returns the "location" as specified in the token information based on the
 * token.
 */
function chado_node_get_location_from_token($token) {

  if (is_array($token) and isset($token['location'])) {
    return $token['location'];
  }
  // If we have been given the token as a string, we can still determine the location
  // but it takes more work...
  // First, lets clarify what the location is: the location shows which keys in which
  // order need to be travelled in order to access the value. For example, the token
  // [feature.organism_id>organism.genus] would have a location of
  // feature > organism_id > genus to show that the value is at
  // $node->feature->organism->genus.
  elseif (is_string($token)) {

    // First, lets break down the token into it's parts.
    // 1. Remove containing brackets.
    $parts = str_replace(['[', ']'], '', $token);
    // 2. Break into table clauses.
    $parts = explode('>', $parts);
    // 3. Break each table clause into table & field.
    foreach ($parts as $k => $v) {
      $parts[$k] = explode('.', $v);
      if (sizeof($parts[$k]) == 1) {
        $parts[$k] = explode(':', $v);
      }
    }

    // This is a base level field that is not a foreign key.
    if (sizeof($parts) == 1 AND sizeof($parts[0]) == 2) {
      return $parts[0][0] . ' > ' . $parts[0][1];
    }
    // Darn, we have at least one foreign key...
    elseif (sizeof($parts) > 1 AND sizeof($parts[0]) == 2) {
      $location = $parts[0][0] . ' > ' . $parts[0][1];
      foreach ($parts as $k => $p) {
        if ($k != 0 AND isset($p[1])) {
          $location .= ' > ' . $p[1];
        }
      }
      return $location;
    }
    else {
      return FALSE;
    }
  }
}