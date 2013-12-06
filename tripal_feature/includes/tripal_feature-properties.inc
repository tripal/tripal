<?php
/**
 * @file
 * @todo Add file header description
 */

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_properties_page($node) {
  $output = '';

  // get the list of properties for this feature
  $values  = array('feature_id' => $node->feature->feature_id);
  $options = array('order_by' => array('type_id' => 'ASC', 'rank' => 'ASC'));
  $properties = tripal_core_generate_chado_var('featureprop', $values, $options);
  $properties = tripal_core_expand_chado_vars($properties, 'field', 'featureprop.value');

  $expand_add = (sizeof($properties)) ? FALSE : TRUE;

  // add the appopriate form sections
  $output .= drupal_get_form('tripal_feature_add_ONE_property_form', $node, $expand_add);
  $output .= drupal_get_form('tripal_feature_edit_ALL_properties_form', $node, $properties);
  $output .= drupal_get_form('tripal_feature_implement_back_to_feature_button', $node->nid);

  return $output;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form($form_state, $node, $expand) {
  $form = array();
  $feature_id = $node->feature->feature_id;

  $form['add_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Property'),
    '#collapsible' => TRUE,
    '#collapsed' => ($expand) ? FALSE : TRUE,
  );

  $form['prop_nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $form['add_properties']['feature_id'] = array(
    '#type' => 'value',
    '#value' => $feature_id,
    '#required' => TRUE
  );

  // right now this defaults to the 'feature_property' CV
  // but in the future it should be more flexible
  $form['cv_name'] = array(
    '#type' => 'hidden',
    '#value' => 'feature_property'
  );

  // get the list of property types
  $prop_type_options = array();
  $columns = array('cvterm_id', 'name');
  $values = array(
    'cv_id' => array(
      'name' => $form['cv_name']['#value'],
    )
  );
  $results = tripal_core_chado_select('cvterm', $columns, $values);
  foreach ($results as $r) {
    $prop_type_options[$r->name] = $r->name;
  }

  $form['add_properties']['property'] = array(
    '#type' => 'select',
    '#title' => t('Type of Property'),
    '#options' => $prop_type_options,
  );

  $form['add_properties']['prop_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
  );

  $form['add_properties']['submit-add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Property')
  );

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form_validate($form, &$form_state) {

  // Only Require if Adding Property
  if ($form_state['clicked_button']['#value'] == t('Add Property') ) {

    // Check that there is a feature
    if ( $form_state['values']['feature_id'] <= 0 ) {
      form_set_error('feature_id', 'There is no associated feature.');
    }

    // Check that Selected a type
    if ( !$form_state['values']['property']) {
      form_set_error('property', 'Please select a type of property.');
    }
  }
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form_submit($form, &$form_state) {
  $feature_id = $form_state['values']['feature_id'];
  $property = $form_state['values']['property'];
  $value = $form_state['values']['prop_value'];
  $cv_name = $form_state['values']['cv_name'];

  $succes = tripal_feature_insert_property($feature_id, $property, $value, 0, $cv_name);
  if ($succes) {
    drupal_set_message(t("Successfully Added Property: %property => %value", array('%property' => $property), array('%value' => $value)));
  }
  else {
    drupal_set_message(t("Failed to Add Property: %property => %value", array('%property' => $property), array('%value' => $value)));
  }
}

/**
 * Implements Hook_form()
 * Handles adding of Properties for features
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_properties_form($form_state, $node, $properties) {
  $form = array();
  $feature_id = $node->feature->feature_id;

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $form['add_properties']['feature_id'] = array(
    '#type' => 'value',
    '#value' => $feature_id,
    '#required' => TRUE
  );

  // right now this defaults to the 'feature_property' CV
  // but in the future it should be more flexible
  $form['cv_name'] = array(
    '#type' => 'hidden',
    '#value' => 'feature_property'
  );

  if (sizeof($properties)) {

    // build the select box options for the property name
    $prop_type_options = array();
    $columns = array('cvterm_id', 'name');
    $values = array(
      'cv_id' => array(
        'name' => $form['cv_name']['#value']
      )
    );
    $results = tripal_core_chado_select('cvterm', $columns, $values);
    foreach ($results as $r) {
      $prop_type_options[$r->name] = $r->name;
    }

    // iterate through all of the properties and create a set of form elements
  foreach ($properties as $i => $property) {
    $form["num-$i"] = array(
      '#type' => 'fieldset',
      '#value' => "Property $i"
    );
    $form["num-$i"]["id-$i"] = array(
      '#type' => 'hidden',
      '#value' => $property->featureprop_id
    );
    $default = array_search($property->type, $prop_type_options);
    $form["num-$i"]["type-$i"] = array(
      '#type' => 'select',
      '#options' => $prop_type_options,
      '#default_value' => $property->type_id->name
    );
    $form["num-$i"]["value-$i"] = array(
      '#type' => 'textfield',
      '#default_value' => $property->value
    );
    $form["num-$i"]["delete-$i"] = array(
      '#type' => 'submit',
      '#value' => t("Delete"),
      '#name' => "delete-$i",
    );
    } //end of foreach property

    $form['num_properties'] = array(
      '#type' => 'hidden',
      '#value' => $i
    );

    $form["submit-edits"] = array(
      '#type' => 'submit',
      '#value' => t('Update All Properties')
    );
  }

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_properties_form_submit($form, &$form_state) {

  $cv_name = $form_state['values']['cv_name'];
  $feature_id = $form_state['values']["feature_id"];
  $all_good = 1;

  // if the update button was clicked then do the update
  if ($form_state['clicked_button']['#value'] == t('Update All Properties') ) {
    // iterate through each of the properties and set each one
    for ($i=1; $i<=$form_state['values']['num_properties']; $i++) {
      $featureprop_id = $form_state['values']["id-$i"];
      $property = $form_state['values']["type-$i"];
      $value = $form_state['values']["value-$i"];
      $success = tripal_feature_update_property_by_id($featureprop_id, $property, $value, $cv_name);
      if (!$success) {
        drupal_set_message(t("Failed to Update Property: %property => %value", array('%property' => $property), array('%value' => $value)));
        $all_good = 0;
      }
    }
    if ($all_good) {
      drupal_set_message(t("Updated all Properties"));
    }
    drupal_goto('node/' . $form_state['values']['nid']);
  }
  // if the delete button was clicked then remove the property
  elseif (preg_match('/delete-(\d+)/', $form_state['clicked_button']['#name'], $matches) ) {
    $i = $matches[1];
    $featureprop_id = $form_state['values']["id-$i"];
    $property = $form_state['values']["type-$i"];
    $value = $form_state['values']["value-$i"];
    $success = tripal_feature_delete_property_by_id($featureprop_id);
    if ($success) {
      drupal_set_message(t("Deleted Property"));
    }
    else {
      drupal_set_message(t("Unable to Delete Property"));
    }
  }
  else {
    drupal_set_message(t("Unrecognized Button Pressed"), 'error');
  }
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_edit_ALL_properties_form($form) {
  $output = '';

  $output .= '<br /><fieldset>';
  $output .= '<legend>Edit Already Existing Properties<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Below is a list of already existing properties for this feature, one property per line. The type refers to the type of ' .
             'property and the value is the value for that property. </p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Type</th><th>Value</th><th></th></tr>';

  for ($i=0; $i<=$form['num_properties']['#value']; $i++) {
    if (isset($form["num-$i"])) {
      $output .= '<tr><td>' . ($i+1) . '</td><td>' . drupal_render($form["num-$i"]["type-$i"]) . '</td><td>' . drupal_render($form["num-$i"]["value-$i"]) . '</td><td>' . drupal_render($form["num-$i"]["delete-$i"]) . '</td></tr>';
      unset($form["num-$i"]);
    }
  }

  $output .= '</table><br />';
  $output .= drupal_render($form);
  $output .= '</fieldset>';

  return $output;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_list_properties_for_node($properties) {

  if (!empty($properties)) {
    $output = '<table>';
    $output .= '<tr><th>Type</th><th>Value</th></tr>';


    if (!empty($properties) ) {
      foreach ($properties as $p) {
        $output .= '<tr><td>' . $p->type . '</td><td>' . $p->value . '</td></tr>';
      } // end of foreach property
    }

    $output .= '</table>';

  }
  else {
    $output = 'No properties exist for the current feature';
  }

  return $output;
}


