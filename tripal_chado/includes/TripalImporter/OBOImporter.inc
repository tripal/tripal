<?php

class OBOImporter extends TripalImporter {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'OBO Vocabulary Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_obo_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import vocabularies and terms in OBO format.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['obo'];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the details for importing a new OBO file. The file must have a .obo extension.';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'New OBO File';

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * The method that Tripal attempts to use for this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = FALSE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = TRUE;

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import OBO File';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => FALSE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  /**
   * Be default, all loaders are automaticlly added to the Admin >
   * Tripal > Data Loaders menu.  However, if this loader should be
   * made available via a different menu path, then set it here.  If the
   * value is empty then the path will be the default.
   */
  public static $menu_path = 'admin/tripal/loaders/chado_vocabs/obo_loader';

  public static $file_required = FALSE;


  /**
   * Keep track of vocabularies that have been added.
   *
   * @var array
   */
  private $obo_namespaces = [];


  /**
   * Holds the list of all CVs on this site. By storing them here it saves
   * us query time later.
   */
  private $all_cvs = [];

  /**
   * Holds the list of all DBs on this site.  By storing them here it saves
   * us query time later.
   *
   * @var array
   */
  private $all_dbs = [];

  /**
   * When adding synonyms we need to know the cvterm_ids of the synonym types.
   * This array holds those.
   *
   * @var array
   */
  private $syn_types = [
    'exact' => NULL,
    'broad' => NULL,
    'narrow' => NULL,
    'related' => NULL,
  ];


  // An alternative cache to the temp_obo table.
  private $termStanzaCache = [
    'ids' => [],
    'count' => [
      'Typedef' => 0,
      'Term' => 0,
      'Instance' => 0,
    ],
    'types' => [
      'Typedef' => [],
      'Term' => [],
      'Instance' => [],
    ],
  ];

  /**
   * Indicates how terms are cached. Values can be 'memory' or 'table'. If
   * 'memory' then the $termStanzaCache variable is used. If 'table', then the
   * tripal_obo_temp table is used.
   *
   * @var string
   */
  private $cache_type = 'memory';

  /**
   * The default namespace for all terms that don't have a 'namespace' in their
   * term stanza.
   *
   * @var string
   */
  private $default_namespace = '';

  /**
   * The default database prefix for this ontology.
   *
   * @var string
   */
  private $default_db = '';


  /**
   * An array of used cvterm objects so that we don't have to lookup them
   * up repeatedly.
   */
  private $used_terms = [];


  /**
   * An array of base IRIs returned from the EBI OLS lookup service.  We
   * don't want to continually query OLS for the same ontology base IRIs.
   */
  private $baseIRIs = [];

  /**
   * A flag to keep track if the user was warned about slowness when doing
   * EBI Lookups.
   *
   * @var string
   */
  private $ebi_warned = FALSE;

  /**
   * A flag that indicates if this ontology is just a subset of a much larger
   * one. Examples include the GO slims.
   *
   * @var string
   */
  private $is_subset = FALSE;

  /**
   * Sometimes an OBO can define two terms with the same name but different
   * IDs (e.g. GO:0001404 and GO:0007125). We need to find these and
   * deal with them.  This array keeps track of term names as we see them for
   * easy lookup later.
   *
   * @var array
   */
  private $term_names = [];

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    // get a list of db from chado for user to choose
    $sql = "SELECT * FROM {tripal_cv_obo} ORDER BY name";
    $results = db_query($sql);

    $obos = [];
    $obos[] = 'Select a Vocabulary';
    foreach ($results as $obo) {
      $obos[$obo->obo_id] = $obo->name;
    }

    $obo_id = '';
    if (array_key_exists('values', $form_state)) {
      $obo_id = array_key_exists('obo_id', $form_state['values']) ? $form_state['values']['obo_id'] : '';
    }


