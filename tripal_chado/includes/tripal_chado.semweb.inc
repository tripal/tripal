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
function tripal_chado_semweb_form($form, &$form_state) {

  $chado_table = '';
  if (array_key_exists('values', $form_state)) {
    $chado_table = $form_state['values']['chado_table'];
  }

  $chado_tables = chado_get_table_names(TRUE);
  $chado_tables = array_merge(array('Select a Chado Table'), $chado_tables);

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

  // If the user has selected a content type, then we need to
  // show some filters.
  if ($chado_table) {
    $form['filters'] = array(
      '#type' => 'fieldset',
      '#title' => 'Filters',
      '#description' => t('Please provide any filters for limiting
          the records. Only those that match the filters specified
          below will be published.  To publish all records of this
          type, leave all filters blank.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );

    $form['publish_btn'] = array(
      '#type' => 'submit',
      '#name' => 'publish_btn',
      '#value' => 'Publish',
    );
  }


  $form['#prefix'] = '<div id="tripal-chado-publish-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

function tripal_chado_semweb_form_validate($form, &$form_state) {

}

function tripal_chado_semweb_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'publish_btn') {
    global $user;

    $chado_table = $form_state['values']['chado_table'];
    $bundle_name = 'bio_data_' . $chado_table;
    $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
    $args = array(
      array('bundle_name' => $bundle_name),
    );
    $includes = array(
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.publish'),
    );
    return tripal_add_job("Publish " . $bundle->label . " records.",
      'tripal_chado', 'tripal_chado_publish_records', $args,
      $user->uid, 10, $includes);
  }
}
/**
 *
 */
function tripal_chado_semweb_form_ajax_callback($form, $form_state) {
  return $form;
}
    