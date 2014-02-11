<?php

/**
 * @file
 * API to manage the chado prop table for various Tripal Node Types
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

    // To add in the chado properties form elements, you first need to prepare the arguments
    // for the function call.

    $details = array(
      'property_table' => 'example_property',      // the name of the table linking additional properties to this node
      'base_foreign_key' => 'example_id',          // key to link to the chado content created by this node
      'base_key_value' => $example_id,             // the value of the above key
      'cv_name' => 'example_prop_cv',              // the name of the cv governing the _prop.type_id
      'fieldset_title' => 'Additional References', // the non-translated title for this fieldset
      'additional_instructions' => ''              // a non-stranslated string providing additional instructions
    );

    // Finally, and add the additional form elements to the form
    chado_add_node_form_properties($form, $form_state, $details);

    return $form;
  }

  function chado_example_insert($node) {

    // if there is an example_id in the $node object then this must be a sync so
    // we can skip adding the chado_example as it is already there, although
    // we do need to proceed with the rest of the insert
    if (!property_exists($node, 'example_id')) {

      // Add record to chado example table

      // Add to any other tables needed

      // Add all properties
      // Existing _property links will be cleared and then re-added
      tripal_api_chado_node_properties_form_update_properties(
        $node,                // the node object passed in via hook_insert()
        'example_property',   // the name of the _property linking table
        'example',            // the name of the base chado table for the node
        'example_id',         // key to link to the chado content created by this node
        $node->example_id     // value of the above key
      );
    }

    // Add record to chado_example linking example_id to new node

  }

  function chado_example_update($node) {


      // Update record in chado example table

      // Update any other tables needed

      // Update all properties
      // Existing _property links will be cleared and then re-added
      tripal_api_chado_node_properties_form_update_properties(
        $node,                // the node object passed in via hook_insert()
        'example_property',   // the name of the _property linking table
        'example',            // the name of the base chado table for the node
        'example_id',         // key to link to the chado content created by this node
        $node->example_id     // value of the above key
      );

    // Don't need to update chado_example linking table since niether example_id or nid can be changed in update

  }

 * @endcode
 *
 * @ingroup tripal_chado_node_api
 */

/**
 * Provides a form for adding to BASEprop table
 *
 * @param $form
 *   The Drupal form array into which the property form elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $details
 *   An array defining details needed by this form. Required Keys are:
 *     - property_table: the name of the property linking table (ie: featureprop)
 *     - base_foreign_key: the name of the foreign key linking this table to the non-property table (ie: feature_id)
 *     - base_key_value: the value of the base_foreign_key for the current form (ie: 999 if the feature_id=999)
 *   Require ONE of the following:
 *     The controlled vocabulary governing the property types
 *       -cv_id: the unique key from the cv table
 *       -cv_name: the cv.name field uniquely identifying the controlled vocab
 *   Optional keys include:
 *     - fieldset_title: the non-translated title for this fieldset
 *     - additional_instructions: a non-translated string providing additional instructions
 *     - select_options: must be an array where the [key] is a valid cvterm_id and
 *       the [value] is the human-readable name of the option. This is generated from the cv_name/id by default
 *
 * @ingroup tripal_chado_node_api
 */
