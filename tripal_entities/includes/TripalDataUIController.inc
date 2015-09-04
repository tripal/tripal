<?php
/**
 * UI controller.
 */
class TripalDataUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    $items = array();

    // Set this on the object so classes that extend hook_menu() can use it.
    $this->id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';
    $id_count = count(explode('/', $this->path));

    $items[$this->path] = array(
      'title' => 'Biological Content',
      'description' => 'Add edit and update models.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );

    // Change the overview menu type for the list of models.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add new biological data',
      'description' => 'Add new biological data',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('tripal_data_form'),
      'access callback'  => 'tripal_data_access',
      'access arguments' => array('edit'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 20,
    );

    // Add menu items to add each different type of entity.
//     foreach (tripal_data_get_types() as $type) {
//       $items[$this->path . '/add/' . $type->type] = array(
//         'title' => 'Add ' . $type->label,
//         'page callback' => 'model_form_wrapper',
//         'page arguments' => array(model_create(array('type' => $type->type))),
//         'access callback' => 'model_access',
//         'access arguments' => array('edit', 'edit ' . $type->type),
//         'file' => 'model.admin.inc',
//         'file path' => drupal_get_path('module', $this->entityInfo['module'])
//       );
//     }

    // Loading and editing model entities
    $items[$this->path . '/model/' . $wildcard] = array(
      'page callback' => 'model_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'model_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'model.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items[$this->path . '/model/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );

    $items[$this->path . '/model/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'model_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'model_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'model.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );

    // Menu item for viewing models
    $items['model/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'model_page_title',
      'title arguments' => array(1),
      'page callback' => 'model_page_view',
      'page arguments' => array(1),
      'access callback' => 'model_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    return $items;

/*     // Set a custom page for adding new tripal data entities.
    $items['data/add'] = array(
      'title' => 'Add Tripal data',
      'description' => 'Add a new tripal data record',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('tripal_data_form'),
      'access callback'  => 'tripal_data_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
    );

    // Link for viewing a tripal data type.
    $items['data/' . $wildcard] = array(
      'title callback' => 'tripal_data_title',
      'title arguments' => array(1),
      'page callback' => 'tripal_data_view',
      'page arguments' => array(1),
      'access callback' => 'tripal_data_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['data/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'tripal_data_view',
      'page arguments' => array(1),
      'access callback' => 'tripal_data_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,

    );

    // 'Edit' tab for an individual entity page.
    $items['data/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_data_form', 1),
      'access callback' => 'tripal_data_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_LOCAL_TASK,

    );

    // Menu item for deleting tripal data entities.
    $items['data/' . $wildcard . '/delete'] = array(
      'title'  => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_data_delete_form', 1),
      'access callback' => 'tripal_data_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_CALLBACK,

      'weight' => 10,
    ); */
    return $items;
  }

}
/**
 * Determines whether the given user has access to a tripal data entity.
 *
 * @param $op
 *   The operation being performed. One of 'view', 'update', 'create', 'delete'
 *   or just 'edit' (being the same as 'create' or 'update').
 * @param $entity
 *   Optionally a tripal data entity or a tripal data type to check access for.
 *   If nothing is given, access for all types is determined.
 * @param $account
 *   The user to check for. Leave it to NULL to check for the global user.
 * @return boolean
 *   Whether access is allowed or not.
 */
