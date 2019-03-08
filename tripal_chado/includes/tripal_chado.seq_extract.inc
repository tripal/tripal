<?php
/**
 * @file
 * Interface for downloading feature sequences
 */

/**
 * The page allowing users to download feature sequences
 *
 * @ingroup tripal_chado_feature
 */
function tripal_chado_feature_seq_extract_download() {

  if (!array_key_exists('tripal_feature_seq_extract', $_SESSION)) {
    drupal_goto('find/sequences');
  }

  $genus = $_SESSION['tripal_feature_seq_extract']['genus'];
  $species = $_SESSION['tripal_feature_seq_extract']['species'];
  $analysis = $_SESSION['tripal_feature_seq_extract']['analysis'];
  $ftype = $_SESSION['tripal_feature_seq_extract']['ftype'];
  $fnames = $_SESSION['tripal_feature_seq_extract']['fnames'];
  $upstream = $_SESSION['tripal_feature_seq_extract']['upstream'];
  $downstream = $_SESSION['tripal_feature_seq_extract']['downstream'];
  $format = $_SESSION['tripal_feature_seq_extract']['format'];
  $use_parent = $_SESSION['tripal_feature_seq_extract']['use_parent'];
  $aggregate = $_SESSION['tripal_feature_seq_extract']['aggregate'];
  $agg_types = $_SESSION['tripal_feature_seq_extract']['agg_types'];

  // Split the sub features and remove any surrounding white space
  $agg_types = preg_split("/[\n|,]/", $agg_types);
  for ($i = 0; $i < count($agg_types); $i++) {
    $agg_types[$i] = trim($agg_types[$i]);
  }


  header('Content-Type: text; utf-8');
  if ($ftype == 'polypeptide') {
    header('Content-Disposition: attachment; filename="sequences.fna"');
  }
  else {
    header('Content-Disposition: attachment; filename="sequences.fnn"');
  }

  $seqs = chado_get_bulk_feature_sequences([
    'genus' => $genus,
    'species' => $species,
    'analysis_name' => $analysis,
    'type' => $ftype,
    'feature_name' => $fnames['items_array'],
    'upstream' => $upstream,
    'downstream' => $downstream,
    'output_format' => $format,
    'derive_from_parent' => $use_parent,
    'aggregate' => $aggregate,
    'sub_feature_types' => $agg_types,
    'width' => 60,
  ]);

  if (count($seqs) == 0) {
    print ">No sequences found that match the criteria.";
  }

  foreach ($seqs as $seq) {
    print ">" . $seq['defline'] . "\r\n";
    print $seq['residues'] . "\r\n";
  }
}


/**
 * Form to choose which features to extract sequence for
 *
 * @ingroup tripal_chado_feature
 */