function chado_add_node_form_properties(&$form, &$form_state, $details) {

  // Set Defaults for optional fields
  $details['fieldset_title'] = 'Properties';
  $details['additional_instructions'] = '';

  // Get Property Types for the Select List
  if (isset($details['select_options'])) {
    $property_options = $details['select_options'];
  }
  else {
    if (isset($details['cv_name'])) {
      $property_options = array();
      $property_options[] = 'Select a Property';
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
        $property_options[$prop->cvterm_id] = $prop->name;
        $details['cv_id'] = $prop->cv_id;
      }
      if (!isset($details['cv_id'])) {
        $result = chado_select_record('cv',array('cv_id'),array('name' => $details['cv_name']));
        $details['cv_id'] = $result[0]->cv_id;
      }
    } elseif (isset($details['cv_id'])) {
      $property_options = array();
      $property_options[] = 'Select a Property';
      $sql = "
        SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition, CV.name as cv_name
        FROM  {cvterm} CVT
          INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
        WHERE
          CV.cv_id = :cv_id AND
          NOT CVT.is_obsolete = 1
        ORDER BY CVT.name ASC
      ";
      $prop_types = chado_query($sql, array(':cv_id' => $details['cv_id']));
      while ($prop = $prop_types->fetchObject()) {
        $property_options[$prop->cvterm_id] = $prop->name;
        $details['cv_name'] = $prop->cv_name;
      }
      if (!isset($details['cv_name'])) {
        $result = chado_select_record('cv',array('name'),array('cv_id' => $details['cv_id']));
        $details['cv_name'] = $result[0]->name;
      }
    }
  }

  // Tell tripal administrators how to add terms to the property types drop down.
  if (empty($property_options)) {
    $tripal_message = tripal_set_message(
      t('There are currently no proeprty types! To add additional properties to the drop
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
          '@cvtermlink' => url('admin/tripal/chado/tripal_cv/cv/'.$details['cv_id'].'/cvterm/add')
        )
      ),
      TRIPAL_INFO,
      array('return_html' => TRUE)
    );
  }

  // the fieldset of the property elements
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => t('You may add additional properties by selecting a property type
      from the dropdown and adding text. You may add as many properties as desired by
      clicking the add button on the right. To remove a property, click the remove button.'
      . $details['additional_instructions']),
    '#prefix' => "<div id='properties-fieldset'>",
    '#suffix' => '</div>',
    '#weight'      => 8
  );

  $form['properties']['admin_message'] = array(
    '#type' => 'markup',
    '#markup' => $tripal_message
  );

  // this form element is a tree, so that we don't puke all of the values into then node variable
  // it is set as a tree, and keeps them in the $form_state['values']['property_table'] heading.
  $form['properties']['property_table'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#prefix' => '<div id="tripal-generic-edit-properties-table">',
    '#suffix' => '</div>',
    '#theme' => 'chado_node_properties_form_table'
  );

  // Add defaults into form_state to be used elsewhere
  $form['properties']['property_table']['details'] = array(
    '#type' => 'hidden',
    '#value' => serialize($details)
  );

  /* Properties can come to us in two ways:
   *
   * 1) In the form state in the $form_state['chado_properties']. Data is in this field
   *    when an AJAX call updates the form state or a validation error.
   *
   * 2) Directly from the database if the record already has properties associated.  This
   *    data is only used the first time the form is loaded. On AJAX calls or validation
   *    errors the fields on the form are populated from the $form_state['chado_properties']
   *    entry.
   */
  if (isset($form_state['chado_properties'])) {
    $existing_properties = $form_state['chado_properties'];
  }
  else {
    if (isset($details['cv_name'])) {
      $existing_properties = chado_query(
        "SELECT PP.".$details['property_table']."_id property_id, CVT.cvterm_id as type_id, CVT.name as type_name, CVT.definition, PP.value, PP.rank
        FROM {" . $details['property_table'] . "} PP
        INNER JOIN {cvterm} CVT ON CVT.cvterm_id = PP.type_id
        INNER JOIN {cv} CV      ON CVT.cv_id     = CV.cv_id
        WHERE
        PP." . $details['base_foreign_key'] . " = :base_key_value AND
        CV.name = '" .$details['cv_name']. "'
        ORDER BY CVT.name, PP.rank",
         array(':base_key_value' => $details['base_key_value'])
      );
    } elseif (isset($details['cv_id'])) {
      $existing_properties = chado_query(
        "SELECT PP.".$details['property_table']."_id property_id, CVT.cvterm_id as type_id, CVT.name as type_name, CVT.definition, PP.value, PP.rank
        FROM {" . $details['property_table'] . "} PP
        INNER JOIN {cvterm} CVT ON CVT.cvterm_id = PP.type_id
        INNER JOIN {cv} CV      ON CVT.cv_id     = CV.cv_id
        WHERE
        PP." . $details['base_foreign_key'] . " = :base_key_value AND
        CV.cv_id = '" .$details['cv_id']. "'
        ORDER BY CVT.name, PP.rank",
         array(':base_key_value' => $details['base_key_value'])
      );
    }
  }

  /* The format of the $existing_properties array is either:
   *
   * From the chado_properties array:
   * $form_state['chado_properties'] = array(
   *   '[type_id]-[rank]' => array(
   *     'type_id' => [the cvterm.cvterm_id value]
   *     'type_name' => [the cvterm.name value]
   *     'property_id' => [the property.property_id value, or NULL if it doesn't yet exist],
   *     'value' => [the BASEprop.value value],
   *     'rank' => [the BASEprop.rank value],
   *   ),
   * );
   *
   * OR
   * Populated from the database:
   * $existing_property = array(
   *   0 => array(
   *     'property_id' => [the property.property_id value, or NULL if it doesn't yet exist],
   *     'type_id' => [the cvterm.cvterm_id value]
   *     'type_name' => [the cvterm.name value]
   *     'value' => [the BASEprop.value value],
   *     'rank' => [the BASEprop.rank value],
   *   ),
   * );
   *
   * NOTE: The main difference is the key
   *
   * Loop on the array elements of the $existing_properties array and add
   * an element to the form for each one as long as it's also in the
   * $properties_options array.
   */
  foreach ($existing_properties as $property) {
    if (array_key_exists($property->type_id, $property_options)) {

      $form['properties']['property_table'][$property->type_id]['#type'] = 'markup';
      $form['properties']['property_table'][$property->type_id]['#value'] = '';

      $form['properties']['property_table'][$property->type_id][$property->rank]['#type'] = 'markup';
      $form['properties']['property_table'][$property->type_id][$property->rank]['#value'] = '';

      $form['properties']['property_table'][$property->type_id][$property->rank]['prop_type_id'] = array(
        '#type' => 'hidden',
        '#value' => $property->type_id
      );

      $form['properties']['property_table'][$property->type_id][$property->rank]['prop_value'] = array(
        '#type' => 'hidden',
        '#value' => $property->value
      );

      $form['properties']['property_table'][$property->type_id][$property->rank]['property_id'] = array(
        '#type' => 'hidden',
        '#value' => $property->property_id
      );

      $form['properties']['property_table'][$property->type_id][$property->rank]['type'] = array(
        '#type' => 'markup',
        '#markup' => $property->type_name
      );

      $form['properties']['property_table'][$property->type_id][$property->rank]['value'] = array(
        '#type' => 'markup',
        '#markup' => $property->value
      );

      $form['properties']['property_table'][$property->type_id][$property->rank]['rank'] = array(
        '#type' => 'markup',
        '#markup' => $property->rank
      );
      // remove button
      $form['properties']['property_table'][$property->type_id][$property->rank]['property_action'] = array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "property_remove-".$property->type_id.'-'.$property->rank,
        '#ajax' => array(
          'callback' => "chado_add_node_form_properties_ajax_update",
          'wrapper' => 'tripal-generic-edit-properties-table',
          'effect'   => 'fade',
          'method'   => 'replace',
          'prevent'  => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default node form submit.  In the validate function we validate only the
        // property fields and in the submit we remove the indicated property
        // from the chado_properties array. In order to keep validate errors
        // from the node form validate and Drupal required errors for non-property fields
        // preventing the user from removing properties we set the #limit_validation_errors below
        '#validate' => array('chado_add_node_form_properties_remove_button_validate'),
        '#submit' => array('chado_add_node_form_properties_remove_button_submit'),
        // Limit the validation of the form upon clicking this button to the property_table tree
        // No other fields will be validated (ie: no fields from the main form or any other api
        // added form).
        '#limit_validation_errors' => array(
          array('property_table')  // Validate all fields within $form_state['values']['property_table']
        )
      );
    }
  }

  // Form elements for adding a new property
  //---------------------------------------------
  $form['properties']['property_table']['new'] = array(
    '#type' => 'markup',
    '#prefix' => '<span class="addtl-properties-add-new-property">',
    '#suffix' => '</span>'
  );

  $form['properties']['property_table']['new']['type'] = array(
    '#type' => 'select',
    '#options' => $property_options, // Set at top of form
  );

  $form['properties']['property_table']['new']['value'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
  );

  // add button
  $form['properties']['property_table']['new']['property_action'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => "property-add",
    '#ajax' => array(
      'callback' => "chado_add_node_form_properties_ajax_update",
      'wrapper' => 'tripal-generic-edit-properties-table',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default node form submit.  In the validate function we validate only the
    // additional property fields and in the submit we add them to the chado_properties
    // array. In order to keep validate errors from the node form validate and Drupal
    // required errors for non-property fields preventing the user from adding properties we
    // set the #limit_validation_errors below
    '#validate' => array('chado_update_node_form_properties_add_button_validate'),
    '#submit' => array('chado_add_node_form_properties_add_button_submit'),
    // Limit the validation of the form upon clicking this button to the property_table tree
    // No other fields will be validated (ie: no fields from the main form or any other api
    // added form).
    '#limit_validation_errors' => array(
      array('property_table')  // Validate all fields within $form_state['values']['property_table']
    )
  );
}

