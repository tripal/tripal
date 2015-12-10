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
      'title' => 'Tripal Content',
      'page callback' => 'tripal_entities_content_view',
      'file' =>  'includes/tripal_entities.admin.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'access arguments' => array('administer tripal data'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -9
    );

    $items['bio-data/add'] = array(
      'title' => 'Add Tripal Content',
      'page callback' => 'tripal_entities_add_page',
      'file' =>  'includes/tripal_entities.entity_form.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'access arguments' => array('administer tripal data'),
    );

    // Add a menu item for creating each bundle
    $bundles = array_keys($this->entityInfo['bundles']);
    foreach ($bundles as $bundle_name) {
      $matches = array();
      if (preg_match('/^bio-data_(.*?)$/', $bundle_name, $matches)) {
        $bundle = tripal_load_bundle_entity($bundle_name);
        // Get the term for this bundle
        $term = entity_load('TripalTerm', array('id' => $matches[1]));
        $term = reset($term);
        // Set a custom page for adding new tripal data entities.
        $items['bio-data/add/' . $term->id] = array(
          'title' => ucfirst($bundle->label),
          'description' => tripal_get_bundle_variable('description', $bundle->id, $term->definition),
          'page callback'  => 'drupal_get_form',
          'page arguments' => array('tripal_entities_entity_form', 2),
          'access callback'  => 'tripal_entities_entity_access',
          'access arguments' => array('edit'),
          'file' =>  'includes/tripal_entities.entity_form.inc',
          'file path' => drupal_get_path('module', 'tripal_entities'),
        );
      }
    }

    // Link for viewing a tripal data type.
    $items['bio-data/' . $wildcard] = array(
      'title callback' => 'tripal_entities_entity_title',
      'title arguments' => array(1),
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['bio-data/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'tripal_entities_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entities_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    // 'Edit' tab for an individual entity page.
    $items['bio-data/' . $wildcard . '/edit'] = array(
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
    $items['bio-data/' . $wildcard . '/delete'] = array(
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
  drupal_goto('admin/structure/bio-data/manage/' . $entity->bundle . '/fields');
  return '';
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
function tripal_entities_view_entity($entity, $view_mode = 'full') {
   $content = '';
   $controller = entity_get_controller($entity->type);
   $content = $controller->view(array($entity->id => $entity));
   drupal_set_title($entity->title);
   return $content;
 }