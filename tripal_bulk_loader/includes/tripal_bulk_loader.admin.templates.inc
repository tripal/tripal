<?php

/**
 * @file
 * All functions in this file pertain to administrative management of bulk loader templates
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * @section
 * Modify Template
 * The main form for creating/editing templates as a whole
 */


/**
 * The main form reached at admin/tripal/tripal_bulk_loader/create and /edit
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_modify_template_base_form($form, &$form_state = NULL, $mode) {

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  drupal_set_breadcrumb($breadcrumb);

   // get template id from path and rebuild form
  if (isset($form_state['build_info']['args'][1])) {
    $mode = 'edit';
    if (preg_match('/^\d+$/', $form_state['build_info']['args'][1])) {
      $form_state['storage']['template_id'] = $form_state['build_info']['args'][1];

      $template_id = $form_state['storage']['template_id'];

      $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
      $result = db_query($sql, array(':template' => $template_id))->fetchObject();
      $form_state['storage']['template'] = unserialize($result->template_array);
      if (!$form_state['storage']['template']) {
        $form_state['storage']['template'] = array();
      }
      $form_state['storage']['template_name'] = $result->name;

      $form_state['storage']['record2priority'] = array();
      foreach ($form_state['storage']['template'] as $priority => $record_array) {
        if (!is_array($record_array)) {
          continue; }
        $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
      }
    }
    else {
      $form_state['storage']['template_id'] = FALSE;
      $form['records']['no_records']['#value'] = TRUE;
      $form['fields']['total_fields']['#value'] = 0;
    }
  }
  else {
    $mode = 'create';
    $form_state['storage']['template_id'] = FALSE;
    $form['records']['no_records']['#value'] = TRUE;
    $form['fields']['total_fields']['#value'] = 0;
  }

  $form['mode'] = array(
    '#type' => 'hidden',
    '#value' => $mode,
  );

  if ($form_state['storage']['template_id']) {
    $form['template_name'] = array(
      '#type' => 'item',
      '#title' => 'Template',
      '#markup' => $form_state['storage']['template_name'],
      '#weight' => 1,
    );
  }
  else {
    if (preg_match('/create/', $mode)) {
      $form['new_template_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Template Name',
        '#weight' => 1,
      );
    }
    elseif (preg_match('/edit/', $mode)) {
      $sql = "SELECT * FROM {tripal_bulk_loader_template}";
      $resource = db_query($sql);
      $templates = array();
      $templates[''] = 'Select a Template';
      while ($r = $resource->fetchObject()) {
        $templates[$r->template_id] = $r->name;
      }

      $form['template_id'] = array(
        '#title'         => t('Template'),
        '#description'   => t('Please select the template you would like to edit.'),
        '#type'          => 'select',
        '#options'       => $templates,
        '#default_value' => $form_state['storage']['template_id'],
        '#weight'        => 0,
        '#required'      => TRUE,
        '#weight' => 1,
      );
    }
  }

  $mode_title = (preg_match('/create/', $mode)) ? 'Create Template' : 'Edit Template';
  $value = ($form_state['storage']['template_id'])? 'Save Template' : $mode_title;
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $value,
    '#weight' => 10,
  );

  if (preg_match('/edit/', $mode) && !$form_state['storage']['template_id']) {
    return $form;
  }

  $form['records'] = array(
    '#type' => (isset($form_state['storage']['template_id']))? 'fieldset' : 'hidden',
    '#title' => t('Current Records'),
    '#collapsible' => TRUE,
    '#weight' => 2,
    '#description' => t('A "Record" is an entry in a database table. Each of the records '
      . 'added to the bulk loader will be added per constant set and per line of you '
      . 'input file when a Tripal Bulk Loading Job is created using this template. '
      . 'Furthermore, if you add two records for the same table (i.e. feature table) then '
      . 'two distinct entries will be created per line (i.e. if recordA specifies a parent '
      . 'feature and recordB specifies the child using different columns in the input file '
      . 'then you will have one entry in the feature table specifying the parent and '
      . 'another one for the child.')
  );

  $form['records']['description'] = array(
    '#type' => 'item',
    '#markup' => 'Records will be inserted into the chado database in the order listed below. To '
      .'change this order: <ul><li>Drag the rows into the correct order <br/>(If you don\'t have javascript enabled then enter '
      .'the numbers 1 and up in the Order textboxes to indicate the correct order).</li></ul>',
  );

  $form['records']['records-data'] = array(
    '#prefix' => '<div id="tripal-bulk-loader-template-records">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'tripal_bulk_loader_modify_template_base_form_records'
  );

  $form['records']['no_records'] = array(
    '#type' => 'hidden',
    '#value' => TRUE,
  );

  $form['records']['submit-new_record'] = array(
    '#type' => 'submit',
    '#value' => 'New Record/Field',
  );

  $form['records']['submit-reorder'] = array(
    '#type' => 'submit',
    '#value' => 'Save Order',
  );

  $form['fields'] = array(
    '#type' => (isset($form_state['storage']['template_id']))? 'fieldset' : 'hidden',
    '#title' => t('Current Fields'),
    '#collapsible' => TRUE,
    '#weight' => 3,
    '#description' => t('A "Field" is a single column in a database entry. As such a field '
     . 'is always associated with a database entry (record).')
  );

  $form['fields']['fields-data'] = array(
    '#tree' => TRUE,
  );

  if (isset($form_state['storage']['template'])) {

    // List Current Fields -------------------------------------------------------------
    $num_fields=0;
    $record_href_template = 'admin/tripal/loaders/bulk/template/%template/%action/%priority';
    $field_href_template = 'admin/tripal/loaders/bulk/template/%template/%action/%priority/%field';
    foreach ($form_state['storage']['template'] as $priority => $table_array) {
      if (!is_array($table_array)) {
      continue; }

        $form['records']['no_records']['#value'] = FALSE;

        $mode_value = '';
        $table_array['optional'] = (!isset($table_array['optional'])) ? FALSE : $table_array['optional'];
        if ($table_array['optional']) {
          $mode_value .= 'optional ';
        }

        // for backwards compatibility we want to convert insert_unique to be 'insert'
        // and optional to 'insert'
        $table_array['mode'] = (!isset($table_array['mode'])) ? 'insert' : $table_array['mode'];
        if (strcmp($table_array['mode'], 'insert_unique')==0) {
          $mode_value .= 'insert or select if duplicate';
        }
        elseif (strcmp($table_array['mode'], 'optional')==0) {
          $mode_value .= 'optional insert';
        }
        elseif (strcmp($table_array['mode'], 'insert_once')==0) {
          $mode_value .= 'insert once';
        }
         elseif (strcmp($table_array['mode'], 'select_once')==0) {
          $mode_value .= 'select once';
        }
        elseif ($table_array['mode']) {
          $mode_value .= $table_array['mode'];
        }
        else {
          $mode_value .= 'insert';
        }

        // add in the select if duplicate
        if (isset($table_array['select_if_duplicate'])) {
          if ($table_array['select_if_duplicate']) {
            $mode_value .= ' or select if duplicate';
          }
        }
        if (isset($table_array['select_optional'])) {
          if ($table_array['select_optional']) {
            $mode_value .= ' (no fail)';
          }
        }
        if (isset($table_array['update_if_duplicate'])) {
          if ($table_array['update_if_duplicate']) {
            $mode_value .= ' or update if duplicate';
          }
        }


        // add in the disabled
        if (isset($table_array['disable'])) {
          if ($table_array['disable']) {
            $mode_value .= '. <font color="Red">DISABLED</font>';
          }
        }

        // if either the template_id or priority are 0 then we need to make the link
        // point to the letter o instead (drupal menu can't have 0)
        $path_template_id = ($template_id > 0) ? $template_id : 'O';
        $path_priority = ($priority > 0) ? $priority : 'O';
        $form['records']['records-data'][$priority] = array(
          'title' => array(
            '#type' => 'item',
            '#markup' => filter_xss($priority . ". " . $table_array['record_id']),
            '#prefix' => "<a name=\"record_$priority\"></a>",
          ),
          'view-fields-link' => array(
            '#type' => 'markup',
            '#markup' => "<a href=\"#fields_$priority\"> View Fields </a>",
          ),
          'chado_table' => array(
            '#type' => 'item',
            '#markup' => filter_xss($table_array['table']),
          ),
          'mode' => array(
            '#type' => 'item',
            '#markup' => $mode_value,
          ),
          'new_priority' => array(
            '#type' => 'select',
            '#options' => range(1, sizeof($form_state['storage']['template'])),
            '#default_value' => $priority,
          ),
          'old_priority' => array(
            '#type' => 'hidden',
            '#value' => $priority,
          ),
          'id'  => array(
            '#type' => 'hidden',
            '#value' => $priority,
          ),
          'submit-edit_record' => array(
            '#type' => 'link',
            '#href' => str_replace(array('%template','%priority','%action'), array($path_template_id, $path_priority,'edit_record'), $record_href_template),
            '#title' => 'Edit',
          ),
          'submit-delete_record' => array(
            '#type' => 'link',
            '#href' => str_replace(array('%template','%priority','%action'), array($path_template_id, $path_priority,'delete_record'), $record_href_template),
            '#options' => array(
              'query' => array(
                'record_name' => $table_array['record_id'],
                'chado_table' => $table_array['table'],
                'record_mode' => $mode_value
              )
            ),
            '#title' => 'Delete',
          ),
          'submit-duplicate_record' => array(
            '#type' => 'link',
            '#href' => str_replace(array('%template','%priority','%action'), array($path_template_id, $path_priority,'duplicate_record'), $record_href_template),
            '#options' => array(
              'query' => array(
                'record_name' => $table_array['record_id'],
                'chado_table' => $table_array['table'],
                'record_mode' => $mode_value
              )
            ),
            '#title' => 'Duplicate'
          ),
          'submit-add_field' => array(
            '#type' => 'link',
            '#href' => str_replace(array('%template','%priority','%action'), array($path_template_id, $path_priority,'add_field'), $record_href_template),
            '#title' => 'Add Field',
          ),
        );

        foreach ($table_array['fields'] as $field_index => $field) {
          $i = $num_fields;
          $fk_value = '';
          if (isset($field['foreign key'])) {
            if (isset($field['foreign field'])) {
              $fk_value = $field['foreign key'] . " (" . $field['foreign field'] . ")";
            }
            else {
              // for backwards compatibility we need to get the FK relationship to find
              // out what field we're joining on.  For templates created using a
              // previous version this information isn't stored in the template
              // so we need to get it.
              $fk_priority = $form_state['storage']['record2priority'][$field['foreign key']];
              $fk_table = $form_state['storage']['template'][$fk_priority]['table'];
              $tbl_description = chado_get_schema($table_array['table']);
              foreach ($tbl_description['foreign keys'] as $key_table => $key_array) {
                foreach ($key_array['columns'] as $left_field => $right_field) {
                  if ($key_table == $fk_table and $left_field == $field['field']) {
                    $fk_value = $field['foreign key'] . " ($right_field)";
                  }
                }
              }
            }
          }

          // if either the template_id or priority are 0 then we need to make the link
          // point to the letter o instead (drupal menu can't have 0)
          $path_template_id = ($template_id > 0) ? $template_id : 'O';
          $path_priority = ($priority > 0) ? $priority : 'O';
          $path_field_index = ($field_index > 0) ? $field_index : 'O';
          $form['fields']['fields-data'][$i] = array(
            'record_id' => array(
              '#type' => 'item',
              '#markup' =>  $table_array['record_id'],
              '#prefix' => "<a name=\"fields_$priority\"></a>",
            ),
            'view-record-link' => array(
            '#type' => 'markup',
            '#markup' => "<a href=\"#record_$priority\"> View Record</a>",
          ),
            'priority_hidden' => array(
              '#type' => 'hidden',
              '#value' => $priority,
            ),
            'field_name' => array(
              '#type' => 'item',
              '#markup' => $field['title'],
            ),
            'chado_table_name' => array(
              '#type' => 'item',
              '#markup' => $table_array['table'],
            ),
            'chado_table_hidden' => array(
              '#type' => 'hidden',
              '#value' => $table_array['table'],
            ),
            'chado_field_name' => array(
              '#type' => 'item',
              '#markup' => $field['field'],
            ),
            'column_num' => array(
              '#type' => 'item',
              '#markup' => (isset($field['spreadsheet column'])) ? $field['spreadsheet column'] : '',
            ),
            'constant_value' => array(
              '#type' => 'item',
              '#markup' => (isset($field['constant value'])) ? $field['constant value'] : '',
            ),
            'field_index' => array(
              '#type' => 'hidden',
              '#value' => $field_index
            ),
            'foreign_record_id' => array(
              '#type' => 'item',
              '#markup' => $fk_value,
            ),
            'edit_submit' => array(
              '#type' => 'link',
              '#href' => str_replace(array('%template','%priority', '%field', '%action'), array($path_template_id, $path_priority, $path_field_index, 'edit_field'), $field_href_template),
              '#title' => "Edit",
            ),
            'delete_submit' => array(
              '#type' => 'link',
              '#href' => str_replace(array('%template','%priority', '%field', '%action'), array($path_template_id, $path_priority, $path_field_index,'delete_field'), $field_href_template),
              '#options' => array(
                'query' => array(
                  'record_name' => $table_array['record_id'],
                  'field_name' => $field['title'],
                  'chado_table' => $table_array['table'],
                  'chado_field' => $field['field'],
                  'data_column' => (isset($field['spreadsheet column'])) ? $field['spreadsheet column'] : '',
                  'constant_value' => (isset($field['constant value'])) ? $field['constant value'] : '',
                  'foreign_record' => $fk_value
                )
              ),
              '#title' => "Delete",
            ),
          );
          $num_fields++;
        }
    }
    $form['fields']['total_fields'] = array(
      '#type' => 'hidden',
      '#value' => $num_fields,
    );

  }

  if ($form['records']['no_records']['#value']) {
    $form['records']['description'] = array(
      '#type' => 'item',
      '#markup' => 'There are currently no records. Click on the "New Record/Field" button below to add a record and the first field associated with it.',
    );
    unset($form['records']['submit-reorder']);

    $form['fields']['description'] = array(
      '#type' => 'item',
      '#markup' => 'There are currently no fields. Each field must be associated with a record, so, first, create a new record above.',
    );

  }

  return $form;
}

/**
 * Submit for tripal_bulk_loader_modify_template_base_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_modify_template_base_form_submit($form, &$form_state) {
  $save = TRUE;
  $form_state['rebuild'] = TRUE;

  if ($form_state['storage']['template_id']) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
    $result = db_query($sql, array(':template' => $form_state['storage']['template_id']))->fetchObject();
    $form_state['storage']['template'] = unserialize($result->template_array);
  }

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];

  // part of fix for 1st button set not working on records list (edit record, duplicate record, add field)
  if ($form_state['clicked_button']['#name'] === 'zero') {
    $form_state['clicked_button']['#name'] = '0';
  }

  switch ($op) {
    // Initialize after template is chosen ----------------------------------------
    case 'Edit Template':
      $form_state['storage']['template_id'] = $form_state['values']['template_id'];

      $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
      $result = db_query($sql, array(':template' => $form_state['storage']['template_id']))->fetchObject();
      $form_state['storage']['template'] = unserialize($result->template_array);
      $form_state['storage']['template_name'] = $result->name;

      $form_state['storage']['record2priority'] = array();
      foreach ($form_state['storage']['template'] as $priority => $record_array) {
        if (!is_array($record_array)) {
          continue;
        }
        $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
      }

      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$form_state['storage']['template_id'].'/edit';
    break;

    case 'Create Template':
      $record = array(
        'name' => $form_state['values']['new_template_name'],
        'template_array' => array(),
        'created' => time(),
        'changed' => time()
      );
      $result = drupal_write_record('tripal_bulk_loader_template', $record);
      $form_state['storage']['template_id'] = $record['template_id'];
      $form_state['storage']['template_name'] = $record['name'];
      $form_state['storage']['template'] = array();
      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$record['template_id'].'/edit';
    break;

    // Save Reordered Records -----------------------------------------------------
    case 'Save Order':
      $new_template = $form_state['storage']['template'];
      // unset old elements
      $form_state['storage']['record2priority'] = array();
      foreach ($new_template as $priority => $record_array) {
        if (preg_match('/\d+/', $priority)) {
          unset($new_template[$priority]);
        }
      }
      //set elements in new order
      foreach ($form_state['values']['records-data'] as $item) {
        $new_template[$item['new_priority']] = $form_state['storage']['template'][$item['old_priority']];
        $record_name = $new_template[$item['new_priority']]['record_id'];
        $form_state['storage']['record2priority'][$record_name] = $item['new_priority'];
      }
      ksort($new_template);
      $form_state['storage']['template'] = $new_template;
    break;

    case 'New Record/Field':
      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$form_state['storage']['template_id'].'/add_record';
      $save = FALSE;
    break;

  } //end of switch

  // Save Template
  if ($save) {
    $record = array(
      'template_id' => $form_state['storage']['template_id'],
      'template_array' => serialize($form_state['storage']['template']),
      'changed' => time()
    );
    drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
    drupal_set_message(t('Template Saved.'));
  }

}

/**
 * @section
 * Delete Template
 */

