<?php
/**
 * UI controller.
 */
class ChadoDataUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    $items = array();

    // Set this on the object so classes that extend hook_menu() can use it.
    $this->id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';


    $items[$this->path] = array(
      'title' => 'Chado Data',
      'description' => 'Add edit and update chado data.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );

    // Change the overview menu type for the list of models.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Add an action link to the admin page for adding new data.
    $items[$this->path . '/add'] = array(
      'title' => 'Add Chado Data',
      'description' => 'Add a new chado data record',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('chado_data_form'),
      'access callback'  => 'chado_data_access',
      'access arguments' => array('edit'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 20,
    );

    // Set a custom page for adding new chado data entities.
    $items['data/add'] = array(
      'title' => 'Add Chado data',
      'description' => 'Add a new chado data record',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('chado_data_form'),
      'access callback'  => 'chado_data_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
    );

    // Link for viewing a chado data type.
    $items['data/' . $wildcard] = array(
      'title callback' => 'chado_data_title',
      'title arguments' => array(1),
      'page callback' => 'chado_data_view',
      'page arguments' => array(1),
      'access callback' => 'chado_data_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['data/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'chado_data_view',
      'page arguments' => array(1),
      'access callback' => 'chado_data_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,

    );

    // 'Edit' tab for an individual entity page.
    $items['data/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('chado_data_form', 1),
      'access callback' => 'chado_data_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_LOCAL_TASK,

    );

    // Menu item for deleting chado data entities.
    $items['data/' . $wildcard . '/delete'] = array(
      'title'  => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('chado_data_delete_form', 1),
      'access callback' => 'chado_data_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_CALLBACK,

      'weight' => 10,
    );
    return $items;
  }

}
/**
 * Determines whether the given user has access to a chado data entity.
 *
 * @param $op
 *   The operation being performed. One of 'view', 'update', 'create', 'delete'
 *   or just 'edit' (being the same as 'create' or 'update').
 * @param $entity
 *   Optionally a chado data entity or a chado data type to check access for.
 *   If nothing is given, access for all types is determined.
 * @param $account
 *   The user to check for. Leave it to NULL to check for the global user.
 * @return boolean
 *   Whether access is allowed or not.
 */