function tripal_data_access($op, $entity = NULL, $account = NULL) {
  if (user_access('administer tripal data', $account)) {
    return TRUE;
  }
  if (isset($entity) && $type_name = $entity->type) {
    $op = ($op == 'view') ? 'view' : 'edit';
    if (user_access("$op any $type_name data", $account)) {
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *
 */
function tripal_data_form($form, &$form_state, $entity = NULL) {

  // Set the defaults.
  $cv_id = NULL;
  $term_name = NULL;
  $cvterm = NULL;

  // Set defaults if an entity was provided.
  if ($entity) {
    drupal_set_title('Edit ' . $entity->title);
    $id = $entity->id;
    $values = array('cvterm_id' => $entity->cvterm_id);
    $cvterm = chado_generate_var('cvterm', $values);
    $cv_id = $cvterm->cv_id->cv_id;
    $term_name = $cvterm->name;
  }

  // Set defaults using the form state.
  if (array_key_exists('values', $form_state)) {
    $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : NULL;
    $term_name = array_key_exists('term_name', $form_state['values']) ? $form_state['values']['term_name'] : NULL;
    // Get the cvterm that matches
    $values = array(
      'cv_id' => $cv_id,
      'name' => $term_name
    );
    $cvterm = chado_generate_var('cvterm', $values);
  }

  // Let the user select the vocabulary          defaut and tripal_data but only if they haven't
  // already selected a tripal_data.
  $cvs = tripal_entities_get_published_vocabularies_as_select_options();
  if (!$term_name) {
    $form['cv_id'] = array(
      '#type' => 'select',
      '#title' => t('Published vocabulary'),
      '#options' => $cvs,
      '#required' => TRUE,
      '#description' => t('Select a vocabulary that contains the term for the type of data you want to add.'),
      '#default_value' => $cv_id,
      '#ajax' => array(
        'callback' => "tripal_data_form_ajax_callback",
        'wrapper' => 'tripal_data_form',
        'effect' => 'fade',
        'method' => 'replace'
      )
    );
  }

  // If we have a CV ID then we want to provide an autocomplete field
  if ($cv_id and !$term_name) {
    $cvterms = tripal_entities_get_published_terms_as_select_options ($cv_id);
    $form['cvterm_select']['term_name'] = array(
      '#title'       => t('Published term'),
      '#type'        => 'select',
      '#options' => $cvterms,
      '#description' => t("Enter the name of a term within the selected vocabulary for the record type you want to enter."),
      '#required'    => TRUE,
    );

    $form['cvterm_select']['select_button'] = array(
      '#type' => 'submit',
      '#value' => t('Use this term'),
      '#name' => 'select_cvterm',
    );
  }

  // Once the CV term is selected then provide the other fields.
  if ($cvterm) {
    $bundle_id = $cvterm->dbxref_id->db_id->name . '_' . $cvterm->dbxref_id->accession;

    $form['cv_id'] = array(
      '#type'  => 'hidden',
      '#value' => $cv_id,
    );
    $form['entity_type'] = array(
      '#type'  => 'hidden',
      '#value' => $cvterm->dbxref_id->db_id->name,
    );
    $form['term_name'] = array(
      '#type'  => 'hidden',
      '#value' => $term_name,
    );
    $form['cvterm_id'] = array(
      '#type'  => 'hidden',
      '#value' => $cvterm->cvterm_id,
    );
    $form['bundle'] = array(
      '#type'  => 'hidden',
      '#value' => $bundle_id,
    );
    $form['details'] = array(
      '#type' => 'fieldset',
      '#title' => 'Record Type',
      '#collapsable' => FALSE,
      '#weight' => -100,
    );
    $form['details']['cv_name_shown'] = array(
      '#type' => 'item',
      '#title' => 'Vocabulary',
      '#markup' => $cvterm->cv_id->name,
    );
    $form['details']['term_name_shown'] = array(
      '#type' => 'item',
      '#title' => 'Term',
      '#markup' => $cvterm->name,
    );

    // If the entity doesn't exist then create one.
    if (!$entity) {
      $entity = entity_get_controller($cvterm->dbxref_id->db_id->name)->create(array('bundle' => $bundle_id));
      field_attach_form($cvterm->dbxref_id->db_id->name, $entity, $form, $form_state);

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add a new ' . $cvterm->name),
        '#name' => 'add_data',
        '#weight' => 1000
      );
    }
    else {
      field_attach_form($cvterm->dbxref_id->db_id->name, $entity, $form, $form_state);

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Update'),
        '#name' => 'update_data',
        '#weight' => 1000
      );
    }

    // The entity object must be added to the $form_state in order for
    // the Entity API to work.  It must have a key of the entity name.
    $form_state[$cvterm->dbxref_id->db_id->name] = $entity;
  }
  $form['#prefix'] = '<div id="tripal_data_form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * An Ajax callback for the tripal_data_form.
 */
function tripal_data_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;

}
/**
 * Implements hook_validate() for the tripal_data_form.
 */
function tripal_data_form_validate($form, &$form_state) {

  if ($form_state['clicked_button']['#name'] == 'add_data') {
    $tripal_data = (object) $form_state['values'];
    $entity_type = $form_state['values']['entity_type'];
    field_attach_form_validate($entity_type, $tripal_data, $form, $form_state);
  }
}

/**
 * Implements hook_submit() for the tripal_data_form.
 *
 */
function tripal_data_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'cancel') {
    if (array_key_exists('id', $form_state['values'])){
      $entity = $form_state['values']['entity'];
      $form_state['redirect'] = "data/$entity->id";
    }
    else {
      $form_state['redirect'] = "admin/structure/tripal_data";
    }
    return;
  }
  if ($form_state['clicked_button']['#name'] == 'select_cvterm') {
    // don't do anything, we just need to know what the term name is.
    $form_state['rebuild'] = TRUE;
  }
  if ($form_state['clicked_button']['#name'] == 'update_data' or
      $form_state['clicked_button']['#name'] == 'add_data') {
    // Use the Entity API to get the entity from the form state, then
    // attach the fields and save.
    $entity_type = $form_state['values']['entity_type'];
    $entity = entity_ui_controller($entity_type)->entityFormSubmitBuildEntity($form, $form_state);
    $entity->save();
    $form_state['redirect'] = "data/$entity->id";
  }
}
/**
 * Form API submit callback for the delete button.
 *
 * @todo Remove hard-coded path
 */
function tripal_data_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/tripal_datas/tripal_data/' . $form_state['tripal_data']->tripal_data_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a tripal_data.
 *
 * @param $tripal_data
 *   The tripal_data to delete
 *
 * @see confirm_form()
 */
function tripal_data_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;

  $form['#submit'][] = 'tripal_data_delete_form_submit';

  $form = confirm_form($form,
    t('Click the delete button below to confirm deltion of the record titled: %title', array('%title' => $entity->title)),
    'admin/content/tripal_data',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for tripal_data_delete_form
 */
function tripal_data_delete_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];

  tripal_data_delete($entity);

  drupal_set_message(t('The tripal_data %name has been deleted.', array('%name' => $tripal_data->name)));
  watchdog('tripal_data', 'Deleted tripal_data %name.', array('%name' => $tripal_data->name));

  $form_state['redirect'] = 'admin/content/tripal_datas';
}

/**
 * Menu callback to display an entity.
 *
 * As we load the entity for display, we're responsible for invoking a number
 * of hooks in their proper order.
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 */
function tripal_data_view($entity, $view_mode = 'full') {
  $controller = entity_get_controller($entity->type);
  $content = $controller->view(array($entity->id => $entity));
  drupal_set_title($entity->title);
  return $content;
}

/**
 * Menu title callback for showing individual entities
 */
function tripal_data_title(TripalData $entity){
  return $entity->title;
}
