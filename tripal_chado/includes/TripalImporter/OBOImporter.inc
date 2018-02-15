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
                             list above for use again later.'),
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
    else if (!empty($obo_name)) {
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
          form_set_error('uobo_file', 'The specified path does not exist or cannot be read.');
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
          form_set_error('obo_file', 'The specified path does not exist or cannot be read.');
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
    foreach ($this->newcvs as $namespace => $cvid) {
      $this->logMessage("- Loading paths for @vocab", array('@vocab' => $namespace));
      tripal_update_cvtermpath($cvid);
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

    $success = $this->loadOBO_v1_2($file);
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
    $this->loadOBO_v1_2($temp);

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
  private function loadOBO_v1_2($file) {

    $header = array();
    $ret = array();

    // Empty the temp table.
    $sql = "DELETE FROM {tripal_obo_temp}";
    chado_query($sql);

    $this->logMessage("Step 1: Preloading File $file...");

    // parse the obo file
    $default_db = $this->parse($file, $header);

    // Add the CV for this ontology to the database.  The v1.2 definition
    // specifies a 'default-namespace' to be used if a 'namespace' is not
    // present for each stanza.  Some ontologies have adopted the v1.4 method
    // in their v1.2 files and not including it.
    if (array_key_exists('default-namespace', $header)) {
      $defaultcv = tripal_insert_cv($header['default-namespace'][0], '');
      if (!$defaultcv) {
        throw new Exception('Cannot add namespace ' . $header['default-namespace'][0]);
      }
      $this->newcvs[$header['default-namespace'][0]] = $defaultcv->cv_id;
    }
    // If the 'default-namespace' is missing.
    else {

      // Look to see if an 'ontology' key is present.  It is part of the v1.4
      // specification so it shouldn't be in the file, but just in case
      if (array_key_exists('ontology', $header)) {
        $defaultcv = tripal_insert_cv(strtoupper($header['ontology'][0]), '');
        if (!$defaultcv) {
          throw new Exception('Cannot add namespace ' . strtoupper($header['ontology'][0]));
        }
        $this->newcvs[strtoupper(strtoupper($header['ontology'][0]))] = $defaultcv->cv_id;
      }
      else {
        throw new Exception("Could not find a namespace for this OBO file: $file");
      }
      $this->logMessage("This OBO is missing the 'default-namespace' header. It " .
          "is not possible to determine which vocabulary terms without a 'namespace' key " .
          "should go.  Instead, those terms will be placed in the '!vocab' vocabulary.",
          array('!vocab' => $defaultcv->name), TRIPAL_WARNING);
    }!
    // Add any typedefs to the vocabulary first.
    $this->logMessage("Step 2: Loading type defs...");
    $this->loadTypeDefs($defaultcv, $default_db);

    // Next add terms to the vocabulary.
    $this->logMessage("Step 3: Loading terms...");
    if (!$this->processTerms($defaultcv, $default_db)) {
      throw new Exception('Cannot add terms from this ontology');
    }

  }

  /**
   * OBO files are divided into a typedefs terms section and vocabulary terms section.
   * This function loads the typedef terms from the OBO.
   *
   * @param $defaultcv
   *   A database object containing a record from the cv table for the
   *   default controlled vocabulary
   * @param $default_db
   *   The name of the default database.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadTypeDefs($defaultcv, $default_db) {
    $sql = "SELECT * FROM {tripal_obo_temp} WHERE type = 'Typedef' ";
    $typedefs = chado_query($sql);

    $sql = "
      SELECT count(*) as num_terms
      FROM {tripal_obo_temp}
      WHERE type = 'Typedef'
    ";
    $result = chado_query($sql)->fetchObject();
    $count = $result->num_terms;
    $this->setTotalItems($count);
    $this->setItemsHandled(0);

    $i = 0;
    foreach ($typedefs as $typedef) {
      $this->setItemsHandled($i);
      $term = unserialize(base64_decode($typedef->stanza));
      $this->processTerm($term, $defaultcv->name, 1, $default_db);
      $i++;
    }

    $this->setItemsHandled($i);
    return 1;
  }

  /**
   * OBO files are divided into a typedefs section and a terms section.
   *
   * This function loads the typedef terms from the OBO.
   *
   * @param $defaultcv
   *   A database object containing a record from the cv table for the
   *   default controlled vocabulary
   * @param $default_db
   *   The name of the default database.
   */
  private function processTerms($defaultcv, $default_db) {
    $i = 0;

    // Iterate through each term from the OBO file and add it.
    $sql = "
      SELECT * FROM {tripal_obo_temp}
      WHERE type = 'Term'
      ORDER BY id
    ";
    $terms = chado_query($sql);

    $sql = "
      SELECT count(*) as num_terms
      FROM {tripal_obo_temp}
      WHERE type = 'Term'
    ";
    $result = chado_query($sql)->fetchObject();
    $count = $result->num_terms;
    $this->setTotalItems($count);
    $this->setItemsHandled(0);

    // Iterate through the terms.
    foreach ($terms as $t) {
      $term = unserialize(base64_decode($t->stanza));
      $this->setItemsHandled($i);

      // Add/update this term.
      if (!$this->processTerm($term, $defaultcv->name, 0, $default_db)) {
        throw new Exception("Failed to process terms from the ontology");
      }

      $i++;
    }
    $this->setItemsHandled($i);
    return 1;
  }

  /**
   * Uses the provided term array to add/update information to Chado about the
   * term including the term, dbxref, synonyms, properties, and relationships.
   *
   * @param $term
   *   An array representing the cvterm.
   * @param $defaultcv
   *   The name of the default controlled vocabulary
   * @is_relationship
   *   Set to 1 if this term is a relationship term
   * @default_db
   *   The name of the default database.
   *
   * @ingroup tripal_obo_loader
   */
  private function processTerm($term, $defaultcv, $is_relationship = 0, $default_db) {

    // make sure we have a namespace for this term
    if (!array_key_exists('namespace', $term) and !($defaultcv or $defaultcv == '')) {
      throw new Exception("Cannot add the term: no namespace defined. " . $term['id'][0]);
    }

    // construct the term array for sending to the tripal_chado_add_cvterm function
    // for adding a new cvterm
    $t = array();
    $t['id'] = $term['id'][0];
    $t['name'] = $term['name'][0];
    if (array_key_exists('def', $term)) {
      $t['definition'] = $term['def'][0];
    }
    if (array_key_exists('subset', $term)) {
      $t['subset'] = $term['subset'][0];
    }
    if (array_key_exists('namespace', $term)) {
      $t['namespace'] = $term['namespace'][0];
    }
    if (array_key_exists('is_obsolete', $term)) {
      $t['is_obsolete'] = $term['is_obsolete'][0];
    }

    $t['cv_name'] = $defaultcv;
    $t['is_relationship'] = $is_relationship;
    $t['db_name'] = $default_db;

    // add the cvterm
    $cvterm = tripal_insert_cvterm($t, array('update_existing' => TRUE));
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

    if (array_key_exists('namespace', $term)) {
      $this->newcvs[$term['namespace'][0]] = $cvterm->cv_id;
    }

    // now handle other properites
    if (array_key_exists('is_anonymous', $term)) {
      //print "WARNING: unhandled tag: is_anonymous\n";
    }
    if (array_key_exists('alt_id', $term)) {
      foreach ($term['alt_id'] as $alt_id) {
        if (!$this->addCvtermDbxref($cvterm, $alt_id)) {
          throw new Exception("Cannot add alternate id $alt_id");
        }
      }
    }

    if (array_key_exists('subset', $term)) {
      //print "WARNING: unhandled tag: subset\n";
    }
    // add synonyms for this cvterm
    if (array_key_exists('synonym', $term)) {
      if (!$this->addSynonym($term, $cvterm)) {
        throw new Exception("Cannot add synonyms");
      }
    }

    // reformat the deprecated 'exact_synonym, narrow_synonym, and broad_synonym'
    // types to be of the v1.2 standard
    if (array_key_exists('exact_synonym', $term) or array_key_exists('narrow_synonym', $term) or array_key_exists('broad_synonym', $term)) {
      if (array_key_exists('exact_synonym', $term)) {
        foreach ($term['exact_synonym'] as $synonym) {
          $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 EXACT $2', $synonym);
          $term['synonym'][] = $new;
        }
      }
      if (array_key_exists('narrow_synonym', $term)) {
        foreach ($term['narrow_synonym'] as $synonym) {
          $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 NARROW $2', $synonym);
          $term['synonym'][] = $new;
        }
      }
      if (array_key_exists('broad_synonym', $term)) {
        foreach ($term['broad_synonym'] as $synonym) {
          $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 BROAD $2', $synonym);
          $term['synonym'][] = $new;
        }
      }

      if (!$this->addSynonym($term, $cvterm)) {
        throw new Exception("Cannot add/update synonyms");
      }
    }

    // add the comment to the cvtermprop table
    if (array_key_exists('comment', $term)) {
      $comments = $term['comment'];
      $j = 0;
      foreach ($comments as $comment) {
        if (!$this->addCvtermProp($cvterm, 'comment', $comment, $j)) {
          throw new Exception("Cannot add/update cvterm property");
        }
        $j++;
      }
    }

    // add any other external dbxrefs
    if (array_key_exists('xref', $term)) {
      foreach ($term['xref'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }

    if (array_key_exists('xref_analog', $term)) {
      foreach ($term['xref_analog'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }
    if (array_key_exists('xref_unk', $term)) {
      foreach ($term['xref_unk'] as $xref) {
        if (!$this->addCvtermDbxref($cvterm, $xref)) {
          throw new Exception("Cannot add/update cvterm database reference (dbxref).");
        }
      }
    }

    // add is_a relationships for this cvterm
    if (array_key_exists('is_a', $term)) {
      foreach ($term['is_a'] as $is_a) {
        if (!$this->addRelationship($cvterm, $defaultcv, 'is_a', $is_a, $is_relationship, $default_db)) {
          throw new Exception("Cannot add relationship is_a: $is_a");
        }
      }
    }

    if (array_key_exists('intersection_of', $term)) {
      //print "WARNING: unhandled tag: intersection_of\n";
    }
    if (array_key_exists('union_of', $term)) {
      //print "WARNING: unhandled tag: union_on\n";
    }
    if (array_key_exists('disjoint_from', $term)) {
      //print "WARNING: unhandled tag: disjoint_from\n";
    }
    if (array_key_exists('relationship', $term)) {
      foreach ($term['relationship'] as $value) {
        $rel = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
        $object = preg_replace('/^.+?\s(.+?)$/', '\1', $value);
        if (!$this->addRelationship($cvterm, $defaultcv, $rel, $object, $is_relationship, $default_db)) {
          throw new Exception("Cannot add relationship $rel: $object");
        }
      }
    }
    if (array_key_exists('replaced_by', $term)) {
      //print "WARNING: unhandled tag: replaced_by\n";
    }
    if (array_key_exists('consider', $term)) {
      //print "WARNING: unhandled tag: consider\n";
    }
    if (array_key_exists('use_term', $term)) {
      //print "WARNING: unhandled tag: user_term\n";
    }
    if (array_key_exists('builtin', $term)) {
      //print "WARNING: unhandled tag: builtin\n";
    }
    return 1;
  }

  /**
   * Adds a cvterm relationship
   *
   * @param $cvterm
   *   A database object for the cvterm
   * @param $rel
   *   The relationship name
   * @param $objname
   *   The relationship term name
   * @param $defaultcv
   *   A database object containing a record from the cv table for the
   *   default controlled vocabulary
   * @object_is_relationship
   *   Set to 1 if this term is a relationship term
   * @default_db
   *   The name of the default database.
   *
   * @ingroup tripal_obo_loader
   */
  private function addRelationship($cvterm, $defaultcv, $rel,
      $objname, $object_is_relationship = 0, $default_db = 'OBO_REL') {

    // Make sure the relationship cvterm exists.
    $term = array(
      'name' => $rel,
      'id' => "$default_db:$rel",
      'definition' => '',
      'is_obsolete' => 0,
      'cv_name' => $defaultcv,
      'is_relationship' => TRUE,
      'db_name' => $default_db
    );
    $relcvterm = tripal_insert_cvterm($term, array('update_existing' => FALSE));

    if (!$relcvterm) {
      // If the relationship term couldn't be found in the default_db provided
      // then do on more check to find it in the relationship ontology
      $term = array(
        'name' => $rel,
        'id' => "OBO_REL:$rel",
        'definition' => '',
        'is_obsolete' => 0,
        'cv_name' => $defaultcv,
        'is_relationship' => TRUE,
        'db_name' => 'OBO_REL'
      );
      $relcvterm = tripal_insert_cvterm($term, array('update_existing' => FALSE));
      if (!$relcvterm) {
        throw new Exception("Cannot find the relationship term in the current ontology or in the relationship ontology: $rel\n");
      }
    }

    // Get the object term.
    $oterm = $this->getTerm($objname);
    if (!$oterm) {
      throw new Exception("Could not find object term $objname\n");
    }

    $objterm = array();
    $objterm['id']            = $oterm['id'][0];
    $objterm['name']          = $oterm['name'][0];
    if (array_key_exists('def', $oterm)) {
      $objterm['definition']           = $oterm['def'][0];
    }
    if (array_key_exists('subset', $oterm)) {
      $objterm['subset']      = $oterm['subset'][0];
    }
    if (array_key_exists('namespace', $oterm)) {
      $objterm['namespace']   = $oterm['namespace'][0];
    }
    if (array_key_exists('is_obsolete', $oterm)) {
      $objterm['is_obsolete'] = $oterm['is_obsolete'][0];
    }

    $objterm['cv_name' ] = $defaultcv;
    $objterm['is_relationship'] = $object_is_relationship;
    $objterm['db_name'] = $default_db;

    $objcvterm = tripal_insert_cvterm($objterm, array('update_existing' => TRUE));
    if (!$objcvterm) {
      throw new Exception("Cannot add cvterm " . $oterm['name'][0]);
    }

    // check to see if the cvterm_relationship already exists, if not add it
    $values = array(
      'type_id'    => $relcvterm->cvterm_id,
      'subject_id' => $cvterm->cvterm_id,
      'object_id'  => $objcvterm->cvterm_id
    );
    $result = chado_select_record('cvterm_relationship', array('*'), $values);
    if (count($result) == 0) {
      $options = array('return_record' => FALSE);
      $success = chado_insert_record('cvterm_relationship', $values, $options);
      if (!$success) {
        throw new Exception("Cannot add term relationship: '$cvterm->name' $rel '$objcvterm->name'");
      }
    }

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
  private function getTerm($id) {
    $values = array('id' => $id);
    $result = chado_select_record('tripal_obo_temp', array('stanza'), $values);
    if (count($result) == 0) {
      return FALSE;
    }
    return unserialize(base64_decode($result[0]->stanza));
  }

  /**
   * Adds the synonyms to a term
   *
   * @param term
   *   An array representing the cvterm. It must have a 'synonym' key/value pair.
   * @param cvterm
   *   The database object of the cvterm to which the synonym will be added.
   *
   * @ingroup tripal_obo_loader
   */
  private function addSynonym($term, $cvterm) {

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
          'cvterm_id' => $cvterm->cvterm_id,
          'synonym' => $def
        );
        $results = chado_select_record('cvtermsynonym', array('*'), $values);
        if (count($results) == 0) {
          $values = array(
            'cvterm_id' => $cvterm->cvterm_id,
            'synonym' => $def,
            'type_id' => $syntype->cvterm_id
          );
          $options = array('return_record' => FALSE);
          $success = chado_insert_record('cvtermsynonym', $values, $options);
          if (!$success) {
            throw new Exception("Failed to insert the synonym for term: $cvterm->name ($def)");
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
    $in_header = 1;
    $stanza = array();
    $default_db = '';
    $line_num = 0;
    $num_read = 0;
    $type = '';

    $filesize = filesize($obo_file);
    $this->setTotalItems($filesize);
    $this->setItemsHandled(0);

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

      //remove comments from end of lines
      $line = preg_replace('/^(.*?)\!.*$/', '\1', $line);  // TODO: if the explamation is escaped

      // at the first stanza we're out of header
      if (preg_match('/^\s*\[/', $line)) {
        $in_header = 0;

        // store the stanza we just finished reading
        if (sizeof($stanza) > 0) {
          // add the term to the temp table
          $values = array(
            'id' => $stanza['id'][0],
            'stanza' => base64_encode(serialize($stanza)),
            'type' => $type,
          );
          $success = chado_insert_record('tripal_obo_temp', $values);
          if (!$success) {
            throw new Exception("Cannot insert stanza into temporary table.");
          }

        }
        // get the stanza type:  Term, Typedef or Instance
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

      // if this is the ID then look for the default DB
      $matches = array();
      if ($tag == 'id' and preg_match('/^(.+?):.*$/', $value, $matches)) {
        $default_db = $matches[1];
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
      $values = array(
        'id' => $stanza['id'][0],
        'stanza' => base64_encode(serialize($stanza)),
        'type' => $type,
      );
      chado_insert_record('tripal_obo_temp', $values);
      if (!$success) {
        throw new Exception("Cannot insert stanza into temporary table.");
      }
      $this->setItemsHandled($num_read);
    }
    return $default_db;
  }

  /**
   * Adds a database reference to a cvterm
   *
   * @param cvterm
   *   The database object of the cvterm to which the synonym will be added.
   * @param xref
   *   The cross refernce.  It should be of the form from the OBO specification
   *
   * @ingroup tripal_obo_loader
   */
  private function addCvtermDbxref($cvterm, $xref) {

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
      'cvterm_id' => $cvterm->cvterm_id,
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
   * @param cvterm
   *   A database object for the cvterm to which properties will be added
   * @param $property
   *   The name of the property to add
   * @param $value
   *   The value of the property
   * @param rank
   *   The rank of the property
   *
   * @ingroup tripal_obo_loader
   */
  private function addCvtermProp($cvterm, $property, $value, $rank) {

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
      $values = array('cvterm_id' => $cvterm->cvterm_id);
      $success = chado_delete_record('cvtermprop', $values);
      if (!$success) {
        throw new Exception("Could not remove existing properties to update property $property for term\n");
      }
    }

    // now add the property
    $values = array(
      'cvterm_id' => $cvterm->cvterm_id,
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
}

/**
 * Ajax callback for the OBOImporter::form() function.
 */
function tripal_cv_obo_form_ajax_callback($form, $form_state) {
  return $form['class_elements']['obo_existing'];
}
