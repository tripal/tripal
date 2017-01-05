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
      'page callback' => 'tripal_content_view',
      'file' =>  'includes/tripal.admin.inc',
      'file path' => drupal_get_path('module', 'tripal'),
      'access arguments' => array('administer tripal data'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -9
    );

    $items['bio_data/add'] = array(
      'title' => 'Add Tripal Content',
      'page callback' => 'tripal_add_page',
      'access arguments' => array('administer tripal data'),
    );

    // Add a menu item for creating each bundle
    $bundles = array_keys($this->entityInfo['bundles']);
    foreach ($bundles as $bundle_name) {
      $matches = array();
      if (preg_match('/^bio_data_(.*?)$/', $bundle_name, $matches)) {
        $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
        if (!$bundle) {
          throw new Exception(t("Cannot find bundle that matches: %bundle_name",
              array('%bundle_name' => $bundle_name)));
        }
        $term_id = $matches[1];
        // Get the term for this bundle
        $term = entity_load('TripalTerm', array('id' => $term_id));
        $term = reset($term);
        $default_description = $term->definition ? $term->definition : '';
        // Set a custom page for adding new tripal data entities.
        $items['bio_data/add/' . $term->id] = array(
          'title' => ucfirst($bundle->label),
          'description' => tripal_get_bundle_variable('description', $bundle->id, $default_description),
          'page callback'  => 'drupal_get_form',
          'page arguments' => array('tripal_entity_form', 2),
          'access callback'  => 'tripal_entity_access',
          'access arguments' => array('edit'),
        );
      }
    }

    // Link for viewing a tripal data type.
    $items['bio_data/' . $wildcard] = array(
      'title callback' => 'tripal_entity_title',
      'title arguments' => array(1),
      'page callback' => 'tripal_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    // 'View' tab for an individual entity page.
    $items['bio_data/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'page callback' => 'tripal_view_entity',
      'page arguments' => array(1),
      'access callback' => 'tripal_entity_access',
      'access arguments' => array('view', 1),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );

    // 'Edit' tab for an individual entity page.
    $items['bio_data/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entity_form', NULL, 1),
      'access callback' => 'tripal_entity_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_LOCAL_TASK,
      'weight' => -8,
    );
