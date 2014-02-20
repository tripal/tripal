<?php
/**
 * @file
 * Interface for downloading feature sequences
 */

/**
 * The page allowing users to download feature sequences
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_page() {

  if ($_SESSION['tripal_feature_seq_extract']['download']) {
    $genus      = $_SESSION['tripal_feature_seq_extract']['genus'];
    $species    = $_SESSION['tripal_feature_seq_extract']['species'];
    $analysis   = $_SESSION['tripal_feature_seq_extract']['analysis'];
    $ftype      = $_SESSION['tripal_feature_seq_extract']['ftype'];
    $fnames     = $_SESSION['tripal_feature_seq_extract']['fnames'];
    $upstream   = $_SESSION['tripal_feature_seq_extract']['upstream'];
    $downstream = $_SESSION['tripal_feature_seq_extract']['downstream'];
    $format     = $_SESSION['tripal_feature_seq_extract']['format'];
    $use_parent = $_SESSION['tripal_feature_seq_extract']['use_parent'];
    $aggregate  = $_SESSION['tripal_feature_seq_extract']['aggregate'];
    $agg_types  = $_SESSION['tripal_feature_seq_extract']['agg_types'];

    unset($_SESSION['tripal_feature_seq_extract']['download']);

    if ($format == 'fasta_html') {
      drupal_add_http_header('Content-Type: text/html');
    }
    else {
      drupal_add_http_header('Content-Type: text');
      drupal_add_http_header('Content-Disposition: attachment; filename="sequences.fasta.txt"');
    }

    tripal_feature_seq_extract_get_features(NULL, $genus, $species, $analysis,
      $ftype, $fnames['items_array'], $upstream, $downstream, $format, $use_parent, $aggregate,
      $agg_types);

    return;
  }


  // generate the search form
  $output .= '';
  if (user_access('administer tripal')) {
    $output .= tripal_set_message("Administrators, the " . 
        l('organism_feature_count', 'admin/tripal/schema/mviews') . " and " .
        l('analysis_organism', 'admin/tripal/schema/mviews') . " materialized
        views must be populated before using this form.  Those views should be re-populated
        when new data is added.");
  }
  $output .= "<div id=\"tripal-feature-seq-extract-form-block\">";
  $output .= drupal_get_form('tripal_feature_seq_extract_form');
  $output .= "</div>";
  return $output;
}

/**
 * Theme the Form to choose which features to extract sequence for
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_seq_extract_form($form) {
  $rows = array(
    0 => array(
      array('data' => drupal_render($form['description']), 'colspan' => 3),
    ),
    1 => array(
      drupal_render($form['genus']),
      drupal_render($form['species']) ,
      drupal_render($form['ftype']),
    ),
    2 => array(
      array('data' => drupal_render($form['analysis']), 'colspan' => 3),
      //drupal_render($form['format']),
    ),
    3 => array(
      array('data' =>  drupal_render($form['fnames']), 'colspan' => 2),
      drupal_render($form['upstream']) . drupal_render($form['downstream']) ,
    ),
    4 => array(
      array(
        'data' =>  drupal_render($form['advanced']),
        'colspan' => 3,
      ),
    ),
    5 => array(
      array(
        'data' =>  drupal_render($form['retrieve_btn']) . drupal_render($form['reset_btn']),
        'colspan' => 3,
      ),
    ),
  );
  $headers = array();
  $table = theme('table', $headers, $rows, array('id' => 'tripal-feature-seq-extract-form-table', 'border' => '0'));

  $markup .= $table;
  $form['criteria'] = array(
    '#markup' =>  $markup,
    '#weight' => -10,
  );
  return drupal_render($form);
}

/**
 * Form to choose which features to extract sequence for
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_form(&$form_state = NULL) {
  tripal_core_ahah_init_form();

  // we want to allow the query string to provide values for the form
  if ($_GET['fnames']) {
    $form_state['values']['fnames']['items'] = $_GET['fnames'];
  }
  if ($_GET['genus']) {
    $form_state['values']['genus'] = $_GET['genus'];
  }
  if ($_GET['species']) {
    $form_state['values']['species'] = $_GET['species'];
  }
  if ($_GET['ftype']) {
    $form_state['values']['ftype'] = $_GET['ftype'];
  }
  if ($_GET['analysis']) {
    $form_state['values']['analysis'] = $_GET['analysis'];
  }
  if ($_GET['upstream']) {
    $form_state['values']['upstream'] = $_GET['upstream'];
  }
  if ($_GET['downstream']) {
    $form_state['values']['downstream'] = $_GET['downstream'];
  }
  if ($_GET['use_parent']) {
    $form_state['values']['use_parent'] = $_GET['use_parent'];
  }
  if ($_GET['aggregate']) {
    $form_state['values']['aggregate'] = $_GET['aggregate'];
  }
  if ($_GET['agg_types']) {
    $form_state['values']['agg_types'] = $_GET['agg_types'];
  }

  // get defaults
  $dgenus      = isset($form_state['values']['genus'])      ? $form_state['values']['genus']      : $_SESSION['tripal_feature_seq_extract']['genus'];
  $dspecies    = isset($form_state['values']['species'])    ? $form_state['values']['species']    : $_SESSION['tripal_feature_seq_extract']['species'];
  $danalysis   = isset($form_state['values']['analysis'])   ? $form_state['values']['analysis']   : $_SESSION['tripal_feature_seq_extract']['analysis'];
  $dftype      = isset($form_state['values']['ftype'])      ? $form_state['values']['ftype']      : $_SESSION['tripal_feature_seq_extract']['ftype'];
  $dfnames     = isset($form_state['values']['fnames'])     ? $form_state['values']['fnames']     : $_SESSION['tripal_feature_seq_extract']['fnames'];
  $dupstream   = isset($form_state['values']['upstream'])   ? $form_state['values']['upstream']   : $_SESSION['tripal_feature_seq_extract']['upstream'];
  $ddownstream = isset($form_state['values']['downstream']) ? $form_state['values']['downstream'] : $_SESSION['tripal_feature_seq_extract']['downstream'];
  $dformat     = isset($form_state['values']['format'])     ? $form_state['values']['format']     : 'fasta_txt';
  $duse_parent = isset($form_state['values']['use_parent']) ? $form_state['values']['use_parent'] : $_SESSION['tripal_feature_seq_extract']['use_parent'];
  $daggregate  = isset($form_state['values']['aggregate'])  ? $form_state['values']['aggregate']  : $_SESSION['tripal_feature_seq_extract']['aggregate'];
  $dagg_types  = isset($form_state['values']['agg_types'])  ? $form_state['values']['agg_types']  : $_SESSION['tripal_feature_seq_extract']['agg_types'];


  $form = array();

  // because we're using Tripal's file_upload_combo form element we
  // need to allow the form to upload files
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['#method'] = 'POST';

  $form['description'] = array(
    '#markup' => t('Use this form to retrieve sequences in FASTA format.')
  );

  $sql = "
    SELECT DISTINCT genus
    FROM {organism}
    ORDER BY genus
  ";
  $results = chado_query($sql);
  $genus = array();
  $genus[] = '';
  while ($organism = $results->fetchObject()) {
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
      'path'    => 'find/sequences/ajax',
      'wrapper' => 'tripal-feature-seq-extract-form-block',
      'event'   => 'change',
      'method'  => 'replace',
    ),
  );

  $species = array();
  $species[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT species
      FROM {organism}
      WHERE genus = :genus
      ORDER BY species
    ";
    $results = chado_query($sql, array(':genus' => $dgenus));
    while ($organism = $results->fetchObject()) {
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
      'path'    => 'find/sequences/ajax',
      'wrapper' => 'tripal-feature-seq-extract-form-block',
      'event'   => 'change',
      'method'  => 'replace',
    ),
  );

  $analyses = array();
  $analyses[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT A.analysis_id, A.name
      FROM {analysis_organism} AO
        INNER JOIN {analysis} A ON A.analysis_id = AO.analysis_id
        INNER JOIN {organism} O ON O.organism_id = AO.organism_id
      WHERE O.genus = :genus
    ";
    $args = array();
    $args[':genus'] = $dgenus;
    if ($dspecies) {
      $sql .= " AND O.species = :species ";
      $args[':species'] = $dspecies;
    }
    $sql .=" ORDER BY A.name ";
    $results = chado_query($sql, $args);
    while ($analysis = $results->fetchObject()) {
      $analyses[$analysis->name] = $analysis->name;
    }
  }
  $form['analysis'] = array(
    '#title'         => t('Analyses'),
    '#type'          => 'select',
    '#options'       => $analyses,
    '#default_value' => $danalysis,
    '#multiple'      => FALSE,
    '#description'  => t('You can limit sequences by the analyses to which it was derived or was used. If specified, only features associated with the specific analysis will be retrieved.'),
  );

  $ftype = array();
  $ftype[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT OFC.cvterm_id, OFC.feature_type
      FROM {organism_feature_count} OFC
      WHERE OFC.genus = :genus
    ";
    $args = array();
    $args['genus'] = $dgenus;
    if ($dspecies) {
      $sql .= " AND OFC.species = :species";
      $args['species'] = $dspecies;
    }
    $sql .= " ORDER BY OFC.feature_type ";
    $results = chado_query($sql, $args);

    while ($type = $results->fetchObject()) {
      $ftype[$type->feature_type] = $type->feature_type;
    }
  }
  $form['ftype'] = array(
    '#title'         => t('Feature Type'),
    '#type'          => 'select',
    '#options'       => $ftype,
    '#multiple'      => FALSE,
    '#default_value' => $dftype,
    '#description'   => t('The type of feature to retrieve (e.g. mRNA). All features that match this type will be retrieved.'),
  );

  $form['fnames'] = array(
    '#title'         => t('Feature Name'),
    '#type'          => 'file_upload_combo',
    '#default_value' => $dfnames,
    '#description'   => t('The names of the features to retrieve. Separate each with a new line or comma. Leave blank to retrieve all features matching other criteria.'),
    '#rows'          => 8
  );
  $form['upstream'] = array(
    '#title'         => t('Upstream Bases'),
    '#type'          => 'textfield',
    '#description'   => t('A numeric value specifying the number of upstream bases to include. Only works if the feature is aligned to a larger sequence.'),
    '#default_value' => $dupstream,
    '#size'          => 5,
  );
  $form['downstream'] = array(
    '#title'         => t('Downstream Bases'),
    '#type'          => 'textfield',
    '#description'   => t('A numeric value specifying the number of downstream bases to incldue.  Only works if the feature is aligned to a larger sequence.'),
    '#default_value' => $ddownstream,
    '#size'          => 5,
  );
  $form['format'] = array(
    '#title'         => t('Output Format'),
    '#type'          => 'hidden',
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
    '#value' => 'Retrieve',
  );
  $form['reset_btn'] = array(
    '#type' => 'submit',
    '#value' => 'Reset',
  );
  return $form;
}

/**
 * AJAX callback
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_form_ahah_update() {
  $status = TRUE;

  // prepare and render the form. If no form is returned that means
  // we got here by an AHAH call after the form has been submitted.  This
  // is possible because results are downloaded and the page is not refreshed
  // but the form_id goes away.  So, we need to rebuild the form in this case.
  // otherwise, if the form already exists we just theme it.
  $form = tripal_core_ahah_prepare_form();
  if ($form) {
    $data = theme('tripal_feature_seq_extract_form', $form);
  }
  else {
    $data = drupal_get_form('tripal_feature_seq_extract_form');
  }

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
 * Validate the extract sequence form
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_form_validate($form, &$form_state) {
  $genus      = $form_state['values']['genus'];
  $species    = $form_state['values']['species'];
  $analysis   = $form_state['values']['analysis'];
  $ftype      = $form_state['values']['ftype'];
  $fnames     = $form_state['values']['fnames'];
  $upstream   = $form_state['values']['upstream'];
  $downstream = $form_state['values']['downstream'];
  $format     = $form_state['values']['format'];
  $use_parent = $form_state['values']['use_parent'];
  $aggregate  = $form_state['values']['aggregate'];
  $agg_types  = $form_state['values']['agg_types'];
  $op         = $form_state['values']['op'];

  if ($op == 'Retrieve') {
    if ($upstream and !preg_match('/^\d+$/', $upstream)) {
      form_set_error('upstream', 'Please enter a positive numeric value for the upstream bases');
    }
     if ($downstream and !preg_match('/^\d+$/', $downstream)) {
      form_set_error('downstream', 'Please enter a positive numeric value for the downstream bases');
    }
    if (!$genus and !$species and !$ftype and !$fnames) {
      form_set_error('', 'Please provide a feature name, a feature type or a genus.');
    }
  }
}

/**
 * Submit the extract sequence form
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_form_submit($form, &$form_state) {
  $genus      = $form_state['values']['genus'];
  $species    = $form_state['values']['species'];
  $analysis   = $form_state['values']['analysis'];
  $ftype      = $form_state['values']['ftype'];
  $fnames     = $form_state['values']['fnames'];
  $upstream   = $form_state['values']['upstream'];
  $downstream = $form_state['values']['downstream'];
  $format     = $form_state['values']['format'];
  $use_parent = $form_state['values']['use_parent'];
  $aggregate  = $form_state['values']['aggregate'];
  $agg_types  = $form_state['values']['agg_types'];
  $op         = $form_state['values']['op'];

  // we must use the parent sequence if the user has selected
  // the upstream, downstream or to aggregate
  if ($upstream or $downstream or $aggregate) {
    $use_parent = 1;
  }

  if ($op == 'Retrieve') {
    $_SESSION['tripal_feature_seq_extract']['genus']      = $genus;
    $_SESSION['tripal_feature_seq_extract']['species']    = $species;
    $_SESSION['tripal_feature_seq_extract']['analysis']   = $analysis;
    $_SESSION['tripal_feature_seq_extract']['ftype']      = $ftype;
    $_SESSION['tripal_feature_seq_extract']['fnames']     = $fnames;
    $_SESSION['tripal_feature_seq_extract']['upstream']   = $upstream;
    $_SESSION['tripal_feature_seq_extract']['downstream'] = $downstream;
    $_SESSION['tripal_feature_seq_extract']['format']     = $format;
    $_SESSION['tripal_feature_seq_extract']['use_parent'] = $use_parent;
    $_SESSION['tripal_feature_seq_extract']['aggregate']  = $aggregate;
    $_SESSION['tripal_feature_seq_extract']['agg_types']  = $agg_types;
    $_SESSION['tripal_feature_seq_extract']['download']   = 1;
  }
  if ($op == 'Reset') {
    unset($_SESSION['tripal_feature_seq_extract']);
  }
}

/**
 * Actually extract the sequences
 *
 * @ingroup tripal_feature
 */
