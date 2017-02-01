<?php


/**
 * Implement hook_entity_info().
 *
 * See the following for documentaiton of this type setup for Entities:
 *
 * http://www.bluespark.com/blog/drupal-entities-part-3-programming-hello-drupal-entity
 * http://dikini.net/31.08.2010/entities_bundles_fields_and_field_instances
 */
function tripal_entity_info() {
  $entities = array();

  // The TripalVocab entity is meant to house vocabularies.  It is these
  // vocabs that are used by the TripalTerm entities.  The storage backend
  // is responsible for setting the values of this entity.
  //
  $entities['TripalVocab'] = array(
    // A human readable label to identify our entity.
    'label' => 'Controlled Vocabulary',
    'plural label' => 'Controlled Vocabularies',

    // The entity class and controller class extend the classes provided by the
    // Entity API.
    'entity class' => 'TripalVocab',
    'controller class' => 'TripalVocabController',

    // Adds Views integration for this entity.
    'views controller class' => 'TripalVocabViewsController',

    // The table for this entity defined in hook_schema()
    'base table' => 'tripal_vocab',

    // If fieldable == FALSE, we can't attach fields.
    'fieldable' => TRUE,

    // entity_keys tells the controller what database fields are used for key
    // functions. It is not required if we don't have bundles or revisions.
    // Here we do not support a revision, so that entity key is omitted.
    'entity keys' => array (
      'id' => 'id',
    ),

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => TRUE,

    // Caching of fields
    'field cache' => TRUE,

    // This entity doesn't support bundles.
    'bundles' => array (),

    'view modes' => array (
      'full' => array (
        'label' => t ('Full content'),
        'custom settings' => FALSE
      ),
      'teaser' => array (
        'label' => t ('Teaser'),
        'custom settings' => TRUE
      ),
    ),
  );

  //
  // The TripalTerm entity is meant to house vocabulary terms.  It is these
  // terms that are used by the TripalEntity entities.  The storage backend
  // is responsible for setting the values of this entity.
  //
  $entities['TripalTerm'] = array(
    // A human readable label to identify our entity.
    'label' => 'Controlled Vocabulary Term',
    'plural label' => 'Controlled Vocabulary Terms',

    // The entity class and controller class extend the classes provided by the
    // Entity API.
    'entity class' => 'TripalTerm',
    'controller class' => 'TripalTermController',

    // Adds Views integration for this entity.
    'views controller class' => 'TripalTermViewsController',

    // The table for this entity defined in hook_schema()
    'base table' => 'tripal_term',

    // If fieldable == FALSE, we can't attach fields.
    'fieldable' => TRUE,

    // entity_keys tells the controller what database fields are used for key
    // functions. It is not required if we don't have bundles or revisions.
    // Here we do not support a revision, so that entity key is omitted.
    'entity keys' => array (
      'id' => 'id',
    ),

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => FALSE,

    // This entity doesn't support bundles.
    'bundles' => array (),

    'view modes' => array (
      'full' => array (
        'label' => t ('Full content'),
        'custom settings' => FALSE
      ),
      'teaser' => array (
        'label' => t ('Teaser'),
        'custom settings' => TRUE
      ),
    ),
  );

  //
  // The TripalEntity is used for all data. It links data from a storage
  // back-end to a TripalTerm entity.
  //
  $entities['TripalEntity'] = array (
    // A human readable label to identify our entity.
    'label' => 'Tripal Content',
    'plural label' => 'Tripal Content',

    // The entity class and controller class extend the classes provided by the
    // Entity API.
    'entity class' => 'TripalEntity',
    'controller class' => 'TripalEntityController',

    // Adds Views integration for this entity.
    'views controller class' => 'TripalEntityViewsController',

    // The table for this entity defined in hook_schema()
    'base table' => 'tripal_entity',

    // Returns the uri elements of an entity.
    'uri callback' => 'tripal_vocbulary_term_uri',

    // IF fieldable == FALSE, we can't attach fields.
    'fieldable' => TRUE,

    // entity_keys tells the controller what database fields are used for key
    // functions. It is not required if we don't have bundles or revisions.
    // Here we do not support a revision, so that entity key is omitted.
    'entity keys' => array (
      'id' => 'id',
      'bundle' => 'bundle'
    ),
    'bundle keys' => array (
      'bundle' => 'name'
    ),

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => TRUE,

    // Caching of fields
    'field cache' => TRUE,

    // Bundles are added dynamically below.
    'bundles' => array (),

    'label callback' => 'tripal_entity_label',

    // The information below is used by the TripalEntityUIController
    // (which extends the EntityDefaultUIController). The admin_ui
    // key here is mean to appear on the 'Find Content' page of the
    // administrative menu.
    'admin ui' => array (
      'path' => 'admin/content/bio_data',
      'controller class' => 'TripalEntityUIController',
      'menu wildcard' => '%TripalEntity',
      'file' => 'includes/TripalEntityUIController.inc'
    ),
    'view modes' => array (
      'full' => array (
        'label' => t ('Full content'),
        'custom settings' => FALSE
      ),
      'teaser' => array (
        'label' => t ('Teaser'),
        'custom settings' => TRUE
      )
    )
  );

  //
  // The TripalBundle entity is used manage the bundle types.  The 'bundle of'
  // attribute links this to the TripalEntity and allows the UI provided
  // by the entity module to work for each TripalEntity bundle.
  //
  $entities['TripalBundle'] = array (
    'label' => 'Tripal Content Type',
    'entity class' => 'TripalBundle',
    'controller class' => 'TripalBundleController',
    'base table' => 'tripal_bundle',
    'fieldable' => FALSE,
    'bundle of' => 'TripalEntity',
    'exportable' => FALSE,
    'entity keys' => array (
      'id' => 'id',
      'name' => 'name',
      'label' => 'label'
    ),
    'access callback' => 'tripal_bundle_access',
    'module' => 'tripal',
    // Enable the entity API's admin UI.
    'admin ui' => array (
      'path' => 'admin/structure/bio_data',
      'controller class' => 'TripalBundleUIController',
      'file' => 'includes/TripalBundleUIController.inc',
      'menu wildcard' => '%TripalBundle',
    )
  );

  return $entities;
}

