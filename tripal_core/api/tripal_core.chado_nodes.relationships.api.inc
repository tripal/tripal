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
    chado_add_node_form_relationships($form, $form_state, $details);

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
      chado_update_node_form_relationships(
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
      chado_update_node_form_relationships(
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
 *     - base_foreign_key: the name of the foreign key in the relationship table field linking this table to the non-relationship table (ie: feature_id)
 *     - base_key_value: the value of the base_foreign_key for the current form (ie: 999 if the feature_id=999)
 *     - nodetype: the non-translated singular title of this node type
 *   One of the following:
 *     - cv_id: the id of the ontology to supply terms for the type dropdown
 *     - cv_name: the name of the ontology to supply terms for the type dropdown
 *   Optional keys include:
 *     - fieldset_title: the non-translated title for this fieldset
 *     - additional_instructions: a non-translated string providing additional instructions
 *     - nodetype_plural: the non-translated plural title of this node type
 *     - select_options: must be an array where the [key] is a valid cvterm_id and
 *       the [value] is the human-readable name of the option. This is generated from the cv_name/id by default
 *     - base_name_field: the field in your base table you want to be used as the name of the record
 *     - subject_field_name: the name of the subject field in your relationship table (default: subject_id)
 *     - object_field_name: the name of the object field in your relationship table (default: object_id)
 *
 * @ingroup tripal_chado_node_api
 */
function chado_add_node_form_relationships(&$form, &$form_state, $details) {

  // make sure the property table exists before proceeding.
  if (!chado_table_exists($details['relationship_table'])) {
    drupal_set_message("Cannot add relationship elements to the form. The relationship table, '" .
      $details['relationship_table'] . "', does not exists", "error");
    tripal_report_error('tcprops_form', TRIPAL_ERROR, "Cannot add relationship elements to the form.
      The relationship table, '%name', cannot be found.", array('%name' => $details['relationship_table']));
    return;
  }
  
  // make sure the specified cv exists
  if (isset($details['cv_name'])) {
    // make sure the cv_name is real
    $result = chado_select_record('cv',array('cv_id'),array('name' => $details['cv_name']));
    if (count($result) == 0) {
      drupal_set_message("Cannot add relationship elements to the form. The CV name, '" .
          $details['cv_name'] . "', does not exists", "error");
          tripal_report_error('tcprops_form', TRIPAL_ERROR, "Cannot add relationship elements to the form.
        The CV named, '%name', cannot be found.", array('%name' => $details['cv_name']));
          return;
    }
    // add the cv_id option to the details array
    $details['cv_id'] = $result[0]->cv_id;
  }
  elseif (isset($details['cv_id'])) {
    // make sure the cv_id is real
    $result = chado_select_record('cv', array('name'), array('cv_id' => $details['cv_id']));
    if (count($result) == 0) {
      drupal_set_message("Cannot add relationship elements to the form. The CV ID, '" .
          $details['cv_id'] . "', does not exist", "error");
          tripal_report_error('tcprops_form', TRIPAL_ERROR, "Cannot add relationship elements
        to the form. The CV ID, '%id', cannot be found.", array('%id' => $details['cv_id']));
          return;
    }
    // add the cv_name option to the details array
    $details['cv_name'] = $result[0]->name;
  }
  else {
    drupal_set_message("Please provide either a 'cv_name' or 'cv_id' as an
        option for adding relationship to the form", "error");
    tripal_report_error('tcprops_form', TRIPAL_ERROR, "Please provide either a
        'cv_name' or 'cv_id' as an option for adding relationship to the form", array());
    return;
  }
  
  $form_state['rebuild'] = TRUE;

  // Set Defaults for optional fields
  $details['fieldset_title'] = (isset($details['fieldset_title'])) ? $details['fieldset_title'] : 'Relationships';
  $details['additional_instructions'] = (isset($details['additional_instructions'])) ? $details['additional_instructions'] : '';
  $details['nodetype_plural']  = (isset($details['nodetype_plural'])) ? $details['nodetype_plural'] : $details['nodetype'] . 's';
  $details['base_name_field'] = (isset($details['base_name_field'])) ? $details['base_name_field'] : 'uniquename';
  $details['subject_field_name'] = (isset($details['subject_field_name'])) ? $details['subject_field_name'] : 'subject_id';
  $details['object_field_name'] = (isset($details['object_field_name'])) ? $details['object_field_name'] : 'object_id';

  // Some relationship tables don't have a rank
  // thus we need to first check this table has a rank before trying to set it
  $table_schema = chado_get_schema($details['relationship_table']);
  $details['table_has_rank'] = (isset($table_schema['fields']['rank'])) ? TRUE : FALSE;

  // Get Property Types for the Select List
  if (isset($details['select_options'])) {
    $type_options = $details['select_options'];
  }
  else {
    if (isset($details['cv_name'])) {
      $type_options = array();
      $type_options[] = 'Select a Property';
      $sql = "
        SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition, CV.cv_id as cv_id
        FROM  {cvterm} CVT
          INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
        WHERE
          CV.name = :cv_name AND
          NOT CVT.is_obsolete = 1
        ORDER BY CVT.name ASC
      ";
      $prop_types = chado_query($sql, array(':cv_name' => $details['cv_name']));
      while ($prop = $prop_types->fetchObject()) {
        $type_options[$prop->cvterm_id] = $prop->name;
        $details['cv_id'] = $prop->cv_id;
      }
    } 
    elseif (isset($details['cv_id'])) {
      $type_options = array();
      $type_options[] = 'Select a Property';
      $sql = "
        SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition, CV.name AS cv_name
        FROM  {cvterm} CVT
          INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
        WHERE
          CV.cv_id = :cv_id AND
          NOT CVT.is_obsolete = 1
        ORDER BY CVT.name ASC
      ";
      $prop_types = chado_query($sql, array(':cv_id' => $details['cv_id']));
      while ($prop = $prop_types->fetchObject()) {
        $type_options[$prop->cvterm_id] = $prop->name;
        $details['cv_name'] = $prop->cv_name;
      }
    }
  }

  // Tell tripal administrators how to add terms to the property types drop down.
  if (empty($type_options)) {
    $tripal_message = tripal_set_message(
      t('There are currently no property types! To add additional properties to the drop
        down list, you need to <a href="@cvtermlink">add a controlled vocabulary term</a>
        to the %cv_name controlled vocabulary.',
        array(
          '%cv_name' => $details['cv_name'],
          '@cvtermlink' => url('admin/tripal/chado/tripal_cv/cv/'.$details['cv_id'].'/cvterm/add')
        )
      ),
      TRIPAL_WARNING,
      array('return_html' => TRUE)
    );
  }
  else {
    $tripal_message = tripal_set_message(
      t('To add additional properties to the drop down list, you need to <a href="@cvtermlink">add
        a controlled vocabulary term</a> to the %cv_name controlled vocabulary.',
        array(
          '%cv_name' => $details['cv_name'],
          '@cvtermlink' => url('admin/tripal/chado/tripal_cv/cv/' . $details['cv_id'] . '/cvterm/add')
        )
      ),
      TRIPAL_INFO,
      array('return_html' => TRUE)
    );
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
    '#suffix' => '</div>',
    '#weight'      => 10
  );

  $form['relationships']['admin_message'] = array(
    '#type' => 'markup',
    '#markup' => $tripal_message
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
      "SELECT
          rel.*,
          rel.".$details['subject_field_name']." as subject_id,
          rel.".$details['object_field_name']." as object_id,
          base1.".$details['base_name_field']." as object_name,
          base2.".$details['base_name_field']." as subject_name,
          cvterm.name as type_name
        FROM {".$details['relationship_table']."} rel
          LEFT JOIN {".$details['base_table']."} base1 ON base1.".$details['base_foreign_key']." = rel.".$details['object_field_name']."
          LEFT JOIN {".$details['base_table']."} base2 ON base2.".$details['base_foreign_key']." = rel.".$details['subject_field_name']."
          LEFT JOIN {cvterm} cvterm ON cvterm.cvterm_id = rel.type_id
        WHERE rel.".$details['object_field_name']." = :base_key_value
            OR rel.".$details['subject_field_name']." = :base_key_value",
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
    if (array_key_exists($relationship->type_id, $type_options)) {

      $rank = (isset($relationship->rank)) ? $relationship->rank : 0;

      $form['relationships']['relationship_table'][$relationship->type_id]['#type'] = 'markup';
      $form['relationships']['relationship_table'][$relationship->type_id]['#type'] = '';

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['#type'] = 'markup';
      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['#value'] = '';

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['object_id'] = array(
        '#type' => 'hidden',
        '#value' => $relationship->object_id
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['subject_id'] = array(
        '#type' => 'hidden',
        '#value' => $relationship->subject_id
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['type_id'] = array(
        '#type' => 'hidden',
        '#value' => $relationship->type_id
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['object_name'] = array(
        '#type' => 'markup',
        '#markup' => $relationship->object_name
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['type_name'] = array(
        '#type' => 'markup',
        '#markup' => $relationship->type_name
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['subject_name'] = array(
        '#type' => 'markup',
        '#markup' => $relationship->subject_name
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['rank'] = array(
        '#type' => 'markup',
        '#markup' => $rank
      );

      $form['relationships']['relationship_table'][$relationship->type_id][$rank]['rel_action'] = array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "rel_remove-".$relationship->type_id.'-'.$rank,
        '#ajax' => array(
          'callback' => 'chado_add_node_form_relationships_ajax_update',
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
        '#validate' => array('chado_add_node_form_relationships_form_remove_button_validate'),
        '#submit' => array('chado_add_node_form_relationships_remove_button_submit'),
        // Limit the validation of the form upon clicking this button to the relationship_table tree
        // No other fields will be validated (ie: no fields from the main form or any other api
        // added form).
        '#limit_validation_errors' => array(
          array('relationship_table')  // Validate all fields within $form_state['values']['relationship_table']
        )
      );
    }
  }

  $form['relationships']['relationship_table']['new']['object_name'] = array(
    '#type' => 'textfield',
    '#autocomplete_path' => 'tripal_ajax/relationship_nodeform/' . $details['base_table'] . '/' . $details['base_name_field'].'/name_to_id'
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
    '#autocomplete_path' => 'tripal_ajax/relationship_nodeform/' . $details['base_table'] . '/' . $details['base_name_field'].'/name_to_id'
  );

  $form['relationships']['relationship_table']['new']['subject_is_current'] = array(
    '#type' => 'checkbox',
    '#title' => t('Current ' . $details['nodetype']),
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
      'callback' => 'chado_add_node_form_relationships_ajax_update',
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
    '#validate' => array('chado_add_node_form_relationships_add_button_validate'),
    '#submit' => array('chado_add_node_form_relationships_add_button_submit'),
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
 * Called by the add button in chado_add_node_form_relationships
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_add_button_validate($form, &$form_state) {

  $details = unserialize($form_state['values']['relationship_table']['details']);

  // First deal with autocomplete fields
  // extract the base_id assuming '(###) NAME FIELD'
  if (!empty($form_state['values']['relationship_table']['new']['subject_name'])) {
    if (preg_match('/\((\d+)\) .*/', $form_state['values']['relationship_table']['new']['subject_name'], $matches)) {
      $form_state['values']['relationship_table']['new']['subject_id'] = $matches[1];
    }
    else {
      form_set_error('subject_name', 'You need to select the subject from the autocomplete drop-down');
    }
  }
  if (!empty($form_state['values']['relationship_table']['new']['object_name'])) {
    if (preg_match('/\((\d+)\) .*/', $form_state['values']['relationship_table']['new']['object_name'], $matches)) {
      $form_state['values']['relationship_table']['new']['object_id'] = $matches[1];
    }
    else {
      form_set_error('object_name', 'You need to select the subject from the autocomplete drop-down');
    }
  }

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
    $result = chado_select_record(
      $details['base_table'],
      array($details['base_name_field']),
      array($details['base_foreign_key'] => $form_state['values']['relationship_table']['new']['subject_id'])
    );
    if (!isset($result[0])) {
      form_set_error('subject_name', 'The subject must be the unique name of an
        existing '.$details['nodetype'].' unless the "Current '.$details['nodetype'].'" checkbox is selected');
    }
    else {
      $form_state['values']['relationship_table']['new']['subject_name'] = $result[0]->{$details['base_name_field']};
    }
  }

  // The non-current uniquename must exist in the base table (object)
  if (!($form_state['values']['relationship_table']['new']['object_is_current'])) {
    $result = chado_select_record(
      $details['base_table'],
      array($details['base_name_field']),
      array($details['base_foreign_key'] => $form_state['values']['relationship_table']['new']['object_id'])
    );
    if (!isset($result[0])) {
      form_set_error('object_name', 'The object must be the unique name of an
        existing '.$details['nodetype'].' unless the "Current '.$details['nodetype'].'" checkbox is selected');
    }
    else {
      $form_state['values']['relationship_table']['new']['object_name'] = $result[0]->{$details['base_name_field']};
    }
  }

  // The type must be a valid cvterm
  if ($form_state['values']['relationship_table']['new']['type_name']) {
    $form_state['values']['relationship_table']['new']['type_id'] = $form_state['values']['relationship_table']['new']['type_name'];
    $result = chado_select_record(
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
 * Called by the add button in chado_add_node_form_relationships
 *
 * Create an array of additional relationships in the form state. This array will then be
 * used to rebuild the form in subsequent builds
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_add_button_submit(&$form, &$form_state) {

  $details = unserialize($form_state['values']['relationship_table']['details']);

  // if the chado_relationships array is not set then this is the first time modifying the
  // relationship table. this means we need to include all the relationships from the db
  if (!isset($form_state['chado_relationships'])) {
    chado_add_node_form_relationships_create_relationship_formstate_array($form, $form_state);
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
    // we don't want the new element to pick up the values from the previous element so wipe them out
    unset($form_state['input']['relationship_table']['new']['object_id']);
    unset($form_state['input']['relationship_table']['new']['object_name']);
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
    // we don't want the new element to pick up the values from the previous element so wipe them out
    unset($form_state['input']['relationship_table']['new']['subject_id']);
    unset($form_state['input']['relationship_table']['new']['subject_name']);
  }
  
  $key = $relationship['type_id'] . '-' . $relationship['rank'];
  $form_state['chado_relationships'][$key] = (object) $relationship;

  // we don't want the new element to pick up the values from the previous element so wipe them out
  unset($form_state['input']['relationship_table']['new']['type_id']);
  unset($form_state['input']['relationship_table']['new']['type_name']);
  
  $form_state['rebuild'] = TRUE;
}

/**
 * There is no user input for the remove buttons so there is no need to validate
 * However, both a submit & validate need to be specified so this is just a placeholder
 *
 * Called by the many remove buttons in chado_add_node_form_relationships
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_form_remove_button_validate($form, $form_state) {
  // No Validation needed for remove
}

/**
 * Remove the correct relationship from the form
 * Called by the many remove buttons in chado_add_node_form_relationships
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_remove_button_submit(&$form, &$form_state) {

  // if the chado_relationships array is not set then this is the first time modifying the
  // relationship table. this means we need to include all the relationships from the db
  if (!isset($form_state['chado_relationships'])) {
    chado_add_node_form_relationships_create_relationship_formstate_array($form, $form_state);
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
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_ajax_update($form, $form_state) {
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
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_create_relationship_formstate_array($form, &$form_state) {

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
function theme_chado_add_node_form_relationships_table($variables) {
  $element = $variables['element'];

  $details = unserialize($element['details']['#value']);

  $header = array(
    'object_name' => t('Object ' . $details['base_name_field']),
    'type_name' => t('Type'),
    'subject_name' => t('Subject ' . $details['base_name_field']),
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
function chado_retrieve_node_form_relationships($node) {
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
 * @param $details
 *  - relationship_table: the name of the _relationship linking table (ie: feature_relationship)
 *  - foreignkey_value: the value of the foreign key (ie: 445, if there exists a feature where feature_id=445)
 * @param $retrieved_relationships
 *   An array of relationships from chado_retrieve_node_form_relationships($node). This
 *   can be used if you need special handling for some of the relationships.
 *
 * @ingroup tripal_chado_node_api
 */
function chado_update_node_form_relationships($node, $details, $retrieved_relationships = FALSE) {

  $relationship_table = $details['relationship_table'];
  $current_id = $details['foreignkey_value'];

  if (isset($node->relationship_table) AND ($current_id > 0)) {

    // determine whether there is a rank in this relationship table
    $form_details = unserialize($node->relationship_table['details']);
    $has_rank = $form_details['table_has_rank'];

    // First remove existing relationships links
    chado_delete_record(
      $relationship_table,
      array($form_details['subject_field_name'] => $current_id)
    );
    chado_delete_record(
      $relationship_table,
      array($form_details['object_field_name'] => $current_id)
    );

    // Add back in relationships as needed
    if ($retrieved_relationships) {
      $relationships = $retrieved_relationships;
    }
    else {
      $relationships = chado_retrieve_node_form_relationships($node);
    }
    foreach ($relationships as $type_id => $ranks) {
      foreach ($ranks as $rank => $element) {

        $values = array(
          $form_details['subject_field_name'] => $element['subject_id'],
          'type_id' => $type_id,
          $form_details['object_field_name'] => $element['object_id']
        );

        // Set the current id if not already
        // this is usually only necessary in an insert
        if (empty($values[$form_details['subject_field_name']])) {
          $values[$form_details['subject_field_name']] = $current_id;
        }
        if (empty($values[$form_details['object_field_name']])) {
          $values[$form_details['object_field_name']] = $current_id;
        }

        if ($has_rank) {
          // Ensure that the rank is Set & Current
          $rank_select = chado_get_table_max_rank(
            $relationship_table,
            array(
              $form_details['subject_field_name'] => $values['subject_id'],
              'type_id' => $values['type_id'],
              $form_details['object_field_name'] => $values['object_id'],
            )
          );
          $values['rank'] = $rank_select + 1;
        }

        // add relationship
        $success_link = chado_insert_record(
          $relationship_table,
          $values
        );

      }
    }
  }
}

/**
 * Handles autocomplete for subject & object id
 *
 * @param $string
 *    The part of the string already typed in the textfield
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_relationships_name_to_id_callback($base_table, $name_field, $string) {
  $matches = array();

  $base_key = $base_table.'_id';

  $result = db_select('chado.'.$base_table, 'b')
    ->fields('b', array($base_key, $name_field))
    ->condition($name_field, '%' . db_like($string) . '%', 'LIKE')
    ->execute();

  // save the query to matches
  foreach ($result as $row) {
    $key = '('.$row->{$base_key}.') '.substr($row->{$name_field},0,50) . '...';
    $matches[$key] = check_plain($row->{$name_field});
  }

  // return for JS
  drupal_json_output($matches);
}