function tripal_chado_feature_seq_extract_form($form, &$form_state) {

  $form['#true'] = TRUE;

  // Intialize the defaults
  $dgenus = '';
  $dspecies = '';
  $danalysis = '';
  $dftype = '';
  $dfnames = '';
  $dupstream = '';
  $ddownstream = '';
  $duse_parent = '';
  $daggregate = '';
  $dagg_types = '';

  if (array_key_exists('tripal_feature_seq_extract', $_SESSION)) {
    $dgenus = $_SESSION['tripal_feature_seq_extract']['genus'];
    $dspecies = $_SESSION['tripal_feature_seq_extract']['species'];
    $danalysis = $_SESSION['tripal_feature_seq_extract']['analysis'];
    $dftype = $_SESSION['tripal_feature_seq_extract']['ftype'];
    $dfnames = $_SESSION['tripal_feature_seq_extract']['fnames'];
    $dupstream = $_SESSION['tripal_feature_seq_extract']['upstream'];
    $ddownstream = $_SESSION['tripal_feature_seq_extract']['downstream'];
    $duse_parent = $_SESSION['tripal_feature_seq_extract']['use_parent'];
    $daggregate = $_SESSION['tripal_feature_seq_extract']['aggregate'];
    $dagg_types = $_SESSION['tripal_feature_seq_extract']['agg_types'];
  }

  // we want to allow the query string to provide values for the form
  if (array_key_exists('fnames', $_GET) and $_GET['fnames']) {
    $dfnames = $_GET['fnames'];
  }
  if (array_key_exists('genus', $_GET) and $_GET['genus']) {
    $dgenus = $_GET['genus'];
  }
  if (array_key_exists('species', $_GET) and $_GET['species']) {
    $dspecies = $_GET['species'];
  }
  if (array_key_exists('ftype', $_GET) and $_GET['ftype']) {
    $dftype = $_GET['ftype'];
  }
  if (array_key_exists('analysis', $_GET) and $_GET['analysis']) {
    $danalysis = $_GET['analysis'];
  }
  if (array_key_exists('upstream', $_GET) and $_GET['upstream']) {
    $dupstream = $_GET['upstream'];
  }
  if (array_key_exists('downstream', $_GET) and $_GET['downstream']) {
    $ddownstream = $_GET['downstream'];
  }
  if (array_key_exists('use_parent', $_GET) and $_GET['use_parent']) {
    $duse_parent = $_GET['use_parent'];
  }
  if (array_key_exists('aggregate', $_GET) and $_GET['aggregate']) {
    $daggregate = $_GET['aggregate'];
  }
  if (array_key_exists('agg_types', $_GET) and $_GET['agg_types']) {
    $dagg_types = $_GET['agg_types'];
  }

  // get defaults from the form state
  if (array_key_exists('values', $form_state)) {
    $dgenus = $form_state['values']['genus'];
    $dspecies = $form_state['values']['species'];
    $danalysis = $form_state['values']['analysis'];
    $dftype = $form_state['values']['ftype'];
    $dfnames = $form_state['values']['fnames'];
    $dupstream = $form_state['values']['upstream'];
    $ddownstream = $form_state['values']['downstream'];
    $dformat = $form_state['values']['format'];
    $duse_parent = $form_state['values']['use_parent'];
    $daggregate = $form_state['values']['aggregate'];
    $dagg_types = $form_state['values']['agg_types'];
  }

  // Because we're using Tripal's file_upload_combo form element we
  // need to allow the form to upload files
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['#method'] = 'POST';

  $form['description'] = [
    '#markup' => t('Use this form to retrieve sequences in FASTA format.'),
  ];

  $sql = "
    SELECT DISTINCT genus
    FROM {organism}
    ORDER BY genus
  ";
  $results = chado_query($sql);
  $genus = [];
  $genus[] = '';
  while ($organism = $results->fetchObject()) {
    $genus[$organism->genus] = $organism->genus;
  }

  $form['genus'] = [
    '#title' => t('Genus'),
    '#type' => 'select',
    '#options' => $genus,
    '#default_value' => $dgenus,
    '#multiple' => FALSE,
    '#description' => t('The organism\'s genus. If specified, features for all organism with this genus will be retrieved.'),
    '#ajax' => [
      'callback' => 'tripal_chado_feature_seq_extract_form_ajax_callback',
      'wrapper' => 'tripal-feature-seq-extract-form',
      'event' => 'change',
      'method' => 'replace',
    ],
  ];

  $species = [];
  $species[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT species
      FROM {organism}
      WHERE genus = :genus
      ORDER BY species
    ";
    $results = chado_query($sql, [':genus' => $dgenus]);
    while ($organism = $results->fetchObject()) {
      $species[$organism->species] = $organism->species;
    }
  }
  $form['species'] = [
    '#title' => t('Species'),
    '#type' => 'select',
    '#options' => $species,
    '#default_value' => $dspecies,
    '#multiple' => FALSE,
    '#description' => t('The organism\'s species name. If specified, features for all organisms with this species will be retrieved.  Please first select a genus'),
    '#ajax' => [
      'callback' => 'tripal_chado_feature_seq_extract_form_ajax_callback',
      'wrapper' => 'tripal-feature-seq-extract-form',
      'event' => 'change',
      'method' => 'replace',
    ],
  ];

  $analyses = [];
  $analyses[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT A.analysis_id, A.name
      FROM {analysis_organism} AO
        INNER JOIN {analysis} A ON A.analysis_id = AO.analysis_id
        INNER JOIN {organism} O ON O.organism_id = AO.organism_id
      WHERE O.genus = :genus
    ";
    $args = [];
    $args[':genus'] = $dgenus;
    if ($dspecies) {
      $sql .= " AND O.species = :species ";
      $args[':species'] = $dspecies;
    }
    $sql .= " ORDER BY A.name ";
    $results = chado_query($sql, $args);
    while ($analysis = $results->fetchObject()) {
      $analyses[$analysis->name] = $analysis->name;
    }
  }
  $form['analysis'] = [
    '#title' => t('Analyses'),
    '#type' => 'select',
    '#options' => $analyses,
    '#default_value' => $danalysis,
    '#multiple' => FALSE,
    '#description' => t('You can limit sequences by the analyses to which it was derived or was used. If specified, only features associated with the specific analysis will be retrieved.'),
  ];

  $ftype = [];
  $ftype[] = '';
  if ($dgenus) {
    $sql = "
      SELECT DISTINCT OFC.cvterm_id, OFC.feature_type
      FROM {organism_feature_count} OFC
      WHERE OFC.genus = :genus
    ";
    $args = [];
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
  $form['ftype'] = [
    '#title' => t('Feature Type'),
    '#type' => 'select',
    '#options' => $ftype,
    '#multiple' => FALSE,
    '#default_value' => $dftype,
    '#description' => t('The type of feature to retrieve (e.g. mRNA). All
        features that match this type will be retrieved.'),
  ];

  $form['fnames'] = [
    '#title' => t('Feature Name'),
    '#type' => 'file_upload_combo',
    '#default_value' => $dfnames,
    '#description' => t('The names of the features to retrieve. Separate each
         with a new line or comma. Leave blank to retrieve all features
        matching other criteria.'),
    '#rows' => 8,
  ];
  $form['upstream'] = [
    '#title' => t('Upstream Bases'),
    '#type' => 'textfield',
    '#description' => t('A numeric value specifying the number of upstream
         bases to include. Only works if the feature is aligned to a larger
         sequence.'),
    '#default_value' => $dupstream,
    '#size' => 5,
  ];
  $form['downstream'] = [
    '#title' => t('Downstream Bases'),
    '#type' => 'textfield',
    '#description' => t('A numeric value specifying the number of downstream
        bases to incldue.  Only works if the feature is aligned to a larger
        sequence.'),
    '#default_value' => $ddownstream,
    '#size' => 5,
  ];
  $form['advanced'] = [
    '#type' => 'fieldset',
    '#title' => 'Advanced',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $form['advanced']['use_parent'] = [
    '#title' => t('Use Parent'),
    '#type' => 'checkbox',
    '#default_value' => $duse_parent,
    '#description' => t('Check this box to retrieve the sequence from the
        parent in an alignment rather than the feature itself. This is useful
        if the same feature is aligned to multiple parents and you would like
        to retrieve the underlying sequence from each parent.'),
  ];
  $form['advanced']['aggregate'] = [
    '#title' => t('Aggregate'),
    '#type' => 'checkbox',
    '#default_value' => $daggregate,
    '#description' => t('Check this box to aggregate sub features into a
        single sequence.  This is useful, for example, for obtaining CDS
        sequence from an mRNA. Rather than retrieve the mRNA sequence, the
        sub features of the mRNA will be aggregated and that will be returned.'),
  ];
  $form['advanced']['agg_types'] = [
    '#title' => t('Types to aggregate'),
    '#type' => 'textarea',
    '#default_value' => $dagg_types,
    '#description' => t('Set this argument to the type of children to
        aggregate.  This is useful in the case where a gene has exons, CDSs
        and UTRs.  In this case, you may only want to aggregate CDSs and
        exclude exons.  If you want to aggregate both CDSs and UTRs you
        could specify both.  Please place each type on a new line.'),
  ];
  $form['retrieve_btn'] = [
    '#type' => 'submit',
    '#name' => 'retrieve',
    '#value' => 'Retrieve Sequences',
  ];

  if (user_access('administer tripal')) {
    $notice = tripal_set_message("Administrators, the " .
      l('organism_feature_count', 'admin/tripal/schema/mviews') . " and " .
      l('analysis_organism', 'admin/tripal/schema/mviews') . " materialized
        views must be populated before using this form.  Those views should be re-populated
        when new data is added.", TRIPAL_NOTICE, ['return_html' => TRUE]);
  }

  $form['#prefix'] = '<div id="tripal-feature-seq-extract-form">';
  $form['#suffix'] = $notice . '</div>';

  return $form;
}

/**
 * Theme the Form to choose which features to extract sequence for
 *
 * @ingroup tripal_chado_feature
 */
function theme_tripal_chado_feature_seq_extract_form(&$variables) {
  $form = $variables['form'];

  $headers = [];
  $rows = [
    0 => [
      ['data' => drupal_render($form['description']), 'colspan' => 3],
    ],
    1 => [
      drupal_render($form['genus']),
      drupal_render($form['species']),
      drupal_render($form['ftype']),
    ],
    2 => [
      ['data' => drupal_render($form['analysis']), 'colspan' => 3],
      //drupal_render($form['format']),
    ],
    3 => [
      ['data' => drupal_render($form['fnames']), 'colspan' => 2],
      drupal_render($form['upstream']) .
      drupal_render($form['downstream']) .
      drupal_render($form['format']),
    ],
    4 => [
      [
        'data' => drupal_render($form['advanced']),
        'colspan' => 3,
      ],
    ],
    5 => [
      [
        'data' => drupal_render($form['retrieve_btn']) . drupal_render($form['reset_btn']),
        'colspan' => 3,
      ],
    ],
  ];

  $table_vars = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal-feature-seq-extract-form-table',
      'border' => '0',
    ],
    'sticky' => FALSE,
    'colgroups' => [],
    'empty' => '',
  ];

  $form['rendered_form'] = [
    '#type' => 'item',
    '#markup' => theme('table', $table_vars),
  ];
  return drupal_render_children($form);
}

/**
 * Ajax function which returns the form via ajax
 */
function tripal_chado_feature_seq_extract_form_ajax_callback($form, &$form_state) {
  return $form;
}

/**
 * Validate the extract sequence form
 *
 * @ingroup tripal_chado_feature
 */
function tripal_chado_feature_seq_extract_form_validate($form, &$form_state) {
  $genus = $form_state['values']['genus'];
  $species = $form_state['values']['species'];
  $analysis = $form_state['values']['analysis'];
  $ftype = $form_state['values']['ftype'];
  $fnames = $form_state['values']['fnames'];
  $upstream = $form_state['values']['upstream'];
  $downstream = $form_state['values']['downstream'];
  $use_parent = $form_state['values']['use_parent'];
  $aggregate = $form_state['values']['aggregate'];
  $agg_types = $form_state['values']['agg_types'];

  if ($upstream and !preg_match('/^\d+$/', $upstream)) {
    form_set_error('upstream', 'Please enter a positive numeric value for the upstream bases');
  }
  if ($downstream and !preg_match('/^\d+$/', $downstream)) {
    form_set_error('downstream', 'Please enter a positive numeric value for the downstream bases');
  }
  if (!$genus and !$species and !$ftype and !$fnames) {
    form_set_error('', 'Please provide a feature name, a feature type or a genus.');
  }
  if ($ftype == 'polypeptide' and $upstream) {
    form_set_error('upstream', 'When the sequence type is protein the upstream value must be unset.');
  }
  if ($ftype == 'polypeptide' and $downstream) {
    form_set_error('downstream', 'When the sequence type is protein the downstream value must be unset.');
  }
  if ($ftype == 'polypeptide' and $use_parent) {
    form_set_error('use_parent', 'When the sequence type is protein the "Use Parent" option must not be set.');
  }
}

/**
 * Submit the extract sequence form
 *
 * @ingroup tripal_chado_feature
 */
function tripal_chado_feature_seq_extract_form_submit($form, &$form_state) {
  $genus = $form_state['values']['genus'];
  $species = $form_state['values']['species'];
  $analysis = $form_state['values']['analysis'];
  $ftype = $form_state['values']['ftype'];
  $fnames = $form_state['values']['fnames'];
  $upstream = $form_state['values']['upstream'];
  $downstream = $form_state['values']['downstream'];
  $use_parent = $form_state['values']['use_parent'];
  $aggregate = $form_state['values']['aggregate'];
  $agg_types = $form_state['values']['agg_types'];

  // we must use the parent sequence if the user has selected
  // the upstream, downstream or to aggregate
  if ($upstream or $downstream or $aggregate) {
    $use_parent = 1;
  }

  if ($form_state['clicked_button']['#name'] == 'retrieve') {
    $_SESSION['tripal_feature_seq_extract']['genus'] = $genus;
    $_SESSION['tripal_feature_seq_extract']['species'] = $species;
    $_SESSION['tripal_feature_seq_extract']['analysis'] = $analysis;
    $_SESSION['tripal_feature_seq_extract']['ftype'] = $ftype;
    $_SESSION['tripal_feature_seq_extract']['fnames'] = $fnames;
    $_SESSION['tripal_feature_seq_extract']['upstream'] = $upstream;
    $_SESSION['tripal_feature_seq_extract']['downstream'] = $downstream;
    $_SESSION['tripal_feature_seq_extract']['format'] = 'fasta_txt';
    $_SESSION['tripal_feature_seq_extract']['use_parent'] = $use_parent;
    $_SESSION['tripal_feature_seq_extract']['aggregate'] = $aggregate;
    $_SESSION['tripal_feature_seq_extract']['agg_types'] = $agg_types;
    $_SESSION['tripal_feature_seq_extract']['download'] = 1;

    drupal_goto('find/sequences/download');
  }
}
