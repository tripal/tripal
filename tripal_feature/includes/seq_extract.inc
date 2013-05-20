<?php
/*
 * 
 */
function tripal_feature_seq_extract_page() {
  // generate the search form 
  $form = drupal_get_form('tripal_feature_seq_extract_form');  
  $output .= $form;
  return $output;
}
/*
 * 
 */
function theme_tripal_feature_seq_extract_form($form) {
  $rows = array();
 
  $rows[] = array(
    drupal_render($form['genus']),
    drupal_render($form['species']) ,
    drupal_render($form['ftypes']) ,
  );
  $rows[] = array(
    drupal_render($form['analysis']) ,
    drupal_render($form['upstream']) ,
    drupal_render($form['downstream']) ,
  );
  $rows[] = array(
    drupal_render($form['format']),
    array(
      'data' =>  drupal_render($form['fname']),
      'colspan' => 2,
    ),
  );
  $rows[] = array(
    array(
      'data' =>  drupal_render($form['advanced']),
      'colspan' => 3,
    ),    
  );
  $rows[] = array(
    array(
      'data' =>  drupal_render($form['retrieve_btn']),
      'colspan' => 3,
    ),    
  );
  $headers = array();
  $table = theme('table', $headers, $rows, array('id' => 'tripal-feature-seq-extract-form-table', 'border' => '0'));   
  
  $markup = $table;
  $form['criteria'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
    '#weight' => -10,
  );
  return drupal_render($form);
}
/**
*
 */