/**
 * Delete Template Form
 * This form allows admin to delete already existing templates
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_base_form($form, $form_state) {
  $form = array();

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  drupal_set_breadcrumb($breadcrumb);

  $template_id = $form_state['build_info']['args'][0];
  $form_state['storage']['template_id'] = $template_id;

  $description = '';
  if (isset($_GET['template_name'])) {
    $form_state['storage']['template_name'] = $_GET['template_name'];
    $description .= '<p><strong>Template:</strong> '.$_GET['template_name'].'</p>';
  }

  $description .= '<p><strong>Are you sure you want to delete this template?</strong></p>';
  $yes = 'Delete Template';
  $no = 'Cancel';

  $form['#attributes']['class'][] = 'confirmation';
  $form['description'] = array('#markup' => $description);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $yes ? $yes : t('Confirm'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => $no ? $no : t('Cancel'),
    '#href' => 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit',
  );
  // By default, render the form using theme_confirm_form().
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'confirm_form';
  }

  return $form;
}

/**
 * Delete Template Form Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage that were submitted
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_base_form_submit($form, &$form_state) {

  if (isset($form_state['storage']['template_id'])) {
    if ($form_state['storage']['template_id'] > 0) {
      db_delete('tripal_bulk_loader_template')
        ->condition('template_id', $form_state['storage']['template_id'])
        ->execute();
      if (isset($form_state['storage']['template_name'])) {
        drupal_set_message('Successfully deleted '.$form_state['storage']['template_name'].' template.');
      }
      else {
        drupal_set_message('Successfully deleted the template');
      }
    }
  }

  $form_state['rebuild'] = FALSE;
  $form_state['redirect'] = 'admin/tripal/loaders/bulk/templates';

}

/**
 * @section
 * Import/Export Template
 */

