<?php
/**
 * Retrieves a TripalTerm entity that matches the given arguments.
 *
 * @param $namespace
 *   The namespace for the vocabulary
 * @param $accession
 *   The ID (accession) of the term in the vocabulary.
 *
 * @return
 *   A TripalTerm entity object or NULL if not found.
 */
function tripal_load_term_entity($namespace, $accession) {
  $query = db_select('tripal_term', 'tt');
  $query->join('tripal_vocab' ,'tv', 'tv.id = tt.vocab_id');
  $query->fields('tt', array('id', 'accession'))
    ->fields('tv', array('namespace'))
    ->condition('tv.namespace', $namespace)
    ->condition('tt.accession', $accession);
  $term = $query->execute()->fetchObject();

  if ($term) {
    $entity = entity_load('TripalTerm', array($term->id));
    return reset($entity);
  }
  return NULL;
}

/**
 * Retrieves a TripalVocab entity that maches the given arguments.
 *
 * @param $namespace
 *
 * @return
 * A TripalVocab entity object or NULL if not found.
 */
function tripal_load_vocab_entity($namespace) {
  $vocab = db_select('tripal_vocab', 'tv')
    ->fields('tv')
    ->condition('tv.namespace', $namespace)
    ->execute()
    ->fetchObject();

  if ($vocab) {
    $entity = entity_load('TripalVocab', array($vocab->id));
    return reset($entity);
  }
  return NULL;
}

/**
 * Retrieves a TripalBundle entity that matches the given arguments.
 *
 * @param $values
 *   An associative array used to match a bundle.  Valid keys may be 'name' or
 *   'label' (e.g. array('name' => 'bio-data_234').
 *
 * @return
 *   A TripalBundle entity object or NULL if not found.
 */
function tripal_load_bundle_entity($values) {

  $query = db_select('tripal_bundle', 'tb');
  $query->fields('tb');
  if (array_key_exists('name', $values)) {
    $query->condition('tb.name', $values['name']);
  }
  if (array_key_exists('label', $values)) {
    $query->condition('tb.label', $values['label']);
  }
  $bundle = $query->execute()->fetchObject();

  if ($bundle) {
    $entity = entity_load('TripalBundle', array($bundle->id));
    return reset($entity);
  }
  return NULL;
}
/**
 * Creates a new Tripal Entity type (i.e. bundle).
 *
 * @param $namespace
 *   The abbreviated namespace for the vocabulary (e.g. RO, SO, PATO).
 * @param $accession
 *   The unique term ID in the vocabulary $namespace (i.e. an accession).
 * @param $term_name
 *   A human-readable name for this term.  This will became the name that
 *   appears for the content type.  In practice, this should be the name
 *   of the term. (E.g. the name for SO:0000704 is gene).
 * @param $error
 *  A string, passed by reference, that is filled with the error message
 *  if the function fails.
 *
 * @return
 *  TRUE if the entity type (bundle) was succesfully created.  FALSE otherwise.
 */
function tripal_create_bundle($namespace, $accession, $term_name, &$error = '') {

  // First create the TripalVocab if it doesn't already exist.
  $vocab = tripal_load_vocab_entity($namespace);
  if (!$vocab) {
    $vocab = entity_get_controller('TripalVocab')->create(array('namespace' => $namespace));
    $vocab->save();
  }

  // Next create the TripalTerm if it doesn't already exist.
  $term = tripal_load_term_entity($namespace, $accession);
  if (!$term) {
    $args = array('vocab_id' => $vocab->id, 'accession' => $accession, 'name' => $term_name);
    $term = entity_get_controller('TripalTerm')->create($args);
    $term = $term->save();
  }

  // If the bundle doesn't already exist, then add it.
  $bundle_name = 'bio-data_' . $term->id;
  $einfo = entity_get_info('TripalEntity');
  if (!in_array($bundle_name, array_keys($einfo['bundles']))) {
    // Insert the bundle.
    db_insert('tripal_bundle')
      ->fields(array(
        'label' => $term_name,
        'type' => 'TripalEntity',
        'name' => $bundle_name,
        'term_id' => $term->id,
      ))
      ->execute();
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');
  variable_set('menu_rebuild_needed', TRUE);

  // Get the bundle object.
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));

  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'TripalEntity', $bundle, $term);

  return TRUE;
}

/**
 * Allows a module to make changes to an entity object after creation.
 *
 * This function is added by Tripal to allow datastore backends to add
 * addition properties to the entity that they themselves will use later.
 *
 * @param $entity
 * @param $entity_type
 */