/**
 * Implements hook_entities_info_alter().
 *
 * Add in the bundles (entity types) to the TripalEntity entity.
 */
function tripal_entity_info_alter(&$entity_info){

  if (array_key_exists('TripalEntity', $entity_info)) {
    // Dynamically add in the bundles. Bundles are alternative groups of fields
    // or configuration associated with an entity type .We want to dynamically
    // add the bundles to the entity.
    $bundles = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->execute();
    while ($bundle = $bundles->fetchObject()) {
      $bundle_name = $bundle->name;
      $term_id = $bundle->term_id;
      $term = entity_load('TripalTerm', array('id' => $term_id));
      $term = reset($term);
      $label = preg_replace('/_/', ' ', ucwords($term->name));

      $entity_info['TripalEntity']['bundles'][$bundle_name] = array (
        'label' => $label,
        'admin' => array (
          'path' => 'admin/structure/bio_data/manage/%TripalBundle',
          'real path' => 'admin/structure/bio_data/manage/' . $bundle_name,
          'bundle argument' => 4,
          'access arguments' => array (
            'administer tripal data types'
          )
        )
      );
    }
  }
}

/**
 * Checks access permissions for a given entity.
 */
function tripal_entity_access($entity) {
  // TODO: need to implement this function.
  return TRUE;
}

function tripal_form_tripal_entity_form_alter(&$form, &$form_state, $form_id) {

}

/**
 * Implements hook_entity_view.
 *
 * Here we want to overwite unattached fields with a div box that will be
 * recognized by JavaScript that will then use AJAX to load the field.
 *
 * The tripal_ajax_attach_field() function is called by an AJAX call to
 * retrieve the field.
 */
function tripal_entity_view($entity, $type, $view_mode, $langcode) {
  if ($type == 'TripalEntity') {
    foreach (element_children($entity->content) as $child_name) {

      // Initailize the prefix and suffix for this field.
      if (!array_key_exists('#prefix', $entity->content[$child_name])) {
        $entity->content[$child_name]['#prefix'] = '';
      }
      if (!array_key_exists('#suffix', $entity->content[$child_name])) {
        $entity->content[$child_name]['#suffix'] = '';
      }

      // Surround the field with a <div> box for AJAX loading if this
      // field is unattached.  this will allow JS code to automatically load
      // the field.
      $instance = field_info_instance('TripalEntity', $child_name, $entity->bundle);
      if ($instance and array_key_exists('settings', $instance)) {
        if (array_key_exists('auto_attach', $instance['settings']) and
            $instance['settings']['auto_attach'] == FALSE) {
          $entity->content[$child_name]['#prefix'] .= '<div id="tripal-entity-' . $entity->id . '--' . $child_name . '" class="tripal-entity-unattached">';
          $entity->content[$child_name]['#suffix'] .= '</div>';
        }
      }
    }
  }
}

/**
 * Responds to an AJAX call for populating a field.
 *
 * @param $id
 *   The ID of the HTML div box where the field is housed. The ID contains the
 *   entity ID and field name.
 */
function tripal_ajax_attach_field($id) {

  $matches = array();
  if (preg_match('/^tripal-entity-(\d+)--(.+)$/', $id, $matches)) {
    $entity_id = $matches[1];
    $field_name = $matches[2];
    $field = field_info_field($field_name);
    $result = tripal_load_entity('TripalEntity', array($entity_id), FALSE, array($field['id']));
    reset($result);
    $entity = $result[$entity_id];

    // Get the element render array for this field and turn off the label
    // display. It's already on the page.  We need to get the display from the
    // instance and pass it into the field_view_field. Otherwise it uses the
    // instance default display settings. Not sure why it does this. It needs
    // more investigation.
    $instance = field_info_instance('TripalEntity', $field_name, $entity->bundle);
    $element = field_view_field('TripalEntity', $entity, $field_name, $instance['display']['default']);
    $element['#label_display'] = 'hidden';

    $content = drupal_render($element);
    return drupal_json_output(array(
      'id' => $id,
      'content' => $content
    ));
  }
}
