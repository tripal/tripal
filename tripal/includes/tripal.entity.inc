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
    'uri callback' => 'tripal_entity_uri',

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
      'file' => 'includes/TripalEntityUIController.inc',
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

  // Search integration
  if (module_exists('search')) {
    $entities['TripalEntity']['view modes'] += array(
      'search_index' => array(
        'label' => t('Search index'),
        'custom settings' => FALSE,
      ),
      'search_result' => array(
        'label' => t('Search result highlighting input'),
        'custom settings' => FALSE,
      ),
    );
  }

  // The TripalBundle entity is used manage the bundle types.  The 'bundle of'
  // attribute links this to the TripalEntity and allows the UI provided
  // by the entity module to work for each TripalEntity bundle.
  //
  $entities['TripalBundle'] = array (
    'label' => 'Tripal Content Type',
    'entity class' => 'TripalBundle',
    'controller class' => 'TripalBundleController',
    'views controller class' => 'TripalBundleViewsController',
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
 * Implements the Entity URI callback function.
 */
function tripal_entity_uri($entity) {
  return array(
    'path' => 'bio-data/' . $entity->id,
    'options' => array(),
  );
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
            'manage tripal content types'
          )
        )
      );
    }
  }
}

/**
 * Implements hook_entity_property_info_alter().
 *
 * For some reason not all our TripalFields end up in the properties field for
 * each bundle. This becomes a problem with Search API integration because only
 * fields listed in the properties for a bundle are available to be indexed.
 * Thus we are altering the property info to add any fields attached to
 * TripalEntities which may have been missed.
 *
 * Furthermore, there are some pecularities with how TripalFields store their
 * value that causes the default getter callback difficulties in some edge cases.
 * Thus we are overriding that function below.
 */
function tripal_entity_property_info_alter(&$info) {

  // Sometimes this function is called when there are no Tripal Entities.
  // Don't bother to do anything in this case.
  if (!isset($info['TripalEntity']['bundles'])) { return TRUE; }

  // For each Tripal Content Type, we want to ensure all attached fields
  // are added to the bundle properties.
  foreach ($info['TripalEntity']['bundles'] as $bundle_name => $bundle) {

    // Retrieve information for all fields attached to this Tripal Content Type.
    $fields = field_info_instances('TripalEntity', $bundle_name);
    foreach ($fields as $field_name => $field_info) {

      // If there is a field attached to the current Tripal Content Type that
      // is not listed in properties, then add it. We use the full defaults here
      // just in case it's not a TripalField or ChadoField.
      if (!isset($info['TripalEntity']['bundles'][$bundle_name]['properties'][$field_name])) {
        $info['TripalEntity']['bundles'][$bundle_name]['properties'][$field_name] = array(
          'label' => $field_info['label'],
          'type' => 'text',
          'description' => $field_info['description'],
          'getter callback' => 'entity_metadata_field_property_get',
          'setter callback' => 'entity_metadata_field_property_set',
          'access callback' => 'entity_metadata_field_access_callback',
          'query callback' => 'entity_metadata_field_query',
          'translatable' => FALSE,
          'field' => TRUE,
          'required' => $field_info['required'],
        );
      }

      // Now, if it's a TripalField or a ChadoField, then we want to use a custom
      // getter callback in order to ensure values are retrieved properly.
      // ASSUMPTION: All TripalFields and ChadoFields have an ontology term
      // defining them.
      if (isset($field_info['settings']['term_accession'])) {
        $info['TripalEntity']['bundles'][$bundle_name]['properties'][$field_name]['getter callback'] = 'tripal_field_property_get';
      }
    }
  }
}

/**
 * Callback for getting TripalField and ChadoField property values.
 *
 * This function retrieves the value from a field. Since the value has already
 * been set by the Tripal/ChadoField class at this point, it should just be a
 * matter of grabbing the value.
 *
 * @param $entity
 *   The fully-loaded entity object to be indexed.
 * @param $options
 *   Options that can be ued when retrieving the value.
 * @param $field_name
 *   The machine name of the field we want to retrieve.
 * @param $entity_type
 *   The type of entity (ie: TripalEntity).
 *
 * @return
 *   The rendered value of the field specified by $field_name.
 */