/**
 * Validate the user input for creating a new property
 * Called by the add button in chado_add_node_form_properties
 *
 * @ingroup tripal_core
 */
function chado_update_node_form_properties_add_button_validate($form, &$form_state) {

  // Ensure the type_id is supplied & Valid
  $cvterm = chado_select_record(
    'cvterm',
    array('cvterm_id', 'name'),
    array('cvterm_id' => $form_state['values']['property_table']['new']['type'])
  );
  if (!isset($cvterm[0])) {
    form_set_error('property_table][new][cvterm', 'Please select a property type before attempting to add a new property.');
  }
  else {
    $form_state['values']['property_table']['new']['type_name'] = $cvterm[0]->name;
  }

  // Ensure value is supplied
  if (empty($form_state['values']['property_table']['new']['value'])) {
    form_set_error('property_table][new][value','You must enter the property value before attempting to add a new property.');
  }
}

/**
 * Called by the add button in chado_add_node_form_properties
 *
 * Create an array of properties in the form state. This array will then be
 * used to rebuild the form in subsequent builds
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_properties_add_button_submit(&$form, &$form_state) {

  $details = unserialize($form_state['values']['property_table']['details']);

  // if the chado_additional_properties array is not set then this is the first time modifying the
  // property table. this means we need to include all the properties from the db
  if (!isset($form_state['chado_properties'])) {
    chado_add_node_form_properties_create_property_formstate_array($form, $form_state);
  }

  // get details for the new property
  $property = array(
    'type_id' => $form_state['values']['property_table']['new']['type'],
    'type_name' => $form_state['values']['property_table']['new']['type_name'],
    'property_id' => NULL,
    'value' => $form_state['values']['property_table']['new']['value'],
    'rank' => '0',
  );

  // get max rank
  $rank = chado_get_table_max_rank(
    $details['property_table'],
    array(
      $details['base_foreign_key'] => $details['base_key_value'],
      'type_id' => $property['type_id']
    )
  );
  $property['rank'] = strval($rank + 1);

  $key = $property['type_id'] . '-' . $property['rank'];
  $form_state['chado_properties'][$key] = (object) $property;

  $form_state['rebuild'] = TRUE;
}

/**
 * There is no user input for the remove buttons so there is no need to validate
 * However, both a submit & validate need to be specified so this is just a placeholder
 *
 * Called by the many remove buttons in chado_add_node_form_properties
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_properties_remove_button_validate($form, $form_state) {
  // No Validation needed for remove
}

/**
 * Remove the correct property from the form
 * Called by the many remove buttons in chado_add_node_form_properties
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_properties_remove_button_submit(&$form, &$form_state) {

  // if the chado_properties array is not set then this is the first time modifying the
  // property table. this means we need to include all the properties from the db
  if (!isset($form_state['chado_properties'])) {
    chado_add_node_form_properties_create_property_formstate_array($form, $form_state);
  }

  // remove the specified property from the form property table
  if(preg_match('/property_remove-([^-]+-[^-]+)/',$form_state['triggering_element']['#name'],$match)) {
    $key = $match[1];
    if (array_key_exists($key, $form_state['chado_properties'])) {
      unset($form_state['chado_properties'][$key]);
    }
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax function which returns the section of the form to be re-rendered
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_properties_ajax_update($form, $form_state) {
  return $form['properties']['property_table'];
}

/**
 * Creates an array in form_state containing the existing properties. This array is
 * then modified by the add/remove buttons and used as a source for rebuilding the form.
 * This function get's called at each button (add and remove) button submits the first
 * time one of the button's is clicked to instantiates the $form_state['chado_properties'] array
 *
 * $form_state['chado_properties'] = array(
 *   '[type_id]-[rank]' => array(
 *     'type_id' => [the cvterm.cvterm_id value]
 *     'type_name' => [the cvterm.name value]
 *     'property_id' => [the property.property_id value, or NULL if it doesn't yet exist],
 *     'value' => [the BASEprop.value value],
 *     'rank' => [the BASEprop.rank value],
 *   ),
 * );
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_properties_create_property_formstate_array($form, &$form_state) {

  $form_state['chado_properties'] = array();

  foreach (element_children($form['properties']['property_table']) as $type_id) {
    if ($type_id != 'new') {
      foreach (element_children($form['properties']['property_table'][$type_id]) as $rank) {
          $element = $form['properties']['property_table'][$type_id][$rank];
          $property = array(
            'type_id' => $element['prop_type_id']['#value'],
            'type_name' => $element['type']['#markup'],
            'property_id' => $element['property_id']['#value'],
            'value' => $element['value']['#markup'],
            'rank' => $element['rank']['#markup']
          );
          $key = $property['type_id'] . '-' . $property['rank'];
          $form_state['chado_properties'][$key] = (object) $property;
      }
    }
  }
}

/**
 * Function to theme the add/remove properties form into a table
 *
 * @ingroup tripal_chado_node_api
 */