function hook_entity_create(&$entity, $entity_type) {

}
/**
 * Allows a module to add fields to a bundle.
 *
 * This function is called after the bundle is created and allows any module
 * to add fields to it.
 *
 * @param $entity_type
 *   The entity type (e.g. TripalEntity).
 * @param $bundle
 *   A TripalBundle object.
 * @param $term
 *   An instance of a TripalTerm object.
 *
 * @return
 *   TRUE on success, FALSE on failure.
 */
function hook_add_bundle_fields($entity_type, $bundle, $term) {

}

/**
 * @section
 * Bundle Variables.
 */

/**
 * Fetch the value for a given tripal variable.
 *
 * @param string $variable_name
 *   The name of the variable as in tripal_variables.name.
 * @param int $bundle_id
 *   The unique identfier for the bundle you want the value for.
 * @return text
 *   The value of the specified variable for the specified bundle.
 */
function tripal_get_bundle_variable($variable_name, $bundle_id, $default = FALSE) {

  $variable = tripal_get_variable($variable_name);

  // Warn if we can't find the tripal_variable.
  if (!$variable) {
//     tripal_report_error(
//       'trpbundle_var',
//       TRIPAL_WARNING,
//       'Unable to fetch tripal bundle variable value due to missing tripal variable (%var).',
//       array('%var' => $variable_name)
//     );
//     return FALSE;
    return $default;
  }

  // Select the value for this variable.
  $value = db_select('tripal_bundle_variables', 'var')
    ->fields('var', array('value'))
    ->condition('var.bundle_id', $bundle_id)
    ->condition('var.variable_id', $variable->variable_id)
    ->execute()
    ->fetchField();

  // Warn if the value appears to be empty.
  if (!$value) {
//     tripal_report_error(
//       'trpbundle_var',
//       TRIPAL_WARNING,
//       'Unable to fetch tripal bundle variable (%var) value.',
//       array('%var' => $variable_name)
//     );
    return $default;
  }

  return $value;
}

/**
 * Save the value of a tripal variable for a given bundle.
 *
 * @param string $variable_name
 *   The name of the variable as in tripal_variables.name.
 * @param int $bundle_id
 *   The unique identfier for the bundle you want the value for.
 * @param $text $value
 *   The value of the variable for the given bundle.
 */
function tripal_set_bundle_variable($variable_name, $bundle_id, $value) {

  $variable = tripal_get_variable($variable_name);

  // And then we need to write the new format to the tripal_bundle_variables table.
  $record = array(
    'bundle_id' => $bundle_id,
    'variable_id' => $variable->variable_id,
    'value' => $value,
  );

  // Check whether there is already a format saved.
  $bundle_variable_id = db_select('tripal_bundle_variables', 'var')
    ->fields('var', array('bundle_variable_id'))
    ->condition('var.bundle_id', $record['bundle_id'])
    ->condition('var.variable_id', $record['variable_id'])
    ->execute()
    ->fetchField();
  if ($bundle_variable_id) {
    $record['bundle_variable_id'] = $bundle_variable_id;
    return drupal_write_record('tripal_bundle_variables', $record, 'bundle_variable_id');
  }
  else {
    return drupal_write_record('tripal_bundle_variables', $record);
  }

}

/**
 * @section
 * Title & URL Formats.
 */

/**
 * Get Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 */
function tripal_get_title_format($entity) {

  // Get the existing title format if it exists.
  $title_format = tripal_get_bundle_variable('title_format', $entity->id);

  // If there isn't yet a title format for this bundle/type then we should
  // determine the default.
  if (!$title_format) {
    $title_format = tripal_get_default_title_format($entity);
    tripal_save_title_format($entity, $title_format);
  }

  return $title_format;
}

/**
 * Save Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 * @param string $format
 *   The pattern to be used when generating entity titles for the above type.
 */
function tripal_save_title_format($entity, $format) {

  return tripal_set_bundle_variable('title_format', $entity->id, $format);
}

/**
 * Determine the default pattern/format to use for an entity type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 * @return string
 *   A default title format.
 */
