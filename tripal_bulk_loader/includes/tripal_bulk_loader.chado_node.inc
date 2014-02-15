<?php

/**
 * @file
 * Tripal Bulk Loader Node functionality (jobs).
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Implements hook_node_info().
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_node_info() {
  $nodes = array();
  $nodes['tripal_bulk_loader'] = array(
      'name'        => t('Bulk Loading Job'),
      'base'        => 'tripal_bulk_loader',
      'description' => t('A bulk loader for inserting tab-delimited data into chado database'),
      'has_title'   => TRUE,
      'locked'      => TRUE
  );
  return $nodes;
}

/**
 * Implements hook_form().
 * Used to gather the extra details stored with a Bulk Loading Job Node
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_form($node, $form_state) {
  $form = array();

  if (isset($form_state['values'])) {
    $node = $form_state['values'] + (array)$node;
    $node = (object) $node;
  }

  $results = db_select('tripal_bulk_loader_template', 't')
              ->fields('t', array('template_id','name'))
              ->execute();
  $templates = array();
  foreach ($results as $template) {
    $templates [$template->template_id] = $template->name;
  }

  if (!$templates) {
    $form['label'] = array(
    '#type' => 'item',
      '#description' => t("Loader template needs to be created before any bulk loader can be added. Go to 'Tripal Management > Bulk Loader Template' to create the template."),
      '#weight'        => -10,
    );

    return $form;
  }

  $form['loader'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic Details'),
  );

  $form['loader']['loader_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Loading Job Name'),
    '#weight'        => -10,
    '#required'      => TRUE,
    '#default_value' => (isset($node->loader_name)) ? $node->loader_name : ''
  );

  $form['loader']['template_id'] = array(
    '#type' => 'select',
    '#title' => t('Template'),
    '#description'   => t('Please specify a template for this loader'),
    '#options'       => $templates,
    '#weight'        => -9,
    '#required'      => TRUE,
    '#default_value' => (isset($node->template_id)) ? $node->template_id : current($templates),
  );

  $form['loader']['file']= array(
    '#type'          => 'textfield',
    '#title'         => t('Data File'),
    '#description'   => t('Please specify the data file to be loaded. This must be a tab-delimited text file with UNIX line endings.'),
    '#weight'        => -8,
    '#default_value' => (isset($node->file)) ? $node->file : '',
    '#maxlength'     => 1024,
  );

  $form['loader']['has_header'] = array(
    '#type' => 'radios',
    '#title' => t('File has a Header'),
    '#options' => array( 1 => 'Yes', 2 => 'No'),
    '#weight' => -7,
    '#default_value' => (isset($node->file_has_header)) ? $node->file_has_header : 1,
  );

  $form['loader']['keep_track_inserted'] = array(
    '#type' => 'radios',
    '#title' => t('Keep track of inserted record IDs'),
    '#description' => t('This enables the ability to revert an entire loading job even if '
      .'it completed successfully. Furthermore, it displays the number of records '
      .'successfully inserted into each table.'),
    '#options' => array( 1 => 'Yes', 0 => 'No'),
    '#weight' => -7,
    '#default_value' => (isset($node->keep_track_inserted)) ? $node->keep_track_inserted : variable_get('tripal_bulk_loader_keep_track_inserted', 0),
  );

  return $form;
}


/**
 * Implements hook_load().
 *
 * D7 Changes: now loads all $nodes at once so need to add loops
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_load($nodes) {

  // Loading Job Details
  // Add fields from the tripal_bulk_loader
  $result = db_select('tripal_bulk_loader', 'tbl')
    ->fields('tbl')
    ->condition('nid',array_keys($nodes),'IN')
    ->execute();
  foreach ($result as $record) {
    $nodes[$record->nid]->loader_name = $record->loader_name;
    $nodes[$record->nid]->template_id = $record->template_id;
    $nodes[$record->nid]->file = $record->file;
    $nodes[$record->nid]->job_id = $record->job_id;
    $nodes[$record->nid]->job_status = $record->job_status;
    $nodes[$record->nid]->file_has_header = $record->file_has_header;
    $nodes[$record->nid]->keep_track_inserted = $record->keep_track_inserted;

    $nodes[$record->nid]->exposed_fields = array();
    $nodes[$record->nid]->constants = array();
  }

  // Job Details
  // Add fields from tripal_jobs
  $result = db_query('SELECT tbl.nid, tj.* FROM {tripal_jobs} tj '
    . 'LEFT JOIN {tripal_bulk_loader} tbl ON tbl.job_id=tj.job_id '
    . 'WHERE tbl.nid IN (:nids)',
    array(':nids' => array_keys($nodes))
  );
  foreach ($result as $record) {
    $nodes[$record->nid]->job = $record;
  }

  // Add the Loader Template
  // Add fields from tripal_bulk_loader_template
  $result = db_query('SELECT tbl.nid, tblt.* FROM {tripal_bulk_loader_template} tblt '
    . 'LEFT JOIN {tripal_bulk_loader} tbl ON tbl.template_id=tblt.template_id '
    . 'WHERE tbl.nid IN (:nids)',
    array(':nids' => array_keys($nodes))
  );
  foreach ($result as $dbrecord) {
    $nodes[$dbrecord->nid]->template = $dbrecord;
    $nodes[$dbrecord->nid]->template->template_array = unserialize($dbrecord->template_array);

    // Add exposed field list
    $template = $nodes[$dbrecord->nid]->template->template_array;
    $nodes[$dbrecord->nid]->exposed_fields = array();
    if ($template) {
      foreach ($template as $record_id => $record) {
        foreach ($record['fields'] as $field_id => $field) {
          if (isset($field['exposed'])) {
            if ($field['exposed']) {
              $nodes[$dbrecord->nid]->exposed_fields[] = array(
                'record_id' => $record_id,
                'field_id' => $field_id,
                'title' => $field['title'],
              );
            }
          }
        }
      }
    }
  }

  // Add inserted records
  // Add fields from tripal_bulk_loader_inserted
  $result = db_query('SELECT tbli.* FROM {tripal_bulk_loader_inserted} tbli '
    . 'WHERE tbli.nid IN (:nids)',
    array(':nids' => array_keys($nodes))
  );
  foreach ($result as $record) {
    $record->num_inserted = sizeof(preg_split('/,/', $record->ids_inserted));
    $nodes[$record->nid]->inserted_records->{$record->table_inserted_into} = $record;
  }

  // Add constants
  // Add fields from tripal_bulk_loader_constants
  $result = db_query('SELECT tblc.* FROM {tripal_bulk_loader_constants} tblc '
    . 'WHERE tblc.nid IN (:nids) '
    . 'ORDER BY group_id, record_id, field_id',
    array(':nids' => array_keys($nodes))
  );
  foreach ($result as $record) {
    $nodes[$record->nid]->constants[$record->group_id][$record->record_id][$record->field_id] = array(
      'constant_id' => $record->constant_id,
      'group_id' => $record->group_id,
      'chado_table' => $record->chado_table,
      'chado_field' => $record->chado_field,
      'record_id' => $record->record_id,
      'field_id' => $record->field_id,
      'value' => $record->value
    );
  }

}

/**
 * Implements hook_node_presave().
 * Acts on all node types.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_node_presave($node) {

  // We need to set the title using loader details before the node is saved
  // which has already been done by the time hook_insert is called
  switch ($node->type) {
    case 'tripal_bulk_loader':
      $node->title = 'Bulk Loading Job: ' . $node->loader_name;
      break;
  }
}


/**
 * Implements hook_insert().
 * Insert the data from the node form on Create content
 *
 * D7 Changes: seems to need db_insert; not recommended to change $node
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_insert($node) {

  db_insert('tripal_bulk_loader')->fields(array(
    'nid' => $node->nid,
    'loader_name' => $node->loader_name,
    'template_id' => $node->template_id,
    'file' => $node->file,
    'file_has_header' => $node->has_header,
    'job_status' => 'Initialized',
    'keep_track_inserted' => $node->keep_track_inserted
  ))->execute();
  drupal_set_message(t('After reviewing the details, please Submit this Job (by clicking the "Submit Job" button below). No data will be loaded until the submitted job is reached in the queue.'));

}

/**
 * Implements hook_delete().
 * Deletes the data when the delete button on the node form is clicked
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete($node) {
  $tables = array();
  $tables[] = 'tripal_bulk_loader';
  $tables[] = 'tripal_bulk_loader_constants';
  $tables[] = 'tripal_bulk_loader_inserted';
  foreach($tables as $table) {
    db_delete($table)
      ->condition('nid',$node->nid)
      ->execute();
  }
}

/**
 * Implements hook_update().
 * Updates the data submitted by the node form on edit
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_update($node) {

  // Update tripal_bulk_loader
  db_update('tripal_bulk_loader')->fields(array(
    'nid' => $node->nid,
    'loader_name' => $node->loader_name,
    'template_id' => $node->template_id,
    'file' => $node->file,
    'file_has_header' => $node->has_header,
    'keep_track_inserted' => $node->keep_track_inserted
  ))->condition('nid',$node->nid)->execute();

  // Add a job if the user want to load the data
  /**
  No job checkbox in the form
  global $user;
  if ($node->job) {
    $job_args[0] =$node->loader_name;
    $job_args[1] = $node->template_id;
    $job_args[2] = $node->file;
    if (is_readable($node->file)) {
      $fname = preg_replace("/.*\/(.*)/", "$1", $node->file);
      tripal_add_job("Bulk Load: $fname", 'tripal_bulk_loader', 'tripal_bulk_loader_load_data', $job_args, $user->uid);
    }
    else {
      drupal_set_message(t("Can not open %file. Job not scheduled.", array('%file' => $node->file)));
    }
  }
  */

}

/**
 * Implement hook_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 *  @param $op
 *  The operation to be performed
 *
 *  @param $node
 *  The node on which the operation is to be performed, or, if it does not yet exist, the
 *  type of node to be created
 *
 *  @param $account
 *  A user object representing the user for whom the operation is to be performed
 *
 *  @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access.  The only exception is when the $op == 'create'.  We will always
 *  return TRUE if the permission is set.
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'tripal_bulk_loader') {
    if ($op == 'create') {
      if (!user_access('create tripal_bulk_loader', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit tripal_bulk_loader', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete tripal_bulk_loader', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access tripal_bulk_loader', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}