function tripal_feature_seq_extract_get_features($org_commonname, $genus, $species, $analysis_name,
  $type, $feature_name, $upstream, $downstream, $output_format, $derive_from_parent, $aggregate,
  $child, $relationship, $rel_part) {

  $sub_features = explode(',', $child);

  if (!$output_format) {
    $output_format = 'fasta_txt';
  }

  if (!$type and !$feature_name and !$genus) {
    print "Please provide a type, feature name or genus\n";
     return;
  }

  // get the list of features
  $vars = array();
  $sql  = "SELECT DISTINCT F.feature_id, F.name, F.uniquename, O.genus, O.species, CVT.name as feature_type " .
          "FROM {feature} F " .
          "  INNER JOIN {organism} O on O.organism_id = F.organism_id " .
          "  INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id ";
  if ($analysis_name) {
     $sql .= "  INNER JOIN {analysisfeature} AF on AF.feature_id = F.feature_id " .
             "  INNER JOIN {analysis} A on AF.analysis_id = A.analysis_id ";
  }
  $sql .= "WHERE (1=1) ";
  if ($org_commonname) {
    $sql .= "AND O.common_name = :common_name ";
    $vars[':common_name'] = $org_commonname;
  }
  if ($genus) {
    $sql .= "AND O.genus = :genus ";
    $vars[':genus'] = $genus;
  }
  if ($species) {
    $sql .= "AND O.species = :species ";
    $vars[':species'] = $species;
  }
  if ($type) {
    $sql .= "AND CVT.name = ':cvtname ";
    $vars[':cvtname'] = $type;
  }
  if ($feature_name) {
    if (is_array($feature_name)) {
      $sql .= "AND F.name IN (";
      foreach ($feature_name as $i => $fname) {
        $sql .= ":fname$i, ";
        $vars[":fname$i"] = $fname;
      }
      // remove the trailing comma and close the paren
      $sql = substr($sql, 0, -2) . ")";
    }
    else {
      $sql .= "AND F.name = :fname";
      $vars[':fname'] = $feature_name;
    }
  }
  if ($analysis_name) {
    $sql .= "AND A.name = :aname";
    $vars[':aname'] = $analysis_name;
  }
  $num_bases_per_line = 50;
  $num_seqs = 0;
  $q = chado_query($sql, $vars);

  while ($feature = $q->fetchObject()) {

    $feature_id = $feature->feature_id;

    // build the header for each FASTA entry
    if ($feature->uniquename == $feature->name) {
      $feature_name = "$feature->uniquename $feature->feature_type ($feature->genus $feature->species)";
    }
    else {
      $feature_name = "$feature->uniquename $feature->name $feature->feature_type ($feature->genus $feature->species)";
    }

    // generate the sequence
    $sequence = tripal_feature_get_formatted_sequence($feature_id, $feature_name,
      $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
      $upstream, $downstream, $sub_features, $relationship, $rel_part);

    // print the sequence
    print $sequence;
    $num_seqs++;
  }
  if ($num_seqs == 0) {
    print "No Sequences Found";
  }
}
