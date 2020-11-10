<?php
/**
 * @file
 * Provides an application programming interface (API) for working with
 * controlled vocabulary terms.
 */

/**
 * @defgroup tripal_terms_api CV Terms
 * @ingroup tripal_api
 * @{
 * Tripal provides an application programming interface (API) for working with
 * controlled vocabulary terms.  Tripal v3 is highly dependent on controlled
 * vocabularies for identifying all content types and fields attached to those
 * content types.  However, Tripal v3 is also database agnostic. Therefore,
 * controlled vocabularies can be stored in any database back-end.  By default
 * the tripal_chado module is used for storing controlled vocabularies. However,
 * if someone wanted to store controlled vocabularies in a database other than
 * Chado they can do so. These API functions provide a convenient wrapper for
 * accession controlled vocabularies no matter where they are stored.
 *
 * @}
 */

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
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_storage_info() {
  return [
    'term_chado_storage' => [
      'label' => t('Chado'),
      'description' => t('Integrates terms stored in the local Chado database with Tripal entities.'),
      'settings' => [],
    ],
  ];
}

/**
 * Provides a form for importing vocabularies and their terms.
 *
 * Tripal allows for vocabularies to be stored separately from the biological
 * data. This hook allows the default term storage backend to provide an
 * appropriate form for importing ontologies (either in OBO or OWL format).
 *
 * @param $form
 * @param $form_state
 *
 * @ingroup tripal_terms_api
 *
 */
function hook_vocab_import_form($form, &$form_state) {
  return $form;
}

/**
 * Validates the hook_vocab_import_form().
 *
 * @param $form
 * @param $form_state
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_import_form_validate($form, &$form_state) {

}

/**
 * Submits the hook_vocab_import_form().
 *
 * @param $form
 * @param $form_state
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_import_form_submit($form, &$form_state) {

}

/**
 * Hook used by the default term storage backend to provide details for a term.
 *
 * This hook is called by the tripal_entity module to retrieve information
 * about the term from the storage backend.  It must return an array with
 * a set of keys.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys:
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *       -urlprefix : (optional) A URL to which the short_name and term
 *        accession can be appended to form a complete URL for a term.  If the
 *        prefix does not support appending then the exact location for the
 *        position of the short_name and the term accession will be
 *        specified with the {db} and {accession} tags respectively.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 *   any other keys may be added as desired. Returns NULL if the term
 *   cannot be found.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_term($vocabulary, $accession) {
  // See the tripal_chado_vocab_get_term() function for an example.

}

/**
 * Retrieves a paged list of terms from a vocabulary.
 *
 * @param $vocabulary
 *   The short name of the vocabulary.
 * @param $limit
 *   The number of results to return.
 * @param $element
 *   The pager element. This is equivalent to the element from the
 *   pager_default_initialize() function of Drupal.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_terms($vocabulary, $limit = 25, $element = 0) {
  // See the tripal_chado_vocab_get_terms() function for an example.
}

/**
 * Hook used by the default term storage backend to provide children for a term.
 *
 * This hook is called by the tripal_entity module to retrieve a list of
 * children for a term from the storage backend.  It must return an array
 * of terms where each term contains the same structure as that of the
 * hook_vocab_get_term().
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array of terms where each term contains the same structure as that of
 *   the hook_vocab_get_term(), or an empty array if no children are present.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_term_children($vocabulary, $accession) {
  // See the tripal_chado_vocab_get_term_children() function for an example.
}

/**
 * Hook used by the default term storage backend to provide root terms.
 *
 * This hook is called by the tripal_entity module to retrieve a list of
 * root terms for a given vocabulary from the storage backend.  It must return
 * an array of terms where each term contains the same structure as that of the
 * hook_vocab_get_term().
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 *
 * @return
 *   An array of root terms where each term contains the same structure as that
 *   of the hook_vocab_get_term(), or an empty array if no children are present.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_root_terms($vocabulary) {
  // See the tripal_chado_vocab_get_root_terms() function for an example.
}

/**
 * Hook used by the default term storage backend to provide details for a vocab.
 *
 * This hook is called by the tripal_entity module to retrieve information
 * about the vocabulary from the storage backend.  It must return an array with
 * a set of keys.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 *
 * @return
 *   An array with at least the following keys:
 *     - name : The full name of the vocabulary.
 *     - short_name : The short name abbreviation for the vocabulary.
 *     - description : A brief description of the vocabulary.
 *     - url : (optional) A URL for the online resources for the vocabulary.
 *     - urlprefix : (optional) A URL to which the short_name and term
 *       accession can be appended to form a complete URL for a term.  If the
 *       prefix does not support appending then the exact location for the
 *       position of the short_name and the term accession will be
 *       specified with the {db} and {accession} tags respectively.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_vocabulary($vocabulary) {
  // See the tripal_chado_vocab_get_vocabulary() function for an example.
}

/**
 * Retrieves the list of vocabularies that are available on the site.
 *
 * @return
 *   An array of vocabularies where each entry in the array is compatible
 *   with the array returned by the tripal_get_vocabulary_details()
 *   function.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_get_vocabularies() {
  // See the tripal_chado_vocab_get_vocabularies() function for an example.
}

/**
 * Hook used by the default term storage backend to add new terms.
 *
 * @param $details
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys:
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *       -urlprefix: (optional) A URL to which the short_name and term
 *         accession can be appended to form a complete URL for a term.  If the
 *         prefix does not support appending then the exact location for the
 *         position of the short_name and the term accession will be
 *         specified with the {db} and {accession} tags respectively.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 *
 * @return
 *   TRUE if the term was added, FALSE otherwise.  If the term already exists
 *   it will be updated and the return value will be TRUE.
 *
 * @ingroup tripal_terms_api
 */