/**
 * Import Template Form
 *
 * On export, simply selects the serialized array from the db for a given template
 * and presents it to the user. On import, a serialized template array and a name is
 * supplied and a template record is created.
 *
 * @todo Make array presented to the user more readable. (ie: unserialize and print to the screen)
 *
 * @param $form_state
 *   The values and storage for the form
 * @param $mode
 *   Either 'import' or 'export' to indicate which function is being performed
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_import_template_form($form, $form_state) {
  $form = array();

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  drupal_set_breadcrumb($breadcrumb);

  $form['new_template_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Template Name',
    '#weight' => 1,
  );

  $form['template_array'] = array(
    '#type' => 'textarea',
    '#title' => 'Template Definition',
    '#default_value' => (isset($form_state['values']['template_array'])) ? $form_state['values']['template_array'] : '',
    '#description' => t('A serialized array describing the template.'),
    '#rows' => 14,
    '#weight' => 5,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
    '#weight' => 10,
  );

  return $form;
}

/**
 * Import Template Form Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage that were submitted
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_import_template_form_submit($form, &$form_state) {

  $record = array(
    'name' => $form_state['values']['new_template_name'],
    'template_array' => $form_state['values']['template_array'],
    'created' => time(),
    'changed' => time()
  );
  $result = drupal_write_record('tripal_bulk_loader_template', $record);
  if ($result) {
    drupal_set_message(t('Successfully imported Tripal Bulk Loader Template.'));
  }

  $form_state['rebuild'] = FALSE;
  $form_state['redirect'] = 'admin/tripal/loaders/bulk/templates';

}

/**
 * Export Template Form
 *
 * On export, simply selects the serialized array from the db for a given template
 * and presents it to the user. On import, a serialized template array and a name is
 * supplied and a template record is created.
 *
 * @todo Make array presented to the user more readable. (ie: unserialize and print to the screen)
 *
 * @param $form_state
 *   The values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_export_template_form($form, $form_state) {

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  drupal_set_breadcrumb($breadcrumb);

  $template_id = $form_state['build_info']['args'][0];
  $form_state['storage']['template_id'] = $template_id;
  if ($template_id > 0) {
    $result = db_select('tripal_bulk_loader_template','t')
      ->fields('t')
      ->condition('t.template_id',$template_id)
      ->execute();
    $template = $result->fetchObject();
    $form_state['storage']['template_array'] = $template->template_array;
  }

  $form['name'] = array(
    '#type' => 'item',
    '#title' => 'Template Name',
    '#markup' => $template->name,
  );

  $form['template_array'] = array(
    '#type' => 'textarea',
    '#title' => 'Export',
    '#default_value' => $form_state['storage']['template_array'],
    '#description' => t('Use this serialized array for import.'),
    '#rows' => 30,
    '#weight' => 5,

  );

  return $form;
}

/**
 * @section
 * Edit Record Form
 */

