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
  $entities = [];

  // The TripalVocab entity is meant to house vocabularies.  It is these
  // vocabs that are used by the TripalTerm entities.  The storage backend
  // is responsible for setting the values of this entity.
  //
  $entities['TripalVocab'] = [
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
    'entity keys' => [
      'id' => 'id',
    ],

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => TRUE,

    // Caching of fields
    'field cache' => TRUE,

    // This entity doesn't support bundles.
    'bundles' => [],

    'view modes' => [
      'full' => [
        'label' => t('Full content'),
        'custom settings' => FALSE,
      ],
      'teaser' => [
        'label' => t('Teaser'),
        'custom settings' => TRUE,
      ],
    ],
  ];

  //
  // The TripalTerm entity is meant to house vocabulary terms.  It is these
  // terms that are used by the TripalEntity entities.  The storage backend
  // is responsible for setting the values of this entity.
  //
  $entities['TripalTerm'] = [
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
    'entity keys' => [
      'id' => 'id',
    ],

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => FALSE,

    // This entity doesn't support bundles.
    'bundles' => [],

    'view modes' => [
      'full' => [
        'label' => t('Full content'),
        'custom settings' => FALSE,
      ],
      'teaser' => [
        'label' => t('Teaser'),
        'custom settings' => TRUE,
      ],
    ],
  ];

  //
  // The TripalEntity is used for all data. It links data from a storage
  // back-end to a TripalTerm entity.
  //
  $entities['TripalEntity'] = [
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
    'entity keys' => [
      'id' => 'id',
      'bundle' => 'bundle',
    ],
    'bundle keys' => [
      'bundle' => 'name',
    ],

    // Callback function for access to this entity.
    'access callback' => 'tripal_entity_access',

    // FALSE disables caching. Caching functionality is handled by Drupal core.
    'static cache' => TRUE,

    // Caching of fields
    'field cache' => TRUE,

    // Bundles are added dynamically below.
    'bundles' => [],

    'label callback' => 'tripal_entity_label',

    // The information below is used by the TripalEntityUIController
    // (which extends the EntityDefaultUIController). The admin_ui
    // key here is mean to appear on the 'Find Content' page of the
    // administrative menu.
    'admin ui' => [
      'path' => 'admin/content/bio_data',
      'controller class' => 'TripalEntityUIController',
      'menu wildcard' => '%TripalEntity',
      'file' => 'includes/TripalEntityUIController.inc',
    ],
    'view modes' => [
      'full' => [
        'label' => t('Full content'),
        'custom settings' => FALSE,
      ],
      'teaser' => [
        'label' => t('Teaser'),
        'custom settings' => TRUE,
      ],
    ],
  ];

  // Search integration
  if (module_exists('search')) {
    $entities['TripalEntity']['view modes'] += [
      'search_index' => [
        'label' => t('Search index'),
        'custom settings' => FALSE,
      ],
      'search_result' => [
        'label' => t('Search result highlighting input'),
        'custom settings' => FALSE,
      ],
    ];
  }

  // The TripalBundle entity is used manage the bundle types.  The 'bundle of'
  // attribute links this to the TripalEntity and allows the UI provided
  // by the entity module to work for each TripalEntity bundle.
  //
  $entities['TripalBundle'] = [
    'label' => 'Tripal Content Type',
    'entity class' => 'TripalBundle',
    'controller class' => 'TripalBundleController',
    'views controller class' => 'TripalBundleViewsController',
    'base table' => 'tripal_bundle',
    'fieldable' => FALSE,
    'bundle of' => 'TripalEntity',
    'exportable' => FALSE,
    'entity keys' => [
      'id' => 'id',
      'name' => 'name',
      'label' => 'label',
    ],
    'access callback' => 'tripal_bundle_access',
    'module' => 'tripal',
    // Enable the entity API's admin UI.
    'admin ui' => [
      'path' => 'admin/structure/bio_data',
      'controller class' => 'TripalBundleUIController',
      'file' => 'includes/TripalBundleUIController.inc',
      'menu wildcard' => '%TripalBundle',
    ],
  ];

  return $entities;
}

/**
 * Implements the Entity URI callback function.
 */
function tripal_entity_uri($entity) {
  return [
    'path' => 'bio_data/' . $entity->id,
    'options' => [],
  ];
}

