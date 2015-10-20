<?php
/**
 * UI controller.
 */
class TripalEntityUIController extends EntityDefaultUIController {

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

    // The content menu.
    $items[$this->path] = array(
      'title' => 'Biological Data',
      'page callback' => 'tripal_entities_content_view',
      'file' =>  'includes/tripal_entities.admin.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'access arguments' => array('administer tripal data'),
      'type' => MENU_LOCAL_TASK,
    );

    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add new biological data',
      'description' => 'Add new biological data',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('tripal_entities_entity_form'),
      'access callback'  => 'tripal_entities_entity_access',
      'access arguments' => array('administer tripal data'),
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 20,
    );

     // Set a custom page for adding new tripal data entities.
    $items['data/add'] = array(
      'title' => 'Add Tripal data',
      'description' => 'Add a new tripal data record',
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('tripal_entities_entity_form'),
      'access callback'  => 'tripal_entities_entity_access',
      'access arguments' => array('edit'),
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
    );

    // Link for viewing a tripal data type.
    $items['data/' . $wildcard] = array(
      'title callback' => 'tripal_entities_entity_title',
      'title arguments' => array(1),
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['data/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,

    );

    // 'Edit' tab for an individual entity page.
    $items['data/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entities_entity_form', 1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('edit', 1),
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_LOCAL_TASK,
    );

    // Menu item for deleting tripal data entities.
    $items['data/' . $wildcard . '/delete'] = array(
      'title'  => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entities_entity_delete_form', 1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('edit', 1),
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_CALLBACK,
      'weight' => 10,
    );
    return $items;
  }
}