//     $items['bio_data/' . $wildcard . '/layout'] = array(
//       'title' => 'Layout',
//       'page callback' => 'drupal_goto',
//       'page arguments' => array(url("admin/structure/bio_data/manage/bio_data_$term_id/display")),
//       'access callback' => 'tripal_entity_access',
//       'access arguments' => array('admin', 1),
//       'type' => MENU_LOCAL_TASK,
//       'weight' => -8,
//     );
//     $items['bio_data/' . $wildcard . '/fields'] = array(
//       'title' => 'Fields',
//       'page callback' => 'drupal_goto',
//       'page arguments' => array(url("admin/structure/bio_data/manage/bio_data_$term_id/fields")),
//       'access callback' => 'tripal_entity_access',
//       'access arguments' => array('admin', 1),
//       'type' => MENU_LOCAL_TASK,
//       'weight' => -8,
//     );

    // Menu item for deleting tripal data entities.
    $items['bio_data/' . $wildcard . '/delete'] = array(
      'title'  => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entity_delete_form', 1),
      'access callback' => 'tripal_entity_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_CALLBACK,
      'weight' => 10,
    );
    return $items;
  }
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
function tripal_view_entity($entity, $view_mode = 'full') {
   $content = '';
   $controller = entity_get_controller($entity->type);
   $content = $controller->view(array($entity->id => $entity));
   drupal_set_title($entity->title);
   return $content;
 }

 /**
  * Provide a data listing for tripal entites (ie: biological data).
  *
  * This function is a callback in a menu item which is set in the
  * TripalEntityUIController class.
  */
 function tripal_content_view() {

   // Retrieve our data listing form and render it.
   $form = drupal_get_form('tripal_content_overview_form');
   $output = drupal_render($form);

   // Set the breadcrumb.
   $breadcrumb = array();
   $breadcrumb[] = l('Home', '<front>');
   $breadcrumb[] = l('Administration', 'admin');
   drupal_set_breadcrumb($breadcrumb);

   return $output;

 }

 /**
  * Display a listing of Tripal entities.
  *
  * @TODO Filters and bulk operations needed to be added to this form.
  *
  * @param array $form
  * @param array $form_state
  * @return
  *   A form array describing this listing to the Form API.
  */
 function tripal_content_overview_form($form, &$form_state) {
   $filter_status = '';
   $filter_type = '';
   if (array_key_exists('values', $form_state)) {
     if ($form_state['values']['status'] != '[any]') {
       $filter_status = preg_replace('/status-(\d+)/', '\1', $form_state['values']['status']);
     }
     if ($form_state['values']['type'] != '[any]') {
       $filter_type = $form_state['values']['type'];
     }
   }
   // Set the title to be informative (defaults to content for some reason).
   drupal_set_title('Tripal Content');

   $form['filter'] = array(
     '#type' => 'fieldset',
     '#title' => 'Filter',
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
   );
   $etypes = db_select('tripal_bundle', 'tb')
     ->fields('tb', array('name', 'label'))
     ->execute()
     ->fetchAllKeyed();
   $etypes = array('[any]' => 'any') +  $etypes;
   $form['filter']['type'] = array(
     '#type' => 'select',
     '#title' => 'Content Type',
     '#options' => $etypes,
   );
   $form['filter']['status'] = array(
     '#type' => 'select',
     '#title' => 'Status',
     '#options' => array(
       '[any]' => 'any',
       'status-1' => 'published',
       'status-0' => 'not published'
     ),
   );
   $form['filter']['filterbtn'] = array(
     '#type' => 'submit',
     '#value' => 'Filter',
   );

   $query = new TripalFieldQuery();
   $query->entityCondition('entity_type', 'TripalEntity');
   if ($filter_type) {
     $query->entityCondition('bundle', $filter_type);
     //$query->fieldCondition('content_type', 'content_type', 'organism');
     //$query->fieldOrderBy('content_type', 'content_type', 'DESC');
     //$query->fieldCondition('organism__genus', 'organism__genus', 'Oryza');
   }
   $query->range(0, 25);
   $results = $query->execute();

   $headers = array('Title', 'Vocabulary', 'Term', 'Author', 'Status', 'Updated', 'Operations');
   $rows = array();

   // For each entity retrieved add a row to the data listing.
   //while ($entity = $entities->fetchObject()) {
   if (!isset($results['TripalEntity'])) $results['TripalEntity'] = array();
   foreach ($results['TripalEntity'] as $entity_id => $stub) {
     $vocabulary = '';
     $term_name = '';

     // We don't need all of the attached fields for an entity so, we'll
     // not use the entity_load() function.  Instead just pull it from the
     // database table.
     $entity = db_select('tripal_entity', 'TE')
       ->fields('TE')
       ->condition('TE.id', $entity_id)
       ->execute()
       ->fetchObject();

     // Get the term
     $term = entity_load('TripalTerm', array('id' => $entity->term_id));
     $term = reset($term);
     if ($term) {
       $term_name = $term->name;
       $vocab = entity_load('TripalVocab', array('id' => $term->vocab_id));
       $vocab = reset($vocab);
       $vocabulary = $vocab->vocabulary;
     }

     // Retrieve details about the user who created this data.
     $author = user_load($entity->uid);

     // Add information to the table.
     $rows[] = array(
       l($entity->title, 'bio_data/' . $entity->id),
       $vocabulary,
       $term_name,
       l($author->name, 'user/' . $entity->uid),
       $entity->status == 1 ? 'published' : 'unpublished',
       format_date($entity->changed, 'short'),
       l('edit', 'bio_data/' . $entity->id . '/edit') . '&nbsp;&nbsp;' .
       l('delete', 'bio_data/' . $entity->id . '/delete')
     );
   }

   // If there are no entites created yet then add a message to the table to
   // provide guidance to administrators.
   if (empty($rows)) {
     $rows[] = array(
       array(
         'data' => t('No content can be found.'),
         'colspan' => 7
       )
     );
   }

   // Render the data listing.
   $table_vars = array(
     'header'     => $headers,
     'rows'       => $rows,
     'attributes' => array(),
     'sticky'     => TRUE,
     'caption'    => '',
     'colgroups'  => array(),
     'empty'      => '',
   );
   $form['results'] = array(
     '#type' => 'markup',
     '#markup' => theme('table', $table_vars),
   );

   return $form;
 }

 /**
  *
  */
 function tripal_content_overview_form_validate($form, &$form_state) {
 }

 /**
  *
  */
 function tripal_content_overview_form_submit($form, &$form_state) {
   // Always just rebuild the form on submit.  that will update the
   // result table using the filters provided.
   $form_state['rebuild'] = TRUE;
 }

 /**
  *
  */
 function tripal_entity_form($form, &$form_state, $term_id = '', $entity = NULL) {
   $bundle_name = 'bio_data_' . $term_id;

   // Add a vertical tabs element
   $form['entity_form_vtabs'] = array(
     '#type' => 'vertical_tabs',
     '#weight' => 999,
   );

   // If the entity doesn't exist then create one.
   if (!$entity) {
     $entity = entity_get_controller('TripalEntity')->create(array(
       'bundle' => $bundle_name,
       'term_id' => $term_id
     ));
     field_attach_form('TripalEntity', $entity, $form, $form_state);

     $form['add_button'] = array(
       '#type' => 'submit',
       '#value' => t('Save'),
       '#name' => 'add_data',
       '#weight' => 1000
     );
   }
   else {
     field_attach_form('TripalEntity', $entity, $form, $form_state);

     $form['update_button'] = array(
       '#type' => 'submit',
       '#value' => t('Update'),
       '#name' => 'update_data',
       '#weight' => 1000
     );

     // Put the delete button on the far-right so that it's harder
     // to accidentally click it.
     $form['delete_button'] = array(
       '#type' => 'submit',
       '#value' => t('Delete'),
       '#name' => 'delete_data',
       '#weight' => 1002,
       '#attributes' => array('style' => 'float: right')
     );
   }
   $form['cancel_button'] = array(
     '#type' => 'submit',
     '#value' => t('Cancel'),
     '#name' => 'cancel_data',
     '#weight' => 1001,
     '#limit_validation_errors' => array(array('')),
   );

   // The entity object must be added to the $form_state in order for
   // the Entity API to work. It must have a key of the entity name.
   $form_state['TripalEntity'] = $entity;

   $form['#prefix'] = "<div id='$bundle_name-entity-form'>";
   $form['#suffix'] = "</div>";
   $form['#submit'][] = 'tripal_entity_form_submit';
   return $form;
 }
 /**
  * An Ajax callback for the tripal_entity_form.
  */
