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
  public static $file_types = array('obo');


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
   * we should always indiate where the data we are uploading comes from.
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
  public static $methods = array(
    // Allow the user to upload a file to the server.
    'file_upload' => FALSE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  );

  /**
   * Be default, all loaders are automaticlly added to the Admin >
   * Tripal > Data Laders menu.  However, if this loader should be
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
  private $newcvs = array();
  
  
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
   * The default database preix for this ontology.
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
   * A flag to keep track if the user was warned about slownesss when doing
   * EBI Lookups.
   * 
   * @var string
   */
  private $ebi_warned = FALSE;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    // get a list of db from chado for user to choose
    $sql = "SELECT * FROM {tripal_cv_obo} ORDER BY name";
    $results = db_query($sql);

    $obos = array();
    $obos[] = 'Select a Vocabulary';
    foreach ($results as $obo) {
      $obos[$obo->obo_id] = $obo->name;
    }

    $obo_id = '';
    if (array_key_exists('values', $form_state)) {
      $obo_id = array_key_exists('obo_id', $form_state['values']) ? $form_state['values']['obo_id'] : '';
    }


    $form['instructions']['info'] = array(
      '#type' => 'item',
      '#markup' => t('This page allows you to load vocabularies and ontologies
        that are in OBO format. Once loaded, the terms from these
        vocabularies can be used to create content.
        You may use the form below to either reload a vocabulary that is already
        loaded (as when new updates to that vocabulary are available) or load a new
        vocabulary.'),
    );

    $form['obo_existing'] = array(
      '#type' => 'fieldset',
      '#title' => t('Use a Saved Ontology OBO Reference'),
      '#prefix' => '<span id="obo-existing-fieldset">',
      '#suffix' => '</span>'
    );

    $form['obo_existing']['existing_instructions']= array(
      '#type' => 'item',
      '#markup' => t('The vocabularies listed in the select box below have bene pre-populated
        upon installation of Tripal or have been previously loaded.  Select one to edit
        its settings or submit for loading.  You may reload any vocabulary that has
        already been loaded to retrieve any new updates.'),
    );

    $form['obo_existing']['obo_id'] = array(
      '#title' => t('Ontology OBO File Reference'),
      '#type' => 'select',
      '#options' => $obos,
      '#ajax' => array(
        'callback' => 'tripal_cv_obo_form_ajax_callback',
        'wrapper' => 'obo-existing-fieldset',
      ),
      '#description' => t('Select a vocabulary to import.')
    );

    // If the user has selected an OBO ID then get the form elements for
    // updating.
    if ($obo_id) {
      $uobo_name = '';
      $uobo_url = '';
      $uobo_file = '';

      $vocab = db_select('tripal_cv_obo', 't')
        ->fields('t', array('name', 'path'))
        ->condition('obo_id', $obo_id)
        ->execute()
        ->fetchObject();

      $uobo_name = $vocab->name;
      if (preg_match('/^http/', $vocab->path)) {
        $uobo_url = $vocab->path;
      }
      else {
        $uobo_file = trim($vocab->path);
        $matches = array();
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

      $form['obo_existing']['uobo_name']= array(
        '#type'          => 'textfield',
        '#title'         => t('Vocabulary Name'),
        '#description'   => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                             list above for use again later.'),
        '#default_value'  => $uobo_name,
      );

      $form['obo_existing']['uobo_url']= array(
        '#type'          => 'textfield',
        '#title'         => t('Remote URL'),
        '#description'   => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                             (e.g. http://www.obofoundry.org/ro/ro.obo)'),
        '#default_value' => $uobo_url,
      );

      $form['obo_existing']['uobo_file']= array(
        '#type'          => 'textfield',
        '#title'         => t('Local File'),
        '#description'   => t('Please enter the file system path for an OBO
          definition file. If entering a path relative to
          the Drupal installation you may use a relative path that excludes the
          Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
          that Drupal relative paths have no preceeding slash.
          Otherwise, please provide the full path on the filesystem.  The path
          must be accessible to the web server on which this Drupal instance is running.'),
        '#default_value' => $uobo_file,
      );
      $form['obo_existing']['update_obo_details'] = array(
        '#type' => 'submit',
        '#value' => 'Update Ontology Details',
        '#name' => 'update_obo_details'
      );
    }

    $form['obo_new'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add a New Ontology OBO Reference'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['obo_new']['path_instructions']= array(
      '#value' => t('Provide the name and path for the OBO file.  If the vocabulary OBO file
                     is stored local to the server provide a file name. If the vocabulry is stored remotely,
                     provide a URL.  Only provide a URL or a local file, not both.'),
    );

    $form['obo_new']['obo_name']= array(
      '#type'          => 'textfield',
      '#title'         => t('New Vocabulary Name'),
      '#description'   => t('Please provide a name for this vocabulary.  After upload, this name will appear in the drop down
                             list above for use again later. Additionally, if a default namespace is not provided in the OBO
                             header this name will be used as the default_namespace.'),
    );

    $form['obo_new']['obo_url']= array(
      '#type'          => 'textfield',
      '#title'         => t('Remote URL'),
      '#description'   => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed.
                             (e.g. http://www.obofoundry.org/ro/ro.obo)'),
    );

    $form['obo_new']['obo_file']= array(
      '#type'  => 'textfield',
      '#title'  => t('Local File'),
      '#description' => t('Please enter the file system path for an OBO
          definition file. If entering a path relative to
          the Drupal installation you may use a relative path that excludes the
          Drupal installation directory (e.g. sites/default/files/xyz.obo). Note
          that Drupal relative paths have no preceeding slash.
          Otherwise, please provide the full path on the filesystem.  The path
          must be accessible to the web server on which this Drupal instance is running.'),
    );

    return $form;
  }
  /**
   * @see TripalImporter::formSubmit()
   */
  public function formSubmit($form, &$form_state) {
    $obo_id    = $form_state['values']['obo_id'];
    $obo_name  = trim($form_state['values']['obo_name']);
    $obo_url   = trim($form_state['values']['obo_url']);
    $obo_file  = trim($form_state['values']['obo_file']);
    $uobo_name  = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
    $uobo_url   = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
    $uobo_file  = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

    // If the user requested to alter the details then do that.
    if ($form_state['clicked_button']['#name'] == 'update_obo_details') {
      $form_state['rebuild'] = TRUE;
      $success = db_update('tripal_cv_obo')
        ->fields(array(
          'name' => $uobo_name,
          'path' => $uobo_url ? $uobo_url : $uobo_file,
        ))
        ->condition('obo_id', $obo_id)
        ->execute();
      if ($success) {
        drupal_set_message(t("The vocabulary !vocab has been updated.", array('!vocab' => $uobo_name)));
      }
      else {
        drupal_set_message(t("The vocabulary !vocab could not be updated.", array('!vocab' => $uobo_name)), 'error');
      }

    }
    elseif (!empty($obo_name)) {
      $obo_id = db_insert('tripal_cv_obo')
        ->fields(array(
          'name' => $obo_name,
          'path' => $obo_url ? $obo_url : $obo_file,
        ))
        ->execute();

        // Add the obo_id to the form_state vaules.
        $form_state['values']['obo_id'] = $obo_id;

      if ($obo_id) {
        drupal_set_message(t("The vocabulary !vocab has been added.", array('!vocab' => $obo_name)));
      }
      else {
        $form_state['rebuild'] = TRUE;
        drupal_set_message(t("The vocabulary !vocab could not be added.", array('!vocab' => $obo_name)), 'error');
      }
    }
  }
  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $obo_id    = $form_state['values']['obo_id'];
    $obo_name  = trim($form_state['values']['obo_name']);
    $obo_url   = trim($form_state['values']['obo_url']);
    $obo_file  = trim($form_state['values']['obo_file']);
    $uobo_name  = array_key_exists('uobo_name', $form_state['values']) ? trim($form_state['values']['uobo_name']) : '';
    $uobo_url   = array_key_exists('uobo_url', $form_state['values']) ? trim($form_state['values']['uobo_url']) : '';
    $uobo_file  = array_key_exists('uobo_file', $form_state['values']) ? trim($form_state['values']['uobo_file']) : '';

    // Make sure if the name is changed it doesn't conflict with another OBO.
    if ($form_state['clicked_button']['#name'] == 'update_obo_details' or
        $form_state['clicked_button']['#name'] == 'update_load_obo') {
      // Get the current record
      $vocab = db_select('tripal_cv_obo', 't')
      ->fields('t', array('obo_id', 'name', 'path'))
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
      ->fields('t', array('obo_id', 'name', 'path'))
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
    $sql =  "SELECT * FROM {cv} CV";
    $cvs = chado_query($sql);
    while ($cv = $cvs->fetchObject()) {
      $this->all_cvs[$cv->name] = $cv;
    }
    
    // Get the list of all DBs so we can save on lookups later
    $sql =  "SELECT * FROM {db} DB";
    $dbs = chado_query($sql);
    while ($db = $dbs->fetchObject()) {
      $this->all_dbs[$db->name] = $db;
    }

    // Get the 'Subgroup' term that we will use for adding subsets.
    $term = chado_get_cvterm(['id' => 'NCIT:C25693']);
    $this->used_terms['NCIT:C25693'] = $term->cvterm_id;
    
    // Run the importer!
    $this->loadOBO_v1_2_id($obo);
  }
  /**
   * @see TripalImporter::postRun()
   *
   */
  public function postRun() {

    // Update the cv_root_mview materiailzed view.
    $this->logMessage("Updating the cv_root_mview materialized view...");
    $mview_id = tripal_get_mview_id('cv_root_mview');
    tripal_populate_mview($mview_id);

    $this->logMessage("Updating the db2cv_mview materialized view...");
    $mview_id = tripal_get_mview_id('db2cv_mview');
    tripal_populate_mview($mview_id);

    // Upate the cvtermpath table for each newly added CV.
    $this->logMessage("Updating cvtermpath table.  This may take a while...");
    foreach ($this->newcvs as $namespace => $cv_id) {
      $this->logMessage("- Loading paths for @vocab", array('@vocab' => $namespace));
      tripal_update_cvtermpath($cv_id);
    }
  }
  /**
   * A wrapper function for importing the user specified OBO file into Chado by
   * specifying the obo_id of the OBO. It requires that the file be in OBO v1.2
   * compatible format.  This function is typically executed via the Tripal jobs
   * management after a user submits a job via the Load Onotloies form.
   *
   * @param $obo_id
   *   An obo_id from the tripal_cv_obo file that specifies which OBO file to import
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
   * specifying the filename and path of the OBO. It requires that the file be in OBO v1.2
   * compatible format.  This function is typically executed via the Tripal jobs
   * management after a user submits a job via the Load Onotloies form.
   *
   * @param $obo_name
   *   The name of the OBO (typially the ontology or controlled vocabulary name)
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
   * specifying the remote URL of the OBO. It requires that the file be in OBO v1.2
   * compatible format.  This function is typically executed via the Tripal jobs
   * management after a user submits a job via the Load Onotloies form.
   *
   * @param $obo_name
   *   The name of the OBO (typially the ontology or controlled vocabulary name)
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
          " if you are unable to download the file you may manually downlod the OBO file and use the web interface to " .
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
   * loadOBO_v1_2_file or tirpal_cv_load_obo_v1_2_url. But, it can
   * be called directly if the full path to an OBO file is available on the
   * file system.
   *
   * @param $flie
   *   The full path to the OBO file on the file system
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2($file, $obo_name) {
    $header = array();
    $ret = array();

    // Empty the temp table.
    $this->clearTermStanzaCache();

    // Parse the obo file.
    $this->parse($file, $header);
    
    // Cache the relationships of terms.
    $this->cacheRelationships();
 
    // Add any typedefs to the vocabulary first.
    $this->processTypeDefs();

    // Next add terms to the vocabulary.
    if (!$this->processTerms()) {
      throw new Exception('Cannot add terms from this ontology');
    }
    // Empty the term cache.
    $this->clearTermStanzaCache();
  }

  /**
   * OBO files are divided into a typedefs terms section and vocabulary terms section.
   * This function loads the typedef terms from the OBO.
   *
   * @ingroup tripal_obo_loader
   */
  private function processTypeDefs() {
    
    $this->logMessage("Step 3: Loading type defs...");
    $typedefs = $this->getCachedTermStanzas('Typedef');
    $count = $this->getCacheSize('Typedef');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(5);

    $i = 1;
    foreach ($typedefs as $t) {
      
      // TODO: it would be better if we had a term iterator so that we
      // don't have to distringuish here between the table vs memory cache type.
      if ($this->cache_type == 'table') {
        $term = unserialize(base64_decode($t->stanza));
      }
      else {
        $term = $this->termStanzaCache['ids'][$t];
      }
      $this->setItemsHandled($i++);
      $this->processTerm($term, TRUE);
    }

    $this->setItemsHandled($i);
    return 1;
  }

  /** 
   * This function loads all of the [Term] termsfrom the OBO.
   */
  private function processTerms() {
    $i = 0;
    $external = FALSE;
    
    $this->logMessage("Step 4: Loading terms...");
    $terms = $this->getCachedTermStanzas('Term');
    $count = $this->getCacheSize('Term');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(1);

    // Iterate through the terms.
    foreach ($terms as $t) {
      // TODO: it would be better if we had a term iterator so that we 
      // don't have to distringuish here between the table vs memory cache type.
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
  private function getDefaults($header) {
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
    
    // If we have the DB short name (or ontology header) but not the default
    // namespace then we may be able to find it via an EBI lookup.
    if (!$namespace and $short_name) {
      $namespace = $this->findEBIOntologyNamespace($short_name);
    }
    
    // If we have the namespace but not the short name then we have to
    // do an exhaustive search of EBI to find it.
    if ($namespace and !$short_name) {
      $short_name = $this->findEBIOntologyPrefix($namespace);
    }
    
    // If we can't find the namespace or the short_name then bust.
    if (!$namespace and !$short_name) {
      throw new ErrorException('Cannot determine the namespace or ontology prefix from this OBO file. It is missing both the "default-namespace" and "ontology" headers.');
    }
    
    // Set the defaults.
    $this->default_namespace = $namespace;
    $this->default_db = $short_name;
    
    // TODO: we need to get the description and title from EBI for these
    // ontology so that we can put something in the proper fields when
    // adding a new CV or DB.
    
    // Add the CV record if it doesn't exist.
    $cv = new ChadoRecord('cv');
    $cv->setValues(['name' => $namespace]);
    if (!$cv->find()) {
      $cv->insert();
    }
    $this->newcvs[$namespace] = $cv->getID();
    
    // Add the DB record if it doesn't exist.
    $db = new ChadoRecord('db');
    $db->setValues(['name' => $this->default_db]);
    if (!$db->find()) {
      $db->insert();
    }
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
    }
    catch (Exception $e) {
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
    
    $options = array();
    $page = 1;
    $size = 25;
    $full_url = 'https://www.ebi.ac.uk/ols/api/ontologies?page=' . $page. '&size=' . $size;
    while ($response = drupal_http_request($full_url, $options)) {
      $response = drupal_json_decode($response->data);
      $page++;
      $full_url = 'https://www.ebi.ac.uk/ols/api/ontologies?page=' . $page. '&size=' . $size;
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
        
    // Get the short name and accession for the term.
    $pair = explode(":", $id, 2);
    $short_name = $pair[0];
    $accession_num = $pair[1];
    
    // Check for the ID of the term in EBI.
    $oterm = NULL;
    $results = $this->oboEbiLookup($id, 'term');
    if (isset($results['label'])) {
      $oterm = $results;
    }
    
    // If we did not get a name for the term from a direct term
    // lookup then let's try a query.
    if (!isset($results['label'])) {
      $results = $this->oboEbiLookup($id, 'query');
      if (array_key_exists('docs', $results)) {
        if (!empty($results['response']['docs'])) {
          if (count($results['response']['docs']) > 1) {
            foreach ($results['response']['docs'] as $doc) {
              if ($doc['obo_id'] == $t['id']) {
                $external = TRUE;
                $oterm = $doc;
              }
            }
          }
          else {
            $external = true;
            $oterm = $results['response']['docs'][0];
          }
        }
      }
    }
    
    // If the accession could not be found in EBI.
    if ($results['response']['numFound'] == 0 && !isset($results['label'])) {
      // The first search doesn't work, so let's try a broader one.
      $results = $this->oboEbiLookup($t['id'], 'query-non-local');
      if (!empty($results)) {
        if (array_key_exists('docs', $results)) {
          if (!empty($results['docs'])) {
            $accession = $t['id'];
            $accession_underscore = str_replace(":", "_", $accession);
            foreach ($results['response']['docs'] as $item) {
              if ($item['label'] != $accession && $item['label'] != $accession_underscore) {
                // Found the first place a label is other than the accession 
                // is used, so take that info and then end the loop.
                $external = TRUE;
                $oterm = $item;
                break;
              }
            }
          }
        }
      }
    }
    // If we found a term then return it.
    if ($oterm) {
      // Make an OBO stanza array as if this term were in the OBO file and
      // return it.
      $stanza = [];
      $stanza['id'][] = $oterm['label'];
      $stanza['name'][] = $oterm['label'];
      $stanza['def'][] = $oterm['def'];
      $stanza['namespace'][] = $oterm['ontology_name'];
      $stanza['is_obsolete'][] = $oterm['is_obsolete'];
      $stanza['subset'][] =  $oterm['subset'];
      $stanza['db_name'][] = $short_name;
      return $stanza;
    }
    return FALSE;
  }
  
  /**
   * Inserts a new cvterm using the OBO stanza array provided.
   * 
   * The stanza passed to this function should always come from the term cache, 
   * not directly from the OBO file because the cached terms have been 
   * updated to include all necessary values.  This function also removes
   * all properties assocaited with the term so that those can be added 
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
   *   A cvterm object. 
   */
  private function saveTerm($stanza, $is_relationship = FALSE, $update_if_exists = FALSE) {
    
    $id = $stanza['id'][0];
    
    // First check if we've already used this term.
    if ($this->used_terms[$id]) {
      return $this->used_terms[$id];
    }
    
    // Convert the stanza into a term array that we can pass to
    // chado_insert_cvterm().
    $t = array();
    $t['id'] = $stanza['id'][0];
    $t['name'] = $stanza['name'][0];
    $t['namespace'] = $stanza['namespace'][0];
    $t['cv_name'] = $stanza['namespace'][0];
    $t['is_relationship'] = $is_relationship;
    $t['db_name'] = $stanza['db_name'][0];
    
    if (array_key_exists('def', $stanza)) {
      $t['definition'] = $stanza['def'][0];
    }
    if (array_key_exists('subset', $stanza)) {
      $t['subset'] = $stanza['subset'][0];
    }
    if (array_key_exists('is_obsolete', $stanza)) {
      $t['is_obsolete'] = $stanza['is_obsolete'][0];
    }
    $cvterm = chado_insert_cvterm($t, ['update_existing' => $update_if_exists]);
    if (!$cvterm) {
      throw new Exception("Cannot add the term " . $term['id'][0]);
    }
    
    // Remove any relationships that this term already has (in case it was
    // updated) and we'll re-add them.
    $sql = "
      DELETE FROM {cvterm_relationship} CVTR
      WHERE CVTR.subject_id = :cvterm_id
    ";
    chado_query($sql, array(':cvterm_id' => $cvterm->cvterm_id));
    
    // Save the cvterm_id for this term so we don't look it up again.
    $this->used_terms[$id] = $cvterm->cvterm_id;
    
    // Return the cvterm_id.
    return $cvterm->cvterm_id;
  }

  /**
   * Uses the provided term array to add/update information to Chado about the
   * term including the term, dbxref, synonyms, properties, and relationships.
   *
   * @param $term
   *   An array representing the cvterm.
   * @is_relationship
   *   Set to 1 if this term is a relationship term
   *
   * @ingroup tripal_obo_loader
   */
  private function processTerm($stanza, $is_relationship = 0) {
    
    //
    // First things first--save the term.
    //
    $cvterm_id = $this->saveTerm($stanza, FALSE, TRUE);

    
    //
    // Handle:  alt_id
    //
    if (array_key_exists('alt_id', $stanza)) {
      foreach ($stanza['alt_id'] as $alt_id) {
        if (!$this->addCvtermDbxref($cvterm_id, $alt_id)) {
          throw new Exception("Cannot add alternate id $alt_id");
        }
      }
    }
    
    //
    // Handle:  synonym
    //
    if (array_key_exists('synonym', $stanza)) {
      if (!$this->addSynonym($stanza, $cvterm_id)) {
        throw new Exception("Cannot add synonyms");
      }
    }

    //
    // Handle:  exact_synonym
    //
    if (array_key_exists('exact_synonym', $stanza)) {
      foreach ($stanza['exact_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 EXACT $2', $synonym);
        $term['synonym'][] = $new;
        if (!$this->addSynonym($term, $cvterm_id)) {
          throw new Exception("Cannot add/update synonyms");
        }
      }
    }
    
    //
    // Handle: narrow_synonym
    //
    if (array_key_exists('narrow_synonym', $stanza)) {
      foreach ($stanza['narrow_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 NARROW $2', $synonym);
        $term['synonym'][] = $new;
        if (!$this->addSynonym($term, $cvterm_id)) {
          throw new Exception("Cannot add/update synonyms");
        }
      }
    }
    
    //
    // Handle:  broad_synonym
    //
    if (array_key_exists('broad_synonym', $stanza)) {
      foreach ($stanza['broad_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 BROAD $2', $synonym);
        $term['synonym'][] = $new;
        if (!$this->addSynonym($term, $cvterm_id)) {
          throw new Exception("Cannot add/update synonyms");
        }
      }
    }

    //
    // Handle:  comment
    //
    if (array_key_exists('comment', $stanza)) {
      $comments = $stanza['comment'];
      $j = 0;
      foreach ($comments as $comment) {
        if (!$this->addCvtermProp($cvterm_id, 'comment', $comment, $j)) {
          throw new Exception("Cannot add/update cvterm property");
        }
        $j++;
      }
    }

    //
    // Handle:  xref
    //
    if (array_key_exists('xref', $stanza)) {
      foreach ($stanza['xref'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm_id, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }

    //
    // Handle:  xref_analog
    //
    if (array_key_exists('xref_analog', $stanza)) {
      foreach ($stanza['xref_analog'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm_id, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }
    
    //
    // Handle:  xref_unk
    //
    if (array_key_exists('xref_unk', $stanza)) {
      foreach ($stanza['xref_unk'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm_id, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }

    //
    // Handle:  is_a
    //
    if (array_key_exists('is_a', $stanza)) {
      foreach ($stanza['is_a'] as $is_a) {
        if (!$this->addRelationship($cvterm_id, 'is_a', $is_a)) {
          throw new Exception("Cannot add relationship is_a: $is_a");
        }
      }
    }
    
    //
    // Handle:  relationship
    //
    if (array_key_exists('relationship', $stanza)) {
      foreach ($stanza['relationship'] as $value) {
        $rel = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
        $object = preg_replace('/^.+?\s(.+?)$/', '\1', $value);
        if (!$this->addRelationship($cvterm_id, $rel, $object)) {
          throw new Exception("Cannot add relationship $rel: $object");
        }
      }
    }
    
    //
    // Handle:  subset
    //
    if (array_key_exists('subset', $stanza)) {
      foreach ($stanza['subset'] as $subset) {
        if (!$this->addSubset($cvterm_id, $subset)) {
          throw new Exception("Cannot add subset $subset");
        }
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
  private function addRelationship($cvterm_id, $rel_id, $obj_id) {
            
    // Get the cached terms for both the relationship and the object. They 
    // shold be there, but just in case something went wrong, we'll throw
    // an exception if we can't find them.
    $rel_stanza = $this->getCachedTermStanza($rel_id);
    if (!$rel_stanza) {      
      throw new Exception(t('Cannot find term, "!name", in the term cache.', ['!name' => $rel_id]));
    }
    $rel_cvterm_id = $this->saveTerm($rel_stanza, TRUE);
    
    // Make sure the object term exists in the cache.
    $obj_stanza = $this->getCachedTermStanza($obj_id);
    if (!$obj_stanza) {
      throw new Exception(t('Cannot find term "!name" in the term cache.', ['!name' => $obj_id]));
    }
    $obj_cvterm_id = $this->saveTerm($obj_stanza);

    // Add the cvterm_relationship.
    $cvterm_relationship = new ChadoRecord('cvterm_relationship');
    $cvterm_relationship->setValues([
      'type_id'    => $rel_cvterm_id,
      'subject_id' => $cvterm_id,
      'object_id'  => $obj_cvterm_id
    ]);
    $cvterm_relationship->insert();
    return TRUE;
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
      $values = array('id' => $id);
      $result = chado_select_record('tripal_obo_temp', array('stanza'), $values);
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
   * Adds a term stanza from the OBO file to the cache for easier lookup.
   * 
   * @param $stanza
   *   The stanza from the OBO file for the term.
   * @throws Exception
   */
  private function addCacheTermStanza($stanza, $type) {

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
      
      // If the DB short name and the default DB short name don't match then
      // let's do a lookup on EBI to get the term details.
      if ($short_name != $this->default_db) {
        
        if ($this->ebi_warned == FALSE) {
          $this->logMessage(
            "A term that belongs to another ontology is used within this " .
            "vocabulary.  Therefore a lookup will be performed with the EBI Ontology " .
            "Lookup Service to retrieve the information for this term. " .
            "Please note, that vocabularies with many non-local terms " .
            "require remote lookups and these lookups can dramatically " .
            "decrease loading time. " ,
            ['!vocab' => $this->default_namespace], TRIPAL_WARNING);
          $this->ebi_warned = TRUE;
        }
        
        // If we found a term then let's create a new stanza as if it existed
        // in the original OBO file but with all the necessary details.
        $oterm = $this->findEBITerm($id);
        if ($oterm) {
          $stanza = $oterm;
        }
        else {
          throw new Exception(t('Cannot find the term defined in the ontology or via an EBI OLS lookup: !term', 
            ['!term' => $id]));
        }
      }
      // If the term belongs to this OBO then let's set the 'db_name'.
      else {
        if (!array_key_exists('namespace', $stanza)) {
          $stanza['namespace'][] = $this->default_namespace;
        }
        $stanza['db_name'][] = $short_name;
      }
    }
    // If there is no DB short name prefix for the id.
    else {
      if (!array_key_exists('namespace', $stanza)) {
        $stanza['namespace'][] = $this->default_namespace;
      }
      $stanza['db_name'][] = $this->default_db;
    }
      
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
    $this->termStanzaCache['ids'][$id] = $stanza;
    $this->termStanzaCache['count'][$type]++;
    $this->termStanzaCache['types'][$type][] = $id;
  }
  
  /**
   * Returns the size of a given term type from the cache.
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
      return $typdefs; 
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
   * @param term
   *   An array representing the cvterm. It must have a 'synonym' key/value pair.
   * @param $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   *
   * @ingroup tripal_obo_loader
   */
  private function addSynonym($term, $cvterm_id) {

    // make sure we have a 'synonym_type' vocabulary
    $syncv = tripal_insert_cv(
        'synonym_type',
        'A local vocabulary added for synonym types.'
    );

    // now add the synonyms
    if (array_key_exists('synonym', $term)) {
      foreach ($term['synonym'] as $synonym) {

        // separate out the synonym definition and the synonym type
        $def = preg_replace('/^\s*"(.*)"\s*.*$/', '\1', $synonym);
        // the scope will be 'EXACT', etc...
        $scope = drupal_strtolower(preg_replace('/^.*"\s+(.*?)\s+.*$/', '\1', $synonym));
        if (!$scope) {  // if no scope then default to 'exact'
          $scope = 'exact';
        }

        // make sure the synonym type exists in the 'synonym_type' vocabulary
        $values = array(
          'name' => $scope,
          'cv_id' => array(
            'name' => 'synonym_type',
          ),
        );
        $syntype = tripal_get_cvterm($values);

        // if it doesn't exist then add it
        if (!$syntype) {
          // build a 'term' object so we can add the missing term
          $term = array(
            'name' => $scope,
            'id' => "synonym_type:$scope",
            'definition' => '',
            'is_obsolete' => 0,
            'cv_name' => $syncv->name,
            'is_relationship' => FALSE
          );
          $syntype = tripal_insert_cvterm($term, array('update_existing' => TRUE));
          if (!$syntype) {
            throw new Exception("Cannot add synonym type: internal:$scope");
          }
        }

        // make sure the synonym doesn't already exists
        $values = array(
          'cvterm_id' => $cvterm_id,
          'synonym' => $def
        );
        $results = chado_select_record('cvtermsynonym', array('*'), $values);
        if (count($results) == 0) {
          $values = array(
            'cvterm_id' => $cvterm_id,
            'synonym' => $def,
            'type_id' => $syntype->cvterm_id
          );
          $options = array('return_record' => FALSE);
          $success = chado_insert_record('cvtermsynonym', $values, $options);
          if (!$success) {
            throw new Exception("Failed to insert the synonym for term: $cvterm_id ($def)");
          }
        }

        // now add the dbxrefs for the synonym if we have a comma in the middle
        // of a description then this will cause problems when splitting os lets
        // just change it so it won't mess up our splitting and then set it back
        // later.
        /**
        $synonym = preg_replace('/(".*?),\s(.*?")/','$1,_$2',$synonym);
        $dbxrefs = preg_split("/, /",preg_replace('/^.*\[(.*?)\]$/','\1',$synonym));
        foreach ($dbxrefs as $dbxref) {
        $dbxref = preg_replace('/,_/',", ",$dbxref);
        if ($dbxref) {
        $this->addCvtermDbxref($syn,$dbxref);
        }
        }
        */
      }
    }

    return TRUE;
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
    $stanza = array();
    // Holds the default database for the term.
    $db_short_name = '';
    $line_num = 0;
    $num_read = 0;
    // The type of term:  Typedef or Term (inside the [] brackets]
    $type = '';

    $this->logMessage("Step 1: Preloading File $file...");
    $filesize = filesize($obo_file);
    $this->setTotalItems($filesize);
    $this->setItemsHandled(0);
    $this->setInterval(1);

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
      
      // Remove annotations surrounded by brackets. These are found
      // in the Trait Ontology (e.g. TO:1000023 {is_inferred="true"})
      // That construct has useful info, but it is not in the OBO 1.4 format
      // specifications.
      $line = preg_replace('/\{.*?\}/', '', $line);

      // At the first stanza we're out of header.
      if (preg_match('/^\s*\[/', $line)) {
        
        // After parsing the header we need to get information about this OBO.
        if ($in_header == TRUE) {
          $this->getDefaults($header);
          $in_header = FALSE;
        }
        
        // Store the stanza we just finished reading.
        if (sizeof($stanza) > 0) {
          $this->addCacheTermStanza($stanza, $type);
        }
        
        // Get the stanza type:  Term, Typedef or Instance
        $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/', '\1', $line);

        // start fresh with a new array
        $stanza = array();
        continue;
      }

      // break apart the line into the tag and value but ignore any escaped colons
      preg_replace("/\\:/", "|-|-|", $line); // temporarily replace escaped colons
      $pair = explode(":", $line, 2);
      $tag = $pair[0];
      $value = ltrim(rtrim($pair[1]));// remove surrounding spaces

      // if this is the ID line then get the database short name from the ID.
      $matches = array();
      if ($tag == 'id' and preg_match('/^(.+?):.*$/', $value, $matches)) {
        $db_short_name = $matches[1];
      }
      $tag = preg_replace("/\|-\|-\|/", "\:", $tag); // return the escaped colon
      $value = preg_replace("/\|-\|-\|/", "\:", $value);
      if ($in_header) {
        if (!array_key_exists($tag, $header)) {
          $header[$tag] = array();
        }
        $header[$tag][] = $value;
      }
      else {
        if (!array_key_exists($tag, $stanza)) {
          $stanza[$tag] = array();
        }
        $stanza[$tag][] = $value;
      }
    }
    // now add the last term in the file
    if (sizeof($stanza) > 0) {
      $this->addCacheTermStanza($stanza, $type);
      $this->setItemsHandled($num_read);
    }
  }
  
  /**
   * Iterates through all of the cached terms and caches any relationships
   */
  private function cacheRelationships() {
    
    // Now that we have all of the terms parsed and loaded into the cache,
    // lets run through them one more time cache any terms in relationships
    // as well.
    $this->logMessage("Step 2: Examining relationships...");
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
          $this->addCacheTermStanza($stanza, 'Term');
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
          $this->addCacheTermStanza($stanza, 'Typedef');
          
          $rstanza = [];
          $rstanza['id'][] = $object_term;
          $this->addCacheTermStanza($stanza, 'Term');
        }
      }
    }
    $this->setItemsHandled($i++);
    
    // Last of all, we need to add the "is_a" relationship It's part of the
    // OBO specification as a built-in relationship but not all vocabularies 
    // include that term.
    if (!$this->getCachedTermStanza('is_a')) {
      $stanza = [];
      $stanza['id'][] = 'is_a';
      $stanza['name'][] = 'is_a';
      $stanza['namespace'][] = $this->default_namespace;
      $stanza['db_name'][] = $this->default_db;
      $this->addCacheTermStanza($stanza, 'Typedef');
    }
  }
  
  /**
   * Adds a new namespace to the database by performing an EBI Lookup.
   * 
   * @param $namespace
   */
  private function addNamespace($namespace) {
    // The controlled vocabulary is not in the cv table and needs to be added.
    $ontology_info = $this->oboEbiLookup($namespace, 'ontology');
    if (!empty($ontology_info['config'])){
      // CV Name.
      if (array_key_exists('namespace', $ontology_info['config'])) {
        $cv_info = $ontology_info['config']['namespace'];
      }
      elseif (array_key_exists('default-namespace', $ontology_info['config']['annotations'])) {
        $cv_info = $ontology_info['config']['annotations']['default-namespace'];
      }
      
      //CV Description.
      if (array_key_exists('description', $ontology_info['config'])) {
        $description = $ontology_info['config']['description'];
      }
      else {
        $description = '';
      }
      $cv_returned = chado_insert_cv($cv_info, $description);
      if($cv_returned) {
        $this->all_cvs[$cv_returned->name] = $cv_returned;
        
        $namespace = $cv_returned->name;
        // Now add the db entry.
        $values = array(
          'name' => $ontology_info['config']['preferredPrefix'],
          'description' => $ontology_info['config']['description'],
          'url' => $ontology_info['config']['versionIri'],
        );
        
        $db_returned = chado_insert_db($values);
        if ($db_returned) {
          $short_name = $db_returned->name;
        }
        return TRUE;
      }
    }
    return FALSE;
  }
  
  /**
   * Adds a property to the cvterm indicating it belongs to a subset.
   * @param $cvterm_id
   *   The cvterm_id of the term to which the subset will be added.
   * @param $subset
   *   The name of the subset.
   */
  private function addSubset($cvterm_id, $subset) {
    $cvtermprop = new ChadoRecord('cvtermprop');
    $cvtermprop->setValues([
      'cvterm_id' => $cvterm_id,
      'type_id' => $this->used_terms['NCIT:C25693'],
      'value' => $subset
    ]);
    if (!$cvtermprop->find()){
      $cvtermprop->insert();
    }
    return TRUE;
  }

  /**
   * Adds a database reference to a cvterm
   *
   * @param $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param xref
   *   The cross refernce.  It should be of the form from the OBO specification
   *
   * @ingroup tripal_obo_loader
   */
  private function addCvtermDbxref($cvterm_id, $xref) {

    $dbname = preg_replace('/^(.+?):.*$/', '$1', $xref);
    $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/', '$1', $xref);
    $description = preg_replace('/^.+?\"(.+?)\".*?$/', '$1', $xref);
    $dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/', '$1', $xref);

    if (!$accession) {
      throw new Exception("Cannot add a dbxref without an accession: '$xref'");
    }

    // if the xref is a database link, handle that specially
    if (strcmp($dbname, 'http') == 0) {
      $accession = $xref;
      $dbname = 'URL';
    }

    // add the database
    $db = tripal_insert_db(array('name' => $dbname));
    if (!$db) {
      throw new Exception("Cannot find database '$dbname' in Chado.");
    }

    // now add the dbxref
    $dbxref = $this->addDbxref($db->db_id, $accession, '', $description);
    if (!$dbxref) {
      throw new Exception("Cannot find or add the database reference (dbxref)");
    }

    // finally add the cvterm_dbxref but first check to make sure it exists
    $values = array(
      'cvterm_id' => $cvterm_id,
      'dbxref_id' => $dbxref->dbxref_id,
    );
    $result = chado_select_record('cvterm_dbxref', array('*'), $values);
    if (count($result) == 0) {
      $ins_options = array('return_record' => FALSE);
      $result = chado_insert_record('cvterm_dbxref', $values, $ins_options);
      if (!$result) {
        throw new Exception("Cannot add cvterm_dbxref: $xref");
      }
    }

    return TRUE;
  }

  /**
   * Adds a property to a cvterm
   *
   * @param $cvterm_id
   *   A cvterm_id of the term to which properties will be added
   * @param $property
   *   The name of the property to add
   * @param $value
   *   The value of the property
   * @param rank
   *   The rank of the property
   *
   * @ingroup tripal_obo_loader
   */
  private function addCvtermProp($cvterm_id, $property, $value, $rank) {

    // make sure the 'cvterm_property_type' CV exists
    $cv = tripal_insert_cv('cvterm_property_type', '');
    if (!$cv) {
      throw new Exception("Cannot add/find cvterm_property_type cvterm");
    }

    // get the property type cvterm.  If it doesn't exist then we want to add it
    $values = array(
      'name' => $property,
      'cv_id' => $cv->cv_id,
    );
    $results = chado_select_record('cvterm', array('*'), $values);
    if (count($results) == 0) {
      $term = array(
        'name' => $property,
        'id' => "internal:$property",
        'definition' => '',
        'is_obsolete' => 0,
        'cv_name' => $cv->name,
        'is_relationship' => FALSE,
      );
      $cvproptype = tripal_insert_cvterm($term, array('update_existing' => FALSE));
      if (!$cvproptype) {
        throw new Exception("Cannot add cvterm property: internal:$property");
      }
    }
    else {
      $cvproptype = $results[0];
    }

    // remove any properties that currently exist for this term.  We'll reset them
    if ($rank == 0) {
      $values = array('cvterm_id' => $cvterm_id);
      $success = chado_delete_record('cvtermprop', $values);
      if (!$success) {
        throw new Exception("Could not remove existing properties to update property $property for term\n");
      }
    }

    // now add the property
    $values = array(
      'cvterm_id' => $cvterm_id,
      'type_id' => $cvproptype->cvterm_id,
      'value' => $value,
      'rank' => $rank,
    );
    $options = array('return_record' => FALSE);
    $result = chado_insert_record('cvtermprop', $values, $options);
    if (!$result) {
      throw new Exception("Could not add property $property for term\n");
    }
    return TRUE;
  }

  /**
   * Adds a database cross reference to a cvterm
   *
   * @param db_id
   *   The database ID of the cross reference
   * @param accession
   *   The cross reference's accession
   * @param $version
   *   The version of the dbxref
   * @param $description
   *   The description of the cross reference
   *
   * @ingroup tripal_obo_loader
   */
  private function addDbxref($db_id, $accession, $version='', $description='') {

    // check to see if the dbxref exists if not, add it
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $result = chado_select_record('dbxref', array('dbxref_id'), $values);
    if (count($result) == 0) {
      $ins_values = array(
        'db_id'       => $db_id,
        'accession'   => $accession,
        'version'     => $version,
        'description' => $description,
      );
      $ins_options = array('return_record' => FALSE);
      $result = chado_insert_record('dbxref', $ins_values, $ins_options);
      if (!$result) {
        throw new Exception("Failed to insert the dbxref record $accession");
      }
      $result = chado_select_record('dbxref', array('dbxref_id'), $values);
    }
    return $result[0];
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
      $options = array();
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology;
      $response = drupal_http_request($full_url, $options);
      if(!empty($response)){
        $response = drupal_json_decode($response->data);
      }
    }
    elseif ($type_of_search == 'term') {
      //The IRI of the terms, this value must be double URL encoded
      $iri = urlencode(urlencode("http://purl.obolibrary.org/obo/" . str_replace(':' , '_', $accession)));
      $options = array();
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . 'terms/' . $iri;
      $response = drupal_http_request($full_url, $options);
      if(!empty($response)){
        $response = drupal_json_decode($response->data);
      }
    }
    elseif($type_of_search == 'query') { 
      $options = array();
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id&local=true';
      $response = drupal_http_request($full_url, $options);
      if(!empty($response)){
        $response = drupal_json_decode($response->data);
      }
    }
    elseif($type_of_search == 'query-non-local') {    
      $options = array();
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id';
      $response = drupal_http_request($full_url, $options);
      if(!empty($response)){
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

