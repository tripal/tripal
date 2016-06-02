<?php
/**
 * Adds defaults to the chado_semweb table.
 */
function tripal_chado_populate_chado_semweb_table() {

  // Add in all tables and fields into the chado_semweb table.
  $chado_tables = chado_get_table_names(TRUE);
  foreach ($chado_tables as $chado_table) {
    tripal_add_chado_semweb_table($chado_table);
  }

  // Now set defaults!

  //
  // VOCABUARIES:

  // Add in vocabularies of terms that will be used for the semantic web
  //
  tripal_insert_db(array(
    'name' => 'foaf',
    'description' => 'Friend of a Friend. A dictionary of people-related terms that can be used in structured data).',
    'url' => 'http://www.foaf-project.org/',
    'urlprefix' => 'http://xmlns.com/foaf/spec/#',
  ));
  tripal_insert_cv('foaf','Friend of a Friend');

  tripal_insert_db(array(
    'name' => 'schema',
    'description' => 'Schema.org. Schema.org is sponsored by Google, Microsoft, Yahoo and Yandex. The vocabularies are developed by an open community process.',
    'url' => 'https://schema.org/',
    'urlprefix' => 'https://schema.org/',
  ));
  tripal_insert_cv('schema','Schema.org');

  tripal_insert_db(array(
    'name' => 'TAXRANK',
    'description' => 'Taxonomic rank vocabulary. A vocabulary of taxonomic ranks (species, family, phylum, etc).',
    'url' => 'https://github.com/phenoscape/taxrank',
    'urlprefix' => 'http://purl.obolibrary.org/obo/TAXRANK_',
  ));
  tripal_insert_cv('taxrank','Taxonomic rank vocabulary');

  tripal_insert_db(array(
    'name' => 'NCBITaxon',
    'description' => 'NCBI organismal classification. An ontology representation of the NCBI organismal taxonomy.',
    'url' => 'http://www.berkeleybop.org/ontologies/ncbitaxon/',
    'urlprefix' => 'http://purl.obolibrary.org/obo/ncbitaxon#',
  ));
  tripal_insert_cv('ncbitaxon','NCBI organismal classification');

  tripal_insert_db(array(
    'name' => 'SWO',
    'description' => 'Software Ontology. An ontology representation of the NCBI organismal taxonomy.',
    'url' => 'http://theswo.sourceforge.net/',
    'urlprefix' => '',
  ));
  tripal_insert_cv('swo','Software Ontology');

  tripal_insert_db(array(
    'name' => 'IAO',
    'description' => 'The Information Artifact Ontology (IAO) is a new ' .
    'ontology of information entities, originally driven by work by the ' .
    'OBI digital entity and realizable information entity branch.',
    'url' => 'https://github.com/information-artifact-ontology/IAO/',
    'urlprefix' => 'http://purl.obolibrary.org/obo/IAO_',
  ));
  tripal_insert_cv('IAO','Information Artifact Ontology');


  tripal_insert_db(array(
    'name' => 'SBO',
    'description' => 'Systems Biology. Terms commonly used in Systems Biology, and in particular in computational modeling.',
    'url' => 'http://www.ebi.ac.uk/sbo/main/',
    'urlprefix' => 'http://purl.obolibrary.org/obo/SBO_',
  ));
  tripal_insert_cv('sbo','Systems Biology');

  //
  // SET TERM DEFAULTS
  //
  $name = tripal_insert_cvterm(array(
    'id' => 'schema:name',
    'name' => 'name',
    'cv_name' => 'schema',
    'definition' => 'The name of the item.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'uniquename', $name);
  tripal_associate_chado_semweb_term(NULL, 'name', $name);

  $alternate_name = tripal_insert_cvterm(array(
    'id' => 'schema:alternateName',
    'name' => 'alternateName',
    'cv_name' => 'schema',
    'definition' => 'The name of the item.',
  ));

  $description = tripal_insert_cvterm(array(
    'id' => 'schema:description',
    'name' => 'description',
    'cv_name' => 'schema',
    'definition' => 'A description of the item.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'description', $description);


  $definition = tripal_insert_cvterm(array(
    'id' => 'IAO:0000115',
    'name' => 'definition',
    'cv_name' => 'iao',
    'definition' => 'The official OBI definition, explaining the meaning of ' .
      'a class or property. Shall be Aristotelian, formalized and normalized. ' .
      'Can be augmented with colloquial definitions.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'definition', $definition);

  $comment = tripal_insert_cvterm(array(
    'id' => 'schema:comment',
    'name' => 'comment',
    'cv_name' => 'schema',
    'definition' => 'Comments, typically from users.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'comment', $comment);

  $time_last_modified = tripal_insert_cvterm(array(
    'id' => 'local:timelastmodified',
    'name' => 'time_last_modified',
    'cv_name' => 'local',
    'definition' => 'The time at which a record for an item was first added.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'timelastmodified', $time_last_modified);


  $time_accessioned = tripal_insert_cvterm(array(
    'id' => 'local:timeaccessioned',
    'name' => 'time_accessioned',
    'cv_name' => 'local',
    'definition' => 'The time at which a record for an item was last upated or modified.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'timeaccessioned', $time_accessioned);

  $time_executed = tripal_insert_cvterm(array(
    'id' => 'local:timeexecuted',
    'name' => 'time_executed',
    'cv_name' => 'local',
    'definition' => 'The time at which a task was executed.',
  ));
  tripal_associate_chado_semweb_term(NULL, 'timeaccessioned', $time_executed);

  $dbxref = tripal_insert_cvterm(array(
    'id' => 'SBO:0000554',
    'name' => 'database cross reference',
    'cv_name' => 'sbo',
    'definition' => 'An annotation which directs one to information contained within a database.',
  ));

  $relationship = tripal_insert_cvterm(array(
    'id' => 'SBO:0000374',
    'name' => 'relationship',
    'cv_name' => 'sbo',
    'definition' => 'connectedness between entities and/or interactions representing their relatedness or influence. [ src_code:NR ]',
  ));

  //
  // ANALYSIS TABLE
  //
  $term = tripal_insert_cvterm(array(
    'id' => 'SWO:0000001',
    'name' => 'software',
    'cv_name' => 'schema',
    'definition' => 'Computer software, or generally just software, is any ' .
      'set of machine-readable instructions (most often in the form of a ' .
      'computer program) that conform to a given syntax (sometimes ' .
      'referred to as a language) that is interpretable by a given ' .
      'processor and that directs a computer\'s processor to perform ' .
      'specific operations.',
  ));
  tripal_associate_chado_semweb_term('analysis', 'program', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'IAO:0000129',
    'name' => 'version number',
    'cv_name' => 'IAO',
    'definition' => 'A version number is an ' .
    'information content entity which is a sequence of characters ' .
    'borne by part of each of a class of manufactured products or its ' .
    'packaging and indicates its order within a set of other products ' .
    'having the same name.',
  ));
  tripal_associate_chado_semweb_term('analysis', 'programversion', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'IAO:0000064',
    'name' => 'algorithm',
    'cv_name' => 'IAO',
    'definition' => 'An algorithm is a set of instructions for performing a paticular calculation.',
  ));
  tripal_associate_chado_semweb_term('analysis', 'algorithm', $term);

  //
  // ORGANISM TABLE
  //
  $term = tripal_insert_cvterm(array(
    'id' => 'TAXRANK:0000005',
    'name' => 'genus',
    'cv_name' => 'taxrank',
  ));
  tripal_associate_chado_semweb_term('organism', 'genus', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'TAXRANK:0000006',
    'name' => 'species',
    'cv_name' => 'taxrank',
  ));
  tripal_associate_chado_semweb_term('organism', 'species', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'TAXRANK:0000045',
    'name' => 'infraspecies',
    'cv_name' => 'taxrank',
  ));
  tripal_associate_chado_semweb_term('organism', 'infraspecific_name', $term);


  $term = tripal_insert_cvterm(array(
    'id' => 'local:infraspecific_type',
    'name' => 'infraspecific_type',
    'definition' => 'The connector type for the infraspecific name',
    'cv_name' => 'local',
  ));
  tripal_associate_chado_semweb_term('organism', 'type_id', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'NCBITaxon:common_name',
    'name' => 'common name',
    'cv_name' => 'ncbitaxon',
  ));
  tripal_associate_chado_semweb_term('organism', 'common_name', $term);

  $term = tripal_insert_cvterm(array(
    'id' => 'local:abbreviation',
    'name' => 'abbreviation',
    'cv_name' => 'local',
  ));
  tripal_associate_chado_semweb_term('organism', 'abbreviation', $term);

  //
  // FEATURE TABLE
  //
  tripal_associate_chado_semweb_term('feature', 'name', $alternate_name);

  //
  // PUB TABLE
  //
  tripal_associate_chado_semweb_term('pub', 'uniquename', $comment);


  //
  // STOCK TABLE
  //
  tripal_associate_chado_semweb_term('stock', 'name', $alternate_name);


}

/**
 * Adds defaults to the chado_semweb table.
 */
function tripal_chado_semweb_form($form, &$form_state, $chado_table = NULL) {
  if (array_key_exists('values', $form_state)) {
    $chado_table = $form_state['values']['chado_table'];
  }

  $chado_tables = chado_get_table_names(TRUE);
  $chado_tables = array_merge(array('Select a Chado Table'), $chado_tables);
  // Make sure the table name exists. If not, reset it.
  $chado_table = in_array($chado_table, $chado_tables) ? $chado_table: NULL;
  
  $form['chado_table'] = array(
    '#type' => 'select',
    '#title' => 'Chado Table',
    '#description' => t('Select a chado table to set web services terms used for its columns.'),
    '#options' => $chado_tables,
    '#default_value' => $chado_table,
    '#ajax' => array(
      'callback' => "tripal_chado_semweb_form_ajax_callback",
      'wrapper' => "tripal-chado-semweb-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  // If the user has selected a chado table, then we need to
  // show the columns for setting terms.
  if ($chado_table) {
    $schema = chado_get_schema($chado_table);
    $pk = $schema['primary key'][0];
    $cv_default = 
      db_select('tripal_cv_defaults', 'tc')
      ->fields('tc', array('field_name'))
      ->condition('table_name', $chado_table)
      ->execute()
      ->fetchField();
    $columns = $schema['fields'];
    $headers = array('Field Name', 'Vocabulary', 'Term Name', 'Term Description', 'Action');
    $rows = array();
    foreach ($columns AS $column => $detail) {
      // Do not show column if it's the primary key or default cv
      if ($column != $pk && $column != $cv_default) {
        $cvterm_id = 
          db_select('chado_semweb', 'cs')
          ->fields('cs', array('cvterm_id'))
          ->condition('chado_table', $chado_table)
          ->condition('chado_column', $column)
          ->execute()
          ->fetchField();
        $sw_voc = '';
        $sw_term = '';
        $sw_desc = '';
        if($cvterm_id) {
          $term = tripal_get_cvterm(array(
            'cvterm_id' => $cvterm_id
          ));
          $sw_voc = $term->cv_id->name;
          $sw_term = $term->name;
          $sw_desc = $term->definition;
        }
        $rows[] = array(
          $column,
          $sw_voc,
          $sw_term,
          $sw_desc,
          l('Edit', '/admin/tripal/storage/chado/semweb/edit/' . $chado_table . '/' . $column) . ' | ' .
          l('Reset', '/admin/tripal/storage/chado/semweb/reset/' . $chado_table . '/' . $column)
        );
      }
    }
    $output = theme('table', array(
      'header' => $headers,
      'rows' => $rows,
    ));
    $form['table'] = array(
      '#markup' => $output,
      '#title' => 'Table',
      '#description' => t('Please provide any filters for limiting
        the records. Only those that match the filters specified
        below will be published.  To publish all records of this
        type, leave all filters blank.'),
    );
  }

  $form['#prefix'] = '<div id="tripal-chado-semweb-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Implements hook_form()
 * Edit terms used by the semantic web
 * 
 * @param $form
 * @param $form_state
 * @param $table
 * @param $column
 * @return $form
 */
function tripal_chado_semweb_edit_form($form, &$form_state, $table = NULL, $column = NULL) {
  
  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  $form['chado_table'] = array(
    '#markup' => 'Term used for the <strong>' . t($column) . '</strong> column of the chado <strong>' . t($table) . '</strong> table:',
  );
  $form['table_name'] = array(
    '#type' => 'value',
    '#value' => $table
  );
  $form['column'] = array(
    '#type' => 'value',
    '#value' => $column
  );
  // If no term has been selected yet then provide the auto complete field.
  $form['term_name'] = array(
    '#title'       => t('Term'),
    '#type'        => 'textfield',
    '#description' => t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO)."),
    '#required'    => TRUE,
    '#default_value' => $term_name,
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  );
  $form['select_button'] = array(
    '#type' => 'button',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm',
    '#ajax' => array(
      'callback' => "tripal_chado_semweb_form_ajax_callback",
      'wrapper' => "tripal-chado-semweb-edit-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  if ($term_name) {
    $form['terms_list'] = array(
      '#type' => 'fieldset',
      '#title' => t('Matching Terms'),
      '#description' => t('Please select the term the best matches the
          content type you want to create. If the same term exists in
          multiple vocabularies you will see more than one option below.')
    );
    $match = array(
      'name' => $term_name,
    );
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    foreach ($terms as $term) {
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $default = FALSE;
      $attrs = array();
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = array('checked' => 'checked');
      }
       $form['terms_list']['term-' . $term->cvterm_id] = array(
         '#type' => 'checkbox',
         '#title' =>  $term->name,
         '#default_value' => $default,
         '#attributes' => $attrs,
         '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name .
             '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
             '<br><b>Definition:</b>  ' . $term->definition,
       );
       $num_terms++;
    }
    if ($num_terms == 0) {
      $form['terms_list']['none'] = array(
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
      );
    }
    // Add in the button for the cases of no terms or too many.
    $form['submit_button'] = array(
      '#type' => 'submit',
      '#value' => t('Use this term'),
      '#name' => 'use_cvterm'
    );
  }
  
  $form['cancel_button'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => array()
  );

  $form['#prefix'] = '<div id = "tripal-chado-semweb-edit-form">';
  $form['#suffix'] = '</div>';

  return $form;
}


/**
 * Implements hook_form_validate()
 *
 * Validate function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_edit_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] =='use_cvterm') {
      $cvterm_id = NULL;
      // Make sure we have a cvterm selected
      $num_selected = 0;
      foreach ($form_state['values'] as $key => $value) {
        $matches = array();
        if (preg_match("/^term-(\d+)$/", $key, $matches) and
            $form_state['values']['term-' . $matches[1]]) {
          $cvterm_id = $matches[1];
          $num_selected++;
        }
      }
      if ($num_selected == 0) {
        form_set_error('', 'Please select at least one term.');
      }
      else if ($num_selected > 1) {
        form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
      }
      else {
        $form_state['values']['#selected_cvterm_id'] = $cvterm_id;
      }
    }
    else if ($form_state['clicked_button']['#name'] =='cancel_button') {
      $table_name = $form_state['values']['table_name'];
      drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
    }
  }
}

/**
 * Implements hook_form_submit()
 *
 * Submit function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_edit_form_submit($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state) && $form_state['clicked_button']['#name'] =='use_cvterm') {
        $table_name = $form_state['values']['table_name'];
        $column = $form_state['values']['column'];
        $cvterm_id = $form_state['values']['#selected_cvterm_id'];
        // Check if there is already a record
        $record_id = 
          db_select('chado_semweb', 'cs')
          ->fields('cs', array('chado_semweb_id'))
          ->condition('chado_table', $table_name)
          ->condition('chado_column', $column)
          ->execute()
          ->fetchField();
        
        // If the record exists, update it
        if ($record_id) {
          db_update('chado_semweb')
          ->fields(array(
            'cvterm_id' => $cvterm_id
          ))
          ->condition('chado_semweb_id', $record_id)
          ->execute();
        }
        // Otherwise, insert a new record
        else {
          db_insert('chado_semweb')
          ->fields(array(
            'chado_table' => $table_name,
            'chado_column' => $column,
            'cvterm_id' => $cvterm_id
          ))
          ->execute();
        }
        drupal_set_message('The term settings have been saved.');
        drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
      }
}


/**
 * Implements hook_form()
 * Reset term used by semantic web
 *
 * @param $form
 * @param $form_state
 * @param $table
 * @param $column
 * @return $form
 */
function tripal_chado_semweb_reset_form($form, &$form_state, $table = NULL, $column = NULL) {

  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  $form['chado_table'] = array(
    '#markup' => 'Are you sure you want to remove the use of this term? ',
  );
  $form['table_name'] = array(
    '#type' => 'value',
    '#value' => $table
  );
  $form['column'] = array(
    '#type' => 'value',
    '#value' => $column
  );
  
  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#name' => 'reset_term'
  );
  
  $form['cancel_button'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => array()
  );
  
  return $form;
}

/**
 * Implements hook_form_validate()
 * 
 * Validate function for resetting the semantic web term
 * 
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_reset_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] =='use_cvterm') {
      $cvterm_id = NULL;
      // Make sure we have a cvterm selected
      $num_selected = 0;
      foreach ($form_state['values'] as $key => $value) {
        $matches = array();
        if (preg_match("/^term-(\d+)$/", $key, $matches) and
            $form_state['values']['term-' . $matches[1]]) {
              $cvterm_id = $matches[1];
              $num_selected++;
            }
      }
      if ($num_selected == 0) {
        form_set_error('', 'Please select at least one term.');
      }
      else if ($num_selected > 1) {
        form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
      }
      else {
        $form_state['values']['#selected_cvterm_id'] = $cvterm_id;
      }
    }
    else if ($form_state['clicked_button']['#name'] =='cancel_button') {
      $table_name = $form_state['values']['table_name'];
      drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
    }
  }
}

/**
 * Implements hook_form_submit()
 *
 * Submit function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_reset_form_submit($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state) && $form_state['clicked_button']['#name'] =='reset_term') {
    $table_name = $form_state['values']['table_name'];
    $column = $form_state['values']['column'];
    // Check if there is already a record
    $record_id =
    db_select('chado_semweb', 'cs')
    ->fields('cs', array('chado_semweb_id'))
    ->condition('chado_table', $table_name)
    ->condition('chado_column', $column)
    ->execute()
    ->fetchField();

    // If the record exists, reset it
    if ($record_id) {
      db_update('chado_semweb')
      ->fields(array(
        'cvterm_id' => NULL
      ))
      ->condition('chado_semweb_id', $record_id)
      ->execute();
    }
    drupal_set_message('The term settings have been reset.');
    drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
  }
}

/**
 *
 */
function tripal_chado_semweb_form_ajax_callback($form, $form_state) {
  return $form;
}
    