function tripal_entity_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;
}

 /**
  * Implements hook_validate() for the tripal_entity_form.
  */
 function tripal_entity_form_validate($form, &$form_state) {

   // If the user is cancelling or deleting the entity then don't validate.
   if ($form_state['clicked_button']['#name'] =='cancel_data' or
       $form_state['clicked_button']['#name'] =='delete_data') {
     return;
   }
   // For adds and updates, perform validation.
   $entity = $form_state['TripalEntity'];
   field_attach_form_validate('TripalEntity', $entity, $form, $form_state);
 }

 /**
  * Implements hook_submit() for the tripal_entity_form.
  */
 function tripal_entity_form_submit($form, &$form_state) {
   $entity = $form_state['TripalEntity'];

   if ($form_state['clicked_button']['#name'] =='cancel_data') {
     if (property_exists($entity, 'id')) {
       $form_state['redirect'] = "bio_data/" . $entity->id;
     }
     else {
       $form_state['redirect'] = 'bio_data/add';
     }
     return;
   }

   if ($form_state['clicked_button']['#name'] =='delete_data') {
     $form_state['redirect'] = 'bio_data/' . $entity->id .'/delete';
     return;
   }
   // Allow the fields to perform actions prior to submit.
   $instances = field_info_instances('TripalEntity', $entity->bundle);
   $langcode = 'und';
   foreach ($instances as $field_name => $instance) {
     $entity_type = $instance['entity_type'];
     if($entity_type == 'TripalEntity') {
       foreach ($form[$field_name][$langcode] as $delta => $field_form) {
         if (!preg_match('/^\d+$/', $delta)) {
           continue;
         }
         $widget_type = $instance['widget']['type'];
         if (tripal_load_include_field_class($widget_type)) {
           $field = $field_form['#field'];
           $widget = new $widget_type($field, $instance);
           $widget->submit($form, $form_state, $entity_type, $entity, $langcode, $delta);
         }
       }
     }
   }

   $entityform = entity_ui_controller('TripalEntity')->entityFormSubmitBuildEntity($form, $form_state);
   if ($entityform->save()) {
     $form_state['redirect'] = "bio_data/" . $entity->id;
   }
   else {
     drupal_set_message('Cannot save entity', 'error');
   }

 }


 /**
  * Provides a list of TripalEntity types (bundles) for the user to add.
  *
  * This function is a callback in a menu item which is set in the
  * TripalEntityUIController class.
  */
 function tripal_add_page() {
   $item = menu_get_item();

   $content = system_admin_menu_block($item);

   // Bypass the node/add listing if only one content type is available.
   if (count($content) == 1) {
     $item = array_shift($content);
     drupal_goto($item['href']);
   }
   return theme('tripal_add_list', array('content' => $content));

 }

 /**
  * Returns HTML for a list of available node types for node creation.
  *
  * @param $variables
  *   An associative array containing:
  *   - content: An array of content types.
  *
  * @ingroup themeable
  */
 function theme_tripal_add_list($variables) {
   $content = $variables['content'];
   $output = '';

   if ($content) {
     $output = '<dl class="node-type-list">';
     foreach ($content as $item) {
       $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
       $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
     }
     $output .= '</dl>';
   }
   else {
     $output = tripal_set_message(
         t('This page is for adding Tripal content to your site. However, before you can add data you have to specify what types of data your site will support. For example, if you want to add genes to be displayed to users, you must first create a data type "gene".'),
         TRIPAL_INFO,
         array('return_html' => TRUE)
     );
     $output .= '<p>' . t('You have not created any biological data types yet. ' .
         'Go to the <a href="@create-content">data type creation page</a> to ' .
         'add a new data type.',
         array('@create-content' => url('admin/structure/bio_data/add'))) . '</p>';
   }
   return $output;
 }

 /**
  * Form callback: confirmation form for deleting a tripal_entity.
  *
  * @param $tripal_entity The
  *          tripal_entity to delete
  *
  * @see confirm_form()
  */
 function tripal_entity_delete_form($form, &$form_state, $entity) {
   $form_state['entity'] = $entity;
   $form['#submit'][] = 'tripal_entity_delete_form_submit';

   $form = confirm_form($form,
       t('Click the delete button below to confirm deletion of the record titled: %title',
           array('%title' => $entity->title)), 'admin/content/tripal_entity',
       '<p>' .t('This action cannot be undone.') .'</p>', t('Delete'), t('Cancel'), 'confirm');

   return $form;
 }

 /**
  * Submit callback for tripal_entity_delete_form
  */
 function tripal_entity_delete_form_submit($form, &$form_state) {
   $entity = $form_state['entity'];

   $entity_controller = new TripalEntityController($entity->type);

   if ($entity_controller->delete(array($entity->id))) {
     drupal_set_message(t('The record title "%name" has been deleted.', array('%name' => $entity->title)));
     $form_state['redirect'] = 'admin/content/bio_data';
   }
   else {
     drupal_set_message(t('The tripal_entity %name was not deleted.', array('%name' => $entity->title)), "error");
   }
 }
