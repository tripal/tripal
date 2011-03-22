<?php
// $Id$

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_implement_back_to_feature_button($form_state, $nid) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid
  );

  $form["submit-back"] = array( 
    '#type' => 'submit',
    '#value' => t('Back to Stock') 
  );

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_implement_back_to_feature_button_submit($form, $form_state) {
  drupal_goto('node/'.$form_state['values']['nid']);
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_implement_add_chado_properties_progress($current) {

    $value = '<div class="form_progress"><div class="form_progress-text">'; 

    if ($current == 'main') { $value .= '<span id="form-step-current">Create Basic Feature</span>'; } 
    else { $value .= '<span id="form-step">Create Basic Stock</span>'; }

    $value .= '<span id="form-segway">  >>  </span>';

    if ($current == 'properties') { $value .= '<span id="form-step-current">Add Synonyms & Properties</span>'; }
    else { $value .= '<span id="form-step">Add Synonyms & Properties</span>'; }

    $value .= '<span id="form-segway">  >>  </span>';                       

    if ($current == 'db_references') { $value .= '<span id="form-step-current">Add Database References</span>'; }
    else { $value .= '<span id="form-step">Add Database References</span>'; }

    $value .= '<span id="form-segway">  >>  </span>';         

    if ($current == 'relationships') { $value .= '<span id="form-step-current">Add Relationships</span>'; }
    else { $value .= '<span id="form-step">Add Relationships</span>'; }

    $value .= '</div></div>';

    return $value;

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_implement_add_chado_properties_navigate($form_state, $step, $nid) {
  $form = array();

  $form['current_step'] = array(
    '#type' => 'hidden',
    '#value' => $step
  );

  // Use this field to set all the steps and the path to each form
  // where each step is of the form name;path and each step is separated by ::
  $steps =array(
    'properties' => 'node/%node/properties',
    'db_references' => 'node/%node/db_references',
    'relationships' => 'node/%node/relationships'
  );
  $steps_value = array();
  foreach ($steps as $k => $v) { $steps_value[] = $k.';'.$v; }
  $form['steps'] = array(
    '#type' => 'hidden',
    '#value' => implode('::', $steps_value)
  );

  $form['first_step'] = array(
    '#type' => 'hidden',
    '#value' => 'properties'
  );

  $form['last_step'] = array(
    '#type' => 'hidden',
    '#value' => 'relationships'
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid
  );

  if ($step != $form['first_step']['#value']) {
    $form['submit-prev'] = array(
      '#type' => 'submit',
      '#value' => t('Previous Step')
    );
  }

  if ($step != $form['last_step']['#value']) {
    $form['submit-next'] = array( 
      '#type' => 'submit',
      '#value' => t('Next Step')
    );
  }

  if ($step == $form['last_step']['#value']) {
    $form['submit-finish'] = array(
      '#type' => 'submit', 
      '#value' => t('Finish')
    );
  }

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_implement_add_chado_properties_navigate_submit($form, $form_state) {

  $raw_steps = preg_split('/::/', $form_state['values']['steps']); 

  $steps = array();
  $current_index = 'EMPTY';
  $i=0;

  foreach ($raw_steps as $raw_step) {
    $step = preg_split('/;/', $raw_step);
    $steps[$i] = $step;
    
    if ($step[0] == $form_state['values']['current_step']) {
      $current_index = $i;
    }

    $i++;
  }
  $num_steps = $i;
  
  if (strcmp($current_index,'EMPTY') == 0) {
    // No Matching Step
    drupal_set_message('Could not determine next step -'.$form_state['values']['current_step'].', please contact the administrator', 'error');
  } elseif ($current_index == 0) {
    $next_goto = $steps[$current_index+1][1];
  } elseif ($current_index == ($num_steps-1)) {
    $prev_goto = $steps[$current_index-1][1];
    $next_goto = 'node/%node';
  } else {
    $prev_goto = $steps[$current_index-1][1];
    $next_goto = $steps[$current_index+1][1];
  }

  if ($form_state['clicked_button']['#value'] == t('Previous Step') ) {
    //replace %node
    $prev_goto = preg_replace('/%node/', $form_state['values']['nid'], $prev_goto);
    $_REQUEST['destination'] = $prev_goto;
  } elseif ($form_state['clicked_button']['#value'] == t('Next Step') ) {
    $next_goto = preg_replace('/%node/', $form_state['values']['nid'], $next_goto);
    $_REQUEST['destination'] = $next_goto;
  } elseif ($form_state['clicked_button']['#value'] == t('Finish') ) {
    $next_goto = preg_replace('/%node/', $form_state['values']['nid'], $next_goto);
    $_REQUEST['destination'] = $next_goto;
  }

}

/**
 * Implements Hook_form()
 * Handles setting the is_obsolete property of stocks
 *
 * @ingroup tripal_feature
 */
function tripal_feature_is_obsolete_form($node, $stock_id) {

  $form['make_obsolete'] = array(
    '#type' => 'submit',
    '#value' => t('Mark Stock as Obsolete')
  );

  $form['make_obsolete_stock_id'] = array(
    '#type' => 'value',
    '#value' => $stock_id,
    '#required' => TRUE
  );

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_is_obsolete_form_submit($form, &$form_state) {

  $previous_db = db_set_active('chado');
  db_query(
    "UPDATE stock SET is_obsolete='t' WHERE stock_id=%d",
    $form_state['values']['make_obsolete_stock_id']
  );
  db_set_active($previous_db);

}