/**
 * Implements hook_entities_info_alter().
 *
 * Add in the bundles (entity types) to the TripalEntity entity.
 */
function tripal_entity_info_alter(&$entity_info) {

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
      $term = entity_load('TripalTerm', ['id' => $term_id]);
      $term = reset($term);
      $label = preg_replace('/_/', ' ', ucwords($term->name));

      $entity_info['TripalEntity']['bundles'][$bundle_name] = [
        'label' => $label,
        'admin' => [
          'path' => 'admin/structure/bio_data/manage/%TripalBundle',
          'real path' => 'admin/structure/bio_data/manage/' . $bundle_name,
          'bundle argument' => 4,
          'access arguments' => [
            'manage tripal content types',
          ],
        ],
      ];
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
 * value that causes the default getter callback difficulties in some edge
 * cases. Thus we are overriding that function below.
 */
function tripal_entity_property_info_alter(&$info) {

  // Sometimes this function is called when there are no Tripal Entities.
  // Don't bother to do anything in this case.
  if (!isset($info['TripalEntity']['bundles'])) {
    return TRUE;
  }

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
        $info['TripalEntity']['bundles'][$bundle_name]['properties'][$field_name] = [
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
        ];
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

  $values = [];
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
        if (isset($tmp['entity'])) {
          unset($tmp['entity']);
        }
        foreach ($tmp as $k => $v) {
          $tmp[$k] = strip_tags($v);
        }
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
 *   The operation. One of: create, view, edit, unpublish, delete.
 * @param $entity
 *   The entity to check access for.
 * @param $account
 *   The user account.
 * @param $entity_type
 *   The type of entity (will always be TripalEntity).
 */
function tripal_entity_access($op, $entity = NULL, $account = NULL, $entity_type = NULL) {
  global $user;
  $cache = &drupal_static(__FUNCTION__, NULL);

  if (!isset($account)) {
    $account = $user;
  }

  if (is_object($entity)) {
    $bundle_name = $entity->bundle;
  }
  elseif (intval($entity) !== 0) {
    if (!isset($cache)) {
      $cache = cache_get("tripal_entity_access_cache");
      if (isset($cache->data)) {
        $cache = $cache->data;
      }
    }

    if (empty($cache)) {
      $cache = [];
    }

    if (isset($cache[$entity])) {
      $bundle_name = $cache[$entity];
    }
    else {
      $sql = 'SELECT {bundle} FROM tripal_entity WHERE id = :id';
      $bundle_name = db_query($sql, [':id' => $entity])->fetchField();
      $cache[$entity] = $bundle_name;
      cache_set("tripal_entity_access_cache", $cache);
    }
  }
  else {
    return FALSE;
  }

  if (!$entity_type) {
    if (is_object($entity)) {
      $entity_type = $entity->type;
    }
    else {
      $entity_type = 'TripalEntity';
    }
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
    case 'unpublish':
      return user_access('unpublish ' . $bundle_name, $account);
    case 'delete':
      return user_access('delete ' . $bundle_name, $account);
  }

  return FALSE;
}

/**
 * Implements hook_entity_prepare_view
 *
 * This function is called before building the content array for an entity. It
 * allows us to load any unattached fields if AJAX is turned off.
 */
function tripal_entity_prepare_view($entities, $type, $langcode) {

  // This is for only TripalEntity content types.
  if ($type != 'TripalEntity') {
    return;
  }

  // Iterate through the entities and instances and if AJAX loading is turned
  // off then we need to load those fields that were not auto attached.
  foreach ($entities as $entity_id => &$entity) {
    $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
    $use_ajax = tripal_get_bundle_variable('ajax_field', $bundle->id);
    $instances = field_info_instances('TripalEntity', $entity->bundle);
    foreach ($instances as $instance) {
      $field_name = $instance['field_name'];
      $field = field_info_field($field_name);

      $auto_attach = array_key_exists('auto_attach', $instance['settings']) ? $instance['settings']['auto_attach'] : TRUE;
      $processed = $entity->{$field_name}['#processed'];

      // If the field is not ajax loadable and the field is not auto attached
      // then we need to add the value.
      if (!$use_ajax and $auto_attach == FALSE and $processed == FALSE) {
        $temp = tripal_load_entity('TripalEntity', [$entity_id], TRUE, [$field['id']]);
        reset($temp);
        $entity->{$field_name} = $temp[$entity_id]->{$field_name};
        $items = field_get_items('TripalEntity', $entity, $field_name);
      }
    }
  }
}

/**
 * Implements hook_entity_view.
 *
 * Here we want to overwrite unattached fields with a div box that will be
 * recognized by JavaScript that will then use AJAX to load the field.
 * The tripal_ajax_attach_field() function is called by an AJAX call to
 * retrieve the field.  We also remove empty fields that were auto attached.
 */
function tripal_entity_view($entity, $type, $view_mode, $langcode) {

  // This is for only TripalEntity content types.
  if ($type != 'TripalEntity') {
    return;
  }

  $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
  $hide_empty = tripal_get_bundle_variable('hide_empty_field', $bundle->id);
  $use_ajax = tripal_get_bundle_variable('ajax_field', $bundle->id);

  // Iterate through the fields attached to this entity and add IDs to them
  // as well as some classes for ajax loading.
  foreach (element_children($entity->content) as $child_name) {

    // Surround the field with a <div> box for AJAX loading if this
    // field is unattached.  this will allow JS code to automatically load
    // the field.
    $instance = field_info_instance('TripalEntity', $child_name, $entity->bundle);
    if (!$instance) {
      continue;
    }
    $field = field_info_field($instance['field_name']);

    // Check if this is an AJAX loadable field, and if so set the class.
    $class = '';
    $auto_attach = array_key_exists('auto_attach', $instance['settings']) ? $instance['settings']['auto_attach'] : TRUE;
    $processed = $entity->{$child_name}['#processed'];
    if ($use_ajax and $auto_attach == FALSE and $processed == FALSE) {
      $class = 'class="tripal-entity-unattached"';
    }

    // Set the prefix and suffix.
    if (!array_key_exists('#prefix', $entity->content[$child_name])) {
      $entity->content[$child_name]['#prefix'] = '';
    }
    if (!array_key_exists('#suffix', $entity->content[$child_name])) {
      $entity->content[$child_name]['#suffix'] = '';
    }
    $entity->content[$child_name]['#prefix'] .= '<div id="tripal-entity-' . $entity->id . '--' . $child_name . '" ' . $class . '>';
    $entity->content[$child_name]['#suffix'] .= '</div>';

    // Remove any auto attached fields if they are empty.
    if ($hide_empty and $processed) {
      $items = field_get_items('TripalEntity', $entity, $child_name);
      if (tripal_field_is_empty($field, $items)) {
        unset($entity->content[$child_name]);
      }
    }
  }

  // Add some settings for AJAX to deal with fields.
  $settings = [
    'tripal_display' => [
      'hide_empty' => $hide_empty,
      'use_ajax' => $use_ajax,
    ],
  ];
  drupal_add_js($settings, 'setting');
}

/**
 * Responds to an AJAX call for populating a field.
 *
 * @param $id
 *   The ID of the HTML div box where the field is housed. The ID contains the
 *   entity ID and field name.
 */
function tripal_ajax_attach_field($id) {

  $matches = [];
  if (preg_match('/^tripal-entity-(\d+)--(.+)$/', $id, $matches)) {
    $entity_id = $matches[1];
    $field_name = $matches[2];
    $field = field_info_field($field_name);
    $result = tripal_load_entity('TripalEntity', [$entity_id], FALSE, [$field['id']]);
    reset($result);
    $entity = $result[$entity_id];

    // Get the element render array for this field and turn off the label
    // display. It's already on the page.  We need to get the display from the
    // instance and pass it into the field_view_field. Otherwise it uses the
    // instance default display settings. Not sure why it does this. It needs
    // more investigation.
    $instance = field_info_instance('TripalEntity', $field_name, $entity->bundle);
    $items = field_get_items('TripalEntity', $entity, $field_name);
    $element = field_view_field('TripalEntity', $entity, $field_name, $instance['display']['default']);
    $element['#label_display'] = 'hidden';

    $content = drupal_render($element);
    return drupal_json_output([
      'id' => $id,
      'content' => $content,
      'is_empty' => tripal_field_is_empty($field, $items),
    ]);
  }
}