function chado_data_access($op, $entity = NULL, $account = NULL) {
  if (user_access('administer chado data', $account)) {
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
function chado_data_form($form, &$form_state, $entity = NULL) {

  // Set the defaults.
  $cv_id = NULL;
  $term_name = NULL;
  $cvterm = NULL;

  // Set defaults if an entity was provided.
  if ($entity) {
    drupal_set_title('Edit ' . $entity->title);
    $entity_id = $entity->entity_id;
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

  // Let the user select the vocabulary and chado_data but only if they haven't
  // already selected a chado_data.
  $cvs = tripal_get_cv_select_options();
  if (!$term_name) {
    $form['cv_id'] = array(
      '#type' => 'select',
      '#title' => t('Vocabulary'),
      '#options' => $cvs,
      '#required' => TRUE,
      '#description' => t('Select a vocabulary that contains the term for the type of data you want to add.'),
      '#default_value' => $cv_id,
      '#ajax' => array(
        'callback' => "chado_data_form_ajax_callback",
        'wrapper' => 'chado_data_form',
        'effect' => 'fade',
        'method' => 'replace'
      )
    );
  }

  // If we have a CV ID then we want to provide an autocomplete field
  if ($cv_id and !$term_name) {
    $form['cvterm_select']['term_name'] = array(
      '#title'       => t('Record Type'),
      '#type'        => 'textfield',
      '#description' => t("Enter the name of a term within the selected vocabulary for the record type you want to enter."),
      '#required'    => TRUE,
      '#default_value' => $term_name,
      '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
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
    $form['term_name'] = array(
      '#type'  => 'hidden',
      '#value' => $term_name,
    );
    $form['cvterm_id'] = array(
      '#type'  => 'hidden',
      '#value' => $cvterm->cvterm_id,
    );
    $form['type'] = array(
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

/*

    // Create the Chado data type entity.
    $data_type_entity = chado_data_type_create(array(
      'type' => $bundle_id,
      'label' => $cvterm->name,
      'module' => 'tripal_entities'
    ));

    $data_type_entity->save();
*/
/*
    // Drupal field types and settings:
    // https://www.drupal.org/node/1879542
    $field = array(
      'field_name' => 'feature__name',
      'type' => 'text',
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'tripal_entities_storage'
      ),
    );
    field_create_field($field);
    $field_instance = array(
      'field_name' => 'feature__name',
      'label' => 'Name',
      'widget' => array(
        'type' => 'text_textfield'
      ),
      'entity_type' => 'chado_data',
      'required' => 'true',
      'settings' => array(
        'max_length' => 255
      ),
      'bundle' => $bundle_id,
    );
    field_create_instance($field_instance);
    $field = array(
      'field_name' => 'feature__uniquename',
      'type' => 'text',
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
        'type' => 'tripal_entities_storage'
      ),
    );
    field_create_field($field);
    $field_instance = array(
      'field_name' => 'feature__uniquename',
      'label' => 'Unique Name',
      'widget' => array(
       'type' => 'text_textfield'
      ),
      'entity_type' => 'chado_data',
      'required' => 'true',
      'settings' => array(
       'max_length' => 255
      ),
      'bundle' => $bundle_id,
    );
    field_create_instance($field_instance);
    $field = array(
      'field_name' => 'feature__organism_id',
      'type' => 'organism_id',
      'cardinality' => 1,
      'locked' => TRUE,
      'storage' => array(
       'type' => 'tripal_entities_storage'
      ),
    );
    field_create_field($field);
    $field_instance = array(
      'field_name' => 'feature__organism_id',
      'label' => 'Organism',
      'entity_type' => 'chado_data',
      'required' => 'true',
      'settings' => array(),
      'bundle' => $bundle_id,
    );
    field_create_instance($field_instance);
*/
    // If the entity doesn't exist then create one.
    if (!$entity) {
      $entity = entity_get_controller('chado_data')->create(array('type' => $bundle_id));
      field_attach_form('chado_data', $entity, $form, $form_state);

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add a new ' . $cvterm->name),
        '#name' => 'add_data',
        '#weight' => 1000
      );
    }
    else {
      field_attach_form('chado_data', $entity, $form, $form_state);

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Update'),
        '#name' => 'update_data',
        '#weight' => 1000
      );
    }
    // The entity object must be added to the $form_state in order for
    // the Entity API to work.  It must have a key of the entity name.
    $form_state['chado_data'] = $entity;
  }
  $form['#prefix'] = '<div id="chado_data_form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * An Ajax callback for the chado_data_form.
 */
function chado_data_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;

}
/**
 * Implements hook_validate() for the chado_data_form.
 */
function chado_data_form_validate($form, &$form_state) {

  if ($form_state['clicked_button']['#name'] == 'add_data') {
    $chado_data = (object) $form_state['values'];
    field_attach_form_validate('chado_data', $chado_data, $form, $form_state);
  }
}

/**
 * Implements hook_submit() for the chado_data_form.
 *
 */
function chado_data_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'cancel') {
    if (array_key_exists('entity_id', $form_state['values'])){
      $entity = $form_state['values']['entity'];
      $form_state['redirect'] = "data/$entity->entity_id";
    }
    else {
      $form_state['redirect'] = "admin/structure/chado_data";
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
    $entity = entity_ui_controller('chado_data')->entityFormSubmitBuildEntity($form, $form_state);
    $entity->save();
    $form_state['redirect'] = "data/$entity->entity_id";
  }
}
/**
 * Form API submit callback for the delete button.
 *
 * @todo Remove hard-coded path
 */
function chado_data_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/chado_datas/chado_data/' . $form_state['chado_data']->chado_data_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a chado_data.
 *
 * @param $chado_data
 *   The chado_data to delete
 *
 * @see confirm_form()
 */
function chado_data_delete_form($form, &$form_state, $chado_data) {
  $form_state['chado_data'] = $chado_data;

  $form['#submit'][] = 'chado_data_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete chado_data %name?', array('%name' => $chado_data->name)),
    'admin/content/chado_datas/chado_data',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for chado_data_delete_form
 */
function chado_data_delete_form_submit($form, &$form_state) {
  $chado_data = $form_state['chado_data'];

  chado_data_delete($chado_data);

  drupal_set_message(t('The chado_data %name has been deleted.', array('%name' => $chado_data->name)));
  watchdog('chado_data', 'Deleted chado_data %name.', array('%name' => $chado_data->name));

  $form_state['redirect'] = 'admin/content/chado_datas';
}

/**
 * Sets the breadcrumb for administrative chado_data pages.
 */
function chado_data_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Chado Data'), 'admin/content/chado_data'),
  );

  drupal_set_breadcrumb($breadcrumb);
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
function chado_data_view($entity, $view_mode = 'full') {
  $controller = entity_get_controller('chado_data');
  $content = $controller->view(array($entity->entity_id => $entity));
  drupal_set_title($entity->title);
  return $content;
}

/**
 * Menu title callback for showing individual entities
 */
function chado_data_title(ChadoData $entity){
  return $entity->title;
}