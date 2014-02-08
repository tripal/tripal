<?php
/**
 * @file
 * Functions related to the UI for integrating tables with views
 */

/**
 * Purpose: Generates a themable table containing the list of integrated tables
 *   The look-and-feel of the table can be altered by overriding the theme for
 *   tables.
 *
 * @return
 *    a themed HTML table
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_setup_list() {
  $output = '';

  $output .= '<ul class="action-links">';
  $output .=  '<li>' . l(t('Add a New Entry'), "admin/tripal/views-integration/new") . '</li>';
  $output .=  '<li style="float: right;">' . l(t('Delete ALL Entries'), "admin/tripal/views-integration/delete-all/confirm") . '</li>';
  $output .= '</ul>';

  $output .= '<p>' . t('The following tables are available for integration with Drupal Views. If '
    . 'a table is integrated more than once, then the setup with the lightest '
    . 'priority will be used. For example, if you have created a custom setup with a priority of -5 then '
    . 'that will be used instead of the default setup with priority 10. '
    . 'Priorities range from -10 to +10  where a setup with -10 has '
    . 'greater precedent than any other and +10 has the least.') . '</p>';

  // Start with materialized views
  $output .= '<br /><h3>Legacy Materialized Views</h3>';
  $header = array('', 'Drupal Views Type Name', 'Table Name', 'Is Legacy?', 'Priority', 'Comment');
  $rows = array();

  // get the list of materialized views
  $tviews = db_query('SELECT tv.setup_id, tv.name, tv.table_name, tc.table_id, tv.priority, tv.comment '
                     .'FROM {tripal_views} tv '
                     .'LEFT JOIN {tripal_custom_tables} tc ON tc.table_name=tv.table_name '
                     .'WHERE tv.mview_id IS NOT NULL '
                     .'ORDER BY tv.table_name ASC, tv.priority ASC');
  foreach ($tviews as $tview) {
    $rows[] = array(
      l(t('Edit'), "admin/tripal/views-integration/edit/" . $tview->setup_id) . "<br />"
        . l(t('Export'), "admin/tripal/views-integration/export/" . $tview->setup_id) . "<br />"
        . l(t('Delete'), "admin/tripal/views-integration/delete/" . $tview->setup_id),
      $tview->name,
      $tview->table_name,
      ($tview->table_id) ? 'No' : 'Yes',
      $tview->priority,
      $tview->comment,
    );
  }

  if ($rows) {
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
  }
  else {
    $output .= '<p>There are currently no Materialized Views defined. ';

  }

  // Now list non-mview custom tables
  $output .= '<br /><h3>Custom Tables & Non-Legacy Materialized Views</h3>';
  $header = array('', 'Drupal Views Type Name', 'Table Name', 'Priority', 'Comment');
  $rows = array();

  // get the list of chado tables
  $tviews = db_query('SELECT tv.setup_id, tv.name, tv.table_name, tv.priority, tv.comment '
                     .'FROM {tripal_views} tv '
                     .'LEFT JOIN {tripal_custom_tables} tc ON tc.table_name=tv.table_name '
                     .'WHERE mview_id IS NULL AND tc.table_id IS NOT NULL '
                     .'ORDER BY table_name ASC, priority ASC');
  foreach ($tviews as $tview) {
    $rows[] = array(
      l(t('Edit'), "admin/tripal/views-integration/edit/" . $tview->setup_id) . "<br />"
        . l(t('Export'), "admin/tripal/views-integration/export/" . $tview->setup_id) . "<br />"
        . l(t('Delete'), "admin/tripal/views-integration/delete/" . $tview->setup_id),
      $tview->name,
      $tview->table_name,
      $tview->priority,
      $tview->comment,
    );
  }

  if ($rows) {
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
  }
  else {
    $output .= '<p>There are currently no non-Materialized View Custom Tables defined.</p>';
  }

  // Now list chado tables
  $output .= '<br /><h3>Chado Tables</h3>';
  $header = array('', 'Drupal Views Type Name', 'Table Name', 'Priority', 'Comment');
  $rows = array();

  // get the list of chado tables
  $tviews = db_query('SELECT tv.setup_id, tv.name, tv.table_name, tv.priority, tv.comment '
                     .'FROM {tripal_views} tv '
                     .'LEFT JOIN {tripal_custom_tables} tc ON tc.table_name=tv.table_name '
                     .'WHERE mview_id IS NULL AND tc.table_id IS NULL '
                     .'ORDER BY table_name ASC, priority ASC');
  foreach ($tviews as $tview) {
    $rows[] = array(
      l(t('Edit'), "admin/tripal/views-integration/edit/" . $tview->setup_id) . "<br />"
        . l(t('Export'), "admin/tripal/views-integration/export/" . $tview->setup_id) . "<br />"
        . l(t('Delete'), "admin/tripal/views-integration/delete/" . $tview->setup_id),
      $tview->name,
      $tview->table_name,
      $tview->priority,
      $tview->comment,
    );
  }

  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

/**
 * Purpose: Deletes integration of a table with the Views module.  This
 *   function is meant to be called from a menu item. After completion it
 *   redirects the user to the views intergation page.
 *
 * @param $setup_id
 *    the unique setup id for the integrated table
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_delete($setup_id) {

  tripal_views_integration_remove_entry_by_setup_id($setup_id);
  drupal_set_message(t("Record Deleted"));
  drupal_goto('admin/tripal/views-integration');

}

/**
 * Purpose: Deletes ALL Tripal Views Integrations.  This
 *   function is meant to be called from a menu item. After completion it
 *   redirects the user to the views intergation page.
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_delete_all_form ($form, $form_state) {

  $form['extra'] = array(
    '#type' => 'item',
    '#markup' => t('This will REMOVE ALL views integrations (both custom and default) '
      . 'from your website. This allows integrations to be rebuilt to new default '
      . 'settings and is especially useful after an upgrade to the views integration system.'),
  );

  $form['description'] = array(
    '#type' => 'item',
    '#markup' => t('Are you sure you want to REMOVE ALL Views Integrations (including custom integrations) from your system?'),
  );

  $form['actions'] = array('#type' => 'actions');

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Confirm'),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/tripal/views-integration/list',
  );

  // By default, render the form using theme_confirm_form().
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'confirm_form';
  }

  return $form;
}

/**
 * Purpose: Deletes ALL Tripal Views Integrations.  This
 *   function is meant to be called from a menu item. After completion it
 *   redirects the user to the views intergation page.
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_delete_all_form_submit ($form, &$form_state) {

  tripal_views_rebuild_views_integrations(TRUE);

  $form_state['redirect'] = 'admin/tripal/views-integration/list';

}

/**
 * Purpose: defines the web form used for specifing the base table, joins and
 *   handlers when integrating a table with views.  This form is used for both
 *   creating a new record and editing an existing record.
 *
 * @param $form_state
 *    The form state which is passed automatically by drupal
 *
 * @param $setup_id
 *    The unique setup for an integrated table.  This value is only set when
 *    the form is used for updating an existing record.
 *
 * @return
 *    A proper Drupal form associative array.
 *
 * D7 @todo: Add ability to manage custom fields
 * D7 @todo: Update relationship handler to work with the new tripal_views_join method
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_form($form, $form_state, $arg) {

  $data = array();
  $form['#cache'] = TRUE;
  if (isset($form_state['build_info']['args'][0])) {
    $setup_id = $form_state['build_info']['args'][0];
  }
  else {
    $setup_id = NULL;
  }

  // if Ajax is triggered to change the fields table
  // then make some tweaks before the form is rendered
  if (isset($form_state['triggering_element'])) {

    $triggering_element = $form_state['triggering_element']['#name'];
    if (($triggering_element == 'table_name' OR $triggering_element == 'mview_id')) {
      $form_state['values'][$triggering_element] = $form_state['triggering_element']['#value'];
      $setup_id = NULL;
    }
  }

  // if a setup_id is provided then we want to get the form defaults
  $setup_obj = array();
  if (isset($setup_id)) {

    // get the default setup values
    $sql = "SELECT * FROM {tripal_views} WHERE setup_id = :setup";
    $setup_obj = db_query($sql, array(':setup' => $setup_id));
    $setup_obj = $setup_obj->fetchObject();
    $mview_id = $setup_obj->mview_id;
    $table_name = $setup_obj->table_name;
    $form_state['values']['mview_id'] = $mview_id;
    $form_state['values']['table_name'] = $table_name;

    // get the default field name/description
    $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=:setup";
    $query = db_query($sql, array(':setup' => $setup_id));
    $default_fields = array();
    foreach ($query as $field) {
      $default_fields[$field->column_name]['name'] = $field->name;
      $default_fields[$field->column_name]['description'] = $field->description;
    }

    // get the default join settings and handlers
    $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = :setup";
    $query = db_query($sql, array(':setup' => $setup_id));
    $default_joins = array();
    foreach ($query as $join) {
      $default_joins[$join->base_field]['left_table'] = $join->left_table;
      $default_joins[$join->base_field]['left_field'] = $join->left_field;
    }

    // get the default handlers
    $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = :setup";
    $query = db_query($sql, array(':setup' => $setup_id));
    $default_handlers = array();
    foreach ($query as $handler) {
      $default_handlers[$handler->column_name][$handler->handler_type]['handler_name'] = $handler->handler_name;
      $default_handlers[$handler->column_name][$handler->handler_type]['arguments'] = $handler->arguments;
    }

    // get the default join handlers
    $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = :setup";
    $query = db_query($sql, array(':setup' => $setup_id));
    foreach ($query as $handler) {
      $default_handlers[$handler->base_field]['join']['handler_name'] = $handler->handler;
      //$default_handlers[$handler->base_field]['join']['arguments'] = $handler->arguments;
    }

    // add in the setup_id for the form so we know this is an update not an insert
    $form['setup_id'] = array(
       '#type' => 'hidden',
       '#value' => $setup_id,
    );

  }

  // add a fieldset for the MView & Chado table selectors
  $form['base_table_type'] = array(
    '#type' => 'fieldset',
    '#title' => 'Base Table',
    '#description' => 'Please select either a materialized view or a Chado table for integration with '.
                       'Drupal Views.  In Drupal Views terminology, the selected table becomes the "base table". '.
                       'After you select a table from either list, the fields from that table will appear below '.
                       'and you can specify other tables to join with and handlers.',
  );

  // build the form element for the Chado tables
  $chado_tables = chado_get_table_names(TRUE);
  $chado_tables = array_merge(array('Select'), $chado_tables);
  $default = '';
  if ($setup_id) {
    $default = (!$setup_obj->mview_id) ? $setup_obj->table_name : '';
  }
  $form['base_table_type']['table_name'] = array(
    '#title' => t('Chado/Custom Table'),
    '#type' => 'select',
    '#options' => $chado_tables,
    '#description' => t('Tables from Chado, custom tables and materialized view tables (non-legacy MViews) can be selected for integration.'),
    '#default_value' => $default,
    '#ajax' => array(
       //D6: 'path' => 'tripal/views-integration/ajax/view_setup_table',
       'callback' => 'tripal_views_integration_ajax_view_setup_table',
       'wrapper' => 'fieldset-table-rows-wrapper',
       'effect' => 'fade',
       'event' => 'change',
       'method' => 'replace',
    ),
  );


  // build the form element that lists the materialized views
  // D7 TODO: Check DBTNG changes work
  $query = db_query("SELECT mview_id, name FROM {tripal_mviews} WHERE mv_schema is NULL or mv_schema = '' ORDER BY name");
  $mview_tables = array();
  $mview_tables['0'] = 'Select';
  foreach ($query as $mview) {
    $mview_tables[$mview->mview_id] = $mview->name;
  }
  $default = '';
  if ($setup_id && isset($setup_obj->mview_id)) {
    $default = $setup_obj->mview_id;
  }
  $form['base_table_type']['mview_id'] = array(
    '#title' => t('Legacy Materialized View'),
    '#type' => 'select',
    '#options' => $mview_tables,
    '#description' => 'Which materialized view to use.',
    '#default_value' => $default,
    '#ajax' => array(
       //D6: 'path' => 'tripal/views-integration/ajax/view_setup_table',
       'callback' => 'tripal_views_integration_ajax_view_setup_table',
       'wrapper' => 'fieldset-table-rows-wrapper',
       'effect' => 'fade',
       'event' => 'change',
       'method' => 'replace',
    ),
  );

  $form['views_type'] = array(
    '#type' => 'fieldset',
    '#title' => 'View Type',
    '#description' => 'Here you can provide the "type" of View you want to create.',
  );

  // field for the name of the
  $default = '';
  if ($setup_id && isset($setup_obj->name)) {
    $default = $setup_obj->name;
  }
  $form['views_type']['row_name'] = array(
    '#title' => t('View Type Name'),
    '#type' => 'textfield',
    '#default_value' => $default,
    '#size' => 60,
    '#maxlength' => 128,
    '#description' => 'Provide the view type name.  This is the name that will appear in '.
                      'the Drupal Views interface when adding a new view.  The view type name '.
                      'must be unique.',
    '#required' => TRUE,
  );

  if (isset($setup_id)) {
    $form['row_name']['#attributes'] = array('readonly' => 'readonly');
  }

  $priorities = array();
  foreach (range(-10, 10) as $v) {
    $priorities[$v] = (string) $v;
  }
  $default = -1;
  if ($setup_id && isset($setup_obj->priority)) {
    $default = $setup_obj->priority;
    if ($setup_obj->priority >= 9) {
      drupal_set_message('You are editing a default views integration. To ensure your changes
        are used, change the priority to -10.', 'warning');
    }
  }
  $form['views_type']['row_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#description' => t('The level of priority your Views integration has in relation to the '
      .'default core and module definitions. The views integration definition with the '
      .'lightest priority will be used. For example, if there is a definition created by '
      .'core with a priority of 10 and another by a custom module of 5 and yours is -1 then '
      .'you definition will be used for that table because -1 is lighter then both 5 and 10.'),
    '#options' => $priorities,
    '#default_value' => $default,
  );

  $default = true;
  if ($setup_id && isset($setup_obj->priority)) {
    $default = ($setup_obj->priority >= 9) ? true : false;
  }
  $form['views_type']['new_integration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create a New Tripal Views Integration Record'),
    '#description' => t('If this checkbox is checked then a new tripal views integration '
      .'will be created rather then overriding the current one with your changes. This '
      .'especially important if you are editing one of the default views integrations '
      .'(ie: if the original priority was either 10 or 9).'),
    '#default_value' => $default,
  );

  $default = true;
  if ($setup_id && isset($setup_obj->base_table)) {
    $default = $setup_obj->base_table;
  }
  $form['views_type']['base_table'] = array(
    '#type' => 'checkbox',
    '#title' => t('Base Table?'),
    '#description' => t('If you want this table to show up as one of the options in the '
      . 'add view page, then check this checkbox. It allows you to create listings '
      . 'primarily from this table'),
    '#default_value' => $default,
  );

  $default = '';
  if ($setup_id && isset($setup_obj->comment)) {
    $default = $setup_obj->comment;
  }
  $form['views_type']['row_description'] = array(
    '#title' => t('Comment'),
    '#type' => 'textarea',
    '#description' => '(Optional). Provide any details regarding this setup you would like. This '.
                      'description will appear when selecting a type for a new Drupal View',
    '#required' => FALSE,
    '#default_value' => $default,
  );

  // we need a div block where the table fields will get put when the
  // AHAH callback is made
  $form['view_setup_table'] = array(
    '#type' => 'item',
    '#prefix' => '<div id="fieldset-table-rows-wrapper">',
    '#suffix' => '</div>',
  );


  // add the fieldset for the table fields, but only if the $mview_id or $table_name
  // is set. The only times these values are set is if we're editing an existing
  // record or if the AHAH callback is being made.
  if (isset($form_state['values']['mview_id']) or isset($form_state['values']['table_name'])) {
    $mview_id = $form_state['values']['mview_id'];
    $table_name = $form_state['values']['table_name'];
    $form['view_setup_table'] = array(
      '#type' => 'fieldset',
      '#title' => t('Table Fields'),
      '#prefix' => '<div id="fieldset-table-rows-wrapper">',
      '#suffix' => '</div>',
      '#collapsible' => TRUE
    );

    // get the columns in this materialized view.  They are separated by commas
    // where the first word is the column name and the rest is the type
    $columns = array();
    if ($mview_id) {
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT mv_specs FROM {tripal_mviews} WHERE mview_id = :id";
      $mview = db_query($sql, array(':id' => $mview_id));
      $mview = $mview->fetchObject();
      $columns = explode(",", $mview->mv_specs);
    }
    else {
      $table_desc = chado_get_schema($table_name);
      if ($table_desc) {
        $fields = $table_desc['fields'];
        // iterate through the columns and build the format
        // compatible with the code below. The column name is first followed
        // by the type with a separating space
        foreach ($fields as $column => $attrs) {
          $columns[] = "$column " . $attrs['type'];
        }
      }
    }

    $i=1;
    $form['view_setup_table']["instructions"] = array(
         '#type' => 'markup',
         '#value' => filter_xss("Select an optional table to which the fields of the ".
                     "materialized view can join.  If a field does not need to ".
                     "join you may leave the selection blank."),
    );
    $data['field_types'] = array();

    $form['view_setup_table']['header'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="joins-new-row field-headers">',
      '#suffix' => '</div>',
    );

    $form['view_setup_table']['header']['column-1'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"column-one\">",
         '#suffix' => "</div>",
         '#markup' => 'Field'
    );

    $form['view_setup_table']['header']['column-2'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"column-two\">",
         '#suffix' => "</div>",
         '#markup' => 'Labels'
    );

    $form['view_setup_table']['header']['column-3'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"column-three\">",
         '#suffix' => "</div>",
         '#markup' => ''
    );

    $form['view_setup_table']['header']['column-4'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"column-four\">",
         '#suffix' => "</div>",
         '#markup' => 'Handlers'
    );

    // get the list of chado tables to join on
    $chado_join_tables = chado_get_table_names(TRUE);
    $chado_join_tables = array_merge(array('Select a Join Table'), $chado_join_tables);

    // get list of all handlers
    $all_handlers = tripal_views_integration_discover_handlers();
    $handlers_fields = array(0 => "Select a field handler");
    $handlers_filters = array(0 => "Select a filter handler");
    $handlers_sort = array(0 => "Select a sort handler");
    $handlers_argument = array(0 => "Select an argument handler");
    $handlers_join = array(0 => "Select a join handler");
    $handlers_rel = array(0 => "Select a relationship handler");
    foreach ($all_handlers as $handler) {
      if (preg_match("/views_handler_field/", $handler)) {
        $handlers_fields[$handler] = $handler;
      }
      if (preg_match("/views_handler_filter/", $handler)) {
        $handlers_filters[$handler] = $handler;
      }
      if (preg_match("/views_handler_sort/", $handler)) {
        $handlers_sort[$handler] = $handler;
      }
      if (preg_match("/views_handler_argument/", $handler)) {
        $handlers_argument[$handler] = $handler;
      }
      if (preg_match("/_join/", $handler)) {
        $handlers_join[$handler] = $handler;
      }
      if (preg_match("/views_handler_relationship/", $handler)) {
        $handlers_rel[$handler] = $handler;
      }
    }

    // generate a unique $table_id for keeping track of the table
    if ($mview_id) {
      $table_id = $mview_id;
    }
    else {
      $table_id = $table_name;
    }

    // Per Row (Fields) --------------
    // now iterate through the columns of the materialized view or
    // chado table and generate the join and handler fields
    foreach ($columns as $column) {
      $column = trim($column);  // trim trailing and leading spaces
      preg_match("/^(.*?)\ (.*?)$/", $column, $matches);
      $column_name = $matches[1];
      $column_type = $matches[2];

      $form['view_setup_table']["$table_id-$i"] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"fields-new-row\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      // COLUMN I
      $form['view_setup_table']["$table_id-$i"]["fields_name_$table_id-$i"] = array(
        '#type' => 'item',
        '#prefix' => "<div class=\"column-one\">",
        '#markup' => '<span class="column-name">' . filter_xss($column_name) . '</span>'.
                    '<br /><span class="column-type">' . filter_xss($column_type) . '</span>',
        '#suffix' => "</div>",
      );
      $data['field_types'][$column_name] = $column_type;

      // COLUMN II
      $form['view_setup_table']["$table_id-$i"]['column-2'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"column-two\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      // set the default values for the human-readable name and description
      $default_name = '';
      $default_descrip = '';
      if (isset($setup_id) && !isset($form_state['values']["fields_readable_name_$table_id-$i"])) {
        $default_name = $default_fields[$column_name]['name'];
        $default_descrip = $default_fields[$column_name]['description'];
      }
      elseif (isset($form_state['values']["fields_readable_name_$table_id-$i"])) {
        $default_name = $form_state['values']["fields_readable_name_$table_id-$i"];
        $default_descrip = $form_state['values']["fields_description_$table_id-$i"];
      }
      else {
        $default_name = ucwords(str_replace('_',' ', $column_name));
        $default_descrip = 'TODO: please describe this field!';
      }
      $form['view_setup_table']["$table_id-$i"]['column-2']["fields_readable_name_$table_id-$i"] = array(
        '#type' => 'textfield',
        '#title' => 'Human-Readable Name',
        '#description' => 'This is the name of the field in the Views UI',
        '#required' => TRUE,
        '#default_value' => $default_name,
        '#size' => 42,
      );

      $form['view_setup_table']["$table_id-$i"]['column-2']["fields_description_$table_id-$i"] = array(
        '#type' => 'textarea',
        '#title' => 'Short Description',
        '#description' => 'This is the field help in the Views UI',
        '#required' => TRUE,
        '#cols' => 42,
        '#rows' => 3,
        '#default_value' => $default_descrip,
      );

      // COLUMN III
      $form['view_setup_table']["$table_id-$i"]['column-3'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"column-three\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      // COLUMN 4
      $form['view_setup_table']["$table_id-$i"]['column-4'] = array(
        '#type' => 'markup',
        '#prefix' => "<div class=\"column-four\">",
        '#suffix' => "</div>",
        '#value' => filter_xss('')
      );

      // create the handler fields
      $default_field_handler = 0;
      if (isset($setup_id) && !isset($form_state['values']["fields_field_handler_$table_id-$i"])) {
        $default_field_handler = $default_handlers[$column_name]['field']['handler_name'];
        $form_state['values']["fields_field_handler_$table_id-$i"] = $default_field_handler;
      }
      else {
        $default_field_handler = $form_state['values']["fields_field_handler_$table_id-$i"];
        if (!$default_field_handler) {
          if ($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial') {
            $default_field_handler = 'chado_views_handler_field_numeric';
          }
          elseif (preg_match("/character varying/", $column_type) or $column_type == 'char' or $column_type == 'text' or $column_type == 'varchar') {
            $default_field_handler = 'chado_views_handler_field';
          }
          elseif ($column_type == 'boolean') {
            $default_field_handler = 'chado_views_handler_field_boolean';
          }
          elseif ($column_type == 'float') {
            $default_field_handler = 'chado_views_handler_field_numeric';
          }
          elseif ($column_type == 'datetime') {
            $default_field_handler = 'chado_views_handler_field_date';
          }
        }
      }

      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_field_handler_$table_id-$i"] = array(
         '#type' => 'select',
         '#prefix' => "<div class=\"fields-field-handler\">",
         '#suffix' => "</div>",
         '#options' => $handlers_fields,
         '#required' => FALSE,
         '#default_value' => $default_field_handler,
      );

      $default_filter_handler = 0;
      if (isset($setup_id) && !isset($form_state['values']["fields_filter_handler_$table_id-$i"])) {
        $default_filter_handler = $default_handlers[$column_name]['filter']['handler_name'];
        $form_state['values']["fields_filter_handler_$table_id-$i"]= $default_filter_handler;
      }
      else {
        $default_filter_handler = $form_state['values']["fields_filter_handler_$table_id-$i"];
        if (!$default_filter_handler) {
          if ($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial') {
            $default_filter_handler = 'chado_views_handler_filter_numeric';
          }
          elseif (preg_match("/^character varying/", $column_type) or $column_type == 'char' or $column_type == 'text' or $column_type == 'varchar') {
            $default_filter_handler = 'chado_views_handler_filter_string';
          }
          elseif ($column_type == 'boolean') {
            $default_filter_handler = 'chado_views_handler_filter_boolean';
          }
          elseif ($column_type == 'float') {
            $default_filter_handler = 'chado_views_handler_filter_float';
          }
          elseif ($column_type == 'datetime') {
            $default_filter_handler = 'chado_views_handler_filter_date';
          }
        }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_filter_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-filter-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_filters,
        '#required' => FALSE,
        '#default_value' => $default_filter_handler,
      );

      $default_sort_handler = 0;
      if (isset($setup_id) && !isset($form_state['values']["fields_sort_handler_$table_id-$i"])) {
        $default_sort_handler = $default_handlers[$column_name]['sort']['handler_name'];
        $form_state['values']["fields_sort_handler_$table_id-$i"] = $default_sort_handler;
      }
      else {
        $default_sort_handler = $form_state['values']["fields_sort_handler_$table_id-$i"];
        if (!$default_sort_handler) {
          if ($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial') {
            $default_sort_handler = 'chado_views_handler_sort';
          }
          elseif (preg_match("/character varying/", $column_type) or $column_type == 'char' or $column_type == 'text' or $column_type == 'varchar') {
            $default_sort_handler = 'chado_views_handler_sort';
          }
          elseif ($column_type == 'boolean') {
            $default_sort_handler = 'chado_views_handler_sort';
          }
          elseif ($column_type == 'float') {
            $default_sort_handler = 'chado_views_handler_sort';
          }
          elseif ($column_type == 'datetime') {
            $default_sort_handler = 'chado_views_handler_sort_date';
          }
        }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_sort_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-sort-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_sort,
        '#required' => FALSE,
        '#default_value' => $default_sort_handler,
      );

      $default_argument_handler = 0;
      if (isset($setup_id)
        && !isset($form_state['values']["fields_argument_handler_$table_id-$i"])
        && isset($default_handlers[$column_name]['argument']))
      {
        $default_argument_handler = $default_handlers[$column_name]['argument']['handler_name'];
        $form_state['values']["fields_argument_handler_$table_id-$i"]=$default_argument_handler ;
      }
      else {
        if (isset($form_state['values']["fields_argument_handler_$table_id-$i"])) {
          $default_argument_handler = $form_state['values']["fields_argument_handler_$table_id-$i"];
        }
        if (!$default_argument_handler) {
          if ($column_type == 'integer' or $column_type == 'int' or $column_type == 'serial') {
            $default_argument_handler = 'views_handler_argument_numeric';
          }
          elseif (preg_match("/character varying/", $column_type) or $column_type == 'char' or $column_type == 'text' or $column_type == 'varchar') {
            $default_argument_handler = 'views_handler_argument_string';
          }
          elseif ($column_type == 'boolean') {
            $default_argument_handler = 'views_handler_argument_numeric';
          }
          elseif ($column_type == 'float') {
            $default_argument_handler = 'views_handler_argument_numeric';
          }
          elseif ($column_type == 'datetime') {
            $default_argument_handler = 'views_handler_argument_date';
          }
        }
      }
      $form['view_setup_table']["$table_id-$i"]['column-4']["fields_argument_handler_$table_id-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-argument-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_argument,
        '#required' => FALSE,
        '#default_value' => $default_argument_handler,
      );

      $i++;
    }

    $form['view_setup_join'] = array(
      '#type' => 'fieldset',
      '#title' => t('Joins & Relationships'),
      '#prefix' => '<div id="fieldset-join-rows-wrapper">',
      '#suffix' => '</div>',
      '#description' => t('Below is a list of the joins/relationships between the '
        . 'current base table and other chado tables.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE
    );

    $form['view_setup_join']['header'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="joins-new-row field-headers">',
      '#suffix' => '</div>',
    );

    $form['view_setup_join']['header']['column-1'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"join-column-one\">",
         '#suffix' => "</div>",
         '#markup' => 'Base'
    );

    $form['view_setup_join']['header']['column-2'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"join-column-two\">",
         '#suffix' => "</div>",
         '#markup' => 'Handlers'
    );

    $form['view_setup_join']['header']['column-3'] = array(
         '#type' => 'markup',
         '#prefix' => "<div class=\"join-column-three\">",
         '#suffix' => "</div>",
         '#markup' => 'Joined to'
    );

    $base_field_options = array('Select the Base Column');
    $table_desc = chado_get_schema($table_name);
    foreach ($table_desc['fields'] as $column => $def) {
      $base_field_options[$column] = $column;
    }
    $chado_join_tables[0] = 'Select the Left Table';
    unset($handlers_join[0]);

    $query = db_select('tripal_views_join','tvj');
    $query->fields('tvj')
      ->condition('tvj.setup_id',$setup_id,'=')
      ->orderBy('tvj.relationship_only','ASC')
      ->orderBy('tvj.base_field', 'ASC')
      ->orderBy('tvj.left_table', 'ASC');
    foreach ($query->execute() as $i => $result) {
      $form['view_setup_join']["$table_id-$i"] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"joins-new-row\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      // COLUMN I
      $form['view_setup_join']["$table_id-$i"]['column-1'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"join-column-one\">",
           '#suffix' => "</div>",
      );

      $form['view_setup_join']["$table_id-$i"]['column-1']['join_base_table'] = array(
        '#type' => 'item',
        '#markup' => '<span class="column-name">' . filter_xss($result->base_table) . '</span>'
      );

      $form['view_setup_join']["$table_id-$i"]['column-1']["join_base_table-$i"] = array(
        '#type' => 'hidden',
        '#value' => $result->base_table
      );

      $form['view_setup_join']["$table_id-$i"]['column-1']["join_base_field-$i"] = array(
        '#type' => 'select',
        '#options' => $base_field_options,
        '#required' => FALSE,
        '#default_value' => $result->base_field
      );

      // COLUMN II
      $form['view_setup_join']["$table_id-$i"]['column-2'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"join-column-two\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      $form['view_setup_join']["$table_id-$i"]['column-2']["join_join_handler-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-join-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_join,
        '#required' => FALSE,
        '#default_value' => $result->handler,
      );

      $form['view_setup_join']["$table_id-$i"]['column-2']["join_relationship_handler-$i"] = array(
        '#type' => 'select',
        '#prefix' => "<div class=\"fields-relationship-handler\">",
        '#suffix' => "</div>",
        '#options' => $handlers_rel,
        '#required' => FALSE,
        '#default_value' => $result->relationship_handler,
      );

      $form['view_setup_join']["$table_id-$i"]['column-2']["join_relationship_only-$i"] = array(
        '#type' => 'checkbox',
        '#title' => 'Relationship Only?',
        '#default_value' => $result->relationship_only
      );

      // COLUMN III
      $form['view_setup_join']["$table_id-$i"]['column-3'] = array(
           '#type' => 'markup',
           '#prefix' => "<div class=\"join-column-three\">",
           '#suffix' => "</div>",
           '#value' => filter_xss('')
      );

      $form['view_setup_join']["$table_id-$i"]['column-3']["join_left_table-$i"] = array(
        '#type' => 'select',
        '#options' => $chado_join_tables,
        '#default_value' => $result->left_table,
      );

      $columns = array();
      if ($result->left_table) {
        // get the table description
        $table_desc = chado_get_schema($result->left_table);
        foreach ($table_desc['fields'] as $column => $def) {
          $columns[$column] = $column;
        }
      }
      else {
        $columns = array('Select Left Column');
      }
      $form['view_setup_join']["$table_id-$i"]['column-3']["join_left_field-$i"] = array(
        '#type' => 'select',
        '#prefix' => "  <div id=\"fields-column-join-column-$table_id-$i\" class=\"fields-column-join-column\">",
        '#suffix' => "</div>",
        '#options' => $columns,
        '#required' => FALSE,
        '#default_value' => $result->left_field
      );
    }

    $form['num_joins'] = array(
      '#type' => 'hidden',
      '#value' => $i
    );

    $form['save'] = array(
      '#type'  => 'submit',
      '#value' => t('Save'),
    );

    $data['row_count'] = $i - 1;
  } //end of if table/mview

  //use this to put values into $form_state['values']
  $form['data'] = array();

  // Ensure that we don't store an array
  // since we will get a check_plain:htmlspecial_characters error if we do
  foreach ($data as $key => $value) {
    if (is_array($value)) {
      $form['data'][$key] = array(
        '#type' => 'hidden',
        '#value' => serialize($value),
      );
    }
    else {
      $form['data'][$key] = array(
        '#type' => 'hidden',
        '#value' => $value,
      );
    }
  }

  $form['#redirect'] = 'admin/tripal/views-integration/list';

  return $form;
}

/**
 * Purpose: validates the tripal_views_integration_form after submission
 *
 * @param $form
 *    The form object which is passed automatically by drupal
 *
 * @param $form_state
 *    The form state pbject which is passed automatically by drupal
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_form_validate($form, &$form_state) {
  $name_array = explode(" ", $form_state['values']['row_name']);
  $mview_id = $form_state['values']['mview_id'];
  $table_name = $form_state['values']['table_name'];

//  if (count($name_array) > 1) {
//    form_set_error($form_state['values']['row_name'], 'The View type name must be a single word only.');
//  }
  if ($mview_id and $table_name) {
    form_set_error('mview_id', 'Please select either a materialized view or a Chado table but not both');
  }
  if (!$mview_id and !$table_name) {
    form_set_error('mview_id', 'Please select either a materialized view or a Chado table');
  }

  // Ensure that users don't override the default integrations
  if ($form_state['values']['row_priority'] >= 9) {
    form_set_error('row_priority', 'A priority of 10 or 9 is reserved for default tripal '
      .'views integrations created by core modules. Please set the priority between '
      .'0 and -10 to ensure your changes are used rather over the defaults.');
  }
}

/**
 * Purpose: inserts or updates the record in the tripal views integration
 *   tables.  This function is only called if validation is passed.
 *
 * @param $form
 *    The form object which is passed automatically by drupal
 *
 * @param $form_state
 *    The form state pbject which is passed automatically by drupal
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_form_submit($form, &$form_state) {
  $name = $form_state['values']['row_name'];
  $mview_id = $form_state['values']['mview_id'];
  $table_name = $form_state['values']['table_name'];
  $setup_id = $form_state['values']['setup_id'];
  $priority = $form_state['values']['row_priority'];
  $comment = $form_state['values']['row_description'];

  // get details about this mview
  if ($mview_id) {
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :id";
    $mview = db_query($sql, array(':id' => $mview_id));
    $mview = $mview->fetchObject();
    $table_name = $mview->mv_table;
    $table_id = $mview_id;
    $type = 'mview';
  }
  else {
    $type = 'chado';
    $table_id = $table_name;
  }

  // If this is for a materialized view then we want to add/update that record
  $tripal_views_record = array();
  if ($mview_id) {
    // get details about this mview
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :id";
    $mview = db_query($sql, array(':id' => $mview_id));
    $mview = $mview->fetchObject();

    // build the record for insert/update
    $tripal_views_record = array(
     'mview_id' => $mview_id,
     'table_name' => $mview->mv_table,
     'name' => $name,
     'priority' => $priority,
     'comment' => $comment,
     'base_table' => $form_state['values']['base_table']
    );
  }
  // if a chado table then...
  else {
    // build the record for insert/update
    $tripal_views_record = array(
     'table_name' => $table_name,
     'name' => $name,
     'priority' => $priority,
     'comment' => $comment,
     'base_table' => $form_state['values']['base_table']
    );
  }

  // perform the insert or update
  if (!$setup_id) {  // this is an insert
    if (!drupal_write_record('tripal_views', $tripal_views_record)) {
      drupal_set_message(t("Failed to add record."), 'error');
      return;
    }
  }
  else {  // this is an update
    // check to see if it was specified to create a new integration
    if ($form_state['values']['new_integration']) {
      $setup_id = NULL;
      if (!drupal_write_record('tripal_views', $tripal_views_record)) {
        drupal_set_message(t("Failed to add record."), 'error');
        return;
      }
    }
    else {
      $tripal_views_record['setup_id'] = $setup_id;
      if (!drupal_write_record('tripal_views', $tripal_views_record, array('setup_id'))) {
        drupal_set_message(t("Failed to update record."), 'error');
        return;
      }
    }
  }

  // if this is an update then clean out the existing joins and handlers so we can add new ones
  if ($setup_id) {
    db_query("DELETE FROM {tripal_views_field} WHERE setup_id = :setup", array(':setup' => $setup_id));
    db_query("DELETE FROM {tripal_views_join} WHERE setup_id = :setup", array(':setup' => $setup_id));
    db_query("DELETE FROM {tripal_views_handlers} WHERE setup_id = :setup", array(':setup' => $setup_id));
  }

  // iterate through the columns of the form and add
  // the joins if provided, and the handlers
  $i = 1;
  foreach (unserialize($form_state['values']['field_types']) as $key => $value) {

    // add the field definition
    $view_field_record = array(
      'setup_id' => $tripal_views_record['setup_id'],
      'column_name' => $key,
      'name' => $form_state['values']["fields_readable_name_$table_id-$i"],
      'description' => $form_state['values']["fields_description_$table_id-$i"],
      'type' => $value,
    );
    drupal_write_record('tripal_views_field', $view_field_record);

    // add the hanlders
    $handlers = array('filter', 'field', 'sort', 'argument');

    foreach ($handlers as $handler) {
      $handler_name = $form_state['values']["fields_" . $handler . "_handler_$table_id-$i"];
      if ($handler_name) {
        $handler_record = array(
          'setup_id' => $tripal_views_record['setup_id'],
          'column_name' => $key,
          'handler_type' => $handler,
          'handler_name' => $handler_name,
        );
        drupal_write_record('tripal_views_handlers', $handler_record);
      }
    }
    $i++;
  }

  // Now add all the joins
  for($i = 0; $i <= $form_state['values']['num_joins']; $i++) {
    $join_record = array(
      'setup_id' => $tripal_views_record['setup_id'],
      'base_table' => $form_state['values']["join_base_table-$i"],
      'base_field' => $form_state['values']["join_base_field-$i"],
      'left_table' => $form_state['values']["join_left_table-$i"],
      'left_field' => $form_state['values']["join_left_field-$i"],
      'handler' => $form_state['values']["join_join_handler-$i"],
      'relationship_handler' => $form_state['values']["join_relationship_handler-$i"],
      'relationship_only' => $form_state['values']["join_relationship_only-$i"]
    );
    drupal_write_record('tripal_views_join', $join_record);
  }

  if ($setup_id) {
    drupal_set_message(t('Record Updated'));
  }
  else {
    drupal_set_message(t('Record Added'));
  }

  $form_state['redirect'] = 'admin/tripal/views-integration/list';

  // now clear all the caches so that Drupal views picks up our chages
  views_invalidate_cache();
}

/**
 * Purpose: this function queries all modules currently enabled on the site
 *   looking for custom handlers and returns a list of all available handerls.
 *   The base View handlers are also included.
 *
 * @return
 *   Returns an array of handler names
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_discover_handlers() {

  $handlers = array();

  // Get handlers from all modules.
  foreach (module_implements('views_handlers') as $module) {
    $function = $module . '_views_handlers';
    $result = $function();
    if (!is_array($result)) {
      continue;
    }
    foreach ($result['handlers'] as $handler => $parent) {
      $handlers[] = $handler;
    }
  }

  // these handlers are hard coded because I could not
  // get the views_handlers() function to be called
  // in the code above.  However, we will be creating
  // Chado wrappers for many of these and once that work
  // is done these will no longer be needed.

  // argument handlers
  $handlers[] = 'views_handler_argument';
  $handlers[] = 'views_handler_argument_numeric';
  $handlers[] = 'views_handler_argument_formula';
  $handlers[] = 'views_handler_argument_date';
  $handlers[] = 'views_handler_argument_string';
  $handlers[] = 'views_handler_argument_many_to_one';
  $handlers[] = 'views_handler_argument_null';

  // field handlers
  $handlers[] = 'views_handler_field';
  $handlers[] = 'views_handler_field_date';
  $handlers[] = 'views_handler_field_boolean';
  $handlers[] = 'views_handler_field_markup';
  $handlers[] = 'views_handler_field_xss';
  $handlers[] = 'views_handler_field_url';
  $handlers[] = 'views_handler_field_file_size';
  $handlers[] = 'views_handler_field_prerender_list';
  $handlers[] = 'views_handler_field_numeric';
  $handlers[] = 'views_handler_field_custom';
  $handlers[] = 'views_handler_field_counter';

  // filter handlers
  $handlers[] = 'views_handler_filter';
  $handlers[] = 'views_handler_filter_equality';
  $handlers[] = 'views_handler_filter_string';
  $handlers[] = 'views_handler_filter_boolean_operator';
  $handlers[] = 'views_handler_filter_boolean_operator_string';
  $handlers[] = 'views_handler_filter_in_operator';
  $handlers[] = 'views_handler_filter_numeric';
  $handlers[] = 'views_handler_filter_float';
  $handlers[] = 'views_handler_filter_date';
  $handlers[] = 'views_handler_filter_many_to_one';

  // relationship handlers
  $handlers[] = 'views_handler_relationship';

  // sort handlers
  $handlers[] = 'views_handler_sort';
  $handlers[] = 'views_handler_sort_formula';
  $handlers[] = 'views_handler_sort_date';
  $handlers[] = 'views_handler_sort_menu_hierarchy';
  $handlers[] = 'views_handler_sort_random';

  // join handler
  $handlers[] = 'views_join';

  return $handlers;
}

/*
 * Ajax Callback: Tripal Views Integration Form
 * Replaces the entire fields table when the table or materialized view is set/changed
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_ajax_view_setup_table($form, $form_state) {
  return $form['view_setup_table'];
}

/*
 * Ajax Callback: Tripal Views Integration Form
 * Replaces the join db field dropdown when the join table dropdown is changed
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_ajax_join_field($form, $form_state) {

  // Determine which row we are dealing with from the name of the triggering element
  if (preg_match('/fields_join_(\w+-\d+)/', $form_state['triggering_element']['#name'], $matches)) {
    $field = $matches[1];
    $join_field = 'fields_join_column_' . $matches[1];
    // return the form element to be updated
    return $form['view_setup_table'][$field]['column-3'][$join_field];
  }
  else {
    tripal_report_error('tripal_views', TRIPAL_ERROR,
      'Tripal Views Integration Ajax failed due to being unable to determine which row needs updating', array());
    return $form;
  }
}