function theme_chado_add_node_form_properties($variables) {
  $element = $variables['element'];

  $header = array(
    'type' => t('Type'),
    'value' => t('Value'),
    'property_action' => t('Actions'),
  );

  $rows = array();
  foreach (element_children($element) as $type_id) {
    if ($type_id == 'new') {
      $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$type_id][$fieldname]);
        }
        $rows[] = $row;
    }
    else {
      foreach (element_children($element[$type_id]) as $version) {
        $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$type_id][$version][$fieldname]);
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
 * when the chado node properties form has been added to the form.  It retrieves all of the properties
 * and returns them in an array of the format:
 *
 *   $dbxefs[<type_id>][<rank>] = <value>
 *
 * This array can then be used for inserting or updating properties
 *
 * @param $node
 *
 * @return
 *   A property array
 *
 * @ingroup tripal_chado_node_api
 */
function chado_retrieve_node_form_properties($node) {
  $properties = array();

  if (isset($node->property_table)) {
    foreach ($node->property_table as $type_id => $elements) {
      if ($type_id != 'new' AND $type_id != 'details') {
        foreach ($elements as $rank => $element) {
          $properties[$type_id][$rank] = $element['prop_value'];
        }
      }
    }
  }

  return $properties;
}

/**
 * This function is used in hook_insert or hook_update and handles inserting of any new
 * properties
 *
 * @param $node
 *    The node passed into hook_insert & hook_update
 * @param $details
 *   - property_table: the name of the _property linking table (ie: feature_property)
 *   - base_table: the name of the base table (ie: feature)
 *   - foreignkey_name: the name of the foreign key used to link to the node content (ie: feature_id)
 *   - foreignkey_value: the value of the foreign key (ie: 445, if there exists a feature where feature_id=445)
 * @param $retrieved_properties
 *   An array of properties from chado_retrieve_node_form_properties($node). This can be used if you need
 *   special handling for some of the properties (See FeatureMap chado_featuremap_insert for an example)
 *
 * @ingroup tripal_chado_node_api
 */
function chado_update_node_form_properties($node, $details, $retrieved_properties = FALSE) {

  $details['foreignkey_value'] = (isset($details['foreignkey_value'])) ? $details['foreignkey_value'] : 0;

  if (isset($node->property_table) AND ($details['foreignkey_value'] > 0)) {
    // First remove existing property links
    chado_delete_record($details['property_table'], array($details['foreignkey_name'] => $details['foreignkey_value']));

    // Add back in property links and insert properties as needed
    if ($retrieved_properties) {
      $properties = $retrieved_properties;
    }
    else {
      $properties = chado_retrieve_node_form_properties($node);
    }
    foreach ($properties as $type_id => $ranks) {
      foreach ($ranks as $rank => $value) {

        $success = chado_insert_record(
          $details['property_table'],
          array(
            $details['foreignkey_name'] => $details['foreignkey_value'],
            'type_id' => $type_id,
            'value' => $value,
            'rank' => $rank
          )
        );

        if (!$success) {
          tripal_report_error('tripal_' . $details['base_table'], TRIPAL_ERROR,
            $details['base_table'] . ' Insert: Unable to insert property type_id %cvterm with value %value.',
            array('%cvterm' => $type_id, '%value' => $value));
        }
      }
    }
  }
}