/**
 * Edit Record Form
 *
 * D7 @todo: Needs to be debugged
 *
 * This form is meant to be called from a bulk loader form. The following should be set
 * in the query section of the path:
 *  - template_id=\d+: the template which the edited record is part of
 *  - record_id=\d+: the priority or key in the template array of the record to be edited
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_edit_template_record_form($form, $form_state) {
  $form['#cache'] = TRUE; // Make sure the form is cached.

  // get args from path
  $template_id = (isset($form_state['build_info']['args'][0])) ? $form_state['build_info']['args'][0] : FALSE;
  $form_state['storage']['template_id'] = $template_id;

  $record_id = (isset($form_state['build_info']['args'][1])) ? $form_state['build_info']['args'][1] : FALSE;
  $form_state['storage']['record_id'] = $record_id;
  $form_state['values']['field_group'] = $record_id;
  $form_state['storage']['original_priority'] = $record_id;

  // if there is no template supplied don't return rest of form
  if (!$template_id) {
    return $form;
  }

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  $breadcrumb[] = l('Edit Template', 'admin/tripal/loaders/bulk/template/' . $template_id . '/edit');
  drupal_set_breadcrumb($breadcrumb);

  // Pre-process values/defaults ---------------------------

  // If this is the first load of the form (no form state) we need to initialize some variables
  if (!isset($form_state['storage']['template'])) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
    $template = db_query($sql, array(':template' => $template_id))->fetchObject();
    $form_state['storage']['template_array'] = unserialize($template->template_array);
    $form_state['storage']['template'] = $template;

    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template_array'] as $priority => $record_array) {
      if (!is_array($record_array)) {
        continue;
      }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }

    $form_state['storage']['referring URL'] = $_SERVER["HTTP_REFERER"];
  }
  else {
    $template = $form_state['storage']['template'];
  }

  // Tables and default table
  $tables = chado_get_table_names(TRUE);
  if (isset($form_state['values']['chado_table'])) {
    $table = $form_state['values']['chado_table'];
  }
  else {
    $table = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['table'];
  }

  // get the default mode
  if (isset($form_state['storage']['original_priority'])) {
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['mode'])) {
      $mode = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['mode'];
    }
    else {
      $mode = 'insert';
    }

    // get default for the select optional
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['select_optional'])) {
      $select_optional = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['select_optional'];
    }
    else {
      $select_optional = 0;
    }

    // get default for the select if duplicate
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['select_if_duplicate'])) {
      $select_if_duplicate = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['select_if_duplicate'];
    }
    else {
      $select_if_duplicate = 1;
    }

    // get default for the update if duplicate
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['update_if_duplicate'])) {
      $update_if_duplicate = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['update_if_duplicate'];
    }
    else {
      $update_if_duplicate = 0;
    }

    // get default for the select if duplicate
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['optional'])) {
      $optional = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['optional'];
    }
    else {
      $optional = 0;
    }

    // get the default for disabling the record
    if (isset($form_state['storage']['template_array'][$form_state['storage']['original_priority']]['disable'])) {
      $disable = $form_state['storage']['template_array'][$form_state['storage']['original_priority']]['disable'];
    }
    else {
      $disable = 0;
    }
  }
  else {
     $mode = 'insert';
     $select_optional = 0;
     $select_if_duplicate = 1;
     $update_if_duplicate = 0;
     $optional = 0;
     $disable = 0;
  }

  // this is just for backwards compatibility. the insert_unique mode type is no longer available
  if (strcmp($mode, 'insert_unique')==0) {
     $mode = 'insert';
     $select_if_duplicate = 1;
  }

  // this is just for backwards compatibility. the insert_unique mode type is no longer available
  if (strcmp($mode, 'optional')==0) {
     $mode = 'insert';
     $optional = 1;
  }

  //dpm($form_state, 'form state');

   // Form Proper -------------------------------------------
  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#markup' => $template->name,
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $template_id,
  );

  $form['edit_record'] = array(
    '#type' => 'markup',
  );

  // check template array for records then add one more
  if (!$form_state['storage']['record2priority']) {
    $groups = array();
  }
  else {
    $groups = array_flip($form_state['storage']['record2priority']);
  }
  $priority_default = $form_state['values']['field_group'];
  $form['edit_record']['field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'By Changing the record here, you can move all the fields from the current record into the selected record.',
    '#options' => $groups,
    '#default_value' => $priority_default,
    '#required' => TRUE,
  );

  $form['edit_record']['record_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Unique Record Name',
    '#default_value' => $groups[$priority_default],
  );

  $form['edit_record']['chado_table'] = array(
    '#type' => 'select',
    '#title' => t('Chado Table'),
    '#description' => 'This changes the chado table for all fields in this record.',
    '#options' => $tables,
    '#default_value' => $table,
  );

  $form['edit_record']['mode'] = array(
    '#type' => 'radios',
    '#title' => 'Action to take when Loading Record',
    '#options' => array(
      'select' => 'SELECT: Don\'t insert this record: it\'s used to define a foreign key in another record',
      'select_once' => 'SELECT ONCE: Select the record only once for each constant set.',
      'insert' => 'INSERT: Insert the record',
      'insert_once' => 'INSERT ONCE: Record will be inserted once for each constant set. If the record is the same across multiple constant sets, make sure to select "SELECT if duplicate" as well.',
    ),
    '#default_value' => $mode
  );

  $form['edit_record']['select_options'] = array(
     '#type' => 'markup',
     '#markup' => t('Additional Select Options:'),
  );
  $form['edit_record']['select_optional'] = array(
     '#type' => 'checkbox',
     '#title' => t('Continue if no record exists or too many exist.'),
     '#description' => t('By default if a select does not find a match the loader will fail, or if it finds too many matches it will fail.  Check here to allow the loader to continue when no match is found. In either case no value is passed on.'),
     '#default_value' => $select_optional
  );

  $form['edit_record']['insert_options'] = array(
     '#type' => 'markup',
     '#prefix' => '<br>',
     '#markup' => t('Additional Insert Options:'),
  );

  $form['edit_record']['select_if_duplicate'] = array(
     '#type' => 'checkbox',
     '#title' => t('SELECT if duplicate (no insert)'),
     '#description' => t('If this is not the first time this record has been added then perform a select rather than an insert.'),
     '#default_value' => $select_if_duplicate
  );

  $form['edit_record']['update_if_duplicate'] = array(
     '#type' => 'checkbox',
     '#title' => t('UPDATE if duplicate (no insert)'),
     '#description' => t('If this is not the first time this record has been added then perform an update rather than an insert.'),
     '#default_value' => $update_if_duplicate
  );

  $form['edit_record']['optional'] = array(
     '#type' => 'checkbox',
     '#title' => t('Optional'),
     '#description' => t('The insert, update or select will only be performed only if all required data are present'),
     '#default_value' => $optional
  );
  $form['edit_record']['disable'] = array(
     '#type' => 'checkbox',
     '#title' => t('Disable this record'),
     '#description' => t("Check this box to ignore this record (not perform select or insert) during template loading.  Uncheck to re-enable the record"),
     '#default_value' => $disable,
  );

  $form['edit_record']['submit-edit_record'] = array(
      '#type' => 'submit',
      '#value' => 'Edit Record'
  );

  $form['edit_record']['submit-cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel'
  );

  return $form;
}

/**
 * Implements hook_form_validate()
 *
 * D7 @todo: Needs to be debugged
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_edit_template_record_form_validate($form, $form_state) {

  // Don't worry about validation when Cancel button is clicked
  if ($form_state['clicked_button']['#value'] == 'Edit Record') {
    $is_unique = tripal_bulk_loader_is_record_name_unique(
      $form_state['values']['record_name'],
      $form_state['values']['template_id'],
      $form_state['storage']['template_array'],
      $form_state['values']['field_group']
    );
    if (!$is_unique) {
      form_set_error('record_name', "New Record Name must be unique. '" . $form_state['values']['record_name'] . "' is not unique.");
    }
  }

}

/**
 * Edit Record Form Submit
 *
 * D7 @todo: Needs to be debugged
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   Contains the values and storage for the form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_edit_template_record_form_submit($form, &$form_state) {

  if ($form_state['values']['op'] ==  'Edit Record') {

    $template = $form_state['storage']['template_array'];
    $original_record_name = $template[ $form_state['storage']['original_priority'] ]['record_id'];

    // Edit Record
    $record = $template[ $form_state['storage']['original_priority'] ];
    $record['record_id'] = $form_state['values']['record_name'];
    $record['mode'] = $form_state['values']['mode'];
    $record['table'] = $form_state['values']['chado_table'];
    $record['select_if_duplicate'] = $form_state['values']['select_if_duplicate'];
    $record['update_if_duplicate'] = $form_state['values']['update_if_duplicate'];
    $record['select_optional'] = $form_state['values']['select_optional'];
    $record['disable'] = $form_state['values']['disable'];
    $record['optional'] = $form_state['values']['optional'];

    if ($form_state['storage']['original_priority'] != $form_state['values']['field_group']) {
      $record['fields'] = array_merge($record['fields'], $template[ $form_state['values']['field_group'] ]['fields']);
      $template[ $form_state['values']['field_group'] ] = $record;
      unset($template[ $form_state['storage']['original_priority'] ]);
    }
    else {
      $template[ $form_state['storage']['original_priority'] ] = $record;
    }

    // Go through all records and update this record name where it was used for a foreign key
    // Foreach field in each record that is of type: foreign key
    foreach ($template as $priority => $record) {
      foreach ($record['fields'] as $field_index => $field) {
        if ($field['type'] === 'foreign key') {
          // Check if this points to the old record name and if so, update it
          if ($field['foreign key'] === $original_record_name) {
            $template[$priority]['fields'][$field_index]['foreign key'] = $form_state['values']['record_name'];
          }
        }
      }
    }

    // Save Template
    $form_state['storage']['template']->template_array = serialize($template);
    $form_state['storage']['template']->changed = time();
    $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));

    if ($success) {
      drupal_set_message(t('Successfully Updated Template Record'));
      drupal_set_message(t('Template Saved.'));

      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/' . $form_state['storage']['template']->template_id . '/edit';
    }
    else {
      drupal_set_message(t('Unable to Save Template!'), 'error');
      tripal_report_error('T_bulk_loader',
        TRIPAL_ERROR,
        'Unable to save bulk loader template: %template',
        array('%template' => print_r($form_state['storage']['template'], TRUE))
      );
    }
  }
  elseif ($form_state['values']['op'] ==  'Cancel') {
      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/' . $form_state['storage']['template']->template_id . '/edit';
  }

}

/**
 * Delete Record Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * D7 @todo: Needs to be debugged
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_record_form($form, $form_state) {
  $form['#cache'] = TRUE; // Make sure the form is cached.

  // get args from path
  $template_id = (isset($form_state['build_info']['args'][0])) ? $form_state['build_info']['args'][0] : FALSE;
  $form_state['storage']['template_id'] = $template_id;
  $record_id = (isset($form_state['build_info']['args'][1])) ? $form_state['build_info']['args'][1] : FALSE;
  $form_state['storage']['record_id'] = $record_id;

  // if there is no template supplied don't return rest of form
  if (!$template_id) {
    return $form;
  }

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  $breadcrumb[] = l('Edit Template', 'admin/tripal/loaders/bulk/template/' . $template_id . '/edit');
  drupal_set_breadcrumb($breadcrumb);

  $_GET['record_name'] = (isset($_GET['record_name'])) ?  $_GET['record_name'] : '';
  $_GET['chado_table'] = (isset($_GET['chado_table'])) ?  $_GET['chado_table'] : '';
  $_GET['record_mode'] = (isset($_GET['record_mode'])) ?  $_GET['record_mode'] : '';

  $description = '';
  $description .= '<table>';
  $description .= '<tr><th></th><th>Record Name</th><th>Chado Table</th><th>Mode</th></tr>';
  $description .= '<tr><td>' . $record_id
                . '</td><td>' . $_GET['record_name']
                . '</td><td>' . $_GET['chado_table']
                . '</td><td>' . $_GET['record_mode']
                . '</td></tr>';
  $description .= '</table>';

  $description .= '<p><strong>Are you sure you want to delete this record?</strong></p>';
  $yes = 'Delete Record';
  $no = 'Cancel';

  $form['#attributes']['class'][] = 'confirmation';
  $form['description'] = array('#markup' => $description);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $yes ? $yes : t('Confirm'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => $no ? $no : t('Cancel'),
    '#href' => 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit',
  );
  // By default, render the form using theme_confirm_form().
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'confirm_form';
  }

  return $form;
}

/**
 * Delete Record Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * D7 @todo: Needs to be debugged
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_record_form_submit($form, &$form_state) {
  $form_state['rebuild']  = FALSE;
  $template_id = $form_state['storage']['template_id'];
  $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit';

  $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
  $result = db_query($sql, array(':template' => $form_state['storage']['template_id']))->fetchObject();
  $form_state['storage']['template'] = unserialize($result->template_array);

  $form_state['storage']['record2priority'] = array();
  $new_template = tripal_bulk_loader_delete_record($form_state['storage']['record_id'], $form_state['storage']['template']);
  if (!empty($new_template)) {
    $form_state['storage']['template'] = $new_template;
  }
  drupal_set_message(t('Deleted Field from Template.'));

  $record = array(
    'template_id' => $form_state['storage']['template_id'],
    'template_array' => serialize($form_state['storage']['template']),
    'changed' => time()
  );
  drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  drupal_set_message(t('Template Saved.'));
}

/**
 * Duplicate Record Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * D7 @todo: Needs to be debugged
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_duplicate_template_record_form($form, &$form_state) {
  $form['#cache'] = TRUE; // Make sure the form is cached.

  // get args from path
  $template_id = (isset($form_state['build_info']['args'][0])) ? $form_state['build_info']['args'][0] : FALSE;
  $form_state['storage']['template_id'] = $template_id;
  $record_id = (isset($form_state['build_info']['args'][1])) ? $form_state['build_info']['args'][1] : FALSE;
  $form_state['storage']['record_id'] = $record_id;

  // if there is no template supplied don't return rest of form
  if (!$template_id) {
    return $form;
  }

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  $breadcrumb[] = l('Edit Template', 'admin/tripal/loaders/bulk/template/' . $template_id . '/edit');
  drupal_set_breadcrumb($breadcrumb);

  $_GET['record_name'] = (isset($_GET['record_name'])) ?  $_GET['record_name'] : '';
  $_GET['chado_table'] = (isset($_GET['chado_table'])) ?  $_GET['chado_table'] : '';
  $_GET['record_mode'] = (isset($_GET['record_mode'])) ?  $_GET['record_mode'] : '';

  $description = '';
  $description .= '<table>';
  $description .= '<caption>Record to be Duplicated</caption>';
  $description .= '<tr><th></th><th>Record Name</th><th>Chado Table</th><th>Mode</th></tr>';
  $description .= '<tr><td>' . $record_id
                . '</td><td>' . $_GET['record_name']
                . '</td><td>' . $_GET['chado_table']
                . '</td><td>' . $_GET['record_mode']
                . '</td></tr>';
  $description .= '</table>';
  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => $description,
  );

  $form['new_record_name'] = array(
    '#type' => 'textfield',
    '#title' => t('New Record Name'),
    '#default_value' => $record_id . '_' . date('YzHi')
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Duplicate Record'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit',
  );

  return $form;
}

/**
 * Delete Record Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * D7 @todo: Needs to be debugged
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_duplicate_template_record_form_submit($form, &$form_state) {
  $form_state['rebuild']  = FALSE;
  $template_id = $form_state['storage']['template_id'];
  $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit';

  $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
  $result = db_query($sql, array(':template' => $form_state['storage']['template_id']))->fetchObject();
  $form_state['storage']['template'] = unserialize($result->template_array);

  // original record (one to be duplicated)
  $orig_priority = $form_state['storage']['record_id'];
  $record = $form_state['storage']['template'][ $orig_priority ];

  // new record
  $record['record_id'] = $form_state['values']['new_record_name'];
  $form_state['storage']['template'][] = $record;

  $record = array(
    'template_id' => $form_state['storage']['template_id'],
    'template_array' => serialize($form_state['storage']['template']),
    'changed' => time()
  );
  drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  drupal_set_message(t('Template Saved.'));
}

/**
 * @section
 * Add/Edit Field Forms
 */