function tripal_field_property_get($entity, array $options, $field_name, $entity_type, $info) {

  // Retrieve information for the field.
  $field = field_info_field($field_name);

  // Retrieve the language code.
  $langcode = isset($options['language']) ? $options['language']->language : LANGUAGE_NONE;
  $langcode = entity_metadata_field_get_language($entity_type, $entity, $field, $langcode, TRUE);

  $values = array();
  if (isset($entity->{$field_name}[$langcode])) {
    // For each value of the field... (this will be multiple if cardinality is > 1).
    foreach ($entity->{$field_name}[$langcode] as $delta => $data) {

      // All Tripal/ChadoFields should have a value key. Only the information
      // stored in this value key should be displayed on the page, available
      // via web services or indexed for searching. This is there is no value
      // key, we will not index anything.
      if (!isset($data['value'])) {
        return NULL;
      }

      // Sometimes TripalFields may return multiple pieces of information in the
      // value field. In this case, the key should be an ontology term describing
      // what each piece of data is and the value should be the data.
      if (is_array($data['value'])) {

        // Just include all the pieces of information seperated by spaces
        // so they are tokenized out later on.
        $tmp = $data['value'];
        if (isset($tmp['entity'])) { unset($tmp['entity']); }
        foreach ($tmp as $k => $v) { $tmp[$k] = strip_tags($v); }
        $curr_val = implode(' ', $tmp);
      }
      else {

        // Otherwise, assume the value is a single piece of information
        // and add that directly to be indexed.
        $curr_val = strip_tags($data['value']);

        // Ensure that we have a clean boolean data type.
        if ($info['type'] == 'boolean' || $info['type'] == 'list<boolean>') {
          $curr_val = (boolean) $curr_val;
        }
      }

      // Only add the current value if it's not empty.
      if (!empty(trim($curr_val))) {
        $values[$delta] = $curr_val;
      }
    }
  }

  // For an empty single-valued field, we have to return NULL.
  return $field['cardinality'] == 1 ? ($values ? reset($values) : NULL) : $values;
}

/**
 * Checks access permissions for a given entity.
 *
 * This function is set for TripalEntity access checking in the
 * tripal_entity_info() under the 'access callback' element.
 *
 * @param $op
 *   The operation. One of: create, view, edit, delete.
 * @param $entity
 *   The entity to check access for.
 * @param $account
 *   The user account.
 * @param $entity_type
 *   The type of entity (will always be TripalEntity).
 */
function tripal_entity_access($op, $entity = NULL, $account = NULL, $entity_type = NULL) {
  global $user;

  if ($entity) {
    $bundle_name = $entity->bundle;
  }
  else {
    return FALSE;
  }

  if (!isset($account)) {
    $account = $user;
  }

  if (!$entity_type) {
    $entity_type = $entity->type;
  }

  // See if other modules want to adust permissions.
  $results = module_invoke_all($entity_type . '_access', $entity, $op, $account);
  if (in_array(TRUE, $results)) {
    return TRUE;
  }

  switch ($op) {
    case 'create':
      return user_access('create ' . $bundle_name, $account);
    case 'view':
      return user_access('view ' . $bundle_name, $account);
    case 'edit':
      return user_access('edit ' . $bundle_name, $account);
    case 'delete':
      return user_access('delete ' . $bundle_name, $account);
  }

  return FALSE;
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
        $class = '';
        if (array_key_exists('auto_attach', $instance['settings']) and
            $instance['settings']['auto_attach'] == FALSE and
            $entity->{$child_name}['#processed'] == FALSE) {
          // If the field is empty then try to use ajax to load it.
          $items = field_get_items('TripalEntity', $entity, $child_name);
          if (count($items) == 0 or empty($items[0]['value'])) {
            $class = 'class="tripal-entity-unattached"';
          }
        }
        $entity->content[$child_name]['#prefix'] .= '<div id="tripal-entity-' . $entity->id . '--' . $child_name . '" ' . $class . '>';
        $entity->content[$child_name]['#suffix'] .= '</div>';
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
