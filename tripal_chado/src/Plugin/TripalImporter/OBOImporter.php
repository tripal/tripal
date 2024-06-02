<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Component\Serialization\Json;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * OBO Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_obo_loader",
 *    label = @Translation("OBO Vocabulary Loader"),
 *    description = @Translation("Import vocabularies and terms in OBO format."),
 *    file_types = {"obo"},
 *    upload_description = @Translation("Please provide the details for importing a new OBO file. The file must have a .obo extension."),
 *    upload_title = @Translation("New OBO File"),
 *    use_analysis = FALSE,
 *    require_analysis = FALSE,
 *    button_text = @Translation("Import OBO File"),
 *    file_upload = FALSE,
 *    file_local = FALSE,
 *    file_remote = FALSE,
 *    file_required = FALSE,
 *  )
 */
class OBOImporter extends ChadoImporterBase {

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
   * Holds the idspace elements from the header. These will correspond
   * to the accession prefixes, or short names (e.g. GO) for the terms. For
   * example, the EDAM vocabulary has several id spaces:
   * format, data, operation and topic.
   */
  private $idspaces = [];

  /**
   * The default database prefix for this ontology.
   *
   * @var string
   */
  private $default_db = '';

  /**
   * An array of used cvterm objects so that we don't have to look them
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
   * {@inheritdoc}
   */
  public function form($form, &$form_state) {

    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $form['instructions']['info'] = [
      '#type' => 'item',
      '#markup' => t('This page allows you to load vocabularies and ontologies
        that are in OBO format. Once loaded, the terms from these
        vocabularies can be used to create content.
        You may use the form below to either reload a vocabulary that is already
        loaded (as when new updates to that vocabulary are available) or load a new
        vocabulary.'),
    ];

    // Add form elements for an existing OBO.
    $this->formExistingOBOElements($form, $form_state);

    // Add form elements for inserting a new OBO.
    $this->formNewOBOElements($form, $form_state);

    return $form;
  }

  /**
   * Adds the fields for selecing an OBO.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface  $form_state
   *   The form state object.
   */
  private function formExistingOBOElements(&$form, &$form_state) {

    $obo_id = $form_state->getValue('obo_id');

    $public = \Drupal::database();

    // get a list of db from chado for user to choose
    $sql = "SELECT * FROM {tripal_cv_obo} ORDER BY name";
    $results = $public->query($sql);

    $obos = [];
    $obos[] = 'Select a Vocabulary';
    foreach ($results as $obo) {
      $obos[$obo->obo_id] = $obo->name;
    }

    $form['obo_existing'] = [
      '#type' => 'details',
      '#title' => t('Use a Saved Ontology OBO Reference'),
      '#prefix' => '<span id="obo-existing-fieldset">',
      '#suffix' => '</span>',
      '#open' => TRUE,
    ];

    $form['obo_existing']['existing_instructions'] = [
      '#type' => 'item',
      '#markup' => t('The vocabularies listed in the select box below have been pre-populated
        upon installation of Tripal or have been previously loaded. Select one to edit
        its settings or submit for loading. You may reload any vocabulary that has
        already been loaded to retrieve any new updates.'),
    ];

    $form['obo_existing']['obo_id'] = [
      '#title' => t('Ontology OBO File Reference'),
      '#type' => 'select',
      '#options' => $obos,
      '#default_value' => $obo_id,
      '#ajax' => [
        'callback' =>  [$this::class, 'formAjaxCallback'],
        'wrapper' => 'obo-existing-fieldset',
      ],
      '#description' => t('Select a vocabulary to import.'),
    ];

    // Add the fields for updating the OBO details
    if ($obo_id) {
      $this->formEditOBOElements($form, $form_state, $obo_id);
    }
  }
  /**
   * Adds fields to the form for updating the OBO.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface  $form_state
   *   The form state object.
   * @param int $obo_id
   *   The ID of the OBO.
   */
  private function formEditOBOElements(&$form, &$form_state, $obo_id) {

    $public = \Drupal::database();

    $uobo_name = '';
    $uobo_url = '';
    $uobo_file = '';

    $query = $public->select('tripal_cv_obo', 't');
    $query->fields('t', ['name', 'path']);
    $query->condition('obo_id', $obo_id);
    $result = $query->execute();
    $vocab = $result->fetchObject();

    // If the name is a URL then keep it as is.
    $uobo_name = $vocab->name;
    if (preg_match('/^http/', $vocab->path)) {
      $uobo_url = $vocab->path;
    }
    // If the name is a local file then fix the path.
    else {
      $uobo_file = trim($vocab->path);
      $matches = [];
      if (preg_match('/\{(.*?)\}/', $uobo_file, $matches)) {
        $modpath = \Drupal::service('file_system')
          ->realpath(\Drupal::service('module_handler')
          ->getModule($matches[1])
          ->getPath());
        $uobo_file = preg_replace('/\{.*?\}/', $modpath, $uobo_file);
      }
    }

    $form['obo_existing']['uobo_name'] = [
      '#type' => 'textfield',
      '#title' => t('Vocabulary Name'),
      '#description' => t('Please provide a name for this vocabulary. After ' .
        'upload, this name will appear in the drop down list above for use again later.'),
      '#default_value' => $uobo_name,
      '#id' => 'edit-uobo-name'
    ];

    $form['obo_existing']['uobo_url'] = [
      '#type' => 'textfield',
      '#title' => t('Remote URL'),
      '#description' => t('Please enter a URL for the online OBO file. The file '.
        'will be downloaded and parsed. (e.g. https://raw.githubusercontent.com/oborel/obo-relations/master/ro.obo)'),
      '#default_value' => $uobo_url,
      '#id' => 'edit-uobo-url'
    ];

    $form['obo_existing']['uobo_file'] = [
      '#type' => 'textfield',
      '#title' => t('Local File'),
      '#description' => t('Please enter the file system path for an OBO ' .
        'definition file. If entering a path relative to ' .
        'the Drupal installation you may use a relative path that excludes the ' .
        'Drupal installation directory (e.g. sites/default/files/xyz.obo). Note ' .
        'that Drupal relative paths have no preceeding slash. ' .
        'Otherwise, please provide the full path on the filesystem. The path ' .
        'must be accessible to the web server on which this Drupal instance is running.'),
      '#default_value' => $uobo_file,
      '#id' => 'edit-uobo-file'
    ];
    $form['obo_existing']['update_obo'] = [
      '#type' => 'submit',
      '#value' => 'Update Ontology Details',
      '#name' => 'update_obo',
    ];
    $form['obo_existing']['delete_obo'] = [
      '#type' => 'submit',
      '#value' => 'Delete Ontology',
      '#name' => 'delete_obo',
    ];
  }

  /**
   * Adds fields to the form for inserting a new OBO.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface  $form_state
   *   The form state object.
   */
  private function formNewOBOElements(&$form, &$form_state) {

    $form['obo_new'] = [
      '#type' => 'details',
      '#title' => t('Add a New Ontology OBO Reference'),
      '#open' => FALSE,
    ];

    $form['obo_new']['path_instructions'] = [
      '#value' => t('Provide the name and path for the OBO file. If the vocabulary OBO file ' .
        'is stored local to the server provide a file name. If the vocabulary is stored remotely, ' .
        'provide a URL. Only provide a URL or a local file, not both.'),
    ];

    $form['obo_new']['obo_name'] = [
      '#type' => 'textfield',
      '#title' => t('New Vocabulary Name'),
      '#description' => t('Please provide a name for this vocabulary. After upload, this name will appear in the drop down ' .
        'list above for use again later. Additionally, if a default namespace is not provided in the OBO ' .
        'header this name will be used as the default_namespace.'),
    ];

    $form['obo_new']['obo_url'] = [
      '#type' => 'textfield',
      '#title' => t('Remote URL'),
      '#description' => t('Please enter a URL for the online OBO file.  The file will be downloaded and parsed. ' .
                          '(e.g. https://raw.githubusercontent.com/oborel/obo-relations/master/ro.obo)'),
    ];

    $form['obo_new']['obo_file'] = [
      '#type' => 'textfield',
      '#title' => t('Local File'),
      '#description' => t('Please enter the file system path for an OBO ' .
        'definition file. If entering a path relative to ' .
        'the Drupal installation you may use a relative path that excludes the ' .
        'Drupal installation directory (e.g. sites/default/files/xyz.obo). Note ' .
        'that Drupal relative paths have no preceeding slash. ' .
        'Otherwise, please provide the full path on the filesystem.  The path ' .
        'must be accessible to the web server on which this Drupal instance is running.'),
    ];
  }

  /**
   * Ajax callback for the OBOImporter::form() function.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public static function formAjaxCallback($form, &$form_state) {

    $uobo_name = $form['obo_existing']['uobo_name']['#default_value'];
    $uobo_url = $form['obo_existing']['uobo_url']['#default_value'];
    $uobo_file = $form['obo_existing']['uobo_file']['#default_value'];

    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#obo-existing-fieldset', $form['obo_existing']));
    $response->addCommand(new InvokeCommand('#edit-uobo-name', 'val', [$uobo_name]));
    $response->addCommand(new InvokeCommand('#edit-uobo-url', 'val', [$uobo_url]));
    $response->addCommand(new InvokeCommand('#edit-uobo-file', 'val', [$uobo_file]));

    return $response;
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    $public = \Drupal::database();

    $obo_id = $form_state->getValue('obo_id');
    $obo_name = $form_state->getValue('obo_name');
    $obo_url = $form_state->getValue('obo_url');
    $obo_file = $form_state->getValue('obo_file');
    $uobo_name = $form_state->getValue('uobo_name');
    $uobo_url = $form_state->getValue('uobo_url');
    $uobo_file = $form_state->getValue('uobo_file');
    // Now trim variables. We do it this way to avoid trimming an empty value.
    foreach(['obo_name', 'obo_url', 'obo_file', 'uobo_name', 'uobo_url', 'uobo_file'] as $varname) {
      if (!empty($$varname)) {
        $$varname = trim($$varname);
      }
    }

    // If the user requested to alter the details then do that.

    if ($form_state->getTriggeringElement()['#name'] == 'update_obo') {

      $form_state->setRebuild(True);
      $query = $public->update('tripal_cv_obo');
      $query->fields([
        'name' => $uobo_name,
        'path' => $uobo_url ? $uobo_url : $uobo_file,
      ]);
      $query->condition('obo_id', $obo_id);
      $success = $query->execute();
      if ($success) {
        \Drupal::messenger()->addMessage(t("The vocabulary @vocab has been updated.", ['@vocab' => $uobo_name]));
      }
      else {
        \Drupal::messenger()->addError(t("The vocabulary @vocab could not be updated.", ['@vocab' => $uobo_name]));
      }

    }
    elseif ($form_state->getTriggeringElement()['#name'] == 'delete_obo') {
      $form_state->setRebuild(True);
      $query = $public->delete('tripal_cv_obo');
      $query->condition('obo_id', $obo_id);
      $success = $query->execute();
      if ($success) {
        \Drupal::messenger()->addMessage(t("The vocabulary @vocab has been deleted.", ['@vocab' => $uobo_name]));
      }
      else {
        \Drupal::messenger()->addError(t("The vocabulary @vocab could not be deleted.", ['@vocab' => $uobo_name]));
      }
    }
    elseif (!empty($obo_name)) {
      $obo_id = $public->insert('tripal_cv_obo')
        ->fields([
          'name' => $obo_name,
          'path' => $obo_url ? $obo_url : $obo_file,
        ])
        ->execute();

      // Add the obo_id to the form_state values.
      $form_state->setValue('obo_id', $obo_id);

      if ($obo_id) {
        \Drupal::messenger()->addMessage(t("The vocabulary @vocab has been added.", ['@vocab' => $obo_name]));
      }
      else {
        $form_state->setRebuild(True);
        \Drupal::messenger()->addError(t("The vocabulary @vocab could not be added.", ['@vocab' => $obo_name]));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function formValidate($form, &$form_state) {

    $obo_id = $form_state->getValue('obo_id');
    $obo_name = trim($form_state->getValue('obo_name') ?? '');
    $obo_url = trim($form_state->getValue('obo_url') ?? '');
    $obo_file = trim($form_state->getValue('obo_file') ?? '');
    $uobo_name = trim($form_state->getValue('uobo_name') ?? '');
    $uobo_url = trim($form_state->getValue('uobo_url') ?? '');
    $uobo_file = trim($form_state->getValue('uobo_file') ?? '');

    // Possible triggering element #name values:
    //   'op' from the default submit 'Import OBO File'
    //   'update_obo' and 'obo_id' from 'Update Ontology Details'
    //   'delete_obo' and 'obo_id' two times from 'Delete Ontology'

    // Submitted with 'Update Ontology Details' button
    if ($form_state->getTriggeringElement()['#name'] == 'update_obo' ) {

      // Make sure if the name is changed it doesn't conflict with another OBO.
      $vocab_id = $this->getVocabID($uobo_name);
      if ($vocab_id and $vocab_id != $obo_id) {
        $form_state->setErrorByName('uobo_name', t('The vocabulary name must be different from existing vocabularies'));
      }
      // If file specified, make sure it exists, either as a relative or absolute path.
      if (!$this->formValidateFile($uobo_file)) {
        $form_state->setErrorByName('uobo_file',
            t('The specified path, @path, does not exist or cannot be read.', ['@path' => $uobo_file]));
      }
      if (!$uobo_url and !$uobo_file) {
        $form_state->setErrorByName('uobo_url', t('Please provide either a URL or a path for the vocabulary.'));
      }
      if ($uobo_url and $uobo_file) {
        $form_state->setErrorByName('uobo_url', t('Please provide only a URL or a path for the vocabulary, but not both.'));
      }
    }

    // Submitted with 'Import OBO File' button. This is used both for
    // reloading a saved ontology and for loading a new ontology.
    if ($form_state->getTriggeringElement()['#name'] == 'op') {
      if ($uobo_name and $obo_name) {
        $form_state->setErrorByName('obo_name', t('New and existing ontologies are both selected, please select only one'));
      }
      // Generate error if supplied new vocabulary name already exists.
      if ($obo_name) {
        $vocab_id = $this->getVocabID($obo_name);
        if ($vocab_id) {
          $form_state->setErrorByName('obo_name', t('The vocabulary name must be different from existing vocabularies'));
        }
      }
      if ($uobo_name) {
        // Validate the update existing ontology section
        if (!$uobo_url and !$uobo_file) {
          $form_state->setErrorByName('uobo_url', t('Please provide either a URL or a path for the vocabulary.'));
        }
        if ($uobo_url and $uobo_file) {
          $form_state->setErrorByName('uobo_url', t('Please provide only a URL or a path for the vocabulary, but not both.'));
        }
        // If file specified, make sure it exists, either as a relative or absolute path.
        if (!$this->formValidateFile($uobo_file)) {
          $form_state->setErrorByName('uobo_file',
              t('The specified path, @path, does not exist or cannot be read.', ['@path' => $uobo_file]));
        }
      }
      else {
        // Validate the load new ontology section
        if (!$obo_url and !$obo_file) {
          $form_state->setErrorByName('obo_url', t('Please provide either a URL or a path for the vocabulary.'));
        }
        if ($obo_url and $obo_file) {
          $form_state->setErrorByName('obo_url', t('Please provide only a URL or a path for the vocabulary, but not both.'));
        }
        // If file specified, make sure it exists, either as a relative or absolute path.
        if (!$this->formValidateFile($obo_file)) {
          $form_state->setErrorByName('obo_file',
              t('The specified path, @path, does not exist or cannot be read.', ['@path' => $obo_file]));
        }
      }
    }
  }

  /**
   * Returns the obo_id in the tripal_cv_obo table
   * from the supplied vocabulary name.
   *
   * @param string $vocab_name
   *   The name of the vocabulary to query in the
   *   public.tripal_cv_obo table.
   * @return int
   *   The obo_id value of the vocabulary, or NULL if this
   *   vocabulary does not exist.
   */
  private function getVocabID($vocab_name) {
    $obo_id = NULL;
    $public = \Drupal::database();
    $vocab = $public->select('tripal_cv_obo', 't')
      ->fields('t', ['obo_id', 'name', 'path'])
      ->condition('name', $vocab_name)
      ->execute()
      ->fetchObject();
    if ($vocab) {
      $obo_id = $vocab->obo_id;
    }
    return $obo_id;
  }

  /**
   * Validates that the passed file specifier exists either as
   * specified, or when the default base path is prepended.
   * Valid also if no file is specified.
   *
   * @param string $file
   *   A file on the local filesystem.
   * @return bool
   *   Returns TRUE if $file exists or if $file evaluates to FALSE.
   *   Returns FALSE if file identifier does not exist.
   */
  private function formValidateFile($file) {
    if ($file) {
      $checkpath = $_SERVER['DOCUMENT_ROOT'] . base_path() . $file;
      if (!file_exists($checkpath) and !file_exists($file)) {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Retrieve a Chado cvterm record using cvterm ID.
   *
   * @param int $cvterm_id
   *   The  CV term Id.
   * @return object
   *   A CVterm object
   */
  private function getChadoCvtermById($cvterm_id) {
    $chado = $this->getChadoConnection();

    $query = $chado->select('1:cvterm', 'CVT');
    $query->fields('CVT');
    $query->condition('CVT.cvterm_id', $cvterm_id);
    $result = $query->execute();
    $cvterm = NULL;
    if ($result) {
      $cvterm = $result->fetchObject();
    }
    return $cvterm;
  }

  /**
   * Retrieve a Chado cvterm record using the full accession.
   *
   * @param string $idSpace
   *   The databaes name
   * @param string $accession
   *   The CV term accession.
   * @return object
   *   A CVterm object
   */
  private function getChadoCvtermByAccession($idSpace, $accession) {
    $chado = $this->getChadoConnection();

    $query = $chado->select('1:cvterm', 'CVT');
    $query->join('1:dbxref', 'DBX', '"DBX".dbxref_id = "CVT".dbxref_id');
    $query->join('1:db', 'DB', '"DB".db_id = "DBX".db_id');
    $query->fields('CVT');
    $query->condition('DB.name', $idSpace, '=');
    $query->condition('DBX.accession', $accession, '=');
    $cvterm = $query->execute()->fetchObject();
    if (!$cvterm) {
      throw new \Exception("OBOImporter: Could not find term: '$idSpace:$accession'");
    }
    return $cvterm;
  }

  /**
   * Retrieve a Chado cvterm record using the cv and term name.

   * @param int $cv_id
   *   The ID of the cv record to which the term belongs.
   * @param string $name
   *   The name of the term.
   * @return object
   *   A CVterm object
   */
  private function getChadoCvtermByName($cv_id, $name) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:cvterm', 'CVT');
    $query->fields('CVT');
    $query->condition('cv_id', $cv_id);
    $query->condition('name', $name);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Retrieve a Chado cvterm record using the dbxref.
   *
   * @param int $dbxref_id
   * @return object
   *   A CVterm object
   */
  private function getChadoCvtermByDbxref($dbxref_id) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:cvterm', 'CVT');
    $query->fields('CVT');
    $query->condition('CVT.dbxref_id', $dbxref_id);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Retreives a dbxref record using the db_id and accession.
   *
   * @param int $db_id
   *   The ID of the database record for the term.
   * @param string $accession
   *   The term accession
   * @return object
   *   An dbxref object.
   */
  private function getChadoDBXrefByAccession($db_id, $accession) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:dbxref', 'DBX');
    $query->fields('DBX');
    $query->condition('DBX.db_id', $db_id);
    $query->condition('DBX.accession', $accession);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Retreives a dbxref record using the dbxref_id.
   *
   * @param int $dbxref_id
   *   The ID of the dbxref record.
   * @return object
   *   An dbxref object.
   */
  private function getChadoDBXrefById($dbxref_id) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:dbxref', 'DBX');
    $query->fields('DBX');
    $query->condition('DBX.dbxref_id', $dbxref_id);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Gets a record from the `db` table of Chado using the anme.
   *
   * @param string $name
   *   The databae name.
   * @return object
   *   A DB record object.
   */
  private function getChadoDbByName($name) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:db', 'db');
    $query->fields('db');
    $query->condition('name', $name);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }
  /**
   * Gets a record from the `db` table of Chado using the Id.
   *
   * @param string $name
   *   The databae name.
   * @return object
   *   A DB record object.
   */
  private function getChadoDbById($db_id) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:db', 'db');
    $query->fields('db');
    $query->condition('db_id', $db_id);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Gets a record from the `cv` table of Chado by name.
   *
   * @param string $name
   *   The vocabulary name.
   * @return object
   *   A CV record object.
   */
  private function getChadoCvByName($name) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:cv', 'cv');
    $query->fields('cv');
    $query->condition('name', $name);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Gets a record from the `cv` table of Chado by the Id.
   *
   * @param int $cv_id
   *   The vocabulary name.
   * @return object
   *   A CV record object.
   */
  private function getChadoCvById($cv_id) {
    $chado = $this->getChadoConnection();
    $query = $chado->select('1:cv', 'cv');
    $query->fields('cv');
    $query->condition('cv_id', $cv_id);
    $result = $query->execute();
    return $result ? $result->fetchObject() : NULL;
  }

  /**
   * Gets a controlled vocabulary IDspace object.
   *
   * @param string $name
   *   The name of the IdSpace
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  private function getIdSpace($name) {
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($name, 'chado_id_space');
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($name, 'chado_id_space');
    }
    return $idSpace;
  }

  /**
   * Gets a controlled vocabulary object.
   *
   * @param string $name
   *   The name of the vocabulary
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalVocabularyBase
   */
  private function getVocabulary($name) {
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocabulary = $vmanager->loadCollection($name, 'chado_vocabulary');
    if (!$vocabulary) {
      $vocabulary = $vmanager->createCollection($name, 'chado_vocabulary');
    }
    return $vocabulary;
  }



  /**
   * {@inheritdoc}
   */
  public function run() {
    $public = \Drupal::database();
    $chado = $this->getChadoConnection();

    $arguments = $this->arguments['run_args'];
    $obo_id = $arguments['obo_id'];

    // Make sure the $obo_id is valid
    $obo = $public->select('tripal_cv_obo', 'tco')
      ->fields('tco')
      ->condition('obo_id', $obo_id)
      ->execute()
      ->fetchObject();
    if (!$obo) {
      throw new \Exception("Invalid OBO ID provided: '$obo_id'.");
    }

    // Get the list of all CVs so we can save on lookups later
    $sql = "SELECT * FROM {1:cv} CV";
    $cvs = $chado->query($sql);
    while ($cv = $cvs->fetchObject()) {
      $this->all_cvs[$cv->name] = $cv;
    }

    // Get the list of all DBs so we can save on lookups later
    $sql = "SELECT * FROM {1:db} DB";
    $dbs = $chado->query($sql);
    while ($db = $dbs->fetchObject()) {
      $this->all_dbs[$db->name] = $db;
    }

    // Get the 'Subgroup' term that we will use for adding subsets.
    $term = $this->getChadoCVtermByAccession('NCIT', 'C25693');
    $this->used_terms['NCIT:C25693'] = $term->cvterm_id;

    // Get the 'Comment' term that we will use for adding comments.
    $term = $this->getChadoCVtermByAccession('rdfs', 'comment');
    $this->used_terms['rdfs:comment'] = $term->cvterm_id;

    // Make sure we have a 'synonym_type' vocabulary.
    $syn_cv = $this->getVocabulary('synonym_type');
    $syn_db = $this->getIdSpace('synonym_type');
    $this->all_cvs['synonym_type'] = $this->getChadoCvByName('synonym_type');
    $this->all_dbs['synonym_type'] = $this->getChadoDbByName('synonym_type');

    // Make sure the synonym types exists in the 'synonym_type' vocabulary.
    foreach (array_keys($this->syn_types) as $syn_type) {
      $syn_term = new TripalTerm([
        'name' => $syn_type,
        'accession' => $syn_type,
        'idSpace' => 'synonym_type',
        'vocabulary' => 'synonym_type',
      ]);
      $syn_db->saveTerm($syn_term);
      $this->syn_types[$syn_type] = $this->getChadoCVtermByAccession('synonym_type', $syn_type);
    }

    // Run the importer!
    $this->loadOBO_v1_2_id($obo);
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {

    // Update the cv_root_mview materialized view.
    $this->logger->notice("Updating the cv_root_mview materialized view...");
    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create('cv_root_mview', $this->chado_schema_main);
    $mview->populate();

    $this->logger->notice("Updating the db2cv_mview materialized view...");
    $mviews = \Drupal::service('tripal_chado.materialized_views');
    $mview = $mviews->create('db2cv_mview', $this->chado_schema_main);
    $mview->populate();

    // @todo uncomment this when the chado_update_cvtermpath() function is ported.
//     // Update the cvtermpath table for each newly added CV.
//     $this->logger->notice("Updating cvtermpath table. This may take a while...");
//     foreach ($this->obo_namespaces as $namespace => $cv_id) {
//       $this->logger->notice("- Loading paths for vocabulary: @vocab", ['@vocab' => $namespace]);
//       chado_update_cvtermpath($cv_id, $this->job);
//     }
  }

  /**
   * Imports an OBO by specifying a tripal_cv_obo ID.
   *
   * It requires that the file be in OBO v1.2 compatible format.
   *
   * @param object $obo
   *   An object containing the record from the tripal_cv_obo table.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_id($obo) {

    // Convert the module name to the real path if present
    $matches = [];
    if (preg_match("/\{(.*?)\}/", $obo->path, $matches)) {
      $module = $matches[1];
      $path = \Drupal::service('file_system')
        ->realpath(\Drupal::service('module_handler')
        ->getModule($module)
        ->getPath());

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
          $this->logger->error( "Could not find OBO file: '$obo->path'");
        }
      }
    }
  }

  /**
   * Imports an OBO by specifying a local file.
   *
   * @param string $obo_name
   *   The name of the OBO (typically the ontology or controlled vocabulary
   *   name)
   * @param string $file
   *   The path on the file system where the ontology can be found
   * @param bool $is_new
   *   Set to TRUE if this is a new ontology that does not yet exist in the
   *   tripal_cv_obo table. If TRUE the OBO will be added to the table.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_file($obo_name, $file, $is_new = TRUE) {
    if ($is_new) {
      tripal_insert_obo($obo_name, $file);
    }
    $this->loadOBO_v1_2($file, $obo_name);
  }

  /**
   * Imports an OBO by specfying a remote URL.
   *
   * @param string $obo_name
   *   The name of the OBO (typically the ontology or controlled vocabulary
   *   name)
   * @param string $url
   *   The remote URL of the OBO file.
   * @param bool $is_new
   *   Set to TRUE if this is a new ontology that does not yet exist in the
   *   tripal_cv_obo table.  If TRUE the OBO will be added to the table.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2_url($obo_name, $url, $is_new = TRUE) {

    // first download the OBO
    $temp = tempnam(sys_get_temp_dir(), 'obo_');
    $this->logger->notice("Downloading URL $url, saving to $temp");
    $url_fh = fopen($url, "r");
    $obo_fh = fopen($temp, "w");
    if (!$url_fh) {
      throw new \Exception("Unable to download the remote OBO file at $url. " .
        "Could a firewall be blocking outgoing connections? If you are unable " .
        "to download the file you may manually download the OBO file and use " .
        "the web interface to specify the location of the file on your server.");
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
   * Imports the OBO.
   *
   * This function should not be called directly. Instead it is called by
   * loadOBO_v1_2_url, loadOBO_v1_2_file or loadOBO_v1_2_id.
   *
   * @param string $file
   *   The full path to the OBO file on the file system.
   *
   * @ingroup tripal_obo_loader
   */
  private function loadOBO_v1_2($file, $obo_name) {
    $header = [];

    // Empty the temp table.
    $this->clearTermStanzaCache();

    $this->logger->notice("Importing into schema: " . $this->chado_schema_main);

    // Parse the obo file.
    $this->logger->notice("Step 1: Preloading File $file...");
    $this->parse($file, $header);

    // Cache the relationships of terms.
    $this->logger->notice("Step 2: Examining relationships...");
    $this->cacheRelationships();

    // Add any typedefs to the vocabulary first.
    $this->logger->notice("Step 3: Loading type defs...");
    $this->processTypeDefs();

    // Next add terms to the vocabulary.
    $this->logger->notice("Step 4: Loading terms...");
    $this->processTerms();

    // Empty the term cache.
    $this->logger->notice("Step 5: Cleanup...");
    $this->clearTermStanzaCache();
  }

  /**
   * Process the type definitions in the OBO.
   *
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
   * Process the terms in the OBO.
   */
  private function processTerms() {
    $i = 0;

    $terms = $this->getCachedTermStanzas('Term');
    $count = $this->getCacheSize('Term');
    $this->setTotalItems($count);
    $this->setItemsHandled(0);
    $this->setInterval(1);

    // Iterate through the terms.
    foreach ($terms as $t) {
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
   * @param array $header
   *   The OBO header.
   */
  private function setDefaults($header) {
    $short_name = '';
    $namespace = '';
    $idspaces = [];

    // Get the 'ontology' and 'default-namespace' headers.  Unfortunately,
    // not all OBO files contain these.
    if (array_key_exists('ontology', $header)) {
      $short_name = strtoupper($header['ontology'][0]);
    }
    if (array_key_exists('default-namespace', $header)) {
      $namespace = $header['default-namespace'][0];
    }
    if (array_key_exists('idspace', $header)) {
      $matches = [];
      foreach ($header['idspace'] as $idspace) {
        if (preg_match('/^(.+?)\s+(.+?)\s+"(.+)"$/', $idspace, $matches)) {
          $idspaces[$matches[1]]['url'] = $matches[2];
          $idspaces[$matches[1]]['description'] = $matches[3];
        }
        elseif (preg_match('/^(.+?)\s+(.+?)$/', $idspace, $matches)) {
          $idspaces[$matches[1]]['url'] = $matches[2];
          $idspaces[$matches[1]]['description'] = '';
        }
      }
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
      $chado = $this->getChadoConnection();

      // First see if we've seen this ontology before and get its currently
      // loaded database.
      $sql = "SELECT dbname FROM {1:db2cv_mview} WHERE cvname = :cvname";
      $short_name = $chado->query($sql, [':cvname' => $namespace])->fetchField();

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

    // If we still don't have a namespace defined, use the one from the form
    // in the "New Vocabulary Name" field
    if (!$namespace and array_key_exists('run_args', $this->arguments)
        and array_key_exists('obo_name', $this->arguments['run_args'])) {
      $namespace = $this->arguments['run_args']['obo_name'];
    }
    if (!$namespace and array_key_exists('run_args', $this->arguments)
        and array_key_exists('uobo_name', $this->arguments['run_args'])) {
      $namespace = $this->arguments['run_args']['uobo_name'];
    }

    // If we can't find the namespace or the short_name then bust.
    if (!$namespace and !$short_name) {
      throw new \Exception('Cannot determine the namespace or ontology " .
        "prefix from this OBO file. It is missing both the "default-namespace " .
        "or a compatible "ontology" header.');
    }

    // Set the defaults.
    $this->default_namespace = $namespace;
    $this->default_db = $short_name;
    $this->insertChadoDb($this->default_db);
    $cv = $this->insertChadoCv($this->default_namespace);
    $this->obo_namespaces[$namespace] = $cv->cv_id;
    $this->idspaces = $idspaces;

    // Add a new database for each idspace.
    foreach ($idspaces as $shortname => $idspace) {
      $this->insertChadoDb($shortname, $idspace['url'], $idspace['description']);
    }
  }

  /**
   * Searches EBI to find the ontology details.
   *
   * @param string $ontology
   *   The ontology name from the OBO headers.
   */
  private function findEBIOntologyNamespace($ontology) {

    // Check if the EBI ontology search has this ontology:
    try {
      $results = $this->oboEbiLookup($ontology, 'ontology');
      if ($results and array_key_exists('config', $results) and array_key_exists('default-namespace', $results['config']['annotations'])) {
        $namespace = $results['config']['annotations']['default-namespace'];
        if (is_array($namespace)) {
          $namespace = $namespace[0];
        }
      }
      elseif ($results and array_key_exists('config', $results) and array_key_exists('namespace', $results['config'])) {
        $namespace = $results['config']['namespace'];
      }
      // If we can't find the namespace at EBI, then just default to using the
      // same namespace as the DB short name.
      else {
        $namespace = $this->default_db;
      }

      return $namespace;
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      throw $e;
    }
  }

  /**
   * Finds the ontology prefix (DB short name) using EBI.
   *
   * @param string $namespace
   *   The namespace for ontology.
   */
  private function findEBIOntologyPrefix($namespace) {

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
   * Gets details about a foreign term.
   *
   * A foreign term is one that does not belong to the ontology.
   *
   * @param int $id
   *   A term array that contains these keys at a minimum: id, name,
   *   definition, subset, namespace, is_obsolete.
   */
  private function findEBITerm($id) {

    // Warn the user if we're looking up terms in EBI as this will slow the
    // loader if there are many lookups.
    if ($this->ebi_warned == FALSE) {
      $this->logger->warning(t(
        "A term that belongs to another ontology is used within this " .
        "vocabulary.  Therefore a lookup will be performed with the EBI Ontology " .
        "Lookup Service to retrieve the information for this term. " .
        "Please note, that vocabularies with many non-local terms " .
        "require remote lookups and these lookups can dramatically " .
        "increase loading time. "));
      $this->ebi_warned = TRUE;

      // This ontology may have multiple remote terms and that takes a while
      // to load so lets change the progress interval down to give
      // updates more often.
      $this->setInterval(1);
    }

    $this->logger->notice('Performing EBI OLS Lookup for: @id', ['@id' => $id]);

    // Get the short name and accession for the term.
    $pair = explode(":", $id, 2);
    $short_name = $pair[0];
    $accession = $pair[1];

    // First get the ontology so we can build an IRI for the term
    $base_iri = '';
    $ontologyID = '';
    $iri = '';
    $type ='';
    if (array_key_exists($short_name, $this->baseIRIs)) {
      list($ontologyID, $base_iri) = $this->baseIRIs[$short_name];
    }
    else {
      $ontology_results =  $this->oboEbiLookup($id, 'query');
      if ($ontology_results === FALSE OR !is_array($ontology_results)) {
        throw new \Exception(t('Did not get a response from EBI OLS trying to lookup ontology: !id',
          ['!id' => $ontologyID]));
      }
      // If results were received but the number of results is 0, do a query-non-local lookup.
      if ($ontology_results['response']['numFound'] == 0) {
        $ontology_results =  $this->oboEbiLookup($id, 'query-non-local');
      }
      if (array_key_exists('error', $ontology_results) AND !empty($ontology_results['error'])) {
        $message = t('Cannot find the ontology via an EBI OLS lookup: @short_name. ' .
          'EBI Reported: @message. Consider finding the OBO file for this ' .
          ' ontology and manually loading it first.', ['@message' => $ontology_results['message'],
            '@short_name' => $short_name]);
        $this->logger->error($message);
        throw new \Exception('Unable to lookup ontology via EBI. See previous error for details.');
      }
      // If results were received but the number of results is 0 and we already
      // tried a query-non-local lookup then we just have to admin defeat.
      if ($ontology_results['response']['numFound'] == 0) {
        $this->logger->error('Cannot find the ontology via an EBI OLS lookup: @short_name. ' .
          'While EBI did not provide an error, no results were found. Consider ' .
          ' finding the OBO file for this ontology and manually loading it first.',
          ['@short_name' => $short_name]);
        throw new \Exception('Unable to lookup ontology via EBI. See previous error for details.');
      }
      // The following foreach code works but, I am not sure that
      // I am retrieving each query result from the json associative
      // array with the correct style
      foreach ($ontology_results['response']['docs'] as $each ) {
        $obo_id = $each['obo_id'];
        $defining_ontology = $each['is_defining_ontology'];
        // First result should have defining_ontology=true, but if
        // it doesn't, use the first result with obo_id=$id
        if ($defining_ontology == 'false' and $obo_id != $id) {
          continue;
        }
        $iri = urlencode(urlencode($each['iri']));
        $ontologyID = $each['ontology_name'];
        // Type should be 'property' or 'class' in the response
        $type = $each['type'];
      }
    }

    // Next get the term.
    $query = $type;
    if ($type == 'class') {
      $query = 'term';
    }
    $results =  $this->oboEbiLookup($id, $query);
    if (!$results) {
      $query = 'query-non-local';
      $ontology_results = $this->oboEbiLookup($id, $query);
      if ($ontology_results) {
        foreach ($ontology_results['response']['docs'] as $each ){
          $obo_id = $each['obo_id'];
          $defining_ontology = $each['is_defining_ontology'];
          if (!$defining_ontology and $obo_id != $id ) {
            continue;
          }
          $found_iri = urlencode(urlencode($each['iri']));
          $ontology = $each['ontology_name'];
          // Type should be 'property' or 'class' in the response
          $type = $each['type'];
          $results = $this->oboEbiLookup($id, $type, $found_iri, $ontology);
          // if this term is the defining_ontology and we have the correct ID,
          // we don't need more, get it and stop
          break;
        }
      }
    }
    if (!$results) {
      $message = t('Did not get a response from EBI OLS trying to lookup: @type @id',
          ['@type'=> $type, '@id' => $id]);
      $this->logger->error($message);
      throw new \Exception($message);
    }

    // If EBI sent an error message then throw an error.
    if (array_key_exists('error', $results) AND !empty($results['error'])) {
      $message = t('Cannot find the term via an EBI OLS lookup: @term. EBI ' .
        'Reported: @message. Consider finding the OBO file for this ontology ' .
         'and manually loading it first.', ['@message' => $results['message'], '@term' => $id]);
      $this->logger->error($message);
      return FALSE;
    }

    // What do we do if the term is not defined by this ontology?
    if ($results['is_defining_ontology'] != 1) {

    }

    // Make an OBO stanza array as if this term were in the OBO file and
    // return it.
    $this->logger->notice("Found @term in EBI OLS.", ['@term' => $id]);
    $stanza = [];
    $stanza['id'][0] = $id;
    $stanza['name'][0] = $results['label'];
    $stanza['def'][0] = (array_key_exists('def', $results)) ? $results['def'] : '';
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
      $this->logger->notice(t("The term, @term, is replaced by, @replaced",
        ['@term' => $id, '@replaced' => $replaced_by]));

      // Before we try to look for the replacement term, let's try to find it.
      // in our list of cached terms.
      if (array_key_exists($replaced_by, $this->termStanzaCache['ids'])) {
        $this->logger->notice(t("Found term, @replaced in the term cache.",
          ['@term' => $id, '!replaced' => $replaced_by]));
        return $this->termStanzaCache['ids'][$id];
      }

      // Next look in the database.
      $rpair = explode(":", $replaced_by, 2);
      $found = $this->lookupTerm($rpair[0], $rpair[1]);
      if ($found) {
        $this->logger->notice(t("Found term, @replaced in the local data store.",
          ['@term' => $id, '@replaced' => $replaced_by]));
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
   * @param array $stanza
   *   An OBO stanza array as returned by getCachedTermStanza().
   * @param bool $is_relationship
   *   Set to TRUE if this term is a relationship term.
   *
   * @return int
   *   The cvterm ID.
   */
  private function saveTerm($stanza, $is_relationship = FALSE) {
    $chado = $this->getChadoConnection();

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

    // Will hold the cvterm object.
    $cvterm = NULL;

    // Get the CV and DB objects.
    $cv = $this->all_cvs[$cvname];
    $db = $this->all_dbs[$dbname];

    try {

      // If this is set to TRUE then we should insert the term.
      $do_cvterm_insert = TRUE;

      // We need to locate terms using their dbxref. This is because term names
      // can sometimes change, so we don't want to look up the term by its name.
      // The unique ID which is in the accession will never change.
      $dbxref = $this->getChadoDBXrefByAccession($db->db_id, $accession);
      if ($dbxref) {

        // Get the cvterm that is associated with this dbxref.
        $cvterm = $this->getChadoCvtermByDbxref($dbxref->dbxref_id);
        if ($cvterm) {
          $do_cvterm_insert = FALSE;

          // We don't want to do any updates for borrowed terms. Just leave them
          // as they are.
          if (!$is_borrowed) {

            // Let's make sure we don't have a conflict in term naming
            // if we change the name of this term.
            $this->fixTermMismatch($stanza, $dbxref, $cv, $name);

            // Now update this cvterm record.
            $query = $chado->update('1:cvterm');
            $query->fields([
              'name' => $name,
              'definition' => $definition,
              'is_obsolete' => $is_obsolete,
              'is_relationshiptype' => $is_relationshiptype,
            ]);
            $query->condition('cvterm_id', $cvterm->cvterm_id);
            $success = $query->execute();
            if (!$success) {
              $message = t('Could not update the term, "@term", with name, ' .
                '"@name" for vocabulary, "@vocab": @error.', [
                '@term' => $id, '@name' => $name, '@vocab' => $cv->name]);
              throw new \Exception($message);
            }
          }
        }
      }
      // The dbxref doesn't exist, so let's add it.
      else {
        $dbxref = $this->insertChadoDbxref($db->db_id, $accession);
      }


      // Add the cvterm if we didn't do an update.
      if ($do_cvterm_insert) {

        // Before updating the term let's check to see if it already exists
        // and if it does we need to fix the other term.
        $cvterm = $this->getChadoCVtermByName($cv->cv_id, $name);
        if ($cvterm) {
          $this->fixTermMismatch($stanza, $dbxref, $cv, $name);
        }

        // Now insert.
        $query = $chado->insert('1:cvterm');
        $query->fields([
          'cv_id' => $cv->cv_id,
          'name' => $name,
          'definition' => $definition,
          'dbxref_id' => $dbxref->dbxref_id,
          'is_relationshiptype' => $is_relationshiptype,
          'is_obsolete' => $is_obsolete,
        ]);
        $success = $query->execute();
        if (!$success) {
          $message = t('Could not insert the cvterm, "@term"', [
            '@term' => $name]);
          throw new \Exception($message);
        }
        $cvterm = $this->getChadoCVtermByName($cv->cv_id, $name);
      }
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      throw $e;
    }

    // Save the cvterm_id for this term so we don't look it up again.
    $cvterm_id = $cvterm->cvterm_id;
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
   * @param array $stanza
   *   The term stanza from the OBO file.
   * @param object $dbxref
   *   The dbxref object containing the dbxref record for the term
   *   to be inserted/updated.
   * @param object $cv
   *   The cvterm object.
   * @param string $name
   *   The name of the term that is a potential conflict.
   *
   * @return bool
   *   Returns TRUE if a conflict was found and corrected.
   */
  private function fixTermMismatch($stanza, $dbxref, $cv, $name) {
    $chado = $this->getChadoConnection();

    $name = $stanza['name'][0];

    // First get the record for any potential conflicting term.
    $query = $chado->select('1:cvterm', 'CVT');
    $query->fields('CVT');
    $query->condition('CVT.name', $name);
    $query->condition('CVT.cv_id', $cv->cv_id);
    $query->condition('CVT.dbxref_id', $dbxref->dbxref_id);
    $results = $query->execute();
    while ($check_cvterm = $results->fetchObject()) {

      // If the dbxref of this matched term is the same as the current term
      // then it is the same term and there is no conflict.
      if ($dbxref->dbxref_id == $check_cvterm->dbxref_id) {
        return FALSE;
      }

      // At this point, we have a cvterm with the same name and vocabulary
      // but with a different dbxref. First let's get that other accession.
      $check_dbxref = $this->getChadoDBXrefById($check_cvterm->dbxref_id);
      $check_db = $this->getChadoDbById($check_dbxref->db_id);
      $check_accession = $check_db->name . ':' . $check_dbxref->accession;

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
        $query = $chado->update('1:cvterm');
        $query->fields([
          'name' => $new_name,
          'is_obsolete' => '1',
        ]);
        $query->condition('cvterm_id', $check_cvterm->cvterm_id);
        $query->execute();
        return TRUE;
      }
      // Case 2:  The conflicting term is in the OBO file (ie. has a stanza) and
      // is obsolete and this one is not. Fix it by adding an (obsolete) suffix
      // to the name to avoid the conflict.
      else {
        if (array_key_exists('is_obsolete', $check_stanza) and ($check_stanza['is_obsolete'][0] == 'true') and (!array_key_exists('is_obsolete', $stanza) or ($stanza['is_obsolete'][0] != 'true'))) {
          $new_name = $check_cvterm->name . ' (obsolete)';
          $query = $chado->update('1:cvterm');
          $query->fields([
            'name' => $new_name,
          ]);
          $query->condition('cvterm_id', $check_cvterm->cvterm_id);
          $query->execute();
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
          $new_name = $check_cvterm->name . ' (' . $check_accession . ')';
          $query = $chado->update('1:cvterm');
          $query->fields([
            'name' => $new_name,
          ]);
          $query->condition('cvterm_id', $check_cvterm->cvterm_id);
          $query->execute();
          return TRUE;
        }
      }
    }

    // We have no conflict so it's safe to update or insert.
    return FALSE;
  }

  /**
   * Uses the provided term array to add/update information to Chado about the
   * term including the term, dbxref, synonyms, properties, and relationships.
   *
   * @param array $stanza
   *   An array representing the cvterm.
   * @param bool is_relationship
   *   Set to 1 if this term is a relationship term
   *
   * @ingroup tripal_obo_loader
   */
  private function processTerm($stanza, $is_relationship = 0) {

    $chado = $this->getChadoConnection();

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
      DELETE FROM {1:cvterm_relationship}
      WHERE subject_id = :cvterm_id
    ";
    $chado->query($sql, [':cvterm_id' => $cvterm_id]);

    // If this is an obsolete term then clear out the relationships where
    // this term is the object.
    if (in_array('is_obsolete', $stanza) and $stanza['is_obsolete'] == 'true') {
      $sql = "
        DELETE FROM {1:cvterm_relationship}
        WHERE object_id = :cvterm_id
      ";
      $chado->query($sql, [':cvterm_id' => $cvterm_id]);
    }

    $sql = "
      DELETE FROM {1:cvtermprop}
      WHERE cvterm_id = :cvterm_id
    ";
    $chado->query($sql, [':cvterm_id' => $cvterm_id]);

    $sql = "
      DELETE FROM {1:cvterm_dbxref}
      WHERE cvterm_id = :cvterm_id
    ";
    $chado->query($sql, [':cvterm_id' => $cvterm_id]);

    $sql = "
      DELETE FROM {1:cvtermsynonym} CVTSYN
      WHERE cvterm_id = :cvterm_id
    ";
    $chado->query($sql, [':cvterm_id' => $cvterm_id]);

    // We should never have the problem where we don't have a cvterm_id. The
    // saveTerm() function should always return one.  But if for some unknown
    // reason we don't have one then fail.
    if (!$cvterm_id) {
      throw new \Exception(t('Missing cvterm after saving term: @term',
        ['@term' => print_r($stanza, TRUE)]));
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
   * @param string $id
   *   The Term ID (e.g. SO:0000704)
   * @param int $cvterm_id
   *   A cvterm_id of the term to which the relationship will be added.
   * @param int $rel_id
   *   The relationship type Id ID
   * @param int $obj_id
   *   The cvterm_id for the object of the relationship.
   *
   * @ingroup tripal_obo_loader
   */
  private function addRelationship($id, $cvterm_id, $rel_id, $obj_id) {

    // Get the cached terms for both the relationship and the object. They
    // should be there, but just in case something went wrong, we'll throw
    // an exception if we can't find them.
    $rel_stanza = $this->getCachedTermStanza($rel_id);
    if (!$rel_stanza) {
      throw new \Exception(t('Cannot add relationship: "@subject @rel @object". ' .
        'The term, @rel, is not in the term cache.',
        ['@subject' => $id, '@rel' => $rel_id, '@name' => $obj_id]));
    }
    $rel_cvterm_id = $this->saveTerm($rel_stanza, TRUE);

    // Make sure the object term exists in the cache.
    $obj_stanza = $this->getCachedTermStanza($obj_id);
    if (!$obj_stanza) {
      throw new \Exception(t('Cannot add relationship: "@source @rel @object". ' .
        'The term, @object, is not in the term cache.',
        ['@source' => $id, '@rel' => $rel_id, '@object' => $obj_id]));
    }
    $obj_cvterm_id = $this->saveTerm($obj_stanza);

    // Add the cvterm_relationship.
    $this->insertChadoCvtermRelationship($cvterm_id, $rel_cvterm_id, $obj_cvterm_id);
  }

  /**
   * Retrieves the term array from the temp loading table for a given term id.
   *
   * @param int id
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
   * @param string $short_name
   *   The term's ontology prefix (database short name)
   * @param string $accession
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
    $dbxref = $this->getChadoDBXrefByAccession($db->db_id, $accession);
    if (!$dbxref) {
      return NULL;
    }

    // If the dbxref exists then see if it has a corresponding cvterm.
    $cvterm = $this->getChadoCvtermByDbxref($dbxref->dbxref_id);
    if (!$cvterm) {
      return NULL;
    }

    // Get the CV for this term.
    $cv = $this->getChadoCvById($cvterm->cv_id);

    // Create a new stanza using the values of this cvterm.
    $stanza = [];
    $stanza['id'][0] = $short_name . ':' . $accession;
    $stanza['name'][0] = $cvterm->name;
    $stanza['def'][0] = $cvterm->definition;
    $stanza['namespace'][0] = $cv->name;
    $stanza['is_obsolete'][0] = ($cvterm->is_obsolete == 1) ? 'true' : '';
    $stanza['is_relationshiptype'][0] = '';
    $stanza['db_name'][0] = $db->name;
    $stanza['cv_name'][0] = $cv->name;
    return $stanza;
  }

  /**
   * Adds a term stanza from the OBO file to the cache for easier lookup.
   *
   * @param array $stanza
   *   The stanza from the OBO file for the term.
   * @param string $type
   *   The term type (e.g. Typedef, Term)
   */
  private function cacheTermStanza($stanza, $type) {

    // Make sure we have defaults.
    if (!$this->default_namespace) {
      throw new \Exception('Cannot cache terms without a default CV.' . print_r($stanza, TRUE));
    }
    if (!$this->default_db) {
      throw new \Exception('Cannot cache terms without a default DB.' . print_r($stanza, TRUE));
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
      $idspaces = array_keys($this->idspaces);
      if ($short_name != $this->default_db and !in_array($short_name, $idspaces)) {

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
          $this->insertChadoDb($stanza['db_name'][0]);
          $this->insertChadoCv($stanza['namespace'][0]);
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
      $this->insertChadoDb($short_name);
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
        throw new \Exception("Cannot insert stanza into temporary table.");
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
   * @param string $type
   *   The term type (e.g. Typedef, Term)
   */
  private function getCacheSize($type) {
    $chado = $this->getChadoConnection();
    if ($this->cache_type == 'table') {
      $sql = "
        SELECT count(*) as num_terms
        FROM {1:tripal_obo_temp}
        WHERE type = :type
      ";
      $result = $chado->query($sql, [':type' => $type])->fetchObject();
      return $result->num_terms;
    }
    return $this->termStanzaCache['count'][$type];
  }

  /**
   * Retrieves all term IDs for a given type.
   *
   * If the cache is using the tripal_obo_temp table then it
   * returns an iterable Database handle.
   *
   * @param string $type
   *   The term type (e.g. Typedef, Term)
   */
  private function getCachedTermStanzas($type) {
    $chado = $this->getChadoConnection();
    if ($this->cache_type == 'table') {
      $sql = "SELECT id FROM {1:tripal_obo_temp} WHERE type = 'Typedef' ";
      $typedefs = $chado->query($sql);
      return $typedefs;
    }
    return $this->termStanzaCache['types'][$type];
  }

  /**
   * Clear's the term cache.
   */
  private function clearTermStanzaCache() {
    $chado = $this->getChadoConnection();
    if ($this->cache_type == 'table') {
      $sql = "DELETE FROM {1:tripal_obo_temp}";
      $chado->query($sql);
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
   * @param string $id
   *   The Term ID (e.g. SO:0000704).
   * @param int $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param string $synonym
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
      throw new \Exception(t('Cannot find synonym type: @type', ['@type' => $syn_type]));
    }

    // The synonym can only be 255 chars in the cvtermsynonym table.
    // to prevent failing we have to truncate.
    if (!empty($def) AND (strlen($def) > 255)) {
      $def = substr($def, 0, 252) . "...";
    }

    $this->insertChadoCvtermSynonym($cvterm_id, $def);
  }

  /**
   * Parse the OBO file and populate the templ loading table
   *
   * @param string $obo_file
   *   The path on the file system where the ontology can be found
   * @param array $header
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
      $size = mb_strlen($line);
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
            // Fix the namespace for EDAM terms so they all use the same
            // namespace (i.e. cv record).
            if ($this->default_namespace == 'EDAM') {
              $stanza['namespace'][0] = 'EDAM';
            }
            $namespace = $stanza['namespace'][0];
            if (array_key_exists($namespace, $this->all_cvs)) {
              $cv = $this->all_cvs[$namespace];
              $this->obo_namespaces[$namespace] = $cv->cv_id;
            }
            else {
              $this->obo_namespaces[$namespace] = NULL;
            }
          }

          // Before caching this stanza...
          // We need to ensure this term has an id.
          // This one is non-negotiable!
          if (!array_key_exists('id', $stanza)) {
            $this->logger->warning('We are skipping the following term because it does not have an id. Term information: ' . print_r($stanza, TRUE));
          }
          else {
            // We need to ensure this term has a name.
            // If it doesn't then we will use the id.
            if (!array_key_exists('name', $stanza)) {
              $stanza['name'][0] = $stanza['id'][0];
            }
            // make sure it doesn't conflict. If it does we'll just
            // add the ID to the name to ensure it doesn't.
            if (array_key_exists($stanza['name'][0], $this->term_names)) {
              $new_name = $stanza['name'][0] . '(' . $stanza['id'][0] . ')';
              $stanza['name'][0] = $new_name;
            }

            $this->cacheTermStanza($stanza, $type);
          }

        }

        // Get the stanza type:  Term, Typedef or Instance
        $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/', '\1', $line);

        // start fresh with a new array
        $stanza = [];
        continue;
      }

      // For EDAM, we have to unfortunately hard-code a fix as the
      // short names of terms are correct.
      $line = preg_replace('/EDAM_(\w+)/', '\1', $line);


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
        if (array_key_exists($namespace, $this->all_cvs)) {
          $cv = $this->all_cvs[$namespace];
          $this->obo_namespaces[$namespace] = $cv->cv_id;
        }
        else {
          $this->obo_namespaces[$namespace] = NULL;
        }
      }
      $this->cacheTermStanza($stanza, $type);
      $this->setItemsHandled($num_read);
    }

    // Make sure there are CV records for all namespaces.
    $message = t('Found the following namespaces: @namespaces.',
      ['@namespaces' => implode(', ', array_keys($this->obo_namespaces))]);
    foreach ($this->obo_namespaces as $namespace => $cv) {
      $this->insertChadoCv($namespace);
    }
    $this->logger->notice($message->getUntranslatedString());
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
   * @param string id
   *   The Term ID (e.g. SO:0000704)
   * @param int $cvterm_id
   *   The cvterm_id of the term to which the subset will be added.
   * @param string $subset
   *   The name of the subset.
   */
  private function addSubset($id, $cvterm_id, $subset) {
    $type_id = $this->used_terms['NCIT:C25693'];
    $this->insertChadoCvtermProp($cvterm_id, $type_id, $subset);
  }

  /**
   * Inserts a database to Chado if it doesn't exist.
   *
   * @param string $dbname
   *   The name of the database to add.
   * @param string $url
   *   The DB URL.
   * @param string $description
   *   The DB description
   * @return object|NULL
   *   The newly inserted DB object.
   */
  private function insertChadoDb($dbname, $url = '',  $description = '') {
    $chado = $this->getChadoConnection();

    // Add the database if it doesn't exist.
    if (array_key_exists($dbname, $this->all_dbs)) {
      return $this->all_dbs[$dbname];
    }
    $query = $chado->insert('1:db');
    $query->fields([
      'name' => $dbname,
      'url' => $url,
      'description' => $description
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add database: @db', ['@db' => $dbname]);
      throw new \Exception($message);
    }
    $db = $this->getChadoDbByName($dbname);
    $this->all_dbs[$dbname] = $db;
    return $db;
  }


  /**
   * Insert a Dbxref record to the database.
   *
   * @param int $db_id
   *   The dbxref Id.
   * @param string $accession
   *   The term accession.
   * @param object|NULL
   *   The newly inserted dbxref object.
   */
  private function insertChadoDbxref($db_id, $accession) {
    $chado = $this->getChadoConnection();

    $dbxref = $this->getChadoDBXrefByAccession($db_id, $accession);
    if ($dbxref) {
      return $dbxref;
    }

    // Add the database if it doesn't exist.
    $query = $chado->insert('1:dbxref');
    $query->fields([
      'db_id' => $db_id,
      'accession' => $accession,
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add dbxref: @acc', ['@acc' => $accession]);
      throw new \Exception($message);
    }
    $dbxref = $this->getChadoDBXrefByAccession($db_id, $accession);
    return $dbxref;
  }

  /**
   * Inserts a record into the cvterm_dbxref table of Chado.
   *
   * @param int $cvterm_id
   *   The cvterm ID.
   * @param int $dbxref_id
   *   The dbxref ID.
   * @param object.
   *   The newly inserted cvterm_dbxref object.
   */
  private function insertChadoCvtermDbxref($cvterm_id, $dbxref_id) {
    $chado = $this->getChadoConnection();

    $squery = $chado->select('1:cvterm_dbxref', 'CVTDBX');
    $squery->fields('CVTDBX');
    $squery->condition('CVTDBX.cvterm_id', $cvterm_id);
    $squery->condition('CVTDBX.dbxref_id', $dbxref_id);
    $cvterm_dbxref = $squery->execute()->fetchObject();
    if ($cvterm_dbxref) {
      return $cvterm_dbxref;
    }

    $query = $chado->insert('1:cvterm_dbxref');
    $query->fields([
      'cvterm_id' => $cvterm_id,
      'dbxref_id' => $dbxref_id,
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add cvterm_dbxref');
      throw new \Exception($message);
    }
    return $squery->execute()->fetchObject();
  }

  /**
   * Inserts a record into the `cvtermsynonym` table.
   *
   * @param int $cvterm_id
   *   The ID of the cvterm.
   * @param string $synonym
   *   The synonym.
   * @return object
   *   The newly inserted cvtermsynonym object.
   */
  private function insertChadoCvtermSynonym($cvterm_id, $synonym) {
    $chado = $this->getChadoConnection();

    $query = $chado->insert('1:cvtermsynonym');
    $query->fields([
      'cvterm_id' => $cvterm_id,
      'synonym' => $synonym,
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add cvtermsynonym: @synonym', ['@synonym', $synonym]);
      throw new \Exception($message);
    }
  }

  /**
   * Inserts a new record into the chadoprop table.
   *
   * @param int $cvterm_id
   *   The ID of the cvterm this property belongs to.
   * @param int $type_id
   *   The CVterm Id of the type.
   * @param string $value
   *   The property value to add.
   * @param int $rank
   *   The rank of the property value
   */
  private function insertChadoCvtermProp($cvterm_id, $type_id, $value, $rank = 0) {
    $chado = $this->getChadoConnection();

    $query = $chado->insert('1:cvtermprop');
    $query->fields([
      'cvterm_id' => $cvterm_id,
      'type_id' => $type_id,
      'value' => $value,
      'rank' => $rank,
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add cvtermprop: @value', ['@value' => $value]);
      throw new \Exception($message);
    }
  }

  /**
   * Inserts a record into the cvterm_relationship table.
   *
   * @param int $subject_id
   *   The cvterm ID for the subject.
   * @param int $type_id
   *   The cvterm ID for the relationship type.
   * @param int $object_id
   *   The cvterm ID for the object.
   */
  private function insertChadoCvtermRelationship($subject_id, $type_id, $object_id) {
    $chado = $this->getChadoConnection();

    $query = $chado->insert('1:cvterm_relationship');
    $query->fields([
      'subject_id' => $subject_id,
      'type_id' => $type_id,
      'object_id' => $object_id,
    ]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add cvterm_relationship');
      throw new \Exception($message);
    }
  }

  /**
   * Inserts a vocabulary to Chado if it doesn't exist.
   *
   * @param string $cvname
   *   The name of the vocabulary to add.
   *
   * @return object|NULL
   *   The newly inserted CV object.
   */
  private function insertChadoCv($cvname) {
    $chado = $this->getChadoConnection();

    // Add the CV record if it doesn't exist.
    if (array_key_exists($cvname, $this->all_cvs)) {
      return $this->all_cvs[$cvname];
    }

    $query = $chado->insert('1:cv');
    $query->fields(['name' => $cvname]);
    $success = $query->execute();
    if (!$success) {
      $message = t('Could not add vocabulary: @cv', ['@cv' => $cvname]);
      throw new \Exception($message);
    }
    $cv = $this->getChadoCvByName($cvname);
    $this->all_cvs[$cvname] = $cv;
    $this->obo_namespaces[$cvname] = $cv->cv_id;

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
   * @param string $id
   *   The Term ID (e.g. SO:0000704).
   * @param int $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param int $alt_id
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
      $this->logMessage("Cannot add an Alt ID without an accession: '@alt_id'", ['@alt_id' => $alt_id]);
      return;
    }

    $db = $this->insertChadoDb($dbname);
    $dbxref = $this->insertChadoDbxref($db->db_id, $accession);
    $this->insertChadoCvtermDbxref($cvterm_id, $dbxref->dbxref_id);
  }

  /**
   * Adds a database reference to a cvterm
   *
   * @param string $id
   *   The Term ID (e.g. SO:0000704).
   * @param int $cvterm_id
   *   The cvterm_id of the term to which the synonym will be added.
   * @param string $xref
   *   The cross reference.  It should be of the form from the OBO specification
   *
   * @ingroup tripal_obo_loader
   */
  private function addXref($id, $cvterm_id, $xref) {

    $dbname = preg_replace('/^(.+?):.*$/', '$1', $xref);
    $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/', '$1', $xref);
    //$description = preg_replace('/^.+?\"(.+?)\".*?$/', '$1', $xref);
    //$dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/', '$1', $xref);

    if (!$accession) {
      throw new \Exception("Cannot add an xref without an accession: '$xref'");
    }

    // If the xref is a database link then skip those for now.
    if (strcmp($dbname, 'http') == 0) {
      return;
    }

    $db = $this->insertChadoDb($dbname);
    $dbxref = $this->insertChadoDbxref($db->db_id, $accession);
    $this->insertChadoCvtermDbxref($cvterm_id, $dbxref->dbxref_id);
  }

  /**
   * Adds a comment to a cvterm.
   *
   * @param string $id
   *   The Term ID (e.g. SO:0000704).
   * @param int $cvterm_id
   *   A cvterm_id of the term to which properties will be added
   * @param string $comment
   *   The comment to add to the cvterm.
   * @param int $rank
   *   The rank of the comment
   *
   * @ingroup tripal_obo_loader
   */
  private function addComment($id, $cvterm_id, $comment, $rank) {

    $comment_type_id = $this->used_terms['rdfs:comment'];
    $this->insertChadoCvtermProp($cvterm_id, $comment_type_id, $comment, $rank);
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
  private function oboEbiLookup($accession, $type_of_search, $found_iri = NULL, $found_ontology = NULL) {
    $client = \Drupal::httpClient();

    // Grab just the ontology from the $accession.
    $parts = explode(':', $accession);
    $ontology = strtolower($parts[0]);
    $ontology = preg_replace('/\s+/', '', $ontology);
    if ($found_iri) {
      // When we cannot grab the ontology from the accession or the IRI cannot
      // be automatically formed we need to use the ontology and the IRI
      // found in the previous query.
      $ontology = $found_ontology;
      $type = '';
      if ($type_of_search == 'property') {
        $type = 'properties';
      }
      elseif ( $type_of_search == 'class') {
        $type = 'terms';
      }
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . $type . '/' . $found_iri;
      $options = [];
    }
    elseif ($type_of_search == 'ontology') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology;
    }
    elseif ($type_of_search == 'term') {
      // The IRI of the terms, this value must be double URL encoded
      $iri = urlencode(urlencode("http://purl.obolibrary.org/obo/" . str_replace(':', '_', $accession)));
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . 'terms/' . $iri;
    }
    elseif ($type_of_search == 'property') {
      // The IRI of the terms, this value must be double URL encoded
      $iri = urlencode(urlencode("http://purl.obolibrary.org/obo/" . str_replace(':', '_', $accession)));
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/ontologies/' . $ontology . '/' . 'properties/' . $iri;
    }
    elseif ($type_of_search == 'query') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id&local=true';
    }
    elseif ($type_of_search == 'query-non-local') {
      $options = [];
      $full_url = 'http://www.ebi.ac.uk/ols/api/search?q=' . $accession . '&queryFields=obo_id';
    }

    try {
      $response = $client->get($full_url, $options);
      $response = $response->getBody();
      $response = Json::decode($response);
      return $response;
    }
    catch (RequestException $e) {
      $this->logger->error('Unable to get response from @url when trying to retrieve data for @accession. @exception',
          ['@url' => $full_url, '@accession' => $accession, '@exception' => $e->getMessage()]);
    }

    return FALSE;
  }
}