/**
 * Determine default values needed for add/edit field form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_template_field_form_default_values($mode, &$form_state) {
  $v = array();

  $v['mode'] = $mode;
  $v['template_id'] = (isset($form_state['build_info']['args'][1])) ? $form_state['build_info']['args'][1] : FALSE;
  $form_state['storage']['template_id'] = $v['template_id'];
  $template_id = $v['template_id'];
  $v['record_id'] = (isset($form_state['build_info']['args'][2])) ? $form_state['build_info']['args'][2] : FALSE;
  $v['no_record_id'] = ($mode == 'create_record') ? TRUE : FALSE;
  $form_state['storage']['record_id'] = $v['record_id'];
  $record_id = $v['record_id'];
  $priority = $record_id;
  $v['field_index'] = (isset($form_state['build_info']['args'][3])) ? $form_state['build_info']['args'][3] : FALSE;
  $v['no_field_index'] = (isset($form_state['build_info']['args'][3])) ? FALSE : TRUE;
  $form_state['storage']['field_index'] = $v['field_index'];
  $field_index = $v['field_index'];

  if (!isset($form_state['values'])) {
    $form_state['values'] = array();
  }

  // Get the template if not already stored
  if (!isset($form_state['storage']['template'])) {
    $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
    $template = db_query($sql, array(':template' => $template_id))->fetchObject();
    $form_state['storage']['template_array'] = unserialize($template->template_array);
    if (!$form_state['storage']['template_array']) {
      $form_state['storage']['template_array'] = array();
    }
    $form_state['storage']['template'] = $template;

    $form_state['storage']['record2priority'] = array();
    foreach ($form_state['storage']['template_array'] as $priority => $record_array) {
      if (!is_array($record_array)) {
        continue;
      }
      $form_state['storage']['record2priority'][$record_array['record_id']] = $priority;
    }
  }
  else {
    $template = $form_state['storage']['template'];
  }

  // If we are editing then get the original values
  if ($mode == 'edit' && !$v['no_field_index']) {
    $template_field = $form_state['storage']['template_array'][$v['record_id']]['fields'][$field_index];
    $original_field = $template_field;
    $form_state['storage']['original_field'] = $template_field;
    $form_state['storage']['original_field']['priority'] = $priority;
    $form_state['storage']['original_field']['field_index'] = $field_index;
  }

  $v['template_name'] = (isset($template->name)) ? $template->name : '';

  if (isset($form_state['values']['field_type'])) {
    $v['field_type'] = $form_state['values']['field_type'];
  }
  elseif (isset($original_field['type'])) {
    $v['field_type'] = $original_field['type'];
  }
  else {
    $v['field_type'] = 'table field';
  }
  $field_type = $v['field_type'];

  // Set field type options
  // First set original field values in $form_state['values'] if not already set
  switch ($v['field_type']) {
    case 'table field':
      if (!isset($form_state['values']['column_number'])) {
        $form_state['values']['column_number'] = (isset($original_field['spreadsheet column'])) ? $original_field['spreadsheet column'] : '';
        $form_state['values']['column_exposed'] = (isset($original_field['exposed'])) ? $original_field['exposed'] : FALSE;
        $form_state['values']['column_exposed_desc'] = (isset($original_field['exposed_description'])) ? $original_field['exposed_description'] : FALSE;
      }
      break;
    case 'constant':
      if (!isset($form_state['values']['constant_value'])) {
        $form_state['values']['constant_value'] = (isset($original_field['constant value'])) ? $original_field['constant value'] : '';
        $form_state['values']['constant_exposed'] = (isset($original_field['exposed'])) ? $original_field['exposed'] : FALSE;
        $form_state['values']['constant_validate'] = (isset($original_field['exposed_validate'])) ? $original_field['exposed_validate'] : FALSE;
      }
      break;
    case 'foreign key':
      if (!isset($form_state['values']['foreign_record'])) {
        $form_state['values']['foreign_record'] = (isset($original_field['foreign key'])) ? $original_field['foreign key'] : FALSE;
        $form_state['values']['foreign_field'] = (isset($original_field['foreign field'])) ? $original_field['foreign field'] : FALSE;
      }
      break;
  }

  // Ensure other field type fields are reset/cleared except for those of the current type
  // ie: if the type is changed from spreadsheet column to constant, the spreadsheet column
  // fields should be cleared since they don't apply to a constant
  switch ($v['field_type']) {
    case 'table field':
      $v['column_number'] = (isset($form_state['values']['column_number'])) ? $form_state['values']['column_number'] : '';
      $v['column_exposed'] = (isset($form_state['values']['column_exposed'])) ? $form_state['values']['column_exposed'] : FALSE;
      $v['column_exposed_desc'] = (isset($form_state['values']['column_exposed_desc'])) ? $form_state['values']['column_exposed_desc'] : FALSE;
      $v['constant_value'] = '';
      $v['constant_exposed'] = FALSE;
      $v['constant_validate'] = FALSE;
      break;
    case 'constant':
      $v['column_number'] = '';
      $v['column_exposed'] = FALSE;
      $v['column_exposed_desc'] = FALSE;
      $v['constant_value'] = (isset($form_state['values']['constant_value'])) ? $form_state['values']['constant_value'] : '';
      $v['constant_exposed'] = (isset($form_state['values']['constant_exposed'])) ? $form_state['values']['constant_exposed'] : FALSE;
      $v['constant_validate'] = (isset($form_state['values']['constant_validate'])) ? $form_state['values']['constant_validate'] : FALSE;
      break;
    case 'foreign key':
      $v['column_number'] = '';
      $v['column_exposed'] = FALSE;
      $v['column_exposed_desc'] = FALSE;
      $v['constant_value'] = '';
      $v['constant_exposed'] = FALSE;
      $v['constant_validate'] = FALSE;
      break;
  }

  // Use the record_id as the priority/field_group
  if ($v['no_record_id']) {
    $form_state['values']['field_group'] = 'NEW';
    $v['field_group'] = 'NEW';
  }
  else {
    $form_state['values']['field_group'] = $record_id;
    $v['field_group'] = $record_id;
    if (preg_match('/\d+/', $record_id)) {
      $priority = $form_state['values']['field_group'];
      $table = $form_state['storage']['template_array'][$priority]['table'];
    }
  }

  // Chado Tables and default table
  $tables = chado_get_table_names(TRUE);
  if (isset($form_state['values']['chado_table'])) {
    if (!preg_match('/^' . current($tables) . '$/', $form_state['values']['chado_table'])) {
      $table = $form_state['values']['chado_table'];
    }
    elseif (isset($form_state['values']['record_name'])) {
      $record_name = $form_state['values']['record_name'];
      $priority = $form_state['storage']['record2priority'][$record_name];
      $table = $form_state['storage']['template_array'][$priority]['table'];
    }
    else {
      $priority = $form_state['values']['field_group'];
      $table = $form_state['storage']['template_array'][$priority]['table'];
    }
  }
  if (!isset($table)) {
    $table = reset($tables);
  }

  // Chado fields
  $chado_fields = array();
  $table_description = chado_get_schema($table);
  foreach ($table_description['fields'] as $field_name => $field_array) {
    $chado_fields[$field_name] = $field_name;
  }
  // Chado Default Field
  if (empty($chado_fields)) {
    $field = NULL;
  }
  elseif (isset($form_state['values']['chado_field'])) {
    if (isset($chado_fields[$form_state['values']['chado_field']])) {
      $field = $form_state['values']['chado_field'];
    }
    else {
      $field = current($chado_fields);
    }
  }
  elseif (isset($original_field['field'])) {
    $field = $original_field['field'];
  }
  else {
    $field = current($chado_fields);
  }

  // Field group options
  if (!$form_state['storage']['record2priority']) {
    $groups = array();
  }
  else {
    $groups = array_flip($form_state['storage']['record2priority']);
  }
  $groups['NONE'] = 'Select a Record';
  $groups['NEW'] = 'New Record';

  // Field Group Default
  if ($record_id) {
    $field_group = $record_id;
  }
  elseif (isset($form_state['values']['field_group'])) {
    $field_group = $form_state['values']['field_group'];
  }
  else {
    if ($mode == 'add') {
      $field_group = 'NEW';
    }
    else {
      $field_group = 'NONE';
    }
  }

  $v['field_group'] = $field_group;
  $v['field_group_options'] = $groups;
  $v['chado_table'] = $table;
  $v['chado_table_options'] = $tables;
  $v['chado_field'] = $field;
  $v['chado_field_options'] = $chado_fields;

  $v['record_name'] = (isset($form_state['values']['record_name'])) ? $form_state['values']['record_name'] : '';
  if (isset($form_state['values']['field_title'])) {
    $v['field_title'] =  $form_state['values']['field_title'];
  }
  elseif (isset($original_field['title'])) {
    $v['field_title'] =  $original_field['title'];
  }
  else {
    $v['field_title'] =  '';
  }

  $v['show_all_records'] = (isset($form_state['values']['show_all_records'])) ? $form_state['values']['show_all_records'] : FALSE;

  // Foreign key options
  $fk_options = array();
  $fk_options['NULL'] = 'None';
  $show_all = $v['show_all_records'];
  if ($field_type == 'foreign key' and !$show_all) {

    $foreign_field2table = array();
    foreach ($table_description['foreign keys'] as $key_table => $key_array) {
      foreach ($key_array['columns'] as $left_field => $right_field) {
        //$chado_fields[$left_field] = $left_field;
        $foreign_field2table[$left_field] = $key_table;
      }
    }

    // Foreign key options
    $foreign_table = $foreign_field2table[$field];
    if ($foreign_table) {
      foreach ($form_state['storage']['record2priority'] as $record_name => $priority ) {
        if (preg_match('/^' . $foreign_table . '$/', $form_state['storage']['template_array'][$priority]['table'])) {
          $fk_options[$record_name] = $record_name;
        }
      }
    }
  }
  // the user wants to see all of the records
  elseif ($field_type == 'foreign key' and $show_all) {
    foreach ($form_state['storage']['record2priority'] as $record_name => $priority ) {
      $fk_options[$record_name] = $record_name;
    }
  }

  // build the list of referrer table fields. This list is used
  // for the referring table field drop down. It has two groups
  // one for foreign keys and another for additional fields.
  $ref_chado_fields = array();
  if ($field_type == 'foreign key') {
    $fk_rec = $form_state['values']['foreign_record'];
    if ($fk_rec and $fk_rec != 'None' and $fk_rec != 'NULL') {

      // first add in the foreign key field in the referring table
      // that corresponds to this field (if one exists).
      $fk_priority = $form_state['storage']['record2priority'][$fk_rec];
      $fk_table = $form_state['storage']['template_array'][$fk_priority]['table'];
      foreach ($table_description['foreign keys'] as $key_table => $key_array) {
        foreach ($key_array['columns'] as $left_field => $right_field) {
          if ($key_table == $fk_table and $left_field == $field) {
            $ref_chado_fields['Foreign Key'][$right_field] = $right_field;
          }
        }
      }
      // now add in the remaining fields of the referring record's table.
      $fk_description = chado_get_schema($fk_table);
      foreach ($fk_description['fields'] as $fk_field_name => $fk_farray) {
        // don't include the FK field it's included above
        if (in_array('Foreign Key', $ref_chado_fields)) {
          if (in_array($fk_field_name, $ref_chado_fields['Foreign Key'])) {
            continue;
          }
        }
        $ref_chado_fields['Additional Table Fields'][$fk_field_name] = $fk_field_name;
      }
    }
  }
  $ref_chado_fields = (!empty($ref_chado_fields)) ? $ref_chado_fields : array('NULL', 'None');

  $v['foreign_record_options'] = $fk_options;
  $v['foreign_record'] = (isset($form_state['values']['foreign_record'])) ? $form_state['values']['foreign_record'] : 'NULL';
  $v['foreign_field_options'] = $ref_chado_fields;
  $v['foreign_field'] = (isset($form_state['values']['foreign_field'])) ? $form_state['values']['foreign_field'] : current($ref_chado_fields);
  $v['required'] = (isset($form_state['values']['required'])) ? $form_state['values']['required'] : FALSE;

  if (isset($original_field['regex']) && empty($form_state['storage']['regex']['pattern'])) {
    $form_state['storage']['regex']['pattern'] = $original_field['regex']['pattern'];
    $form_state['storage']['regex']['replace'] = $original_field['regex']['replace'];
  }
  $v['regex_are_set'] = (isset($form_state['storage']['regex']['pattern'])) ? TRUE : FALSE;
  $v['regex-pattern'] = (isset($form_state['storage']['regex']['pattern'])) ? $form_state['storage']['regex']['pattern'] : array();
  $v['regex-replace'] = (isset($form_state['storage']['regex']['replace'])) ? $form_state['storage']['regex']['replace'] : array();

  $v['regex-test-string'] = (isset($form_state['storage']['test_regex_test'])) ? $form_state['storage']['test_regex_test'] : '';
  $v['regex-test-result'] = (isset($form_state['storage']['test_regex_result'])) ? $form_state['storage']['test_regex_result'] : '';

  return $v;
}

/**
 * Add Field Form
 *
 * This form is meant to be called from a bulk loader form. Blank Defaults are in place but you
 * can use the following in the query of the path to set defaults for a given template:
 *  - template_id=\d+: the template to add the field to
 *  - record_id=\d+: the priority or key in the template array of the record to add the field to
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_template_field_form($form, $form_state = NULL) {
  $form['#cache'] = TRUE; // Make sure the form is cached.

  // if there is no template supplied don't return rest of form
  if (!isset($form_state['build_info']['args'][1])) {
    return $form;
  }

  $v['mode'] = (isset($form_state['build_info']['args'][0])) ? $form_state['build_info']['args'][0] : 'create';
  $form_state['storage']['mode'] = $v['mode'];
  $mode = $v['mode'];

  $values = tripal_bulk_loader_template_field_form_default_values($mode, $form_state);

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  $breadcrumb[] = l('Edit Template', 'admin/tripal/loaders/bulk/template/' . $values['template_id'] . '/edit');
  drupal_set_breadcrumb($breadcrumb);

  $form['template_name'] = array(
    '#type' => 'item',
    '#title' => 'Template',
    '#markup' => $values['template_name'],
  );

  $form['template_id'] = array(
    '#type' => 'hidden',
    '#value' => $values['template_id'],
  );

  $form['fields'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="tripal_bulk_loader_template-template_fields">',
    '#suffix' => '</div>',
  );

  $form['fields']['basic'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="basic">',
    '#suffix' => '</div>'
  );

  $form['fields']['basic']['field_type'] = array(
    '#type' => 'radios',
    '#title' => t('Type of Field'),
    '#options' => array(
      'table field' => t('Data: A Field which maps to a column in the supplied file.'),
      'constant' => t('Constant: Field which remains Constant throughout the file'),
      'foreign key' => t('Record Referral: Fields which map to a record in another table'),
    ),
    '#required' => TRUE,
    '#default_value' => $values['field_type'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
    ),
  );

  $form['fields']['basic']['field_group']  = array(
    '#type' => 'select',
    '#title' => 'Record',
    '#description' => 'This is used to group a set of fields together allowing '
      .'multiple records to be inserted into the same table per line of the file',
    '#options' => $values['field_group_options'],
    '#default_value' => $values['field_group'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
    ),
    '#required' => TRUE,
  );

  $form['fields']['basic']['record_name'] = array(
    '#type' => ($values['no_record_id']) ? 'textfield' : 'hidden',
    '#title' => 'Unique Record Name',
    '#prefix' => '<div id="tripal_bulk_loader_template-add_record">',
    '#suffix' => '</div>',
    '#description' => 'A human-readable name for the record; it should be unique.',
    '#default_value' => $values['record_name'],
  );

  $form['fields']['basic']['field_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Human-readable Title for Field'),
    '#default_value' => $values['field_title'],
  );


  // Chado Field
  $form['fields']['chado'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chado Field/Column Details'),
    '#description' => t('Specify the Table/Field in chado that this field maps to.'),
  );

  $form['fields']['chado']['chado_table'] = array(
    '#type' => 'select',
    '#title' => t('Chado Table'),
    '#options' => $values['chado_table_options'],
    '#default_value' => $values['chado_table'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
      ),
  );

  $form['fields']['chado']['chado_field'] = array(
    '#type' => 'select',
    '#title' => t('Chado Field/Column'),
    '#options' => $values['chado_field_options'],
    '#default_value' => $values['chado_field'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
    ),
  );

  // loading file data column
  $form['fields']['columns'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data File Column'),
    '#collapsible' => TRUE,
    '#collapsed' => ($values['field_type'] == 'table field')? FALSE : TRUE,
  );

  $form['fields']['columns']['column_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Column'),
    '#description' => t('Specify the column in the data that this field maps to where the first column is 1.'),
    '#size' => 5,
    '#default_value' => $values['column_number'],
  );

  $form['fields']['columns']['column_exposed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Column to be set for each Bulk Loading Job'),
    '#description' => t('Adds a textbox field to the Bulk Loader Page to allow users to set this value.'),
    '#default_value' => $values['column_exposed'],
  );

  $form['fields']['columns']['column_exposed_desc'] = array(
    '#type' => 'textfield',
    '#title' => t('Description for exposed field on bulk loading job'),
    '#description' => t('This description should tell the user what column should be entered here.'),
    '#default_value' => $values['column_exposed_desc'],
  );

  // Global Value
  $form['fields']['constant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Constant'),
    '#collapsible' => TRUE,
    '#collapsed' => ($values['field_type'] == 'constant')? FALSE : TRUE,
  );

  $form['fields']['constant']['constant_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Value'),
    '#description' => t('Specify the value you wish this field to have regardless of data file value.'),
    '#default_value' => $values['constant_value']
  );

  $form['fields']['constant']['constant_exposed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Constant to be set for each Bulk Loading Job'),
    '#description' => t('Adds a textbox field to the Create Bulk Loader Form to allow users to set this value.'),
    '#default_value' => $values['constant_exposed'],
  );

  $form['fields']['constant']['constant_validate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ensure value is in table'),
    '#description' => t('Checks the database when a bulk loading job is created to ensure the value entered already exists in the database.'),
    '#default_value' => $values['constant_validate'],
  );

  // Foreign Key / Referrer
  $form['fields']['foreign_key'] = array(
    '#type' => 'fieldset',
    '#title' => 'Record Referral',
    '#collapsible' => TRUE,
    '#collapsed' => ($values['field_type'] == 'foreign key')? FALSE : TRUE,
  );

  $form['fields']['foreign_key']['show_all_records'] = array(
    '#type' => 'checkbox',
    '#title' => 'Refer to any record',
    '#description' => t('By default, the bulk loader will only allow referral to records in a foreign key relationship.  To allow referral to any previous record, check this box'),
    '#default_value' => $values['show_all_records'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
    ),
  );

  $form['fields']['foreign_key']['foreign_record'] = array(
    '#type' => 'select',
    '#title' => 'Record to refer to',
    '#descripion' => 'Select the record that this value should refer to. The record needs to already exist.',
    '#options' => $values['foreign_record_options'],
    '#ajax' => array(
      'callback' => 'tripal_bulk_loader_template_fields_ahah',
      'wrapper' => 'tripal_bulk_loader_template-template_fields',
      'effect' => 'fade'
    ),
    '#default_value' => $values['foreign_record'],
  );

  $form['fields']['foreign_key']['foreign_field'] = array(
    '#type' => 'select',
    '#title' => 'Field to refer to',
    '#descripion' => 'Select the record that this value should refer to. The record needs to already exist.',
    '#options' => $values['foreign_field_options'],
    '#default_value' => $values['foreign_field'],
  );


  $form['fields']['additional'] = array(
    '#type' => 'fieldset',
    '#title' => 'Additional Options',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  $form['fields']['additional']['required'] = array(
    '#type' => 'checkbox',
    '#title' => 'Make this field required',
    '#default_value' => $values['required'],
  );

  $form['fields']['additional']['regex_transform'] = array(
    '#type' => 'fieldset',
    '#title' => 'Transform Data File Value Rules',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE//$values['regex_are_set'],
  );

  $form['fields']['additional']['regex_transform']['regex_description'] = array(
    '#type' => 'item',
    '#markup' => 'A transformation rule allows you to transform the original value '
      .'(usually from a user submitted data file) into the form you would like it stored '
      .'in the chado database. Each rule consists of a match pattern (a php regular expression '
      .'which determines which replacement patterns are applied and captures regions of the '
      .'original value) and a replacement pattern (a string which may contain capture references '
      .'that describes what the new value should be). Each rule is applied to the result of the '
      .'previous rule.'
  );

  $form['fields']['additional']['regex_transform']['regex-data'] = array(
    '#tree' => TRUE,
    '#theme' => 'tripal_bulk_loader_field_regex_fieldset'
  );
  foreach ($values['regex-pattern'] as $index => $pattern) {
    $data_element = array(
      'pattern' => array(
        '#type' => 'item',
        '#markup' => check_plain($pattern),
      ),
      'replace' => array(
        '#type' => 'item',
        '#markup' => check_plain($values['regex-replace'][$index]),
      ),
      'old_index' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'new_index' => array(
        '#type' => 'select',
        '#options' => range(0, sizeof($values['regex-pattern'])-1),
        '#default_value' => $index,
        '#attributes' => array('class' => array('rank-weight')), // needed for table dragging
      ),
      'id' => array(
        '#type' => 'hidden',
        '#value' => $index,
      ),
      'submit-delete' => array(
        '#type' => 'submit',
        '#value' => 'Delete Transformation',
        '#name' => $index,
      ),
    );
    $form['fields']['additional']['regex_transform']['regex-data'][$index] = $data_element;
  }

  $form['fields']['additional']['regex_transform']['submit-reorder_regex'] = array(
    '#type' => ($values['regex_are_set']) ? 'submit' : 'hidden',
    '#value' => 'Save Transformation Rule Order'
  );

  $form['fields']['additional']['regex_transform']['new_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Add a new Transformation Rule',
  );

  $form['fields']['additional']['regex_transform']['new_regex']['pattern'] = array(
    '#type' => 'textfield',
    '#title' => 'Match Pattern',
    '#description' => 'You can use standard php regular expressions in this field to specify a '
      .'pattern. Only if this pattern matches the value in the data file does the replacement '
      .'pattern get applied to the value. To capture a section of your value for use in the '
      .'replacement patten surround with round brackets. For example, <i>GI:(\d+)</i> will match '
      .' NCBI gi numbers and will capture the numerical digits for use in the replacement pattern. '
      .' To match and capture any value use <i>.*</i>',
  );

  $form['fields']['additional']['regex_transform']['new_regex']['replace'] = array(
    '#type' => 'textfield',
    '#title' => 'Replacement Pattern',
    '#description' => 'This pattern should contain the text you want to replace the match pattern '
    .'mentioned above. It can include references of the form \n where n is the number of the '
    .'capture in the match pattern. For example, \1 will be replaced with the text matched in your '
    .'first set of round brackets.',
  );

  if ($values['field_type'] == 'table field') {
    $tab = '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp';
    $form['fields']['additional']['regex_transform']['new_regex']['replace']['#description'] .= '<p>'
      .'The following references are also available for data file fields: <b><#column:<i>number</i>#></b>. '
      .'This allows you to substitute other data file values into the current field. For example, '
      .'if you had the following line:<br />'
      . $tab . 'SNP' . $tab . '15-Jan-2011' . $tab . '1' . $tab . '54' . $tab . 'Contig34355'
      .'<br /> and your current field is for column #1 and you\'re inserting into the chado field '
      .'feature.uniquename then you might want to add in the data to ensure your uniquename is '
      .'unique. The Match Pattern is (.*) to select all the first column and the Replacement '
      .'Pattern could be \1_<#column:2#> which would insert SNP_15-Jan-2011 into the database.</p>';
  }

  $form['fields']['additional']['regex_transform']['new_regex']['submit-add_transform'] = array(
    '#type' => 'submit',
    '#value' => 'Add Transformation',
  );

  $form['fields']['additional']['regex_transform']['test_regex'] = array(
    '#type' => 'fieldset',
    '#title' => 'Test Transformation Rules',
  );

  $form['fields']['additional']['regex_transform']['test_regex']['test_string'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Value',
    '#description' => 'This should be a value that you expect the above transformation rules '
      .'to be applied to.',
    '#default_value' => $values['regex-test-string'],
  );

  $form['fields']['additional']['regex_transform']['test_regex']['test_result'] = array(
    '#type' => 'textfield',
    '#title' => 'Test Result',
    '#description' => 'This is the value that would be saved to the database after the above transformation '
      .'riles were applied to the Test Value.',
    '#default_value' => $values['regex-test-result'],
  );

  $form['fields']['additional']['regex_transform']['test_regex']['submit-test'] = array(
    '#type' => 'submit',
    '#value' => 'Test Transformation Rules'
  );

  $form['submit-save'] = array(
      '#type' => 'submit',
      '#value' => 'Save Changes'
  );

  $form['submit-cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel'
  );

  return $form;
}

/**
 * Implements hook_form_validate().
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_template_field_form_validate($form, $form_state) {

  // Don't worry about validation when Cancel button is clicked
  if ($form_state['clicked_button']['#value'] == 'Save Changes') {
    $is_unique = tripal_bulk_loader_is_record_name_unique(
      $form_state['values']['record_name'],
      $form_state['values']['template_id'],
      $form_state['storage']['template_array']
    );
    $new_record = ($form_state['values']['field_group'] == 'NEW') ? TRUE : FALSE;
    if ((!$is_unique) AND $new_record) {
      form_set_error('record_name', "New Record Name must be unique. '" . $form_state['values']['record_name'] . "' is not unique.");
    }
  }

}

/**
 * Add Field Submit
 *
 * @param $form
 *   The form that was submitted
 * @param $form_state
 *   The values and storage for the form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_template_field_form_submit($form, &$form_state) {

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  if ($op ==  'Save Changes') {

    $template = $form_state['storage']['template_array'];

     // If new record
    if (preg_match('/NEW/', $form_state['values']['field_group'])) {
      $record_name = $form_state['values']['record_name'];
      $priority = sizeof($form_state['storage']['template_array']) + 1;
      $record2priority[$record_name] = $priority;
      $template[$priority]['table'] = $form_state['values']['chado_table'];
      $template[$priority]['record_id'] = $record_name;

    }
    else {
      $priority = $form_state['values']['field_group'];
    }

    // Add field to template array
    if ($form_state['values']['field_type'] == 'table field') {
      $field = array(
        'type' => 'table field',
        'title' => $form_state['values']['field_title'],
        'field' => $form_state['values']['chado_field'],
        'required' => $form_state['values']['required'],
        //'allowed values' => empty by default,
        'spreadsheet column' => $form_state['values']['column_number'],
        'exposed' => $form_state['values']['column_exposed'],
        'exposed_description' => $form_state['values']['column_exposed_desc'],
      );
    }
    elseif ($form_state['values']['field_type'] == 'constant') {
      $field = array(
        'type' => 'constant',
        'title' => $form_state['values']['field_title'],
        'field' => $form_state['values']['chado_field'],
        'required' => $form_state['values']['required'],
        //'allowed values' => empty by default,
        'constant value' => $form_state['values']['constant_value'],
        'exposed' => $form_state['values']['constant_exposed'],
        'exposed_validate' => $form_state['values']['constant_validate'],
      );
    }
    elseif ($form_state['values']['field_type'] == 'foreign key') {
      $field = array(
        'type' => 'foreign key',
        'title' => $form_state['values']['field_title'],
        'field' => $form_state['values']['chado_field'],
        'show_all_records' => $form_state['values']['show_all_records'],
        'foreign key' => $form_state['values']['foreign_record'],
        'foreign field' => $form_state['values']['foreign_field'],
        'required' => $form_state['values']['required'],
      );
    }

    // Deal with any additional options
    if (isset($form_state['storage']['regex'])) {
      $field['regex'] = $form_state['storage']['regex'];
    }

    // Save Template
    if ($form_state['storage']['mode'] == 'create') {
      $template[$priority]['fields'][] = $field;
    }
    else {
      $template[$priority]['fields'][$form_state['storage']['field_index']] = $field;
    }
    $form_state['storage']['template']->template_array = serialize($template);
    $form_state['storage']['template']->changed = time();
    $success = drupal_write_record('tripal_bulk_loader_template', $form_state['storage']['template'], array('template_id'));

    if ($success) {
      drupal_set_message(t('Successfully Added Field to Template'));
      drupal_set_message(t('Template Saved.'));

      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$form_state['storage']['template_id'].'/edit';
    }
    else {
      drupal_set_message(t('Unable to Save Template!'), 'error');
      tripal_report_error('T_bulk_loader',
        TRIPAL_ERROR,
        'Unable to save bulk loader template: %template',
        array('%template' => print_r($form_state['storage']['template'], TRUE))
      );
    }
  }
  elseif ($op ==  'Cancel') {
      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$form_state['storage']['template_id'].'/edit';
  }
  elseif ($op == 'Add Transformation') {

    // Add transformation rule to original field
    $form_state['storage']['regex']['pattern'][] = '/' . $form_state['values']['pattern'] . '/';
    $form_state['storage']['regex']['replace'][] = $form_state['values']['replace'];
    $form_state['rebuild'] = TRUE;
    drupal_set_message(t('Successfully Added Transformation Rule'));

  }
  elseif ($op == 'Save Transformation Rule Order') {

    // Generate new regex array
    $new_regex = array();
    $old_regex = $form_state['storage']['regex'];
    foreach ($form_state['values']['regex-data'] as $key => $element) {
      $new_regex['pattern'][ $element['new_index'] ] = $old_regex['pattern'][ $element['old_index'] ];
      $new_regex['replace'][ $element['new_index'] ] = $old_regex['replace'][ $element['old_index'] ];
    }

    // sort new regex arrays
    ksort($new_regex['pattern']);
    ksort($new_regex['replace']);

    $form_state['storage']['regex'] = $new_regex;
  }
  elseif ($op == 'Delete Transformation') {

    // Unset regex rule
    $index = $form_state['clicked_button']['#name'];
    unset($form_state['storage']['regex']['pattern'][$index]);
    unset($form_state['storage']['regex']['replace'][$index]);

  }
  elseif ($op == 'Test Transformation Rules') {

    $patterns = $form_state['storage']['regex']['pattern'];
    $replaces = $form_state['storage']['regex']['replace'];
    $test_string = $form_state['values']['test_string'];
    $form_state['storage']['test_regex_result'] = preg_replace($patterns, $replaces, $test_string);
    $form_state['storage']['test_regex_test'] = $test_string;
  }

}

/**
 * Delete Field Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_field_form($form, $form_state) {
  $form['#cache'] = TRUE; // Make sure the form is cached.

  // get args from path
  $template_id = (isset($form_state['build_info']['args'][0])) ? $form_state['build_info']['args'][0] : FALSE;
  $form_state['storage']['template_id'] = $template_id;
  $record_id = (isset($form_state['build_info']['args'][1])) ? $form_state['build_info']['args'][1] : FALSE;
  $form_state['storage']['record_id'] = $record_id;
  $field_id = (isset($form_state['build_info']['args'][2])) ? $form_state['build_info']['args'][2] : FALSE;
  $form_state['storage']['field_id'] = $field_id;

  // if there is no template supplied don't return rest of form
  if (!$template_id) {
    return $form;
  }

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  $breadcrumb[] = l('Templates', 'admin/tripal/loaders/bulk/templates');
  $breadcrumb[] = l('Edit Template', 'admin/tripal/loaders/bulk/template/' . $template_id . '/edit');
  drupal_set_breadcrumb($breadcrumb);

  $_GET['record_name'] = (isset($_GET['record_name'])) ?  $_GET['record_name'] : '';
  $_GET['field_name'] = (isset($_GET['field_name'])) ?  $_GET['field_name'] : '';
  $_GET['chado_table'] = (isset($_GET['chado_table'])) ?  $_GET['chado_table'] : '';
  $_GET['chado_field'] = (isset($_GET['chado_field'])) ?  $_GET['chado_field'] : '';
  $_GET['data_column'] = (isset($_GET['data_column'])) ?  $_GET['data_column'] : '';
  $_GET['constant_value'] = (isset($_GET['constant_value'])) ?  $_GET['constant_value'] : '';
  $_GET['foreign_record'] = (isset($_GET['foreign_record'])) ?  $_GET['foreign_record'] : '';

  $description = '';
  $description .= '<table>';
  $description .= '<tr><th>Record Name</th><th>Field Name</th><th>Chado Table</th><th>Chado Field</th><th>Data Column</th><th>Constant Value</th><th>Foreign Key</th></tr>';
  $description .= '<tr><td>' . $_GET['record_name']
                . '</td><td>' . $_GET['field_name']
                . '</td><td>' . $_GET['chado_table']
                . '</td><td>' . $_GET['chado_field']
                . '</td><td>' . $_GET['data_column']
                . '</td><td>' . $_GET['constant_value']
                . '</td><td>' . $_GET['foreign_record']
                . '</td></tr>';
  $description .= '</table>';

  $description .= '<p><strong>Are you sure you want to delete this field?</strong></p>';
  $yes = 'Delete Field';
  $no = 'Cancel';

  $form['#attributes']['class'][] = 'confirmation';
  $form['description'] = array('#markup' => $description);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $yes ? $yes : t('Confirm'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => $no ? $no : t('Cancel'),
    '#href' => 'admin/tripal/loaders/bulk/template/'.$template_id.'/edit',
  );
  // By default, render the form using theme_confirm_form().
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'confirm_form';
  }

  return $form;
}

/**
 * Delete Field Form
 *
 * This form is meant to be called from a bulk loader form
 *
 * @param $form_state
 *   Contains the values and storage for the form
 * @return
 *   A form array to be rendered by drupal_get_form
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_template_field_form_submit($form, &$form_state) {
  $form_state['rebuild']  = FALSE;
  $form_state['redirect'] = 'admin/tripal/loaders/bulk/template/'.$form_state['storage']['template_id'].'/edit';

  $sql = "SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template";
  $result = db_query($sql, array(':template' => $form_state['storage']['template_id']))->fetchObject();
  $form_state['storage']['template'] = unserialize($result->template_array);

  $new_template = tripal_bulk_loader_delete_field(
    $form_state['storage']['record_id'],
    $form_state['storage']['field_id'],
    $form_state['storage']['template']
  );
  if (!empty($new_template)) {
    $form_state['storage']['template'] = $new_template;
  }
  drupal_set_message(t('Deleted Field from Template.'));

  $record = array(
    'template_id' => $form_state['storage']['template_id'],
    'template_array' => serialize($form_state['storage']['template']),
    'changed' => time()
  );
  drupal_write_record('tripal_bulk_loader_template', $record, array('template_id'));
  drupal_set_message(t('Template Saved.'));
}

/**
 * @section
 * AHAH Callbacks
 */

/**
 * AHAH Function: Replace $form['fields'] in tripal_bulk_loader_add_template_field_form
 *
 * @return
 *  JSON Data printed to the screen
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_template_fields_ahah($form, $form_state) {
  return $form['fields'];
}