function tripal_get_default_title_format($entity) {
  $format = NULL;

  // Retrieve all available tokens.
  $tokens = tripal_get_tokens($entity);

  // A) Check to see if more informed modules have suggested a title for this type.
  // Invoke hook_tripal_default_title_format() to get all suggestions from other modules.
  $suggestions = module_invoke_all('tripal_default_title_format', $entity, $tokens);
  if ($suggestions) {
    // Use the suggestion with the lightest weight.
    $lightest_key = NULL;
    foreach ($suggestions as $k => $s) {
      if ($lightest_key === NULL) $lightest_key = $k;
      if ($s['weight'] < $lightest_key) $lightest_key = $k;
    }
    $format = $suggestions[$lightest_key]['format'];
  }

  // B) Check to see if any fields contain "name" in the machine name and if so, use them.
  $name_fields = preg_grep('/name/', array_keys($tokens));
  if ($name_fields AND !$format) {
    $format = implode(', ', $name_fields);
  }

  // C) Generate our own ugly title by simply comma-separating all the required fields.
  if (!$format) {
    $tmp = array();

    // Check which tokens are required fields and join them into a default format.
    foreach($tokens as $token) {
      if ($token['required']) {
        $tmp[] = $token['token'];
      }
    }
    $format = implode(', ', $tmp);
  }

  return $format;
}

/**
 * Implement this hook to define default formats for Tripal Content Types.
 *
 * @param TripalBundle $entity
 *   A tripal content type entity with information to be used for determining the default title format.
 * @param array $available_tokens
 *   An array of available tokens for this particular tripal content type.
 *
 * @return array
 *   An array of potential formats. The lightest weighted format suggested by all modules will be chosen.
 *   Each array item should consist of a 'weight' and 'format'. See the hook implementation below
 *   for examples.
 *    - weight: an integer used to determine priority of suggestions.
 *        The smaller/lighter the number the higher the priority.
 *        Best practice is to use a weight less than 0 for extension modules.
 *        specifically, -2 is a good weight for calculated formats and -5 is a
 *        good weight for hard-coded formats specific to a given type.
 *    - format: a string including approved tokens used to determine the title
 *        on Tripal content pages.
 */
function hook_tripal_default_title_format($entity, $available_tokens) {
  $format = array();

  // If you want to suggest a default format for a particular vocabulary term:
  //---------------------------------------------------------------------------
  // Load the term associated with this Tripal Content type.
  $term = entity_load('TripalTerm', array('id' => $entity->term_id));
  $term = reset($term);

  // If it's the term you are interested in then suggest a format.
  if ($term->name == 'organism') {

    // To suggest a format, add an element to the array with a format & weight key.
    $format[] = array(
      // This is the format/pattern you suggest be used to determine the title of organism pages.
      'format' => '[organism__genus] [organism__species]',
      // The weight/priority of your suggestion.
      'weight' => -5
    );

  }

  // Say you know that in your particular site, all 'names' are required
  // and you want to only use the human-readable name:
  //---------------------------------------------------------------------------
  $name_field = preg_grep('/__name]$/', array_keys($available_tokens));
  $name_field = reset($name_field);
  if (is_string($name_field)) {
    $format[] = array(
      'format' => $name_field,
      'weight' => -2,
    );
  }

  return $format;
}

/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param TripalBundle $entity
 *    The bundle entity for which you want tokens.
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 */
function tripal_get_tokens($entity, $options = array()) {
  $tokens = array();

  // Set default options.
  $options['required only'] = (isset($options['required only'])) ? $options['required only'] : FALSE;
  $options['include id'] = (isset($options['include id'])) ? $options['include id'] : TRUE;

  if ($options['include id']) {
    $token = '[TripalBundle__bundle_id]';
    $tokens[$token] = array(
      'label' => 'Bundle ID',
      'description' => 'The unique identifier for this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE
    );

    $token = '[TripalEntity__entity_id]';
    $tokens[$token] = array(
      'label' => 'Content/Entity ID',
      'description' => 'The unique identifier for an individual piece of Tripal Content.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE
    );
  }

  $fields = field_info_instances('TripalEntity', $entity->name);
  foreach ($fields as $f) {

    // Build the token from the field information.
    $token = '[' . $f['field_name'] . ']';
    $current_token = array(
      'label' => $f['label'],
      'description' => $f['description'],
      'token' => $token,
      'field_name' => $f['field_name'],
      'required' => $f['required']
    );

    // If the required only option is set then we only want to add
    // required fields to the token list.
    if ($options['required only'] AND $current_token['required']) {
      $tokens[$token] = $current_token;
    }
    // If the required only option is not set then add everything.
    elseif (!$options['required only']) {
      $tokens[$token] = $current_token;
    }
  }

  return $tokens;
}

/**
 * Replace all Tripal Tokens in a given string.
 *
 * NOTE: If there is no value for a token then the token is removed.
 *
 * @param string $string
 *   The string containing tokens.
 * @param TripalEntity $entity
 *   The entity with field values used to find values of tokens.
 * @param TripalBundle $bundle_entity
 *   The bundle enitity containing special values sometimes needed for token replacement.
 *
 * @return
 *   The string will all tokens replaced with values.
 */
