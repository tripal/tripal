<?php

/**
 * @file
 * API to manage the chado _relationship table for various Tripal Node Types
 *
 * How To Use:
 * @code

  function chado_example_form($form, $form_state) {

    // Default values for form elements can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing node
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks when the ajax call originates from non-submit fields other
    //    than button
    // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
    //    form elements (e.g. buttons) and the form is being rebuilt but has not yet
    //    been validated
    //
    // The reference elements added by this function do use AJAX calls from buttons,
    // therefore, it is important to check for form values in the $form_state['values']
    // for case #2 above, and in the $form_state['input'] for case #3.
    // See the chado analysis node form for an example.


    // Next, add in all the form array definition particular to your node type

    // To add in the relationship form elements, you first need to prepare the arguments
    // for the function call.

    $details = array(
      'relationship_table' => 'example_relationship',    // the name of the table linking additional dbxrefs to this node
      'base_table' => 'example',                         // the name of the chado table this node links to
      'base_foreign_key' => 'example_id',                // key to link to the chado content created by this node
      'base_key_value' => $example_id,                   // the value of the above key
      'fieldset_title' => 'Relationships',               // the non-translated title for this fieldset
      'additional_instructions' => ''                    // a non-stranslated string providing additional instructions
    );

    // Finally, and add the additional form elements to the form
    chado_node_relationships_form($form, $form_state, $details);

    return $form;
  }

  function chado_example_insert($node) {

    // if there is an example_id in the $node object then this must be a sync so
    // we can skip adding the chado_example as it is already there, although
    // we do need to proceed with the rest of the insert
    if (!property_exists($node, 'example_id')) {

      // Add record to chado example table

      // Add to any other tables needed

      // Add all relationships
      // Existing _relationship links with the current example as either the subject_id
      // or object_id will be cleared and then re-added
      chado_node_relationships_form_update_relationships(
        $node,
        'example_relationship',
        $node->example_id
      );
    }

    // Add record to chado_example linking example_id to new node

  }

  function chado_example_update($node) {


      // Update record in chado example table

      // Update any other tables needed

      // Update all additional database references
      // Existing _relationship links with the current example as either the subject_id
      // or object_id will be cleared and then re-added
      chado_node_relationships_form_update_relationships(
        $node,
        'example_relationship',
        $node->example_id
      );

    // Don't need to update chado_example linking table since niether example_id or nid can be changed in update

  }

 * @endcode
 *
 * @ingroup tripal_chado_node_api
 */

