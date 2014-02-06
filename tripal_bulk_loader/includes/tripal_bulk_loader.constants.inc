<?php
/**
 * @file
 * Manages the constants form added to the tripal bulk loader node form
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Inserts/Updates a tripal bulk loading job constant
 *
 * @param $nid
 *   The node ID of the the tripal bulk loading job the constant is associated with
 * @param $table
 *   The chado table the constant is associated with
 * @param $field
 *   The chado field the constant is associated with
 * @param $record_id
 *   The index in the template array for this record
 * @param $field_id
 *   The index in the template array for this field
 *
 * NOTE: $template_array[$record_id]['table'] = $table and $template_array[$record_id]['fields'][$field_id]['field'] = $field
 *   both are included as a means of double-checking the constant still is still in thesame place in the template array.
 *   For example, that the template was not edited and the records moved around after the job was submitted but before it was run.
 *
 * @return
 *   On success it returns the object (with primary key if inserted);
 *   on failure it returns FALSE
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_update_constant($nid, $group_id, $table, $field, $record_id, $field_id, $value) {

  $record = array(
    'nid' => $nid,
    'group_id' => $group_id,
    'chado_table' => $table,
    'chado_field' => $field,
    'record_id' => $record_id,
    'field_id' => $field_id,
    'value' => $value
  );

  // Check to see if already exists
  $exists = db_select('tripal_bulk_loader_constants', 'c')
    ->fields('c',array('constant_id'))
    ->condition('nid',$record['nid'])
    ->condition('record_id',$record['record_id'])
    ->condition('field_id',$record['field_id'])
    ->condition('group_id',$record['group_id'])
    ->execute();
  $exists = $exists->fetchObject();
  if (!isset($exists)) {
    $record['constant_id'] = $exists->constant_id;
    $status = drupal_write_record('tripal_bulk_loader_constants', $record, 'constant_id');
    if ($status) {
      return $record;
    }
    else {
      return FALSE;
    }
  }
  else {

    $status = drupal_write_record('tripal_bulk_loader_constants', $record);
    if ($status) {
      return $record;
    }
    else {
      return FALSE;
    }
  }
}

/**
 * Check if a bulk loading job has exposed constants
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_has_exposed_fields($node) {

  // exposed fields isn't set
  if (!isset($node->exposed_fields)) {
    return FALSE;
  }

  // exposed fields has at least one element
  if (sizeof($node->exposed_fields) == 1) {
    // need to check if single element is an empty array
    $element = reset($node->exposed_fields);
    if ($element) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  elseif (sizeof($node->exposed_fields) > 1) {
    return TRUE;
  }
  else {
    return FALSE;
  }

  return FALSE;
}

///////////////////////////////////////////////////////////
// Set Constants Form (on Bulk Loader Node)
///////////////////////////////////////////////////////////

/**
 * Set constants (exposed fields in template)
 *
 * @param $form_state
 *   The current state of the form
 * @param $node
 *   The node to set constants for
 *
 * @return
 *   A form array to be rendered by drupal_get_form()
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_set_constants_form($form, $form_state, $node) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  if (!tripal_bulk_loader_has_exposed_fields($node)) {
    return $form;
  }

  $form['exposed_array'] = array(
    '#type' => 'hidden',
    '#value' => serialize($node->exposed_fields),
  );

  $form['exposed_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Constant Values'),
    '#collapsible' => TRUE,
    '#collapsed' => ($node->template_id) ? FALSE : TRUE,
    '#prefix' => '<div id="set-constants">',
    '#suffix' => '</div>',
  );

  // Display table of already added constant sets with the ability to re-arrange and delete
  $first_constant = reset($node->constants);
  if (sizeof($node->constants) > 0) {
    $form['exposed_fields']['explanation-1'] = array(
      '#type' => 'item',
      '#markup' => t('You have already added constants to this bulk loading job. Each '
        .'row in the following table represents a set of constants. Each set will be used '
        .'to load your data file with the specified template resulting in the each record '
        .'in the template to be loaded x number of times where there are x sets of '
        .'constants (rows in the following table).')
    );

    $rows = array();
    foreach ($node->constants as $group_id => $set) {
      $row = array();
      $row['group_id'] = $group_id;
      $header['group_id'] = t('Group');
      foreach ($set as $record) {
        foreach ($record as $field) {
          $index = $field['record_id'] . '-' . $field['field_id'];
          $header[$index] = $field['value'];
          $row[$index] = $field['value'];
        }
      }
      $header['operations'] = t('Operations');
      $row['operations'] = filter_xss(l(t('Edit'), 'node/' . $node->nid . '/constants/' . $group_id . '/edit') . '  |  '  .
            l(t('Delete'), 'node/' . $node->nid . '/constants/' . $group_id . '/delete'));

      $rows[] = $row;
    }

    //get the header using the last constant set
    $header = array();
    $header['group_id'] = '';
    $header_row = array();
    $header_row[0]['group_id'] = array('data' => t('Group'),'header'=>TRUE);
    foreach ($set as $record) {
      foreach ($record as $field) {
        $header[$field['record_id']]['data'] = $node->template->template_array[$field['record_id']]['record_id'];
        $header[$field['record_id']]['colspan'] = sizeof($record);

        $index = $field['record_id'] . '-' . $field['field_id'];
        $header_row[0][$index] = array(
          'data' => $node->template->template_array[$field['record_id']]['fields'][$field['field_id']]['title'],
          'header' => TRUE
        );
      }
    }
    $header['operations'] = '';
    $header_row[0]['operations'] = array('data' => t('Operations'),'header'=>TRUE);

    $full_header = array(
      0 => $header,
      1 => $header_row[0]
    );
    $all_rows = array_merge($header_row, $rows);
    $form['exposed_fields']['existing'] = array(
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => $header,
        'rows' => $all_rows
      )),
      // Added to make accessible to subthemeing
      '#rows' => serialize($rows),
      '#header' => serialize($full_header)
    );
  }

  $form['exposed_fields']['new'] = array(
    '#type' => 'fieldset',
    '#title' => t('New set of Constants'),
  );

  $form['exposed_fields']['new']['explanation-2'] = array(
    '#type' => 'item',
    '#markup' => t('The following fields are constants in the selected template that you need to set values for.')
  );

  // Add textifelds for exposed fields of the current template
  $exposed_fields = FALSE;
  $indexes = array();
  if (tripal_bulk_loader_has_exposed_fields($node)) {
    foreach ($node->exposed_fields as $exposed_index) {

      $record_id = $exposed_index['record_id'];
      $field_id = $exposed_index['field_id'];
      $field = $node->template->template_array[$record_id]['fields'][$field_id];

      if ($field['exposed']) {
        $exposed_fields = TRUE;
        $indexes[$record_id][] = $field_id;

        $default_value = '';
        if (isset($node->constants[$record_id])) {
          if (isset($node->constants[$record_id][$field_id])) {
            $default_value = $node->constants[$record_id][$field_id]['value'];
          }
        } elseif (isset($field['constant value'])) {
          $default_value = $field['constant value'];
        } elseif(isset($field['spreadsheet column'])) {
          $default_value = $field['spreadsheet column'];
        }

        switch ($field['type']) {
          case 'table field':
            $form['exposed_fields']['new'][$record_id . '-' . $field_id] = array(
              '#type' => 'textfield',
              '#title' => t('%title', array('%title' => $field['title'])),
              '#description' => t('%exposed_description', array('%exposed_description' => $field['exposed_description'])),
              '#default_value' => $default_value,
            );
          break;
          case 'constant':
            $form['exposed_fields']['new'][$record_id . '-' . $field_id] = array(
              '#type' => 'textfield',
              '#title' => t('%title', array('%title' => $field['title']) ),
              '#description' => t('Enter the case-sensitive value of this constant for your data file'),
              '#default_value' => $default_value,
            );
          break;
        }

        $form['exposed_fields']['new'][$record_id . '-' . $field_id . '-table'] = array(
          '#type' => 'hidden',
          '#value' => $node->template->template_array[$record_id]['table'],
        );
        $form['exposed_fields']['new'][$record_id . '-' . $field_id . '-field'] = array(
          '#type' => 'hidden',
          '#value' => $field['field'],
        );
        $form['exposed_fields']['new'][$record_id . '-' . $field_id . '-type'] = array(
          '#type' => 'hidden',
          '#value' => $field['type'],
        );

      }
    }
  }
  $form['template'] = array(
    '#type' => 'hidden',
    '#value' => serialize($node->template->template_array)
  );

  $form['exposed_fields']['new']['indexes'] = array(
    '#type' => 'hidden',
    '#value' => serialize($indexes),
  );

  if (!$exposed_fields) {
    $form['exposed_fields']['new']['explanation'] = array(
      '#type' => 'item',
      '#value' => t('There are no exposed fields for this template.')
    );
  }

  $form['exposed_fields']['new']['submit-2'] = array(
    '#type' => 'submit',
    '#name' => 'add_constant',
    '#value' => t('Add Constant Set')
  );

  return $form;
}

/**
 * Validate that the values entered exist in the database
 * if indicated in hte template array
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_set_constants_form_validate($form, $form_state) {

  $template = unserialize($form_state['values']['template']);
  $indexes = unserialize($form_state['values']['indexes']);

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  if (strcmp('Add Constant Set', $op) == 0) {
      foreach ($indexes as $record_id => $array) {
        foreach ($array as $field_id) {
          if (isset($template[$record_id]['fields'][$field_id]['exposed_validate'])) {
            if ($template[$record_id]['fields'][$field_id]['exposed_validate']) {
              $result = db_fetch_object(chado_query(
                "SELECT 1 as valid FROM {%s} WHERE %s='%s'",
                $template[$record_id]['table'],
                $template[$record_id]['fields'][$field_id]['field'],
                $form_state['values'][$record_id . '-' . $field_id]
              ));

              if (!$result->valid) {
                $msg = 'A ' . $form['exposed_fields']['new'][$record_id . '-' . $field_id]['#title'] . ' of "' . $form['exposed_fields']['new'][$record_id . '-' . $field_id]['#value'] . '" must already exist!';
                form_set_error($record_id . '-' . $field_id, $msg);
              }
              else {
                drupal_set_message(
                  t(
                    'Confirmed a %title of "%value" already exists.',
                    array(
                      '%title' => $form['exposed_fields']['new'][$record_id . '-' . $field_id]['#title'],
                      '%value' => $form['exposed_fields']['new'][$record_id . '-' . $field_id]['#value']
                    )
                  )
                );
              }
            }
          }
        }
      }
  }

}

/**
 * Insert/update the constants associated with this node
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_set_constants_form_submit($form, $form_state) {

  // Insert/Update constants
  $template = unserialize($form_state['values']['template']);
  $indexes = unserialize($form_state['values']['indexes']);

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  if (strcmp('Add Constant Set', $op) == 0) {
      $max_group = db_query("SELECT max(group_id) as value FROM {tripal_bulk_loader_constants} WHERE nid=:nid", array(':nid' => $form_state['values']['nid']))->fetchObject();
      foreach ($indexes as $record_id => $array) {
        foreach ($array as $field_id) {
          tripal_bulk_loader_update_constant(
            $form_state['values']['nid'],
            $max_group->value+1,
            $form_state['values'][$record_id . '-' . $field_id . '-table'],
            $form_state['values'][$record_id . '-' . $field_id . '-field'],
            $record_id,
            $field_id,
            $form_state['values'][$record_id . '-' . $field_id]
          );
        }
      }
  }

}

///////////////////////////////////////////////////////////
// Set Constants Form (on Bulk Loader Node)
///////////////////////////////////////////////////////////

/**
 * Edit a constant set (exposed fields in template)
 *
 * @param $form_state
 *   The current state of the form
 * @param $node
 *   The node to set constants for
 * @param $group_id
 *   The constant set to edit
 *
 * @return
 *   A form array to be rendered by drupal_get_form()
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_edit_constant_set_form($form, $form_state, $node, $group_id) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  $form['group_id'] = array(
    '#type' => 'hidden',
    '#value' => $group_id,
  );


  $form['explanation'] = array(
    '#type' => 'item',
    '#value' => t('The following fields are constants in the selected template that you need to set values for.')
  );

  // Add textifelds for exposed fields of the current template
  $exposed_fields = FALSE;
  $indexes = array();
  if (tripal_bulk_loader_has_exposed_fields($node)) {
    foreach ($node->exposed_fields as $exposed_index) {

      $record_id = $exposed_index['record_id'];
      $record = $node->template->template_array[$record_id];
      $field_id = $exposed_index['field_id'];
      $field = $node->template->template_array[$record_id]['fields'][$field_id];

      if ($field['exposed']) {
        $exposed_fields = TRUE;
        $indexes[$record_id][] = $field_id;

        switch ($field['type']) {
          case 'table field':
            $form[$record_id . '-' . $field_id] = array(
              '#type' => 'textfield',
              '#title' => t('%title', array('%title' => $field['title'])),
              '#description' => t('%exposed_description', array('%exposed_description' => $field['exposed_description'])),
              '#default_value' => (isset($node->constants[$group_id][$record_id][$field_id]['value'])) ? $node->constants[$group_id][$record_id][$field_id]['value'] : $field['constant value'],
            );
          break;
          case 'constant':
            $form[$record_id . '-' . $field_id] = array(
              '#type' => 'textfield',
              '#title' => t('%title', array('%title' => $field['title'])),
              '#description' => t('Enter the case-sensitive value of this constant for your data file'),
              '#default_value' => (isset($node->constants[$group_id][$record_id][$field_id]['value'])) ? $node->constants[$group_id][$record_id][$field_id]['value'] : $field['constant value'],
            );
          break;
        }

        $form[$record_id . '-' . $field_id . '-table'] = array(
          '#type' => 'hidden',
          '#value' => $record['table'],
        );
        $form[$record_id . '-' . $field_id . '-field'] = array(
          '#type' => 'hidden',
          '#value' => $field['field'],
        );
        $form[$record_id . '-' . $field_id . '-type'] = array(
          '#type' => 'hidden',
          '#value' => $field['type'],
        );
      }

    }
  }
  $form['template'] = array(
    '#type' => 'hidden',
    '#value' => serialize($node->template->template_array)
  );

  $form['indexes'] = array(
    '#type' => 'hidden',
    '#value' => serialize($indexes),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => 'Cancel',
    '#href' => 'node/'.$node->nid
  );

  return $form;
}

/**
 * Edit constants in the current constant set
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_edit_constant_set_form_submit($form, &$form_state) {

  // Update constants
  $template = unserialize($form_state['values']['template']);
  $indexes = unserialize($form_state['values']['indexes']);
  $nid = $form_state['values']['nid'];

  $op = $form_state['values'][ $form_state['clicked_button']['#name'] ];
  if (strcmp('Save', $op) == 0) {
    $form_state['redirect'] = 'node/' . $nid;
    $form_state['rebuild'] = FALSE;

    foreach ($indexes as $record_id => $array) {
      foreach ($array as $field_id) {
        tripal_bulk_loader_update_constant(
          $form_state['values']['nid'],
          $form_state['values']['group_id'],
          $form_state['values'][$record_id . '-' . $field_id . '-table'],
          $form_state['values'][$record_id . '-' . $field_id . '-field'],
          $record_id,
          $field_id,
          $form_state['values'][$record_id . '-' . $field_id]
        );
      }
    }
    drupal_set_message(t('The constant set was successfully updated.'));

  }

}

/**
 * Delete a constant set (exposed fields in template)
 *
 * @param $form_state
 *   The current state of the form
 * @param $node
 *   The node to set constants for
 * @param $group_id
 *   The constant set to delete
 *
 * @return
 *   A form array to be rendered by drupal_get_form()
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_constant_set_form($form, $form_state, $node, $group_id) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['group_id'] = array(
    '#type' => 'hidden',
    '#value' => $group_id,
  );

  return confirm_form($form,
      t('Are you sure you want to delete this constant set?'),
      'node/' . $node->nid,
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );

}

/**
 * Delete the current constant set
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_delete_constant_set_form_submit($form, &$form_state) {

  $group_id = $form_state['values']['group_id'];
  $nid = $form_state['values']['nid'];
  if ($nid && $form_state['values']['confirm']) {
    $form_state['redirect'] = 'node/' . $nid;
    $form_state['rebuild'] = FALSE;

    db_delete('tripal_bulk_loader_constants')
      ->condition('nid', $nid)
      ->condition('group_id',$group_id)
      ->execute();
    drupal_set_message(t('Constant set successfully deleted.'));
  }

}