function tripal_replace_tokens($string, $entity, $bundle_entity = NULL) {

  // Determine which tokens were used in the format string
  if (preg_match_all('/\[\w+\]/', $string, $matches)) {
    $used_tokens = $matches[0];

    foreach($used_tokens as $token) {
      $field = str_replace(array('.','[',']'),array('__','',''),$token);

      $value = '';
      if (isset($entity->{$field})) {

        // Render the value from the field.
        // First get the items to be rendered.
        $field_value = field_get_items('TripalEntity', $entity, $field);
        if (isset($field_value[0])) {
          // Then get a render array for just the value of the first item (no markup).
          $field_render_arr = field_view_value('TripalEntity', $entity, $field, $field_value[0]);
          // Finally render the value from the render array.
          $value = render($field_render_arr);
        }
      }
      elseif ($field === 'TripalBundle__bundle_id') {

        // Load the bundle entity if we weren't given it.
        if (!$bundle_entity) {
          $bundle_entity = tripal_load_bundle_entity(array('name' => $entity->bundle));
        }

        // This token should be the id of the TripalBundle.
        $value = $bundle_entity->id;
      }
      elseif ($field === 'TripalEntity__entity_id') {

        // This token should be the id of the TripalEntity.
        $value = $entity->id;
      }
      $string = str_replace($token, $value, $string);
    }
  }

  return $string;
}

/**
 * Formats the tokens for display.
 *
 * @param array $tokens
 *   A list of tokens generated via tripal_get_tokens().
 * @return
 *   Rendered output describing the available tokens.
 */
function theme_token_list($tokens) {

  $header = array('Token', 'Name', 'Description');
  $rows = array();
  foreach ($tokens as $details) {
    $rows[] = array(
      $details['token'],
      $details['label'],
      $details['description'],
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * @section
 * Vocabulary Hooks.
 */

/**
 * A hook for specifying information about the data store for vocabularies.
 *
 * The storage backend for controlled vocabularies has traditionally been
 * the Chado CV term tables. However, Tripal v3.0 introduces APIs for supporting
 * other backends.  Therefore, this function indicates to Tripal which
 * data stores are capable of providing support for terms.
 *
 * @return
 *   An array describing the storage backends implemented by the module. The
 *   keys are storage backend names. To avoid name clashes, storage
 *   backend names should be prefixed with the name of the module that
 *   exposes them. The values are arrays describing the storage backend,
 *   with the following key/value pairs:
 *
 *   label: The human-readable name of the storage backend.
 *   module:  The name of the module providing the support for this backend.
 *   description: A short description for the storage backend.
 *   settings: An array whose keys are the names of the settings available for
 *     the storage backend, and whose values are the default values for
 *     those settings.
 */
function hook_vocab_storage_info() {
  return array(
    'term_chado_storage' => array(
      'label' => t('Chado storage'),
      'description' => t('Integrates terms stored in the local Chado database with Tripal entities.'),
      'settings' => array(),
    ),
  );
}

/**
 * Creates a form for specifying a term for TripalEntity creation.
 *
 * This hook allows the module that implements a vocabulary storage backend
 * to provide the form necessary to select a term that will then be used for
 * creating a new TripalEntity type.  Tripal will expect that a 'namespace' and
 * 'accession' are in the $form_state['storage'] array. The 'namespace' and
 * must be the abbreviated uppercase namespace for the vocabulary (e.g. 'RO',
 * 'SO', 'PATO', etc.).  The 'accession' must be the unique term ID (or
 * accession) for the term in the vocabulary.
 *
 * @param $form
 * @param $form_state
 *
 * @return
 *   A form object.
 */
function hook_vocab_select_term_form(&$form, &$form_state) {

  return $form;
}
/**
 * Validates the hook_vocab_select_term_form().
 *
 * @param $name
 */
function hook_vocab_select_term_form_validate($form, &$form_state) {

}


/**
 * Hook used by the default term storage backend to provide details for a term.
 *
 * This hook is called by the tripal_entity module to retrieve information
 * about the term from the storage backend.  It must return an array with
 * a set of keys.
 *
 * @param $namespace
 *   The namespace of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     namespace : The namespace of the vocabulary.
 *     accession : The name unique ID of the term.
 *     url_prefix : The URL by which terms (accessions) can be appended.
 *     name : The name of the term.
 *     definition : The term's description.
 *   any other keys may be added as desired. Returns NULL if the term
 *   cannot be found.
 */
function hook_vocab_get_term($namespace, $accession) {

}