/**
 * Provides a form for adding to BASE_relationship and relationship tables
 *
 * @param $form
 *   The Drupal form array into which the relationship elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $details
 *   An array defining details needed by this form. Required Keys are:
 *     - relationship_table: the name of the relationship table (ie: feature_relationship)
 *     - base_table: the name of the base table (ie: feature)
 *     - base_foreign_key: the name of the foreign key linking this table to the non-relationship table (ie: feature_id)
 *     - base_key_value: the value of the base_foreign_key for the current form (ie: 999 if the feature_id=999)
 *     - nodetype: the non-translated singular title of this node type
 *   One of the following:
 *     - cv_id: the id of the ontology to supply terms for the type dropdown
 *     - cv_name: the name of the ontology to supply terms for the type dropdown
 *   Optional keys include:
 *     - fieldset_title: the non-translated title for this fieldset
 *     - additional_instructions: a non-translated string providing additional instructions
 *     - nodetype_plural: the non-translated plural title of this node type
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form(&$form, &$form_state, $details) {

  $form_state['rebuild'] = TRUE;

  // Set Defaults for optional fields
  $details['fieldset_title'] = (isset($details['fieldset_title'])) ? $details['fieldset_title'] : 'Relationships';
  $details['additional_instructions'] = (isset($details['additional_instructions'])) ? $details['additional_instructions'] : '';
  $details['nodetype_plural']  = (isset($details['nodetype_plural'])) ? $details['nodetype_plural'] : $details['nodetype'] . 's';

  // Get relationship type options
  if (isset($details['cv_id'])) {
    $query = "SELECT cvterm_id, name FROM {cvterm} cvterm WHERE cv_id = :cv_id";
    $result = chado_query($query, array(':cv_id' => $details['cv_id']));
  } elseif (isset($details['cv_name'])) {
    $query = "SELECT cvterm_id, name FROM {cvterm} cvterm WHERE cv_id IN (SELECT cv_id FROM cv WHERE name = :cv_name)";
    $result = chado_query($query, array(':cv_name' => $details['cv_name']));
  }
  $type_options = array(0 => 'Select a Type');
  foreach ($result as $cvterm) {
    $type_options[ $cvterm->cvterm_id ] = $cvterm->name;
  }

  $form['relationships'] = array(
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => t('You may add relationships between this %nodetype and other
      %nodetype_plural by entering the details below.  You may add
      as many relationships as desired by clicking the add button on the right.  To
      remove a relationship, click the remove button. ' . $details['additional_instructions'],
      array('%nodetype' => $details['nodetype'], '%nodetype_plural' => $details['nodetype_plural'])),
    '#prefix' => "<div id='relationships-fieldset'>",
    '#suffix' => '</div>'
  );

  // this form element is a tree, so that we don't puke all of the values into then node variable
  // it is set as a tree, and keeps them in the $form_state['values']['relationship_table'] heading.
  $form['relationships']['relationship_table'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#prefix' => '<div id="tripal-generic-edit-relationships-table">',
    '#suffix' => '</div>',
    '#theme' => 'chado_node_relationships_form_table'
  );

  // Add defaults into form_state to be used elsewhere
  $form['relationships']['relationship_table']['details'] = array(
    '#type' => 'hidden',
    '#value' => serialize($details)
  );

  // Add relationships already attached to the node
  //---------------------------------------------
  /* Relationships can come to us in two ways:
   *
   * 1) In the form state in the $form_state['chado_relationships']. Data is in this field
   *    when an AJAX call updates the form state or a validation error.
   *
   * 2) Directly from the database if the record already has _relationships associated.  This
   *    data is only used the first time the form is loaded. On AJAX calls or validation
   *    errors the fields on the form are populated from the $form_state['chado_relationships']
   *    entry.
   */
  if (isset($form_state['chado_relationships'])) {
    $existing_rels = $form_state['chado_relationships'];
  }
  else {
    $existing_rels = chado_query(
      "SELECT rel.*, base1.uniquename as object_name, base2.uniquename as subject_name, cvterm.name as type_name
        FROM {".$details['relationship_table']."} rel
        LEFT JOIN {".$details['base_table']."} base1 ON base1.".$details['base_foreign_key']." = rel.object_id
        LEFT JOIN {".$details['base_table']."} base2 ON base2.".$details['base_foreign_key']." = rel.subject_id
        LEFT JOIN {cvterm} cvterm ON cvterm.cvterm_id = rel.type_id
        WHERE rel.object_id = :base_key_value OR rel.subject_id = :base_key_value",
        array(':base_key_value' => $details['base_key_value'])
    );
  }

  /* The format of the $existing_rels' array is either:
   *
   * From the chado_relationships array:
   * $form_state['chado_relationships'] = array(
   *   '[type_id]-[rank]' => array(
   *     'object_id' => [the _relationship.object_id value],
   *     'object_name' => [the base_table.uniquename value linked on base_foreign_key=object_id],
   *     'subject_id' => [the _relationship.subject_id value],
   *     'subject_name' => [the base_table.uniquename value linked on base_foreign_key=subject_id],
   *     'type_id' => [the _relationship.type_id value],
   *     'type_name' => [the cvterm.name value linked on type_id],
   *     'rank' => [the _relationship.rank value],
   *   ),
   * );
   *
   * OR
   * Populated from the database:
   * $existing_rels = array(
   *   0 => array(
   *     'relationship_id' => [the _relationship.relationship_id value],
   *     'object_id' => [the _relationship.object_id value],
   *     'object_name' => [the base_table.uniquename value linked on base_foreign_key=object_id],
   *     'subject_id' => [the _relationship.subject_id value],
   *     'subject_name' => [the base_table.uniquename value linked on base_foreign_key=subject_id],
   *     'type_id' => [the _relationship.type_id value],
   *     'type_name' => [the cvterm.name value linked on type_id],
   *     'rank' => [the _relationship.rank value],
   *   ),
   * );
   *
   * NOTE: The main difference is the key
   *
   * Loop on the array elements of the $existing_rels array and add
   * an element to the form for each one.
   */
  foreach ($existing_rels as $relationship) {

    $form['relationships']['relationship_table'][$relationship->type_id]['#type'] = 'markup';
    $form['relationships']['relationship_table'][$relationship->type_id]['#type'] = '';

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['#type'] = 'markup';
    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['#value'] = '';

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['object_id'] = array(
      '#type' => 'hidden',
      '#value' => $relationship->object_id
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['subject_id'] = array(
      '#type' => 'hidden',
      '#value' => $relationship->subject_id
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['type_id'] = array(
      '#type' => 'hidden',
      '#value' => $relationship->type_id
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['object_name'] = array(
      '#type' => 'markup',
      '#markup' => $relationship->object_name
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['type_name'] = array(
      '#type' => 'markup',
      '#markup' => $relationship->type_name
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['subject_name'] = array(
      '#type' => 'markup',
      '#markup' => $relationship->subject_name
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['rank'] = array(
      '#type' => 'markup',
      '#markup' => $relationship->rank
    );

    $form['relationships']['relationship_table'][$relationship->type_id][$relationship->rank]['rel_action'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#name' => "rel_remove-".$relationship->type_id.'-'.$relationship->rank,
      '#ajax' => array(
        'callback' => 'chado_node_relationships_form_ajax_update',
        'wrapper' => 'tripal-generic-edit-relationships-table',
        'effect'   => 'fade',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default node form submit.  In the validate function we validate only the
      // relationship fields and in the submit we remove the indicated relationship
      // from the chado_relationships array. In order to keep validate errors
      // from the node form validate and Drupal required errors for non-relationship fields
      // preventing the user from removing relationships we set the #limit_validation_errors below
      '#validate' => array('chado_node_relationships_form_remove_button_validate'),
      '#submit' => array('chado_node_relationships_form_remove_button_submit'),
      // Limit the validation of the form upon clicking this button to the relationship_table tree
      // No other fields will be validated (ie: no fields from the main form or any other api
      // added form).
      '#limit_validation_errors' => array(
        array('relationship_table')  // Validate all fields within $form_state['values']['relationship_table']
      )
    );
  }

  $form['relationships']['relationship_table']['new']['object_name'] = array(
    '#type' => 'textfield',
  );

  $form['relationships']['relationship_table']['new']['object_is_current'] = array(
    '#type' => 'checkbox',
    '#title' => t('Current '.$details['nodetype']),
  );

  $form['relationships']['relationship_table']['new']['type_name'] = array(
    '#type' => 'select',
    '#options' => $type_options,
  );

  $form['relationships']['relationship_table']['new']['subject_name'] = array(
    '#type' => 'textfield',
  );

  $form['relationships']['relationship_table']['new']['subject_is_current'] = array(
    '#type' => 'checkbox',
    '#title' => t('Current '.$details['nodetype']),
  );

  $form['relationships']['relationship_table']['new']['rank'] = array(
    '#type' => 'markup',
    '#markup' => ''
  );

  $form['relationships']['relationship_table']['new']['rel_action'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => 'rel_add',
    '#ajax' => array(
      'callback' => 'chado_node_relationships_form_ajax_update',
      'wrapper' => 'tripal-generic-edit-relationships-table',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default node form submit.  In the validate function we validate only the
    // relationship fields and in the submit we add them to the chado_relationships
    // array. In order to keep validate errors from the node form validate and Drupal
    // required errors for non-relationship fields preventing the user from adding relationships we
    // set the #limit_validation_errors below
    '#validate' => array('chado_node_relationships_form_add_button_validate'),
    '#submit' => array('chado_node_relationships_form_add_button_submit'),
    // Limit the validation of the form upon clicking this button to the relationship_table tree
    // No other fields will be validated (ie: no fields from the main form or any other api
    // added form).
    '#limit_validation_errors' => array(
      array('relationship_table')  // Validate all fields within $form_state['values']['relationship_table']
    )
  );

}

/**
 * Validate the user input for creating a new relationship
 * Called by the add button in chado_node_relationships_form
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_add_button_validate($form, &$form_state) {

  $details = unserialize($form_state['values']['relationship_table']['details']);

  // At least one of the participants must be the current node
  if (!($form_state['values']['relationship_table']['new']['subject_is_current'] OR $form_state['values']['relationship_table']['new']['object_is_current'])) {
    // If the checkbox isn't set then check to see if either has the same uniquename as the node
    if ($form_state['values']['relationship_table']['new']['subject_name'] == $form_state['values']['uniquename']) {
      $form_state['values']['relationship_table']['new']['subject_is_current'] = 1;
      form_set_error('subject_is_current', 'To set the current '.$details['nodetype'].', select the
        checkbox. You entered the unique name of the current '.$details['nodetype'].' as the subject,
        is this what you meant to do?');
    }
    elseif ($form_state['values']['relationship_table']['new']['subject_name'] == $form_state['values']['uniquename']) {
      $form_state['values']['relationship_table']['new']['object_is_current'] = 1;
      form_set_error('subject_is_current', 'To set the current '.$details['nodetype'].', select the
        checkbox. You entered the unique name of the current '.$details['nodetype'].' as the subject,
        is this what you meant to do?');
    }
    else {
      form_set_error('object_is_current', 'At least one member of the relationship must be
        the current '.$details['nodetype'].'. This is specified by checking the "Current '.$details['nodetype'].'"
        checkbox for either the subject or object.');
    }
  }

  // The non-current uniquename must be exist in the base table (subject)
  if (!($form_state['values']['relationship_table']['new']['subject_is_current'])) {
    $result = tripal_core_chado_select(
      $details['base_table'],
      array($details['base_foreign_key']),
      array('uniquename' => $form_state['values']['relationship_table']['new']['subject_name'])
    );
    if (!isset($result[0])) {
      form_set_error('subject_name', 'The subject must be the unique name of an
        existing '.$details['nodetype'].' unless the "Current '.$details['nodetype'].'" checkbox is selected');
    }
    else {
      $form_state['values']['relationship_table']['new']['subject_id'] = $result[0]->{$details['base_foreign_key']};
    }
  }

  // The non-current uniquename must exist in the base table (object)
  if (!($form_state['values']['relationship_table']['new']['object_is_current'])) {
    $result = tripal_core_chado_select(
      $details['base_table'],
      array($details['base_foreign_key']),
      array('uniquename' => $form_state['values']['relationship_table']['new']['object_name'])
    );
    if (!isset($result[0])) {
      form_set_error('object_name', 'The object must be the unique name of an
        existing '.$details['nodetype'].' unless the "Current '.$details['nodetype'].'" checkbox is selected');
    }
    else {
      $form_state['values']['relationship_table']['new']['object_id'] = $result[0]->{$details['base_foreign_key']};
    }
  }

  // The type must be a valid cvterm
  if ($form_state['values']['relationship_table']['new']['type_name']) {
    $form_state['values']['relationship_table']['new']['type_id'] = $form_state['values']['relationship_table']['new']['type_name'];
    $result = tripal_core_chado_select(
      'cvterm',
      array('name'),
      array('cvterm_id' => $form_state['values']['relationship_table']['new']['type_id'])
    );
    if (!isset($result[0])) {
      form_set_error('type_id', 'The select type is not a valid controlled vocabulary term.');
    }
    else {
      $form_state['values']['relationship_table']['new']['type_name'] = $result[0]->name;
    }
  }
  else {
    form_set_error('type_id', 'Please select a type of relationship');
  }
}

/**
 * Called by the add button in chado_node_relationships_form
 *
 * Create an array of additional relationships in the form state. This array will then be
 * used to rebuild the form in subsequent builds
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_add_button_submit(&$form, &$form_state) {

  $details = unserialize($form_state['values']['relationship_table']['details']);

  // if the chado_relationships array is not set then this is the first time modifying the
  // relationship table. this means we need to include all the relationships from the db
  if (!isset($form_state['chado_relationships'])) {
    chado_node_relationships_form_create_relationship_formstate_array($form, $form_state);
  }

  $name = (isset($form_state['node']->{$details['base_table']}->uniquename)) ? $form_state['node']->{$details['base_table']}->uniquename : 'CURRENT';

  // get details for the new relationship
  if ($form_state['values']['relationship_table']['new']['subject_is_current']) {

    $relationship = array(
      'type_id' => $form_state['values']['relationship_table']['new']['type_id'],
      'type_name' => $form_state['values']['relationship_table']['new']['type_name'],
      'object_id' => $form_state['values']['relationship_table']['new']['object_id'],
      'object_name' => $form_state['values']['relationship_table']['new']['object_name'],
      'subject_id' => $form_state['node']->{$details['base_table']}->{$details['base_foreign_key']},
      'subject_name' => $name,
      'rank' => '0',
    );
  }
  else {
    $relationship = array(
      'type_id' => $form_state['values']['relationship_table']['new']['type_id'],
      'type_name' => $form_state['values']['relationship_table']['new']['type_name'],
      'object_id' => $form_state['node']->{$details['base_table']}->{$details['base_foreign_key']},
      'object_name' => $name,
      'subject_id' => $form_state['values']['relationship_table']['new']['subject_id'],
      'subject_name' => $form_state['values']['relationship_table']['new']['subject_name'],
      'rank' => '0',
    );
  }

  // get max rank
  /**
  if (isset($form_state['node']->{$details['base_table']}->{$details['base_foreign_key']})) {
    $rank = tripal_core_get_max_chado_rank(
      $details['relationship_table'],
      array(
        'subject_id' => $relationship['subject_id'],
        'type_id' => $relationship['type_id'],
        'object_id' => $relationship['object_id'],
      )
    );
    $relationship['rank'] = strval($rank + 1);
  }
  */

  $key = $relationship['type_id'] . '-' . $relationship['rank'];
  $form_state['chado_relationships'][$key] = (object) $relationship;

  $form_state['rebuild'] = TRUE;
}

/**
 * There is no user input for the remove buttons so there is no need to validate
 * However, both a submit & validate need to be specified so this is just a placeholder
 *
 * Called by the many remove buttons in chado_node_relationships_form
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_remove_button_validate($form, $form_state) {
  // No Validation needed for remove
}

/**
 * Remove the correct relationship from the form
 * Called by the many remove buttons in chado_node_relationships_form
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_remove_button_submit(&$form, &$form_state) {

  // if the chado_relationships array is not set then this is the first time modifying the
  // relationship table. this means we need to include all the relationships from the db
  if (!isset($form_state['chado_relationships'])) {
    chado_node_relationships_form_create_relationship_formstate_array($form, $form_state);
  }

  // remove the specified relationship from the form relationship table
  if(preg_match('/rel_remove-([^-]+-[^-]+)/',$form_state['triggering_element']['#name'],$match)) {
    $key = $match[1];
    if (array_key_exists($key, $form_state['chado_relationships'])) {
      unset($form_state['chado_relationships'][$key]);
    }
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax function which returns the section of the form to be re-rendered
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_ajax_update($form, $form_state) {
  return $form['relationships']['relationship_table'];
}

/**
 * Creates an array in form_state containing the existing relationships. This array is
 * then modified by the add/remove buttons and used as a source for rebuilding the form.
 *
 * $form_state['chado_relationships'] = array(
 *   '[type_id]-[rank]' => array(
 *     'object_id' => [the _relationship.object_id value],
 *     'object_name' => [the base_table.uniquename value linked on base_foreign_key=object_id],
 *     'subject_id' => [the _relationship.subject_id value],
 *     'subject_name' => [the base_table.uniquename value linked on base_foreign_key=subject_id],
 *     'type_id' => [the _relationship.type_id value],
 *     'type_name' => [the cvterm.name value linked on type_id],
 *     'rank' => [the _relationship.rank value],
 *   ),
 * );
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_create_relationship_formstate_array($form, &$form_state) {

  $form_state['chado_relationships'] = array();

  foreach (element_children($form['relationships']['relationship_table']) as $type_id) {
    if ($type_id != 'new') {
      foreach (element_children($form['relationships']['relationship_table'][$type_id]) as $rank) {
          $element = $form['relationships']['relationship_table'][$type_id][$rank];
          $rel = array(
            'type_id' => $element['type_id']['#value'],
            'object_id' => $element['object_id']['#value'],
            'subject_id' => $element['subject_id']['#value'],
            'type_name' => $element['type_name']['#markup'],
            'object_name' => $element['object_name']['#markup'],
            'subject_name' => $element['subject_name']['#markup'],
            'rank' => $element['rank']['#markup']
          );
          $key = $rel['type_id'] . '-' . $rel['rank'];
          $form_state['chado_relationships'][$key] = (object) $rel;
      }
    }
  }
}

/**
 * Function to theme the add/remove relationships form into a table
 *
 * @ingroup tripal_chado_node_api
 */
function theme_chado_node_relationships_form_table($variables) {
  $element = $variables['element'];

  $header = array(
    'object_name' => t('Object Unique Name'),
    'type_name' => t('Type'),
    'subject_name' => t('Subject Unique Name'),
    'rank' => t('Rank'),
    'rel_action' => t('Action')
  );

  $rows = array();
  foreach (element_children($element) as $type_id) {
    if ($type_id == 'new') {
      $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          if ($fieldname == 'subject_name') {
            $row['data'][] = drupal_render($element[$type_id][$fieldname]) . drupal_render($element[$type_id]['subject_is_current']);
          }
          elseif ($fieldname == 'object_name') {
            $row['data'][] = drupal_render($element[$type_id][$fieldname]) . drupal_render($element[$type_id]['object_is_current']);
          }
          else {
            $row['data'][] = drupal_render($element[$type_id][$fieldname]);
          }
        }
        $rows[] = $row;
    }
    else {
      foreach (element_children($element[$type_id]) as $rank) {
        $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$type_id][$rank][$fieldname]);
        }
        $rows[] = $row;
      }
    }
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
}

/**
 * This function is used in a hook_insert, hook_update for a node form
 * when the relationships form has been added to the form.  It retrieves all of the relationships
 * and returns them in an array of the format:
 *
 *   $relationships[<type_id>][<rank>] = array(
 *         'subject_id' => <subject_id>,
 *         'object_id'  => <object_id>,
 *   );
 *
 * This array can then be used for inserting or updating relationships manually
 *
 * @param $node
 *
 * @return
 *   A relationship array
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_retreive($node) {
  $rels = array();

  if (isset($node->relationship_table)) {
    foreach ($node->relationship_table as $type_id => $elements) {
      if ($type_id != 'new' AND $type_id != 'details') {
        foreach ($elements as $rank => $relationships) {
          $rels[$type_id][$rank]['subject_id'] = $relationships['subject_id'];
          $rels[$type_id][$rank]['object_id'] = $relationships['object_id'];
        }
      }
    }
  }

  return $rels;
}

/**
 * This function is used in hook_insert or hook_update and handles inserting of
 * relationships between the current nodetype and other memebers of the same nodetype
 *
 * @param $node
 *    The node passed into hook_insert & hook_update
 * @param $relationship_table
 *    The name of the _relationship linking table (ie: feature_relationship)
 * @param $current_id
 *    The value of the foreign key (ie: 445, if there exists a feature where feature_id=445)
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_relationships_form_update_relationships($node, $relationship_table, $current_id) {

  if (isset($node->relationship_table) AND ($current_id > 0)) {
    // First remove existing relationships links
    tripal_core_chado_delete($relationship_table, array('subject_id' => $current_id));
    tripal_core_chado_delete($relationship_table, array('object_id' => $current_id));

    // Add back in relationships as needed
    $relationships = chado_node_relationships_form_retreive($node);
    foreach ($relationships as $type_id => $ranks) {
      foreach ($ranks as $rank => $element) {

        $values = array(
          'subject_id' => $element['subject_id'],
          'type_id' => $type_id,
          'object_id' => $element['object_id'],
          'rank' => $rank
        );

        // Set the current id if not already
        // this is usually only necessary in an insert
        if (empty($values['subject_id'])) {
          $values['subject_id'] = $current_id;
        }
        if (empty($values['object_id'])) {
          $values['object_id'] = $current_id;
        }

        // Ensure that the rank is Set & Current
        $rank_select = tripal_core_get_max_chado_rank(
          $relationship_table,
          array(
            'subject_id' => $values['subject_id'],
            'type_id' => $values['type_id'],
            'object_id' => $values['object_id'],
          )
        );
        $values['rank'] = $rank_select + 1;

        // add relationship
        $success_link = tripal_core_chado_insert(
          $relationship_table,
          $values
        );

      }
    }
  }
}