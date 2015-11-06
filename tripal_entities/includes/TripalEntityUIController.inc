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

    $items['BioData/add'] = array(
      'title' => 'Add Biological Data',
      'page callback' => 'tripal_entities_add_page',
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'access callback'  => 'tripal_entities_entity_access',
      'access arguments' => array('edit'),
    );


    // Add a menu item for creating each bundle
    $bundles = array_keys($this->entityInfo['bundles']);
    foreach ($bundles as $bundle) {
      $matches = array();
      preg_match('/^dbxref_(.*?)$/', $bundle, $matches);
      $dbxref_id = $matches[1];
      $cvterm = chado_generate_var('cvterm', array('dbxref_id' => $dbxref_id));

      // Set a custom page for adding new tripal data entities.
      $items['BioData/add/' . $dbxref_id] = array(
        'title' => 'Add ' . ucfirst($cvterm->name),
        'description' => 'A ' . $cvterm->name . ' record.',
        'page callback'  => 'drupal_get_form',
        'page arguments' => array('tripal_entities_entity_form', 2),
        'access callback'  => 'tripal_entities_entity_access',
        'access arguments' => array('edit'),
        'file' =>  'includes/tripal_entities.entity_form.inc',
        'file path' => drupal_get_path('module', 'tripal_entities'),
      );
    }

    // Link for viewing a tripal data type.
    $items['BioData/' . $wildcard] = array(
      'title callback' => 'tripal_entities_entity_title',
      'title arguments' => array(1),
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['BioData/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    // 'Edit' tab for an individual entity page.
    $items['BioData/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entities_entity_form', NULL, 1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('edit', 1),
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -8,
    );

    // Menu item for deleting tripal data entities.
    $items['BioData/' . $wildcard . '/delete'] = array(
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

/**
 *
 * @param unknown $entity
 */
function tripal_entity_manage_fields($entity) {
  drupal_goto('admin/structure/BioData/manage/' . $entity->bundle . '/fields');
  return '';
}