function hook_vocab_add_term($details) {
  // See the tripal_chado_vocab_set_term() function for an example.
}

/**
 * Adds a term to the vocabulary storage backend.
 *
 * Use this function to add new terms dynamically to the vocabulary storage
 * backend.  If the term already exists no new term is added.
 *
 * @param $details
 *   An array with at least the following keys:
 *     -vocabulary : An associative array with the following keys
 *       -name:  The short name for the vocabulary (e.g. SO, PATO, etc).
 *       -description: The description of this vocabulary.
 *       -url: The URL for the vocabulary.
 *     -accession : The name unique ID of the term.
 *     -url : The URL for the term.
 *     -name : The name of the term.
 *     -definition : The term's description.
 *
 * @return
 *   TRUE if the term was added, FALSE otherwise.  If the term already exists
 *   it will be updated and the return value will be TRUE.
 *
 * @ingroup tripal_terms_api
 */
function tripal_add_term($details) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_add_term';
    if (function_exists($function)) {
      return $function($details);
    }
  }
}


/**
 * Retrieves full information about a vocabulary term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   An array with at least the following keys:
 *     - vocabulary : An array containing the following keys:
 *       - name : The full name of the vocabulary.
 *       - short_name : The short name abbreviation for the vocabulary.
 *       - description : A brief description of the vocabulary.
 *       - url : (optional) A URL for the online resources for the vocabulary.
 *       - urlprefix : (optional) A URL to which the short_name and term
 *         accession can be appended to form a complete URL for a term.  If the
 *         prefix does not support appending then the exact location for the
 *         position of the short_name and the term accession will be
 *         specified with the {db} and {accession} tags respectively.
 *     - accession : The name unique ID of the term.
 *     - url : The URL for the term.
 *     - name : The name of the term.
 *     - definition : The term's description.
 *   any other keys may be added as desired. Returns NULL if the term
 *   cannot be found.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_details($vocabulary, $accession) {

  if (empty($vocabulary) OR empty($accession)) {
    tripal_report_error('tripal_term', TRIPAL_ERROR, "Unable to retrieve details for term due to missing vocabulary and/or accession");
    return FALSE;
  }

  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_term';
    if (function_exists($function)) {

      $term = $function($vocabulary, $accession);
      if (!$term) {
        tripal_report_error(
          'tripal',
          TRIPAL_ERROR,
          "Unable to find term for :vocab, :accession using :function.",
          [':vocab' => $vocabulary, ':accession' => $accession, ':function' => $function]
        );
        return FALSE;
      }

      // If the vocabulary is missing then we have a problem.
      if (!array_key_exists('vocabulary', $term)) {
        tripal_report_error('tripal_term', TRIPAL_ERROR, "The term is missing a vocabulary entry.");
        return FALSE;
      }

      // Make sure the term has a URL. If it does not, then use the Tripal
      // interface as the URL for the term.
      $url_missing = FALSE;
      if (!$term['url']) {
        $url_missing = TRUE;
        $term['url'] = url('cv/lookup/' . $term['vocabulary']['short_name'] . '/' . $term['accession'], ['absolute' => TRUE]);
      }
      if (!$term['vocabulary']['sw_url']) {
        $url_missing = TRUE;
        $term['vocabulary']['sw_url'] = url('cv/lookup/' . $term['vocabulary']['short_name'] . '/' . $term['accession'], ['absolute' => TRUE]);
      }
      // Let the user know that the url is missing.
      if ($url_missing) {
        //         tripal_add_notification(
        //           "Missing CV term URL", 
        //           t("The controlled vocabulary, %vocab, is missing a URL. Tripal will handle " .
        //             "this by linking to the cv/lookup page of this site. However, the correct " .
        //             "should be updated for this site", 
        //             ['%vocab' => $term['vocabulary']['short_name']]),
        //           'Controlled Vocabularies', 
        //           NULL, 
        //           'mising-vocab-' . $term['vocabulary']['short_name']
        //         );
      }

      return $term;
    }
  }
}

/**
 * Retrieves the immediate children of the given term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   Returns an array of terms where each term is compatible with the
 *   array returned by the tripal_get_term_details() function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_root_terms($vocabulary) {
  if (empty($vocabulary)) {
    tripal_report_error('tripal_term', TRIPAL_ERROR, 'Unable to retrieve details for term due to missing vocabulary.');
  }

  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_root_terms';
    if (function_exists($function)) {
      return $function($vocabulary);
    }
  }
}

/**
 * Retrieves the immediate children of the given term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 * @param $accession
 *   The unique identifier (accession) for this term.
 *
 * @return
 *   Returns an array of terms where each term is compatible with the
 *   array returned by the tripal_get_term_details() function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_children($vocabulary, $accession) {
  if (empty($vocabulary) OR empty($accession)) {
    tripal_report_error('tripal_term', TRIPAL_ERROR, 'Unable to retrieve details for term due to missing vocabulary and/or accession.');
  }

  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_term_children';
    if (function_exists($function)) {
      return $function($vocabulary, $accession);
    }
  }
}

/**
 * Retrieves full information about a vocabulary.
 *
 * Vocabularies are stored in a database backend.  Tripal has no requirements
 * for how terms are stored.  By default, the tripal_chado modules provides
 * storage for vocabularies and terms. This function will call the
 * hook_vocab_get_term() function for the database backend that is housing the
 * vocabularies and allow it to return the details about the term.
 *
 * @param $vocabulary
 *   The vocabulary of the vocabulary in which the term is found.
 *
 * @return
 *   An array with at least the following keys:
 *     - name: The full name of the vocabulary.
 *     - short_name: The short name abbreviation for the vocabulary.
 *     - description: A brief description of the vocabulary.
 *     - url:  A URL for the online resources for the vocabulary.
 *     - urlprefix: A URL to which the short_name and term
 *       accession can be appended to form a complete URL for a term.  If the
 *       prefix does not support appending then the exact location for the
 *       position of the short_name and the term accession will be
 *       specified with the {db} and {accession} tags respectively.
 *     - sw_url: The URL for mapping terms via the semantic web.
 *     - num_terms: The number of terms loaded in the vocabulary.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_details($vocabulary) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_vocabulary';
    if (function_exists($function)) {
      return $function($vocabulary);
    }
  }
}


/**
 * Retrieves a paged list of terms from a vocabulary.
 *
 * @param $vocabulary
 *   The short name of the vocabulary.
 * @param $limit
 *   The number of results to return.
 * @param $element
 *   The pager element. This is equivalent to the element from the
 *   pager_default_initialize() function of Drupal.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabulary_terms($vocabulary, $limit = 25, $element = 0) {
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_terms';
    if (function_exists($function)) {
      return $function($vocabulary, $limit, $element);
    }
  }
}

/**
 * Retrieves the list of vocabularies that are available on the site.
 *
 * @return
 *   An array of vocabularies where each entry in the array is compatible
 *   with the array returned by the tripal_get_vocabulary_details()
 *   function.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_vocabularies() {
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_get_vocabularies';
    if (function_exists($function)) {
      return $function();
    }
  }
}


/**
 * Provides a term lookup form.
 *
 * It may be necessary at times for a form to provide to the user the ability
 * to lookup and use a controlled vocabulary term.  However, a simple text box
 * or auto-lookup is not sufficient because a term name may be identical in
 * multiple vocabularies and the user would need to select the proper term.
 *
 * This function will add the form elements necessary to provide a lookup form.
 * The form elements should work with a flat form (no #tree set for the form)
 * or with a form in a TripalField.
 *
 * Use the tripal_get_term_lookup_form_result() function to retreive the
 * result in a form submit or validate.
 *
 * @param $form
 *   The form (or $widget form).
 * @param $form_state
 *   The form state.
 * @param $title
 *   The title to give to the field set.
 * @param $description
 *   A description for the lookup field element.
 * @param $is_required
 *   Indicates if this form element is required.
 * @param $field_name
 *   The name of the field, if this form is being added to a field widget.
 * @param  $delta
 *   The delta value for the field if this form is being added to a field
 *   widget.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_lookup_form(&$form, &$form_state, $default_name = '',
                                     $title = 'Vocabulary Term', $description = '', $is_required,
                                     $field_name = '', $delta = 0, $callback = '', $wrapper = '', $validate = [],
                                     $weight = 0) {

  if (!$callback) {
    $callback = 'tripal_get_term_lookup_form_ajax_callback';
  }

  if (!$wrapper) {
    $ajax_wrapper_id = 'tripal-vocab-select-form-' . $delta;
    if ($field_name) {
      $ajax_wrapper_id = $field_name . '-' . $delta;
    }
  }
  else {
    $ajax_wrapper_id = $wrapper;
  }

  $term_name = $default_name;
  if (array_key_exists('values', $form_state) and array_key_exists('term_name' . $delta, $form_state['values'])) {
    $term_name = $form_state['values']['term_name' . $delta];
  }
  if (array_key_exists('input', $form_state) and array_key_exists('term_name' . $delta, $form_state['input'])) {
    $term_name = $form_state['input']['term_name' . $delta];
  }
  if ($field_name and array_key_exists('input', $form_state) and array_key_exists($field_name, $form_state['input'])) {
    $term_name = $form_state['input'][$field_name]['und'][$delta]['term_match' . $delta]['term_name' . $delta];
  }

  if (!$description) {
    $description = t('Enter the name of the term that specifies the type. ' .
      'The type must be the name of a term in a controlled vocabulary and ' .
      'the controlled vocabulary should already be loaded into this site.');
  }

  $form_state['storage'][$ajax_wrapper_id]['term_match_field'] = $field_name;
  $form_state['storage'][$ajax_wrapper_id]['term_match_delta'] = $delta;

  $form['term_match' . $delta] = [
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t($title),
    '#prefix' => '<div id = "' . $ajax_wrapper_id . '">',
    '#suffix' => '</div>',
    '#weight' => $weight,
  ];
  $form['term_match' . $delta]['term_name' . $delta] = [
    '#title' => t('Type'),
    '#type' => 'textfield',
    '#description' => $description,
    '#required' => $is_required,
    '#default_value' => $term_name,
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  ];
  $form['term_match' . $delta]['select_button' . $delta] = [
    '#type' => 'button',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm_' . $ajax_wrapper_id,
    '#validate' => $validate,
    '#ajax' => [
      'callback' => $callback,
      'wrapper' => $ajax_wrapper_id,
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  if (empty($validate)) {
    $form['term_match' . $delta]['select_button' . $delta]['#limit_validation_errors'] = [];
  }


  // If the term has been provided by the user then we want to search for
  // matching terms in the database and let them select among any matches.
  if ($term_name) {
    $submit_disabled = TRUE;
    $form['term_match' . $delta]['terms_list' . $delta] = [
      '#type' => 'fieldset',
      '#title' => t('Matching Terms'),
      '#description' => t('Please select the best matching term. If the 
        same term exists in multiple vocabularies you will see more than 
        one option below.'),
    ];
    $match = [
      'name' => $term_name,
    ];
    // TODO: this should not call the chado functions because we're in the
    // tripal module.
    $terms = chado_generate_var('cvterm', $match, ['return_array' => TRUE]);
    if ($terms) {
      $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    }
    $num_terms = 0;
    $selected_term = '';

    // Let the user select from any matching terms. Sometimes there may be
    // more than one that match.
    foreach ($terms as $term) {
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $checked = FALSE;
      $attrs = [];
      if ($num_terms == 0 and count($terms) == 1) {
        $checked = TRUE;
        $attrs = ['checked' => 'checked'];
      }
      $term_element_name = 'term-' . $term->cvterm_id . '-' . $delta;
      $definition = property_exists($term, 'definition') ? $term->definition : '';
      $form['term_match' . $delta]['terms_list' . $delta][$term_element_name] = [
        '#type' => 'checkbox',
        '#title' => $term->name,
        '#default_value' => $checked,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term ID: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $definition,
      ];

      if (array_key_exists('values', $form_state) and array_key_exists($term_element_name, $form_state['values']) and
        $form_state['values'][$term_element_name] == 1) {
        $selected_term = $term;
      }
      $num_terms++;
    }

    // Next find terms that are synonyms
    $match = [
      'synonym' => $term_name,
    ];
    $termsyn = chado_generate_var('cvtermsynonym', $match, ['return_array' => TRUE]);
    // Let the user select from any matching terms. Sometimes there may be
    // more than one that match.
    foreach ($termsyn as $synonym) {
      $term = $synonym->cvterm_id;
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $checked = FALSE;
      $attrs = [];
      if ($num_terms == 0 and count($terms) == 1) {
        $checked = TRUE;
        $attrs = ['checked' => 'checked'];
      }
      $term_element_name = 'term-' . $term->cvterm_id . '-' . $delta;
      $definition = property_exists($term, 'definition') ? $term->definition : '';
      $form['term_match' . $delta]['terms_list' . $delta][$term_element_name] = [
        '#type' => 'checkbox',
        '#title' => $term->name,
        '#default_value' => $checked,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $definition .
          '<br><b>Synonym:</b> ' . $synonym->synonym,
      ];

      if (array_key_exists('values', $form_state) and array_key_exists($term_element_name, $form_state['values']) and
        $form_state['values'][$term_element_name] == 1) {
        $selected_term = $term;
      }
      $num_terms++;
    }


    if ($num_terms == 0) {
      $form['term_match' . $delta]['terms_list' . $delta]['none' . $delta] = [
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>',
      ];
    }
  }
}

/**
 * Returns the terms selected from the tripal_get_term_lookup_form.
 *
 * @param $form
 *   The form (or $widget form).
 * @param $form_state
 *   The form state.
 * @param $field_name
 *   The name of the field, if this form is being added to a field widget.
 * @param  $delta
 *   The delta value for the field if this form is being added to a field
 *   widget.
 *
 * @return
 *   An array of term objects for each of the user selected terms.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_lookup_form_result($form, $form_state, $field_name = '', $delta = 0) {
  $values = [];
  $selected = [];
  if ($field_name) {
    if (array_key_exists('term_match' . $delta, $form_state['values'][$field_name]['und'][$delta]) and
      array_key_exists('terms_list' . $delta, $form_state['values'][$field_name]['und'][$delta]['term_match' . $delta])) {
      $values = $form_state['values'][$field_name]['und'][$delta]['term_match' . $delta]['terms_list' . $delta];
    }
  }
  else {
    $values = array_key_exists('values', $form_state) ? $form_state['values'] : [];
  }

  if (is_array($values)) {
    foreach ($values as $key => $value) {
      $matches = [];
      if (preg_match("/^term-(\d+)-$delta$/", $key, $matches) and $values['term-' . $matches[1] . '-' . $delta]) {
        $cvterm_id = $matches[1];
        $selected[] = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
      }
    }
  }
  return $selected;
}

/**
 * Implements an AJAX callback for the tripal_chado_vocab_select_term_form.
 *
 * @ingroup tripal_terms_api
 */
function tripal_get_term_lookup_form_ajax_callback($form, $form_state) {
  $ajax_wrapper_id = $form_state['triggering_element']['#ajax']['wrapper'];

  $field_name = $form_state['storage'][$ajax_wrapper_id]['term_match_field'];
  $delta = $form_state['storage'][$ajax_wrapper_id]['term_match_delta'];

  // If this form is in a field then we need to dig a bit deeper to return
  // the form elements.
  if ($field_name) {
    return $form[$field_name]['und'][$delta]['term_match' . $delta];
  }
  else {
    return $form['term_match' . $delta];
  }
}