function tripal_feature_seq_extract_form(&$form_state = NULL) {   
  tripal_core_ahah_init_form();
  
  // get defaults
  $dgenus      = $form_state['values']['genus'];
  $dspecies    = $form_state['values']['species'];
  $danalysis   = $form_state['values']['analysis'];
  $dftypes     = $form_state['values']['ftypes'];
  $dfname      = $form_state['values']['fname'];
  $dupstream   = $form_state['values']['upstream'];
  $ddownstream = $form_state['values']['downstream'];
  $dformat     = $form_state['values']['format'] ? $form_state['values']['format'] : 'fasta_txt';
  $duse_parent = $form_state['values']['use_parent'];
  $aggregate   = $form_state['values']['aggregate'];
  $dagg_types  = $form_state['values']['agg_types'];

  $sql = "
    SELECT DISTINCT genus 
    FROM {organism} 
    ORDER BY genus
  ";
  $results = chado_query($sql);
  $genus = array();
  $genus[] = '';
  while ($organism = db_fetch_object($results)) {
    $genus[$organism->genus] = $organism->genus;  
  }
  $form['genus'] = array(
    '#title'         => t('Genus'),
    '#type'          => 'select',
    '#options'       => $genus, 
    '#default_value' => $dgenus,    
    '#multiple'      => FALSE,
    '#description'   => t('The organism\'s genus. If specified, features for all organism with this genus will be retrieved.'),
    '#ahah' => array(
      'path'    => 'seq_extract/set_genus',
      'wrapper' => 'tripal-feature-seq-extract-form-table',
      'event'   => 'change',
      'method'  => 'replace',
    ), 
  );
  
  $species = array();  
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT species 
      FROM {organism} 
      WHERE genus = '%s'
      ORDER BY species
    ";
    $results = chado_query($sql, $dgenus);
    $species[] = '';
    while ($organism = db_fetch_object($results)) {
      $species[$organism->species] = $organism->species;  
    }
  }  
  $form['species'] = array(
    '#title'         => t('Species'),
    '#type'          => 'select',
    '#options'       => $species,
    '#default_value' => $dspecies,     
    '#multiple'      => FALSE,
    '#description'   => t('The organism\'s species name. If specified, features for all organisms with this species will be retrieved.  Please first select a genus'),
    '#ahah' => array(
      'path'    => 'seq_extract/set_species',
      'wrapper' => 'tripal-feature-seq-extract-form-table',
      'event'   => 'change',
      'method'  => 'replace',
    ), 
  );

  $analyses = array();
  $args = array();
  $sql = "
    SELECT DISTINCT A.analysis_id, A.name
    FROM {analysis} A 
      INNER JOIN {analysisfeature} AF  ON A.analysis_id = AF.analysis_id
  ";
  if ($dgenus) {
    $sql .= "
        INNER JOIN {feature} F on F.feature_id = AF.feature_id
        INNER JOIN {organism} O on O.organism_id = F.organism_id
      WHERE O.genus = '%s'
    ";
    $args[] = $dgenus;
  }
  if ($dgenus and $dspecies) {
    $sql .= " AND O.species = '%s' ";
    $args[] = $dspecies;
  }
  
  $results = chado_query($sql, $args);  
  $analyses[] = '';
  while ($analysis = db_fetch_object($results)) {
    $analyses[$analysis->analysis_id] = $analysis->name;  
  }
  $form['analysis'] = array(
    '#title'         => t('Source'),
    '#type'          => 'select',
    '#options'       => $analyses, 
    '#default_value' => $danalysis,    
    '#multiple'      => FALSE,
    '#description'  => t('The feature source. If specified, only features derived or part of the selected source will be retrieved.'),
    '#ahah' => array(
      'path'    => 'seq_extract/set_source',
      'wrapper' => 'tripal-feature-seq-extract-form-table',
      'event'   => 'change',
      'method'  => 'replace',
    ), 
  );
  
  $ftypes = array();
  if ($dgenus) {  
    $sql = "
      SELECT DISTINCT CVT.cvterm_id, CVT.name
      FROM {cvterm} CVT 
        INNER JOIN {feature} F on CVT.cvterm_id = F.type_id
        INNER JOIN {organism} O on O.organism_id = F.organism_id
      WHERE O.genus = '%s'
    ";
    $args = array();
    $args[] = $dgenus;
    if ($dspecies) {
      $sql .= " AND O.species = '%s'";
      $args[] = $dspecies;
    }
    $results = chado_query($sql, $args);
    
    $ftypes[] = '';
    while ($type = db_fetch_object($results)) {
      $ftypes[$type->cvterm_id] = $type->name;  
    }
  }
  $form['ftypes'] = array(
    '#title'         => t('Feature Types'),
    '#type'          => 'select',
    '#options'       => $ftypes,     
    '#multiple'      => FALSE,
    '#default_value' => $dftypes,
    '#description'   => t('The type of feature to retrieve (e.g. mRNA). All features that match this type will be retrieved.'),
  );
  
  $form['fname'] = array(
    '#title'         => t('Feature Name'),
    '#type'          => 'textarea',
    '#default_value' => $dfname,
    '#description'   => t('The names of the features to retrieve. Separate each with a space. Leave blank to retrieve all features matching other criteria.'),
  );
  $form['upstream'] = array(
    '#title'         => t('Upstream Bases'),
    '#type'          => 'textfield',
    '#description'   => t('An numeric value specifying the number of upstream bases to include.'),
    '#default_value' => $dupstream,
    '#size'          => 5,
  );
  $form['downstream'] = array(
    '#title'         => t('Downstream Bases'),
    '#type'          => 'textfield',
    '#description'   => t('An numeric value specifying the number of downstream bases to incldue.'),
    '#default_value' => $ddownstream,
    '#size'          => 5,
  );
  $form['format'] = array(
    '#title'         => t('Output Format'),
    '#type'          => 'select',
    '#default_value' => $dformat,
    '#options'       => array(
      'fasta_html'  => 'FASTA (in browser)',
      'fasta_txt'   => 'FASTA (download)',
    ),
  );
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => 'Advanced',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  
  $form['advanced']['use_parent'] = array(
    '#title'         => t('Use Parent'),
    '#type'          => 'checkbox',
    '#default_value' => $duse_parent,
    '#description'   => t('Check this box to retrieve the sequence from the parent in an alignment rather than the feature itself. This is useful if the same feature is aligned to multiple parents and you would like to retrieve the underlying sequence from each parent.'),
  );
  $form['advanced']['aggregate'] = array(
    '#title'         => t('Aggregate'),
    '#type'          => 'checkbox',
    '#default_value' => $daggregate,
    '#description'   => t('Check this box to aggregate sub features into a single sequence.  This is useful, for example, for obtaining CDS sequence from an mRNA. Rather than retrieve the mRNA sequence, the sub features of the mRNA will be aggregated and that will be returned.')
  );
  $form['advanced']['agg_types'] = array(
    '#title'         => t('Types to aggregate'),
    '#type'          => 'textarea',
    '#default_value' => $dagg_types,
    '#description'   => t('Set this argument to the type of children to aggregate.  This is useful in the case where a gene has exons, CDSs and UTRs.  In this case, you may only want to aggregate CDSs and exclude exons.  If you want to aggregate both CDSs and UTRs you could specify both.')
  );
  $form['retrieve_btn'] = array(
    '#type' => 'submit',
    '#value' => 'Retrieve'
  );
  return $form;
}

/**
 * 
 */
function tripal_feature_seq_extract_set_genus() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('tripal_feature_seq_extract_form', $form);  

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}
/**
 * 
 */
function tripal_feature_seq_extract_set_species() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('tripal_feature_seq_extract_form', $form); 

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}
/**
 * 
 */
function tripal_feature_seq_extract_set_source() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('tripal_feature_seq_extract_form', $form); 

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}