    $form['instructions']['info'] = [
      '#type' => 'item',
      '#markup' => t('This page allows you to load vocabularies and ontologies
        that are in OBO format. Once loaded, the terms from these
        vocabularies can be used to create content.
        You may use the form below to either reload a vocabulary that is already
        loaded (as when new updates to that vocabulary are available) or load a new
        vocabulary.'),
    ];

    $form['obo_existing'] = [
      '#type' => 'fieldset',
      '#title' => t('Use a Saved Ontology OBO Reference'),
      '#prefix' => '<span id="obo-existing-fieldset">',
      '#suffix' => '</span>',
    ];

    $form['obo_existing']['existing_instructions'] = [
      '#type' => 'item',
      '#markup' => t('The vocabularies listed in the select box below have been pre-populated
        upon installation of Tripal or have been previously loaded.  Select one to edit
        its settings or submit for loading.  You may reload any vocabulary that has
        already been loaded to retrieve any new updates.'),
    ];

    $form['obo_existing']['obo_id'] = [
      '#title' => t('Ontology OBO File Reference'),
      '#type' => 'select',
      '#options' => $obos,
      '#ajax' => [
        'callback' => 'tripal_cv_obo_form_ajax_callback',
        'wrapper' => 'obo-existing-fieldset',
      ],
      '#description' => t('Select a vocabulary to import.'),
    ];

    // If the user has selected an OBO ID then get the form elements for
    // updating.
    if ($obo_id) {
      $uobo_name = '';
      $uobo_url = '';
      $uobo_file = '';

      $vocab = db_select('tripal_cv_obo', 't')
        ->fields('t', ['name', 'path'])
        ->condition('obo_id', $obo_id)
        ->execute()
        ->fetchObject();

      $uobo_name = $vocab->name;
      if (preg_match('/^http/', $vocab->path)) {
        $uobo_url = $vocab->path;
      }
      else {
        $uobo_file = trim($vocab->path);
        $matches = [];
        if (preg_match('/\{(.*?)\}/', $uobo_file, $matches)) {
          $modpath = drupal_get_path('module', $matches[1]);
          $uobo_file = preg_replace('/\{.*?\}/', $modpath, $uobo_file);
        }
      }
      // We don't want the previous value to remain. We want the new default to
      // show up, so remove the input values
      unset($form_state['input']['uobo_name']);
      unset($form_state['input']['uobo_url']);
      unset($form_state['input']['uobo_file']);

      $form['obo_existing']['uobo_name'] = [
        '#type' => 'textfield',
        '#title' => t('Vocabulary Name'),
        '#description' => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                             list above for use again later.'),
        '#default_value' => $uobo_name,
      ];

      $form['obo_existing']['uobo_url'] = [
        '#type' => 'textfield',
        '#title' => t('Remote URL'),
        '#description' => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                             (e.g. http://www.obofoundry.org/ro/ro.obo)'),
        '#default_value' => $uobo_url,
      ];

      $form['obo_existing']['uobo_file'] = [
        '#type' => 'textfield',
        '#title' => t('Local File'),
        '#description' => t('Please enter the file system path for an OBO
          definition file. If entering a path relative to
          the Drupal installation you may use a relative path that excludes the
          Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
          that Drupal relative paths have no preceding slash.
          Otherwise, please provide the full path on the filesystem.  The path
          must be accessible to the web server on which this Drupal instance is running.'),
        '#default_value' => $uobo_file,
      ];
      $form['obo_existing']['update_obo_details'] = [
        '#type' => 'submit',
        '#value' => 'Update Ontology Details',
        '#name' => 'update_obo_details',
      ];
    }

    $form['obo_new'] = [
      '#type' => 'fieldset',
      '#title' => t('Add a New Ontology OBO Reference'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $form['obo_new']['path_instructions'] = [
      '#value' => t('Provide the name and path for the OBO file.  If the vocabulary OBO file
                     is stored local to the server provide a file name. If the vocabulary is stored remotely,
                     provide a URL.  Only provide a URL or a local file, not both.'),
    ];

    $form['obo_new']['obo_name'] = [
      '#type' => 'textfield',
      '#title' => t('New Vocabulary Name'),
      '#description' => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                             list above for use again later. Additionally, if a default namespace is not provided in the OBO
                             header this name will be used as the default_namespace.'),
    ];

    $form['obo_new']['obo_url'] = [
      '#type' => 'textfield',
      '#title' => t('Remote URL'),
      '#description' => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                             (e.g. http://www.obofoundry.org/ro/ro.obo)'),
    ];

    $form['obo_new']['obo_file'] = [
      '#type' => 'textfield',
      '#title' => t('Local File'),
      '#description' => t('Please enter the file system path for an OBO
          definition file. If entering a path relative to
          the Drupal installation you may use a relative path that excludes the
          Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
          that Drupal relative paths have no preceding slash.
          Otherwise, please provide the full path on the filesystem.  The path
          must be accessible to the web server on which this Drupal instance is running.'),
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formSubmit()
   */
  public function formSubmit($form, &$form_state) {
    $obo_id = $form_state['values']['obo_id'];
    $obo_name = trim($form_state['values']['obo_name']);
    $obo_url = trim($form_state['values']['obo_url']);
    $obo_file = trim($form_state['values']['obo_file']);
    $uobo_name = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
    $uobo_url = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
    $uobo_file = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

    // If the user requested to alter the details then do that.
    if ($form_state['clicked_button']['#name'] == 'update_obo_details') {
      $form_state['rebuild'] = TRUE;
      $success = db_update('tripal_cv_obo')
        ->fields([
          'name' => $uobo_name,
          'path' => $uobo_url ? $uobo_url : $uobo_file,
        ])
        ->condition('obo_id', $obo_id)
        ->execute();
      if ($success) {
        drupal_set_message(t("The vocabulary !vocab has been updated.", ['!vocab' => $uobo_name]));
      }
      else {
        drupal_set_message(t("The vocabulary !vocab could not be updated.", ['!vocab' => $uobo_name]), 'error');
      }

    }
    elseif (!empty($obo_name)) {
      $obo_id = db_insert('tripal_cv_obo')
        ->fields([
          'name' => $obo_name,
          'path' => $obo_url ? $obo_url : $obo_file,
        ])
        ->execute();

      // Add the obo_id to the form_state values.
      $form_state['values']['obo_id'] = $obo_id;

      if ($obo_id) {
        drupal_set_message(t("The vocabulary !vocab has been added.", ['!vocab' => $obo_name]));
      }
      else {
        $form_state['rebuild'] = TRUE;
        drupal_set_message(t("The vocabulary !vocab could not be added.", ['!vocab' => $obo_name]), 'error');
      }
    }
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $obo_id = $form_state['values']['obo_id'];
    $obo_name = trim($form_state['values']['obo_name']);
    $obo_url = trim($form_state['values']['obo_url']);
    $obo_file = trim($form_state['values']['obo_file']);
    $uobo_name = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
    $uobo_url = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
    $uobo_file = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

    // Make sure if the name is changed it doesn't conflict with another OBO.
    if ($form_state['clicked_button']['#name'] == 'update_obo_details' or
      $form_state['clicked_button']['#name'] == 'update_load_obo') {
      // Get the current record
      $vocab = db_select('tripal_cv_obo', 't')
        ->fields('t', ['obo_id', 'name', 'path'])
        ->condition('name', $uobo_name)
        ->execute()
        ->fetchObject();
      if ($vocab and $vocab->obo_id != $obo_id) {
        form_set_error('uobo_name', 'The vocabulary name must be different from existing vocabularies');
      }
      // Make sure the file exists. First check if it is a relative path
      $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $uobo_file;
      if (!file_exists($dfile)) {
        if (!file_exists($uobo_file)) {
          form_set_error('uobo_file', t('The specified path, !path, does not exist or cannot be read.'), ['!path' => $dfile]);
        }
      }
      if (!$uobo_url and !$uobo_file) {
        form_set_error('uobo_url', 'Please provide a URL or a path for the vocabulary.');
      }
      if ($uobo_url and $uobo_file) {
        form_set_error('uobo_url', 'Please provide only a URL or a path for the vocabulary, but not both.');
      }
    }
    if ($form_state['clicked_button']['#name'] == 'add_new_obo') {
      // Get the current record
      $vocab = db_select('tripal_cv_obo', 't')
        ->fields('t', ['obo_id', 'name', 'path'])
        ->condition('name', $obo_name)
        ->execute()
        ->fetchObject();
      if ($vocab) {
        form_set_error('obo_name', 'The vocabulary name must be different from existing vocabularies');
      }
      // Make sure the file exists. First check if it is a relative path
      $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo_file;
      if (!file_exists($dfile)) {
        if (!file_exists($obo_file)) {
          form_set_error('obo_file', t('The specified path, !path, does not exist or cannot be read.'), ['!path' => $dfile]);
        }
      }
      if (!$obo_url and !$obo_file) {
        form_set_error('obo_url', 'Please provide a URL or a path for the vocabulary.');
      }
      if ($obo_url and $obo_file) {
        form_set_error('obo_url', 'Please provide only a URL or a path for the vocabulary, but not both.');
      }
    }
  }

  /**
   * @see TripalImporter::run()
   *
   * @param $details
   *   The following arguments are supported:
   *     - obo_id:  (required) The ID of the ontology to be imported.
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $obo_id = $arguments['obo_id'];

    // Make sure the $obo_id is valid
    $obo = db_select('tripal_cv_obo', 'tco')
      ->fields('tco')
      ->condition('obo_id', $obo_id)
      ->execute()
      ->fetchObject();
    if (!$obo) {
      throw new Exception("Invalid OBO ID provided: '$obo_id'.");
    }

    // Get the list of all CVs so we can save on lookups later
    $sql = "SELECT * FROM {cv} CV";
    $cvs = chado_query($sql);
    while ($cv = $cvs->fetchObject()) {
      $this->all_cvs[$cv->name] = $cv;
    }

    // Get the list of all DBs so we can save on lookups later
    $sql = "SELECT * FROM {db} DB";
    $dbs = chado_query($sql);
    while ($db = $dbs->fetchObject()) {
      $this->all_dbs[$db->name] = $db;
    }

    // Get the 'Subgroup' term that we will use for adding subsets.
    $term = chado_get_cvterm(['id' => 'NCIT:C25693']);
    $this->used_terms['NCIT:C25693'] = $term->cvterm_id;

    // Get the 'Comment' term that we will use for adding comments.
    $term = chado_get_cvterm(['id' => 'rdfs:comment']);
    $this->used_terms['rdfs:comment'] = $term->cvterm_id;

    // Make sure we have a 'synonym_type' vocabulary.
    $syn_cv = new ChadoRecord('cv');
    $syn_cv->setValues(['name' => 'synonym_type']);
    $syn_cv->save();
    $this->all_cvs['synonym_type'] = (object) $syn_cv->getValues();

    // Make sure we have a 'synonym_type' database.
    $syn_db = new ChadoRecord('db');
    $syn_db->setValues(['name' => 'synonym_type']);
    $syn_db->save();
    $this->all_dbs['synonym_type'] = (object) $syn_db->getValues();

    // Make sure the synonym types exists in the 'synonym_type' vocabulary.
    foreach (array_keys($this->syn_types) as $syn_type) {
      $syn_dbxref = new ChadoRecord('dbxref');
      $syn_dbxref->setValues([
        'accession' => $syn_type,
        'db_id' => $syn_db->getID(),
      ]);
      $syn_dbxref->save();

      $syn_term = new ChadoRecord('cvterm');
      $syn_term->setValues([
        'name' => $syn_type,
        'cv_id' => $syn_cv->getID(),
      ]);
      if (!$syn_term->find()) {
        $syn_term->setValues([
          'name' => $syn_type,
          'definition' => '',
          'is_obsolete' => 0,
          'cv_id' => $syn_cv->getID(),
          'is_relationshiptype' => 0,
          'dbxref_id' => $syn_dbxref->getID(),
        ]);
        $syn_term->insert();
      }
      $this->syn_types[$syn_type] = (object) $syn_term->getValues();
    }

    // Run the importer!
    $this->loadOBO_v1_2_id($obo);
  }

  /**
   * @see TripalImporter::postRun()
   *
   */
  public function postRun() {

    // Clear the cached terms
    cache_clear_all('tripal_chado:term:*', 'cache', TRUE);

    // Update the cv_root_mview materialized view.
    $this->logMessage("Updating the cv_root_mview materialized view...");
    $mview_id = tripal_get_mview_id('cv_root_mview');
    tripal_populate_mview($mview_id);

    $this->logMessage("Updating the db2cv_mview materialized view...");
    $mview_id = tripal_get_mview_id('db2cv_mview');
    tripal_populate_mview($mview_id);

    // Update the cvtermpath table for each newly added CV.
    $this->logMessage("Updating cvtermpath table.  This may take a while...");
    foreach ($this->obo_namespaces as $namespace => $cv_id) {
      $this->logMessage("- Loading paths for vocabulary: @vocab", ['@vocab' => $namespace]);
      chado_update_cvtermpath($cv_id, $this->job);
    }
  }

  /**
   * A wrapper function for importing the user specified OBO file into Chado by
   * specifying the obo_id of the OBO. It requires that the file be in OBO v1.2
   * compatible format.  This function is typically executed via the Tripal
   * jobs
   * management after a user submits a job via the Load Ontologies form.
   *
   * @param $obo_id
   *   An obo_id from the tripal_cv_obo file that specifies which OBO file to
   *   import
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_id($obo) {

    // Convert the module name to the real path if present
    if (preg_match("/\{(.*?)\}/", $obo->path, $matches)) {
      $module = $matches[1];
      $path = drupal_realpath(drupal_get_path('module', $module));
      $obo->path = preg_replace("/\{.*?\}/", $path, $obo->path);
    }

    // if the reference is for a remote URL then run the URL processing function
    if (preg_match("/^https:\/\//", $obo->path) or
      preg_match("/^http:\/\//", $obo->path) or
      preg_match("/^ftp:\/\//", $obo->path)) {
      $this->loadOBO_v1_2_url($obo->name, $obo->path, 0);
    }
    // if the reference is for a local file then run the file processing function
    else {
      // check to see if the file is located local to Drupal
      $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo->path;
      if (file_exists($dfile)) {
        $this->loadOBO_v1_2_file($obo->name, $dfile, 0);
      }
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      else {
        if (file_exists($obo->path)) {
          $this->loadOBO_v1_2_file($obo->name, $obo->path, 0);
        }
        else {
          print "ERROR: could not find OBO file: '$obo->path'\n";
        }
      }
    }
  }

  /**
   * A wrapper function for importing the user specified OBO file into Chado by
   * specifying the filename and path of the OBO. It requires that the file be
   * in OBO v1.2 compatible format.  This function is typically executed via
   * the Tripal jobs management after a user submits a job via the Load
   * Ontologies form.
   *
   * @param $obo_name
   *   The name of the OBO (typically the ontology or controlled vocabulary
   *   name)
   * @param $file
   *   The path on the file system where the ontology can be found
   * @param $is_new
   *   Set to TRUE if this is a new ontology that does not yet exist in the
   *   tripal_cv_obo table.  If TRUE the OBO will be added to the table.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_file($obo_name, $file, $is_new = TRUE) {
    if ($is_new) {
      tripal_insert_obo($obo_name, $file);
    }

    $success = $this->loadOBO_v1_2($file, $obo_name);
  }

  /**
   * A wrapper function for importing the user specified OBO file into Chado by
   * specifying the remote URL of the OBO. It requires that the file be in OBO
   * v1.2 compatible format.  This function is typically executed via the
   * Tripal jobs management after a user submits a job via the Load Ontologies
   * form.
   *
   * @param $obo_name
   *   The name of the OBO (typically the ontology or controlled vocabulary
   *   name)
   * @param $url
   *   The remote URL of the OBO file.
   * @param $is_new
   *   Set to TRUE if this is a new ontology that does not yet exist in the
   *   tripal_cv_obo table.  If TRUE the OBO will be added to the table.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_url($obo_name, $url, $is_new = TRUE) {

    // first download the OBO
    $temp = tempnam(sys_get_temp_dir(), 'obo_');
    print "Downloading URL $url, saving to $temp\n";
    $url_fh = fopen($url, "r");
    $obo_fh = fopen($temp, "w");
    if (!$url_fh) {
      throw new Exception("Unable to download the remote OBO file at $url. Could a firewall be blocking outgoing connections? " .
        " if you are unable to download the file you may manually download the OBO file and use the web interface to " .
        " specify the location of the file on your server.");

    }
    while (!feof($url_fh)) {
      fwrite($obo_fh, fread($url_fh, 255), 255);
    }
    fclose($url_fh);
    fclose($obo_fh);

    if ($is_new) {
      tripal_insert_obo($obo_name, $url);
    }

    // second, parse the OBO
    $this->loadOBO_v1_2($temp, $obo_name);

    // now remove the temp file
    unlink($temp);
  }

  /**
   * Imports a given OBO file into Chado.  This function is usually called by
   * one of three wrapper functions:  loadOBO_v1_2_id,
   * loadOBO_v1_2_file or tripal_cv_load_obo_v1_2_url. But, it can
   * be called directly if the full path to an OBO file is available on the
   * file system.
   *
   * @param $file
   *   The full path to the OBO file on the file system
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2($file, $obo_name) {
    $header = [];
    $ret = [];

    // Empty the temp table.
    $this->clearTermStanzaCache();

    // Parse the obo file.
    $this->logMessage("Step 1: Preloading File $file...");
    $this->parse($file, $header);

    // Cache the relationships of terms.
    $this->logMessage("Step 2: Examining relationships...");
    $this->cacheRelationships();

    // Add any typedefs to the vocabulary first.
    $this->logMessage("Step 3: Loading type defs...");
    $this->processTypeDefs();

    // Next add terms to the vocabulary.
    $this->logMessage("Step 4: Loading terms...");
    $this->processTerms();

    // Empty the term cache.
    $this->logMessage("Step 5: Cleanup...");
    $this->clearTermStanzaCache();
  }

  /**
   * OBO files are divided into a typedefs terms section and vocabulary terms
   * section. This function loads the typedef terms from the OBO.
   *
   * @ingroup tripal_obo_loader
   */
  private function processTypeDefs() {

    $typedefs = $this->getCachedTermStanzas('Typedef');
    $count = $this->getCacheSize('Typedef');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(5);

    $i = 1;
    foreach ($typedefs as $t) {

      // TODO: it would be better if we had a term iterator so that we
      // don't have to distinguish here between the table vs memory cache type.
      if ($this->cache_type == 'table') {
        $stanza = unserialize(base64_decode($t->stanza));
      }
      else {
        $stanza = $this->termStanzaCache['ids'][$t];
      }
      $this->setItemsHandled($i++);
      $this->processTerm($stanza, TRUE);
    }

    $this->setItemsHandled($i);
    return 1;
  }

  /**
   * This function loads all of the [Term] terms from the OBO.
   */
  private function processTerms() {
    $i = 0;
    $external = FALSE;

    $terms = $this->getCachedTermStanzas('Term');
    $count = $this->getCacheSize('Term');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(1);

    // Iterate through the terms.
    foreach ($terms as $t) {
      // TODO: it would be better if we had a term iterator so that we
      // don't have to distinguish here between the table vs memory cache type.
      if ($this->cache_type == 'table') {
        $term = unserialize(base64_decode($t->stanza));
      }
      else {
        $term = $this->termStanzaCache['ids'][$t];
      }
      $this->setItemsHandled($i);

      // Add/update this term.
      $this->processTerm($term, FALSE);
      $i++;
    }
    $this->setItemsHandled($i);
    return 1;
  }

  /**
   * Sets the default CV and DB for this loader.
   *
   * Unfortunately, not all OBOs include both the 'ontology' and the
   * 'default-namespace' in their headers, so we have to do our best to
   * work out what these two should be.
   *
   */
  private function setDefaults($header) {
    $short_name = '';
    $namespace = '';

    // Get the 'ontology' and 'default-namespace' headers.  Unfortunately,
    // not all OBO files contain these.
    if (array_key_exists('ontology', $header)) {
      $short_name = strtoupper($header['ontology'][0]);
    }
    if (array_key_exists('default-namespace', $header)) {
      $namespace = $header['default-namespace'][0];
    }

    // The OBO specification allows the 'ontology' header tag to be nested for
    // subsets (e.g. go/subsets/goslim_plant).  We need to simplify that down
    // to the top-level item.
    $matches = [];
    if (preg_match('/^(.+?)\/.*/', $short_name, $matches)) {
      $short_name = $matches[1];
      $this->is_subset = TRUE;
    }

    // If we have the DB short name (or ontology header) but not the default
    // namespace then we may be able to find it via an EBI lookup.
    if (!$namespace and $short_name) {
      $namespace = $this->findEBIOntologyNamespace($short_name);
    }

    // If we have the namespace but not the short name then we have to
    // do a few tricks to try and find it.
    if ($namespace and !$short_name) {

      // First see if we've seen this ontology before and get it's currently
      // loaded database.
      $sql = "SELECT dbname FROM {db2cv_mview} WHERE cvname = :cvname";
      $short_name = chado_query($sql, [':cvname' => $namespace])->fetchField();

      if (!$short_name and array_key_exists('namespace-id-rule', $header)) {
        $matches = [];
        if (preg_match('/^.*\s(.+?):.+$/', $header['namespace-id-rule'][0], $matches)) {
          $short_name = $matches[1];
        }
      }

      // Try the EBI Lookup: still experimental.
      if (!$short_name) {
        //$short_name = $this->findEBIOntologyPrefix($namespace);
      }
    }

    // If we can't find the namespace or the short_name then bust.
    if (!$namespace and !$short_name) {
      throw new ErrorException('Cannot determine the namespace or ontology prefix from this OBO file. It is missing both the "default-namespace" and "ontology" headers.');
    }

    // Set the defaults.
    $this->default_namespace = $namespace;
    $this->default_db = $short_name;
    $this->addDB($this->default_db);
    $cv = $this->addCV($this->default_namespace);
    $this->obo_namespaces[$namespace] = $cv->cv_id;


  }

  /**
   * This function searches EBI to find the ontology details for this OBO.
   *
   * @param $ontology
   *   The ontology name from the OBO headers.
   *
   * @throws Exception
   */
  private function findEBIOntologyNamespace($ontology) {

    // Check if the EBI ontology search has this ontology:
    try {
      $results = $this->oboEbiLookup($ontology, 'ontology');
      if (array_key_exists('default-namespace', $results['config']['annotations'])) {
        $namespace = $results['config']['annotations']['default-namespace'];
        if (is_array($namespace)) {
          $namespace = $namespace[0];
        }
      }
      elseif (array_key_exists('namespace', $results['config'])) {
        $namespace = $results['config']['namespace'];
      }
      // If we can't find the namespace at EBI, then just default to using the
      // same namespace as the DB short name.
      else {
        $namespace = $this->default_db;
      }

      return $namespace;
    } catch (Exception $e) {
      watchdog_exception('Cannot find the namespace for this ontology.', $e);
      throw $e;
    }
  }

  /**
   * Finds the ontology prefix (DB short name) using EBI.
   *
   * @param $namespace
   *   The namespace for ontology.
   */
  private function findEBIOntologyPrefix($namespace) {

    // NOTE: this code is not yet completed.. It's not clear it will
    // actually work.

    $options = [];
    $page = 1;
    $size = 25;
    $full_url = 'https://www.ebi.ac.uk/ols/api/ontologies?page=' . $page . '&size=' . $size;
    while ($response = drupal_http_request($full_url, $options)) {
      $response = drupal_json_decode($response->data);
      foreach ($response['_embedded']['ontologies'] as $ontology) {
        $namespace = $ontology['config']['namespace'];
      }
      $page++;
      $full_url = 'https://www.ebi.ac.uk/ols/api/ontologies?page=' . $page . '&size=' . $size;
    }
  }

  /**
   * A helper function to get details about a foreign term.
   *
   * A foreign term is one that does not belong to the ontology.
   *
   * @param $t
   *   A term array that contains these keys at a minimum: id, name,
   *   definition, subset, namespace, is_obsolete.
   */
  private function findEBITerm($id) {

    // Warn the user if we're looking up terms in EBI as this will slow the
    // loader if there are many lookups.
    if ($this->ebi_warned == FALSE) {
      $this->logMessage(
        "A term that belongs to another ontology is used within this " .
        "vocabulary.  Therefore a lookup will be performed with the EBI Ontology " .
        "Lookup Service to retrieve the information for this term. " .
        "Please note, that vocabularies with many non-local terms " .
        "require remote lookups and these lookups can dramatically " .
        "decrease loading time. ",
        ['!vocab' => $this->default_namespace], TRIPAL_WARNING);
      $this->ebi_warned = TRUE;

      // This ontology may have multiple remote terms and that takes a while
      // to load so lets change the progress interval down to give
      // updates more often.
      $this->setInterval(1);
    }

    $this->logMessage("Performing EBI OLS Lookup for: !id", ['!id' => $id]);

    // Get the short name and accession for the term.
    $pair = explode(":", $id, 2);
    $short_name = $pair[0];
    $accession = $pair[1];

    // First get the ontology so we can build an IRI for the term
    $base_iri = '';
    $ontologyID = '';
    if (array_key_exists($short_name, $this->baseIRIs)) {
      list($ontologyID, $base_iri) = $this->baseIRIs[$short_name];
    }
    else {
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $short_name;
      $response = drupal_http_request($full_url, []);
      if (!$response) {
        throw new Exception(t('Did not get a response from EBI OLS trying to lookup ontology: !ontology',
          ['!ontology' => $short_name]));
      }
      $ontology_results = drupal_json_decode($response->data);
      if ($ontology_results['error']) {

        $this->logMessage(t('Cannot find the ontology via an EBI OLS lookup: !short_name. \n' .
          'We tried to access: !url' .
          'EBI Reported: !message. ' .
          'Consider finding the OBO file for this ontology and manually loading it first.',
          [
            '!message' => $ontology_results['message'],
            '!short_name' => $short_name,
            '!url' => $full_url,
          ]), TRIPAL_WARNING);
      }
      //What should happen with this stuff?
      $base_iri = $ontology_results['config']['baseUris'][0];
      $ontologyID = $ontology_results['ontologyId'];
      $this->baseIRIs[$short_name] = [$ontologyID, $base_iri];
    }

    // Next get the term.
    $iri = urlencode(urlencode($base_iri . $accession));
    $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontologyID . '/terms/' . $iri;
    $response = drupal_http_request($full_url, []);
    if (!$response) {
      throw new Exception(t('Did not get a response from EBI OLS trying to lookup term: !id',
        ['!id' => $id]));
    }
    $results = drupal_json_decode($response->data);
    if (!$results) {
      $this->logMessage('Error: no data with !url.  The response was: !response', [
        '!url' => $full_url,
        '!response' => $response,
      ]);

      return FALSE;
    }

    // If EBI sent an error message then throw an error.
    if ($results['error']) {
      $this->logMessage('Cannot find the term via an EBI OLS lookup: !term. ' .
        'We tried to access: !url' .
        'EBI Reported: !message. Consider finding the OBO file for this ontology and manually loading it first.',
        [
          '!message' => $results['message'],
          '!term' => $id,
          '!url' => $full_url,
        ], TRIPAL_WARNING);

      return FALSE;
    }

    // TODO: what do we do if the term is not defined by this ontology?
    if ($results['is_defining_ontology'] != 1) {

    }

    // Make an OBO stanza array as if this term were in the OBO file and
    // return it.
    $this->logMessage("Found !term in EBI OLS.", ['!term' => $id]);
    $stanza = [];
    $stanza['id'][0] = $id;
    $stanza['name'][0] = $results['label'];
    $stanza['def'][0] = $results['def'];
    $stanza['namespace'][0] = $results['ontology_name'];
    $stanza['is_obsolete'][0] = $results['is_obsolete'] ? 'true' : '';
    $stanza['is_relationshiptype'][0] = '';
    $stanza['db_name'][0] = $short_name;
    $stanza['comment'][0] = 'Term obtained using the EBI Ontology Lookup Service.';
    if (array_key_exists('in_subset', $results)) {
      if (is_array($results['in_subset'])) {
        $stanza['subset'] = $results['in_subset'];
      }
      elseif ($results['in_subset']) {
        $stanza['subset'][0] = $results['in_subset'];
      }
    }

    // If this term has been replaced then get the new term.
    if (array_key_exists('term_replaced_by', $results) and isset($results['term_replaced_by'])) {
      $replaced_by = $results['term_replaced_by'];
      $replaced_by = preg_replace('/_/', ':', $replaced_by);
      $this->logMessage("The term, !term, is replaced by, !replaced",
        ['!term' => $id, '!replaced' => $replaced_by]);

      // Before we try to look for the replacement term, let's try to find it.
      // in our list of cached terms.
      if (array_key_exists($replaced_by, $this->termStanzaCache['ids'])) {
        $this->logMessage("Found term, !replaced in the term cache.",
          ['!term' => $id, '!replaced' => $replaced_by]);
        return $this->termStanzaCache['ids'][$id];
      }

      // Next look in the database.
      $rpair = explode(":", $replaced_by, 2);
      $found = $this->lookupTerm($rpair[0], $rpair[1]);
      if ($found) {
        $this->logMessage("Found term, !replaced in the local data store.",
          ['!term' => $id, '!replaced' => $replaced_by]);
        return $found;
      }

      // Look for this new term.
      $stanza = $this->findEBITerm($replaced_by);
    }
    return $stanza;
  }

  /**
   * Inserts a new cvterm using the OBO stanza array provided.
   *
   * The stanza passed to this function should always come from the term cache,
   * not directly from the OBO file because the cached terms have been
   * updated to include all necessary values.  This function also removes
   * all properties associated with the term so that those can be added
   * fresh.
   *
   * @param $stanza
   *   An OBO stanza array as returned by getCachedTermStanza().
   * @param $is_relationship
   *   Set to TRUE if this term is a relationship term.
   * @param $update_if_exists
   *   Set to TRUE to update the term if it exists.
   *
   * @return
   *   The cvterm ID.
   */
  private function saveTerm($stanza, $is_relationship = FALSE) {

    // Get the term ID.
    $id = $stanza['id'][0];

    // First check if we've already used this term.
    if (array_key_exists($id, $this->used_terms)) {
      return $this->used_terms[$id];
    }

    // Get the term properties.
    $id = $stanza['id'][0];
    $name = $stanza['name'][0];
    $cvname = $stanza['namespace'][0];
    $dbname = $stanza['db_name'][0];
    $namespace = $stanza['namespace'][0];

    // Does this term ID have both a short name and accession? If so, then
    // separate out these components, otherwise we will use the id as both
    // the id and accession.
    $accession = '';
    $matches = [];
    if (preg_match('/^(.+?):(.*)$/', $id, $matches)) {
      $accession = $matches[2];
    }
    else {
      $accession = $id;
    }

    // Get the definition if there is one.
    $definition = '';
    if (array_key_exists('def', $stanza)) {
      $definition = preg_replace('/^\"(.*)\"/', '\1', $stanza['def'][0]);
    }

    // Set the flag if this term is obsolete.
    $is_obsolete = 0;
    if (array_key_exists('is_obsolete', $stanza)) {
      $is_obsolete = $stanza['is_obsolete'][0] == 'true' ? 1 : 0;
    }

    // Set the flag if this is a relationship type.
    $is_relationshiptype = 0;
    if (array_key_exists('is_relationshiptype', $stanza)) {
      $is_relationshiptype = $stanza['is_relationshiptype'][0] == 'true' ? 1 : 0;
    }

    // Is this term borrowed from another ontology?
    $is_borrowed = $this->isTermBorrowed($stanza);

    // Will hold the cvterm ChadoRecord object.
    $cvterm = NULL;

    // Get the CV and DB objects.
    $cv = $this->all_cvs[$cvname];
    $db = $this->all_dbs[$dbname];

    // If this is set to TRUE then we should insert the term.
    $do_cvterm_insert = TRUE;

    // We need to locate terms using their dbxref. This is because term names
    // can sometimes change, so we don't want to look up the term by it's name.
    // the unique ID which is in the accession will never change.
    $dbxref = new ChadoRecord('dbxref');
    $dbxref->setValues([
      'db_id' => $db->db_id,
      'accession' => $accession,
    ]);
    if ($dbxref->find()) {

      // Does this accession already have a cvterm it's associated with? Then
      // we need to make we will update the name. Names change but accessions
      // always refer to the same term.
      $dbx_cvterm = new ChadoRecord('cvterm');
      $dbx_cvterm->setValues(['dbxref_id' => $dbxref->getID()]);
      if ($dbx_cvterm->find()) {
        $do_cvterm_insert = FALSE;
        $cvterm = $dbx_cvterm;

        // We don't want to do any updates for borrowed terms. Just leave them
        // as they are.
        if (!$is_borrowed) {

          // Let's make sure we don't have a conflict in term naming
          // if we change the name of this term.
          $this->fixTermMismatch($stanza, $dbxref, $cv, $name);

          // Now update this cvterm record.
          $cvterm->setValue('name', $name);
          $cvterm->setValue('definition', $definition);
          $cvterm->setValue('is_obsolete', $is_obsolete);
          $cvterm->setValue('is_relationshiptype', $is_relationshiptype);

          try {
            $cvterm->update();
          } catch (Exception $e) {
            $this->logMessage('Could not update the term, "!term", with name, "!name" for vocabulary, "!vocab". ERROR: !error.',
              [
                '!term' => $id,
                '!name' => $name,
                '!vocab' => $cv->name,
                '!error' => $e->getMessage(),
              ],
              TRIPAL_ERROR);
            throw $e;
          }
        }
      }
    }
    // The dbxref doesn't exist, so let's add it.
    else {
      $dbxref->insert();
    }

    // Add the cvterm if we didn't do an update.
    if ($do_cvterm_insert) {

      // Before updating the term let's check to see if it already exists
      // and make corrections.
      $cvterm = new ChadoRecord('cvterm');
      $cvterm->setValue('cv_id', $cv->cv_id);
      $cvterm->setValue('name', $name);
      if ($cvterm->find()) {
        $fixed = $this->fixTermMismatch($stanza, $dbxref, $cv, $name);
      }

      // The term doesn't exist, so let's just do our insert.
      $cvterm->setValues([
        'cv_id' => $cv->cv_id,
        'name' => $name,
        'definition' => $definition,
        'dbxref_id' => $dbxref->getID(),
        'is_relationshiptype' => $is_relationshiptype,
        'is_obsolete' => $is_obsolete,
        'dbxref_id' => $dbxref->getValue('dbxref_id'),
      ]);

      // If the insert fails lets catch the error so we can
      // give a more informative message.
      try {
        $cvterm->insert();
      } catch (Exception $e) {
        $this->logMessage('Could not insert the term, "!term", with name, "!name" for vocabulary, "!vocab". ERROR: !error.',
          [
            '!term' => $id,
            '!name' => $name,
            '!vocab' => $cv->name,
            '!error' => $e->getMessage(),
          ],
          TRIPAL_ERROR);
        throw $e;
      }
    }

    // Save the cvterm_id for this term so we don't look it up again.
    $cvterm_id = $cvterm->getID();
    $this->used_terms[$id] = $cvterm_id;

    // Return the cvterm_id.
    return $cvterm_id;
  }

  /**
   * Fixes mismatches between two terms with the same name.
   *
   * If it has been determined that a term's name has changed. Before we update
   * or insert it we must check to make sure no other terms have that name. If
   * they do we must make a correction.
   *
   * @param $dbxref
   *   The ChadoRecord object containing the dbxref record for the term
   *   to be inserted/updated.
   * @param $cv
   *   The cvterm object.
   * @param $name
   *   The name of the term that is a potential conflict.
   *
   * @return
   *   Returns TRUE if a conflict was found and corrected.
   */
  public function fixTermMismatch($stanza, $dbxref, $cv, $name) {

    $id = $stanza['id'][0];
    $name = $stanza['name'][0];

    // First get the record for any potential conflicting term.
    $sql = "
      SELECT cvterm_id
      FROM {cvterm}
      WHERE name = :name and cv_id = :cv_id and dbxref_id != :dbxref_id
    ";
    $args = [
      ':name' => $name,
      ':cv_id' => $cv->cv_id,
      ':dbxref_id' => $dbxref->getID(),
    ];
    $results = chado_query($sql, $args);
    while ($conflict_id = $results->fetchField()) {
      $check_cvterm = new ChadoRecord('cvterm', $conflict_id);

      // If the dbxref of this matched term is the same as the current term
      // then it is the same term and there is no conflict.
      if ($dbxref->getID() == $check_cvterm->getValue('dbxref_id')) {
        return FALSE;
      }

      // At this point, we have a cvterm with the same name and vocabulary
      // but with a different dbxref. First let's get that other accession.
      $check_dbxref = new ChadoRecord('dbxref', $check_cvterm->getValue('dbxref_id'));
      $check_db = new ChadoRecord('db', $check_dbxref->getValue('db_id'));
      $check_accession = $check_db->getValue('name') . ':' . $check_dbxref->getValue('accession');

      // Case 1:  The other term that currently has the same name is
      // missing in the OBO file (i.e. no stanza).  So, that means that this
      // term probably got relegated to an alt_id on another term.  We do
      // not want to delete a term because it may be linked to other
      // records. Instead, let's update its name to let folks know
      // what happened to it and so we can get around the unique
      // constraint.  An example of this is the GO:0015881 and
      // GO:1902598 terms where the latter became an alt_id of the
      // first and no longer has its own entry.
      $check_stanza = $this->getCachedTermStanza($check_accession);
      if (!$check_stanza) {
        $new_name = $check_cvterm->getValue('name') . ' (' . $check_accession . ')';
        $check_cvterm->setValue('name', $new_name);
        $check_cvterm->setValue('is_obsolete', '1');
        $check_cvterm->update();
        return TRUE;
      }
      // Case 2:  The conflicting term is in the OBO file (ie. has a stanza) and
      // is obsolete and this one is not. Fix it by adding an (obsolete) suffix
      // to the name to avoid the conflict.
      else {
        if (array_key_exists('is_obsolete', $check_stanza) and ($check_stanza['is_obsolete'][0] == 'true') and (!array_key_exists('is_obsolete', $stanza) or ($stanza['is_obsolete'][0] != 'true'))) {
          $new_name = $check_cvterm->getValue('name') . ' (obsolete)';
          $check_cvterm->setValue('name', $new_name);
          $check_cvterm->update();
          return TRUE;
        }
        // Case 3:  The conflicting term is in the OBO file (ie. has a stanza).
        // That means that there has been some name swapping between
        // terms. We need to temporarily rename the term so that
        // we don't have a unique constraint violation when we update
        // the new one.  An example of this is where GO:000425 and
        // GO:0030242 changed names and one was renamed to the previous
        // name of the other.
        else {
          $new_name = $check_cvterm->getValue('name') . ' (' . $check_accession . ')';
          $check_cvterm->setValue('name', $new_name);
          $check_cvterm->update();
          return TRUE;
        }
      }
    }

    // We have no conflict so it's save to update or insert.
    return FALSE;
  }

  /**
   * Uses the provided term array to add/update information to Chado about the
   * term including the term, dbxref, synonyms, properties, and relationships.
   *
   * @param $term
   *   An array representing the cvterm.
   *
   * @is_relationship
   *   Set to 1 if this term is a relationship term
   *
   * @ingroup tripal_obo_loader
   */
  private function processTerm($stanza, $is_relationship = 0) {

    //
    // First things first--save the term.
    //
    // If the term does not exist it is inserted, if it does exist it just
    // retrieves the cvterm_id.
    //
    $cvterm_id = $this->saveTerm($stanza, FALSE);
    $id = $stanza['id'][0];

    // If this term is borrowed from another ontology? If so then we will
    // not update it.
    if ($this->isTermBorrowed($stanza)) {
      return;
    }

    // If this term belongs to this OBO (not borrowed from another OBO) then
    // remove any relationships, properties, xrefs, and synonyms that this
    // term already has so that they can be re-added.
    $sql = "
      DELETE FROM {cvterm_relationship}
      WHERE subject_id = :cvterm_id
    ";
    chado_query($sql, [':cvterm_id' => $cvterm_id]);

    // If this is an obsolete term then clear out the relationships where
    // this term is the object.
    if (in_array('is_obsolete', $stanza) and $stanza['is_obsolete'] == 'true') {
      $sql = "
        DELETE FROM {cvterm_relationship}
        WHERE object_id = :cvterm_id
      ";
      chado_query($sql, [':cvterm_id' => $cvterm_id]);
    }

    $sql = "
      DELETE FROM {cvtermprop}
      WHERE cvterm_id = :cvterm_id
    ";
    chado_query($sql, [':cvterm_id' => $cvterm_id]);

    $sql = "
      DELETE FROM {cvterm_dbxref}
      WHERE cvterm_id = :cvterm_id
    ";
    chado_query($sql, [':cvterm_id' => $cvterm_id]);

    $sql = "
      DELETE FROM {cvtermsynonym} CVTSYN
      WHERE cvterm_id = :cvterm_id
    ";
    chado_query($sql, [':cvterm_id' => $cvterm_id]);

    // We should never have the problem where we don't have a cvterm_id. The
    // saveTerm() function should always return on.  But if for some unknown
    // reason we don't have one then fail.
    if (!$cvterm_id) {
      throw new Exception(t('Missing cvterm after saving term: !term',
        ['!term' => print_r($stanza, TRUE)]));
    }

    //
    // Handle:  alt_id
    //
    if (array_key_exists('alt_id', $stanza)) {
      foreach ($stanza['alt_id'] as $alt_id) {
        $this->addAltID($id, $cvterm_id, $alt_id);
      }
    }

    //
    // Handle:  synonym
    //
    if (array_key_exists('synonym', $stanza)) {
      foreach ($stanza['synonym'] as $synonym) {
        $this->addSynonym($id, $cvterm_id, $synonym);
      }
    }

    //
    // Handle:  exact_synonym
    //
    if (array_key_exists('exact_synonym', $stanza)) {
      foreach ($stanza['exact_synonym'] as $synonym) {
        $fixed = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 EXACT $2', $synonym);
        $this->addSynonym($id, $cvterm_id, $fixed);
      }
    }

    //
    // Handle: narrow_synonym
    //
    if (array_key_exists('narrow_synonym', $stanza)) {
      foreach ($stanza['narrow_synonym'] as $synonym) {
        $fixed = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 NARROW $2', $synonym);
        $this->addSynonym($id, $cvterm_id, $fixed);
      }
    }

    //
    // Handle:  broad_synonym
    //
    if (array_key_exists('broad_synonym', $stanza)) {
      foreach ($stanza['broad_synonym'] as $synonym) {
        $fixed = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 BROAD $2', $synonym);
        $this->addSynonym($id, $cvterm_id, $fixed);
      }
    }

    //
    // Handle:  comment
    //
    if (array_key_exists('comment', $stanza)) {
      $comments = $stanza['comment'];
      foreach ($comments as $rank => $comment) {
        $this->addComment($id, $cvterm_id, $comment, $rank);
      }
    }

    //
    // Handle:  xref
    //
    if (array_key_exists('xref', $stanza)) {
      foreach ($stanza['xref'] as $xref) {
        $this->addXref($id, $cvterm_id, $xref);
      }
    }

    //
    // Handle:  xref_analog
    //
    if (array_key_exists('xref_analog', $stanza)) {
      foreach ($stanza['xref_analog'] as $xref) {
        $this->addXref($id, $cvterm_id, $xref);
      }
    }

    //
    // Handle:  xref_unk
    //
    if (array_key_exists('xref_unk', $stanza)) {
      foreach ($stanza['xref_unk'] as $xref) {
        $this->addXref($id, $cvterm_id, $xref);
      }
    }

    //
    // Handle:  subset
    //
    if (array_key_exists('subset', $stanza)) {
      foreach ($stanza['subset'] as $subset) {
        $this->addSubset($id, $cvterm_id, $subset);
      }
    }

    //
    // Handle:  is_a
    //
    if (array_key_exists('is_a', $stanza)) {
      foreach ($stanza['is_a'] as $is_a) {
        $this->addRelationship($id, $cvterm_id, 'is_a', $is_a);
      }
    }

    //
    // Handle:  relationship
    //
    if (array_key_exists('relationship', $stanza)) {
      foreach ($stanza['relationship'] as $value) {
        $rel = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
        $object = preg_replace('/^.+?\s(.+?)$/', '\1', $value);
        $this->addRelationship($id, $cvterm_id, $rel, $object);
      }
    }


    /**
     * The following properties are currently unsupported:
     *
     * - intersection_of
     * - union_of
     * - disjoint_from
     * - replaced_by
     * - consider
     * - use_term
     * - builtin
     * - is_anonymous
     *
     */
  }

  /**
   * Adds a cvterm relationship
   *
   * @param $cvterm_id
   *   A cvterm_id of the term to which the relationship will be added.
   * @param $rel_id
   *   The relationship term ID
   * @param $obj_id
   *   The relationship object term ID.
   *
   * @ingroup tripal_obo_loader
   */
  private function addRelationship($id, $cvterm_id, $rel_id, $obj_id) {

    // Get the cached terms for both the relationship and the object. They
    // should be there, but just in case something went wrong, we'll throw
    // an exception if we can't find them.
    $rel_stanza = $this->getCachedTermStanza($rel_id);
    if (!$rel_stanza) {
      throw new Exception(t('Cannot add relationship: "!source !rel !object". ' .
        'The term, !rel, is not in the term cache.',
        ['!source' => $id, '!rel' => $rel_id, '!name' => $obj_id]));
    }
    $rel_cvterm_id = $this->saveTerm($rel_stanza, TRUE);

    // Make sure the object term exists in the cache.
    $obj_stanza = $this->getCachedTermStanza($obj_id);
    if (!$obj_stanza) {
      throw new Exception(t('Cannot add relationship: "!source !rel !object". ' .
        'The term, !object, is not in the term cache.',
        ['!source' => $id, '!rel' => $rel_id, '!object' => $obj_id]));
    }
    $obj_cvterm_id = $this->saveTerm($obj_stanza);

    // Add the cvterm_relationship.
    $cvterm_relationship = new ChadoRecord('cvterm_relationship');
    $cvterm_relationship->setValues([
      'type_id' => $rel_cvterm_id,
      'subject_id' => $cvterm_id,
      'object_id' => $obj_cvterm_id,
    ]);

    // If the insert fails then catch the error and generate a more meaningful
    // message that helps with debugging.
    try {
      $cvterm_relationship->insert();
    } catch (Exception $e) {
      throw new Exception(t('Cannot add relationship: "!source !rel !object". ' .
        'ERROR: !error.',
        [
          '!source' => $id,
          '!rel' => $rel_id,
          '!object' => $obj_id,
          '!error' => $e->getMessage(),
        ]));
    }
  }

  /**
   * Retrieves the term array from the temp loading table for a given term id.
   *
   * @param id
   *   The id of the term to retrieve
   *
   * @ingroup tripal_obo_loader
   */
  private function getCachedTermStanza($id) {
    if ($this->cache_type == 'table') {
      $values = ['id' => $id];
      $result = chado_select_record('tripal_obo_temp', ['stanza'], $values);
      if (count($result) == 0) {
        return FALSE;
      }
      return unserialize(base64_decode($result['stanza']));
    }

    if (array_key_exists($id, $this->termStanzaCache['ids'])) {
      return $this->termStanzaCache['ids'][$id];
    }
    else {
      return FALSE;
    }
  }

  /**
   * Using the term's short-name and accession try to find it in Chado.
   *
   * @param $short_name
   *   The term's ontology prefix (database short name)
   * @param $accession
   *   The term's accession.
   *
   * @return array|NULL
   */
  private function lookupTerm($short_name, $accession) {

    // Does the database already exist?
    if (!array_key_exists($short_name, $this->all_dbs)) {
      return NULL;
    }
    $db = $this->all_dbs[$short_name];

    // Check if the dbxref exists.
    $dbxref = new ChadoRecord('dbxref');
    $dbxref->setValues([
      'db_id' => $db->db_id,
      'accession' => $accession,
    ]);
    if (!$dbxref->find()) {
      return NULL;
    }

    // If the dbxref exists then see if it has a corresponding cvterm.
    $cvterm = new ChadoRecord('cvterm');
    $cvterm->setValues(['dbxref_id' => $dbxref->getID()]);
    if (!$cvterm->find()) {
      return NULL;
    }

    // Get the CV for this term.
    $cv = new ChadoRecord('cv');
    $cv->setValues(['cv_id' => $cvterm->getValue('cv_id')]);
    $cv->find();

    // Create a new stanza using the values of this cvterm.
    $stanza = [];
    $stanza['id'][0] = $short_name . ':' . $accession;
    $stanza['name'][0] = $cvterm->getValue('name');
    $stanza['def'][0] = $cvterm->getValue('definition');
    $stanza['namespace'][0] = $cv->getValue('name');
    $stanza['is_obsolete'][0] = $cvterm->getValue('is_obsolete') == 1 ? 'true' : '';
    $stanza['is_relationshiptype'][0] = '';
    $stanza['db_name'][0] = $db->name;
    $stanza['cv_name'][0] = $cv->getValue('name');
    return $stanza;
  }

  /**
   * Adds a term stanza from the OBO file to the cache for easier lookup.
   *
   * @param $stanza
   *   The stanza from the OBO file for the term.
   *
   * @throws Exception
   */
  private function cacheTermStanza($stanza, $type) {

    // Make sure we have defaults.
    if (!$this->default_namespace) {
      throw new Exception('Cannot cache terms without a default CV.' . print_r($stanza, TRUE));
    }
    if (!$this->default_db) {
      throw new Exception('Cannot cache terms without a default DB.' . print_r($stanza, TRUE));
    }

    $id = $stanza['id'][0];

    // First check if this term is already in the cache, if so then skip it.
    if ($this->getCachedTermStanza($id)) {
      return;
    }

    // Does this term have a database short name prefix in the ID (accession)?
    // If not then we'll add the default CV as the namespace. If it does and
    // the short name is not the default for this vocabulary then we'll look
    // it up.
    $matches = [];
    if (preg_match('/^(.+):(.+)$/', $id, $matches)) {
      $short_name = $matches[1];
      $accession = $matches[2];

      // If the term is borrowed then let's try to deal with it.
      if ($short_name != $this->default_db) {

        // First try to lookup the term and replace the stanza with the updated
        // details.
        $found = $this->lookupTerm($short_name, $accession);

        if ($found) {
          $stanza = $found;
        }
        // If we can't find the term in the database then do an EBI lookup.
        else {
          $stanza = $this->findEBITerm($id);

          if (!$stanza) {
            return FALSE;
          }

          // Make sure the DBs and CVs exist and are added to our cache.
          $this->addDB($stanza['db_name'][0]);
          $this->addCV($stanza['namespace'][0]);
        }
      }
      // If the term belongs to this OBO then let's set the 'db_name'.
      else {
        if (!array_key_exists('namespace', $stanza)) {
          $stanza['namespace'][0] = $this->default_namespace;
        }
        $stanza['db_name'][0] = $short_name;
      }

      // Make sure the db for this term is added to Chado. If it already is
      // then this function won't re-add it.
      $this->addDB($short_name);
    }
    // If there is no DB short name prefix for the id.
    else {
      if (!array_key_exists('namespace', $stanza)) {
        $stanza['namespace'][0] = $this->default_namespace;
      }
      $stanza['db_name'][0] = $this->default_db;
    }

    $stanza['is_relationshiptype'][0] = '';
    if ($type == 'Typedef') {
      $stanza['is_relationshiptype'][0] = 'true';
    }

    // The is_a field can have constructs like this:  {is_inferred="true"}
    // We need to remove those if they exist.
    if (array_key_exists('is_a', $stanza)) {
      foreach ($stanza['is_a'] as $index => $is_a) {
        $stanza['is_a'][$index] = trim(preg_replace('/\{.+?\}/', '', $is_a));
      }
    }
    if (array_key_exists('relationship', $stanza)) {
      foreach ($stanza['relationship'] as $index => $relationship) {
        $stanza['relationship'][$index] = trim(preg_replace('/\{.+?\}/', '', $relationship));
      }
    }

    // Clean up any synonym definitions. We only handle the synonym in
    // quotes and the type.
    if (array_key_exists('synonym', $stanza)) {
      foreach ($stanza['synonym'] as $index => $synonym) {
        if (preg_match('/\"(.*?)\".*(EXACT|NARROW|BROAD|RELATED)/', $synonym, $matches)) {
          $stanza['synonym'][$index] = '"' . $matches[1] . '" ' . $matches[2];
        }
      }
    }

    // Now before saving, remove any duplicates.  Sometimes the OBOs have
    // the same item duplicated in the stanza multiple times. This will
    // result in duplicate constraint violations in the tables. We can either
    // check on every insert if the record exists increasing loading time or
    // remove duplicates here.
    foreach ($stanza as $key => $values) {
      $stanza[$key] = array_unique($values);
    }

    // If we should use the cache_type is to cache in the tripal_obo_temp
    // table then handle that now.
    if ($this->cache_type == 'table') {
      // Add the term to the temp table.
      $values = [
        'id' => $id,
        'stanza' => base64_encode(serialize($stanza)),
        'type' => $type,
      ];
      $success = chado_insert_record('tripal_obo_temp', $values);
      if (!$success) {
        throw new Exception("Cannot insert stanza into temporary table.");
      }
      return;
    }

    // Cache the term stanza
    $this->termStanzaCache['ids'][$id] = $stanza;
    $this->termStanzaCache['count'][$type]++;
    $this->termStanzaCache['types'][$type][] = $id;

    // Cache the term name so we don't have conflicts.
    $name = $stanza['name'][0];
    $this->term_names[$name] = 1;

  }

  /**
   * Returns the size of a given term type from the cache.
   *
   * @param $type
   *   The term type: Typedef, Term, etc.
   */
  private function getCacheSize($type) {
    if ($this->cache_type == 'table') {
      $sql = "
        SELECT count(*) as num_terms
        FROM {tripal_obo_temp}
        WHERE type = :type
      ";
      $result = chado_query($sql, [':type' => $type])->fetchObject();
      return $result->num_terms;
    }
    return $this->termStanzaCache['count'][$type];
  }

  /**
   * Retrieves all term IDs for a given type.
   *
   * If the cache is using the tripal_obo_temp table then it
   * returns an iterable Database handle.
   */
  private function getCachedTermStanzas($type) {
    if ($this->cache_type == 'table') {
      $sql = "SELECT id FROM {tripal_obo_temp} WHERE type = 'Typedef' ";
      $typedefs = chado_query($sql);
      return $typedefs;
    }
    return $this->termStanzaCache['types'][$type];
  }

  /**
   * Clear's the term cache.
   */
  private function clearTermStanzaCache() {
    if ($this->cache_type == 'table') {
      $sql = "DELETE FROM {tripal_obo_temp}";
      chado_query($sql);
      return;
    }
    $this->termStanzaCache = [
      'ids' => [],
      'count' => [
        'Typedef' => 0,
        'Term' => 0,
        'Instance' => 0,
      ],
      'types' => [
        'Typedef' => [],
        'Term' => [],
        'Instance' => [],
      ],
    ];
  }

  /**
   * Adds the synonyms to a term
   *
   * @param $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param $synonym
   *   The value of the 'synonym' line of the term stanza.
   *
   * @ingroup tripal_obo_loader
   */
  private function addSynonym($id, $cvterm_id, $synonym) {
    $def = $synonym;
    $syn_type = '';

    // Separate out the synonym definition and type (e.g. EXACT).
    $matches = [];
    if (preg_match('/\"(.*?)\".*(EXACT|NARROW|BROAD|RELATED)/', $synonym, $matches)) {
      $def = $matches[1];
      $syn_type = strtolower($matches[2]);
    }

    // Get the syn type cvterm.
    if (!$syn_type) {
      $syn_type = 'exact';
    }
    $syn_type_term = $this->syn_types[$syn_type];
    if (!$syn_type_term) {
      throw new Exception(t('Cannot find synonym type: !type', ['!type' => $syn_type]));
    }

    // The synonym can only be 255 chars in the cvtermsynonym table.
    // to prevent failing we have to truncate.
    if (strlen($def) > 255) {
      $def = substr($def, 0, 252) . "...";
    }

    // Now save the new synonym.
    $cvtermsynonym = new ChadoRecord('cvtermsynonym');
    $cvtermsynonym->setValues([
      'cvterm_id' => $cvterm_id,
      'synonym' => $def,
    ]);

    // If the insert fails then catch the error and generate a more meaningful
    // message that helps with debugging.
    try {
      // The unique constraint for the cvterm_synonym table is with the
      // cvterm_id and the synonym. It does not include the type_id.
      // The CHEBI contains terms with the same synonym but with different
      // types. For example:
      // synonym: "2-chloro-N-(2-chloroethyl)-N-methylethanamine hydrochloride" EXACT IUPAC_NAME [IUPAC]
      // synonym: "2-chloro-N-(2-chloroethyl)-N-methylethanamine hydrochloride" RELATED [ChemIDplus]
      // In this case on the first one is added.
      // @todo: This is a deficiency with Chado that should be corrected.
      if (!$cvtermsynonym->find()) {
        $cvtermsynonym->setValue('type_id', $syn_type_term->cvterm_id);
        $cvtermsynonym->insert();
      }
    } catch (Exception $e) {
      throw new Exception(t('Cannot add synonym, "!synonym" to term: !id. ' .
        'ERROR: !error.',
        ['!synonym' => $def, '!id' => $id, '!error' => $e->getMessage()]));
    }
  }

  /**
   * Parse the OBO file and populate the templ loading table
   *
   * @param $file
   *   The path on the file system where the ontology can be found
   * @param $header
   *   An array passed by reference that will be populated with the header
   *   information from the OBO file
   *
   * @ingroup tripal_obo_loader
   */
  private function parse($obo_file, &$header) {
    // Set to 1 if we are in the top header lines of the file.
    $in_header = TRUE;
    // Holds the full stanza for the term.
    $stanza = [];
    // Holds the default database for the term.
    $db_short_name = '';
    $line_num = 0;
    $num_read = 0;
    // The type of term:  Typedef or Term (inside the [] brackets]
    $type = '';

    $filesize = filesize($obo_file);
    $this->setTotalItems($filesize);
    $this->setItemsHandled(0);
    $this->setInterval(5);

    // iterate through the lines in the OBO file and parse the stanzas
    $fh = fopen($obo_file, 'r');
    while ($line = fgets($fh)) {
      $line_num++;
      $size = drupal_strlen($line);
      $num_read += $size;
      $line = trim($line);
      $this->setItemsHandled($num_read);

      // remove newlines
      $line = rtrim($line);

      // remove any special characters that may be hiding
      $line = preg_replace('/[^(\x20-\x7F)]*/', '', $line);

      // skip empty lines
      if (strcmp($line, '') == 0) {
        continue;
      }

      // Remove comments from end of lines.
      $line = preg_replace('/^(.*?)\!.*$/', '\1', $line);


      // At the first stanza we're out of header.
      if (preg_match('/^\s*\[/', $line)) {

        // After parsing the header we need to get information about this OBO.
        if ($in_header == TRUE) {
          $this->setDefaults($header);
          $in_header = FALSE;
        }

        // Store the stanza we just finished reading.
        if (sizeof($stanza) > 0) {

          // If this term has a namespace then we want to keep track of it.
          if (array_key_exists('namespace', $stanza)) {
            $namespace = $stanza['namespace'][0];
            $cv = $this->all_cvs[$namespace];
            $this->obo_namespaces[$namespace] = $cv->cv_id;
          }

          // Before caching this stanza, check the term's name to
          // make sure it doesn't conflict. If it does we'll just
          // add the ID to the name to ensure it doesn't.
          if (array_key_exists($stanza['name'][0], $this->term_names)) {
            $new_name = $stanza['name'][0] . '(' . $stanza['id'][0] . ')';
            $stanza['name'][0] = $stanza['name'][0];
          }

          $this->cacheTermStanza($stanza, $type);

        }

        // Get the stanza type:  Term, Typedef or Instance
        $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/', '\1', $line);

        // start fresh with a new array
        $stanza = [];
        continue;
      }

      // break apart the line into the tag and value but ignore any escaped colons
      preg_replace("/\\:/", "|-|-|", $line); // temporarily replace escaped colons
      $pair = explode(":", $line, 2);
      $tag = $pair[0];
      $value = ltrim(rtrim($pair[1]));// remove surrounding spaces

      // if this is the ID line then get the database short name from the ID.
      $matches = [];
      if ($tag == 'id' and preg_match('/^(.+?):.*$/', $value, $matches)) {
        $db_short_name = $matches[1];
      }
      $tag = preg_replace("/\|-\|-\|/", "\:", $tag); // return the escaped colon
      $value = preg_replace("/\|-\|-\|/", "\:", $value);
      if ($in_header) {
        if (!array_key_exists($tag, $header)) {
          $header[$tag] = [];
        }
        $header[$tag][] = $value;
      }
      else {
        if (!array_key_exists($tag, $stanza)) {
          $stanza[$tag] = [];
        }
        $stanza[$tag][] = $value;
      }
    }
    // now add the last term in the file
    if (sizeof($stanza) > 0) {
      // If this term has a namespace then we want to keep track of it.
      if (array_key_exists('namespace', $stanza)) {
        $namespace = $stanza['namespace'][0];
        $cv = $this->all_cvs[$namespace];
        $this->obo_namespaces[$namespace] = $cv->cv_id;
      }
      $this->cacheTermStanza($stanza, $type);
      $this->setItemsHandled($num_read);
    }

    // Make sure there are CV records for all namespaces.
    $message = t('Found the following namespaces: !namespaces.',
      ['!namespaces' => implode(', ', array_keys($this->obo_namespaces))]);
    foreach ($this->obo_namespaces as $namespace => $cv_id) {
      $this->addCV($namespace);
    }
    $this->logMessage($message);
  }

  /**
   * Iterates through all of the cached terms and caches any relationships
   */
  private function cacheRelationships() {

    // Now that we have all of the terms parsed and loaded into the cache,
    // lets run through them one more time cache any terms in relationships
    // as well.
    $terms = $this->getCachedTermStanzas('Term');
    $count = $this->getCacheSize('Term');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(25);

    // Iterate through the terms.
    $i = 1;
    foreach ($terms as $t) {

      // TODO: it would be better if we had a term iterator so that we
      // don't have to distinguish here between the table vs memory cache type.
      if ($this->cache_type == 'table') {
        $stanza = unserialize(base64_decode($t->stanza));
      }
      else {
        $stanza = $this->termStanzaCache['ids'][$t];
      }

      // Check if this stanza has an is_a relationship that needs lookup.
      if (array_key_exists('is_a', $stanza)) {
        foreach ($stanza['is_a'] as $object_term) {
          $rstanza = [];
          $rstanza['id'][] = $object_term;
          $this->cacheTermStanza($rstanza, 'Term');
        }
      }

      // Check if this stanza has any additional relationships for lookup.
      if (array_key_exists('relationship', $stanza)) {
        foreach ($stanza['relationship'] as $value) {

          // Get the relationship term and the object term
          $rel_term = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
          $object_term = preg_replace('/^.+?\s(.+?)$/', '\1', $value);

          $rstanza = [];
          $rstanza['id'][] = $rel_term;
          $this->cacheTermStanza($rstanza, 'Typedef');

          $rstanza = [];
          $rstanza['id'][] = $object_term;
          $this->cacheTermStanza($rstanza, 'Term');
        }
      }
    }
    $this->setItemsHandled($i++);

    // Last of all, we need to add the "is_a" relationship It's part of the
    // OBO specification as a built-in relationship but not all vocabularies
    // include that term.
    if (!$this->getCachedTermStanza('is_a')) {
      $stanza = [];
      $stanza['id'][0] = 'is_a';
      $stanza['name'][0] = 'is_a';
      $stanza['namespace'][0] = $this->default_namespace;
      $stanza['db_name'][0] = $this->default_db;
      $this->cacheTermStanza($stanza, 'Typedef');
    }
  }


  /**
   * Adds a property to the cvterm indicating it belongs to a subset.
   *
   * @param $cvterm_id
   *   The cvterm_id of the term to which the subset will be added.
   * @param $subset
   *   The name of the subset.
   */
  private function addSubset($id, $cvterm_id, $subset) {
    $cvtermprop = new ChadoRecord('cvtermprop');
    $cvtermprop->setValues([
      'cvterm_id' => $cvterm_id,
      'type_id' => $this->used_terms['NCIT:C25693'],
      'value' => $subset,
    ]);

    // If the insert fails then catch the error and generate a more meaningful
    // message that helps with debugging.
    try {
      $cvtermprop->insert();
    } catch (Exception $e) {
      throw new Exception(t('Cannot add subset, "!subset" to term: !id. ' .
        'ERROR: !error.',
        ['!subset' => $subset, '!id' => $id, '!error' => $e->getMessage()]));
    }
  }

  /**
   * Adds a database to Chado if it doesn't exist.
   *
   * @param $dbname
   *   The name of the database to add.
   *
   * @return
   *   A Chado database object.
   */
  private function addDB($dbname) {
    // Add the database if it doesn't exist.
    $db = NULL;
    if (array_key_exists($dbname, $this->all_dbs)) {
      $db = $this->all_dbs[$dbname];
    }
    else {
      // If it's not in the cache we can assume it doesn't exist and insert.
      $db = new ChadoRecord('db');
      $db->setValues(['name' => $dbname]);
      $db->insert();
      $db = (object) $db->getValues();
      $this->all_dbs[$dbname] = $db;
    }
    return $db;
  }

  /**
   * Adds a vocabulary to Chado if it doesn't exist.
   *
   * @param $cvname
   *   The name of the vocabulary to add.
   *
   * @return
   *   A Chado cv object.
   */
  private function addCV($cvname) {
    // TODO: we need to get the description and title from EBI for these
    // ontology so that we can put something in the proper fields when
    // adding a new CV or DB.

    // Add the CV record if it doesn't exist.
    $cv = NULL;
    if (array_key_exists($cvname, $this->all_cvs)) {
      $cv = $this->all_cvs[$cvname];
    }
    else {
      // If it's not in the cache we can assume it doesn't exist and insert.
      $cv = new ChadoRecord('cv');
      $cv->setValues(['name' => $cvname]);
      $cv->insert();
      $cv = (object) $cv->getValues();
      $this->all_cvs[$cvname] = $cv;
      $this->obo_namespaces[$cvname] = $cv->cv_id;
    }

    return $cv;
  }

  /**
   * Indicates if the term belongs to this OBO or if it was borrowed
   * .
   *
   * @param $stanza
   */
  private function isTermBorrowed($stanza) {
    $namespace = $stanza['namespace'][0];
    if (array_key_exists($namespace, $this->obo_namespaces)) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Adds an alternative ID
   *
   * @param $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param $alt_id
   *   The cross reference.  It should be of the form from the OBO specification
   *
   * @ingroup tripal_obo_loader
   */
  private function addAltID($id, $cvterm_id, $alt_id) {

    $dbname = '';
    $accession = '';
    $matches = [];
    if (preg_match('/^(.+?):(.*)$/', $alt_id, $matches)) {
      $dbname = $matches[1];
      $accession = $matches[2];
    }

    if (!$accession) {
      throw new Exception("Cannot add an Alt ID without an accession: '$alt_id'");
    }

    // Add the database if it doesn't exist.
    $db = $this->addDB($dbname);
    $db_id = $db->db_id;

    // Now add the dbxref if it doesn't already exist
    $dbxref = new ChadoRecord('dbxref');
    $dbxref->setValues([
      'db_id' => $db_id,
      'accession' => $accession,
    ]);
    if (!$dbxref->find()) {
      $dbxref->insert();
    }

    // Now add the cvterm_dbxref record.
    $cvterm_dbxref = new ChadoRecord('cvterm_dbxref');
    $cvterm_dbxref->setValues([
      'cvterm_id' => $cvterm_id,
      'dbxref_id' => $dbxref->getID(),
    ]);
    if (!$cvterm_dbxref->find()) {
      $cvterm_dbxref->insert();
    }
  }

  /**
   * Adds a database reference to a cvterm
   *
   * @param $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param xref
   *   The cross reference.  It should be of the form from the OBO specification
   *
   * @ingroup tripal_obo_loader
   */
  private function addXref($id, $cvterm_id, $xref) {

    $dbname = preg_replace('/^(.+?):.*$/', '$1', $xref);
    $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/', '$1', $xref);
    $description = preg_replace('/^.+?\"(.+?)\".*?$/', '$1', $xref);
    $dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/', '$1', $xref);

    if (!$accession) {
      throw new Exception("Cannot add an xref without an accession: '$xref'");
    }

    // If the xref is a database link then skip those for now.
    if (strcmp($dbname, 'http') == 0) {
      return;
    }

    // Add the database if it doesn't exist.
    $db = $this->addDB($dbname);
    $db_id = $db->db_id;

    // Now add the dbxref if it doesn't already exist
    $dbxref = new ChadoRecord('dbxref');
    $dbxref->setValues([
      'db_id' => $db_id,
      'accession' => $accession,
    ]);
    if (!$dbxref->find()) {
      $dbxref->insert();
    }

    // Now add the cvterm_dbxref record.
    $cvterm_dbxref = new ChadoRecord('cvterm_dbxref');
    $cvterm_dbxref->setValues([
      'cvterm_id' => $cvterm_id,
      'dbxref_id' => $dbxref->getID(),
    ]);
    if (!$cvterm_dbxref->find()) {
      $cvterm_dbxref->insert();
    }
  }

  /**
   * Adds a comment to a cvterm.
   *
   * @param $cvterm_id
   *   A cvterm_id of the term to which properties will be added
   * @param $comment
   *   The comment to add to the cvterm.
   * @param rank
   *   The rank of the comment
   *
   * @ingroup tripal_obo_loader
   */
  private function addComment($id, $cvterm_id, $comment, $rank) {

    // Get the comment type id.
    $comment_type_id = $this->used_terms['rdfs:comment'];

    // Add the comment as a property of the cvterm.
    $cvtermprop = new ChadoRecord('cvtermprop');
    $cvtermprop->setValues([
      'cvterm_id' => $cvterm_id,
      'type_id' => $comment_type_id,
      'value' => $comment,
      'rank' => $rank,
    ]);

    // If the insert fails then catch the error and generate a more meaningful
    // message that helps with debugging.
    try {
      $cvtermprop->insert();
    } catch (Exception $e) {
      throw new Exception(t('Cannot add comment, "!comment" to term: !id. ' .
        'ERROR: !error.',
        ['!comment' => $comment, '!id' => $id, '!error' => $e->getMessage()]));
    }
  }

  /**
   * API call to Ontology Lookup Service provided by
   * https://www.ebi.ac.uk/ols/docs/api#resources-terms
   *
   * @param accession
   *   Accession term for query
   * @param type_of_search
   *   Either ontology, term, query, or query-non-local
   *
   * @ingroup tripal_obo_loader
   */
  private function oboEbiLookup($accession, $type_of_search) {
    //Grab just the ontology from the $accession.
    $parts = explode(':', $accession);
    $ontology = strtolower($parts[0]);
    $ontology = preg_replace('/\s+/', '', $ontology);
    if ($type_of_search == 'ontology') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology;
      $response = drupal_http_request($full_url, $options);
      if (!empty($response)) {
        $response = drupal_json_decode($response->data);
      }
    }
    elseif ($type_of_search == 'term') {
      //The IRI of the terms, this value must be double URL encoded
      $iri = urlencode(urlencode("http://purl.obolibrary.org/obo/" . str_replace(':', '_', $accession)));
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . 'terms/' . $iri;
      $response = drupal_http_request($full_url, $options);
      if (!empty($response)) {
        $response = drupal_json_decode($response->data);
      }
    }
    elseif ($type_of_search == 'query') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id&local=true';
      $response = drupal_http_request($full_url, $options);
      if (!empty($response)) {
        $response = drupal_json_decode($response->data);
      }
    }
    elseif ($type_of_search == 'query-non-local') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id';
      $response = drupal_http_request($full_url, $options);
      if (!empty($response)) {
        $response = drupal_json_decode($response->data);
      }
    }
    return $response;
  }
}

/**
 * Ajax callback for the OBOImporter::form() function.
 */
function tripal_cv_obo_form_ajax_callback($form, $form_state) {
  return $form['obo_existing'];
}
