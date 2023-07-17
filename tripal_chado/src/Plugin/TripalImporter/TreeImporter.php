<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Tree Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_newick_loader",
 *    label = @Translation("Tree Loader"),
 *    description = @Translation("Import a Phylogenetic or Gene Tree into Chado"),
 *    file_types = {"newick", "tree", "txt"},
 *    upload_description = @Translation("Please provide the tree file. Currently only Newick format is supported."),
 *    upload_title = @Translation("Tree File"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    use_analysis = True,
 *    require_analysis = True,
 *    button_text = @Translation("Import tree file"),
 *    file_upload = True,
 *    file_load = True,
 *    file_remote = True,
 *    file_required = True,
 *    cardinality = 1,
 *    menu_path = "",
 *    callback = "",
 *    callback_module = "",
 *    callback_path = "",
 *  )
 */
class TreeImporter extends ChadoImporterBase {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Tree Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_tree_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Load a phylogenetic or gene tree from a file.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['newick', 'tree', 'txt'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the tree file (one tree per file only).  The file must have a .newick, .tree, or .txt extension.';

  /**
   * The title that should appear above the file upload section.
   */
  public static $upload_title = 'Tree File Upload';

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import Tree file';


  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

  /**
   * Indicates if the file must be provided.  An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   */
  public static $file_required = TRUE;

  /**
   * The array of arguments used for this loader.  Each argument should
   * be a separate array containing a machine_name, name, and description
   * keys.  This information is used to build the help text for the loader.
   */
  public static $argument_list = [];

  /**
   * Indicates how many files are allowed to be uploaded.  By default this is
   * set to allow only one file.  Change to any positive number. A value of
   * zero indicates an unlimited number of uploaded files are allowed.
   */
  public static $cardinality = 1;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    // Default values can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing phylotree
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks from non submit form elements
    // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
    //    form elements and the form is being rebuilt
    //
    // set form field defaults
    $phylotree = NULL;
    $phylotree_id = NULL;
    $tree_name = '';
    $leaf_type = '';
    $analysis_id = '';
    $dbxref = '';
    $comment = '';
    $tree_required = TRUE;
    $tree_file = '';
    $name_re = '';
    $match = '';
    $load_later = FALSE;  // Default is to combine tree import with current job

    $form_state_values = $form_state->getValues();

    // If we are re constructing the form from a failed validation or ajax callback
    // then use the $form_state['values'] values.
    if (array_key_exists('tree_name', $form_state_values)) {
      $tree_name = $form_state_values['tree_name'];
      $leaf_type = $form_state_values['leaf_type'];
      $analysis_id = $form_state_values['analysis_id'];
      $comment = $form_state_values['description'];
      $dbxref = $form_state_values['dbxref'];
    }

    // If we are re building the form from after submission (from ajax call) then
    // the values are in the $form_state_values['input'] array.
    if (array_key_exists('input', $form_state_values) and !empty($form_state_values['input'])) {
      $tree_name = $form_state_values['input']['tree_name'];
      $leaf_type = $form_state_values['input']['leaf_type'];
      $analysis_id = $form_state_values['input']['analysis_id'];
      $comment = $form_state_values['input']['description'];
      $dbxref = $form_state_values['input']['dbxref'];
    }

    $form['tree_name'] = [
      '#type' => 'textfield',
      '#title' => t('Tree Name'),
      '#required' => TRUE,
      '#default_value' => $tree_name,
      '#description' => t('Enter the name used to refer to this phylogenetic tree.'),
      '#maxlength' => 255,
    ];

    $so_cv = chado_get_cv(['name' => 'sequence']);
    $cv_id = $so_cv->cv_id;
    if (!$so_cv) {
      drupal_set_message('The Sequence Ontolgoy does not appear to be imported.
        Please import the Sequence Ontology before adding a tree.', 'error');
    }

    $form['leaf_type'] = [
      '#title' => t('Tree Type'),
      '#type' => 'textfield',
      '#description' => t("Choose the tree type. The type is
        a valid Sequence Ontology (SO) term. For example, trees derived
        from protein sequences should use the SO term 'polypeptide'.
        Alternatively, a phylotree can be used for representing a taxonomic
        tree. In this case, the word 'taxonomy' should be used."),
      '#required' => TRUE,
      '#default_value' => $leaf_type,
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv_id",
    ];

    $form['dbxref'] = [
      '#title' => t('Database Cross-Reference'),
      '#type' => 'textfield',
      '#description' => t("Enter a database cross-reference of the form
        [DB name]:[accession]. The database name must already exist in the
        database. If the accession does not exist it is automatically added."),
      '#required' => FALSE,
      '#default_value' => $dbxref,
    ];

    $form['description'] = [
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#required' => TRUE,
      '#default_value' => $comment,
      '#description' => t('Enter a description for this tree.'),
    ];

    $form['name_re'] = [
      '#title' => t('Feature Name Regular Expression'),
      '#type' => 'textfield',
      '#description' => t('The tree nodes will be automatically associated with
          features, or in the case of taxonomic trees, with organisms. However,
          if the nodes in the tree file are not exactly as the names of features
          or organisms but have enough information to uniquely identify them,
          then you may provide a regular expression that the importer will use to
          extract the appropriate names from the node names. For example, remove
          a prefix ABC_ with ^ABC_(.*)$'),
      '#default_value' => $name_re,
    ];
    $form['match'] = [
      '#title' => t('Use Unique Feature Name'),
      '#type' => 'checkbox',
      '#description' => t('If this is a phylogenetic (non taxonomic) tree and the nodes ' .
        'should match the unique name of the feature rather than the name of the feature ' .
        'then select this box. If unselected the loader will try to match the feature ' .
        'using the feature name.'),
      '#default_value' => $match,
    ];
    $form['load_later'] = [
      '#title' => t('Run Tree Import as a Separate Job'),
      '#type' => 'checkbox',
      '#description' => t('Check if tree loading should be performed as a separate job. ' .
        'If not checked, tree loading will be combined with this job.'),
      '#default_value' => $load_later,
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    $values = $form_state->getValues();
    $schema = $values['schema_name'];
    $options = [
      'name' => trim($values["tree_name"]),
      'description' => trim($values["description"]),
      'analysis_id' => $values["analysis_id"] ?? NULL,
      'leaf_type' => $values["leaf_type"],
      'format' => 'newick',
      'dbxref' => trim($values["dbxref"]),
      'match' => $values["match"],
      'name_re' => $values["name_re"],
      'load_later' => $values["load_later"],
    ];

    $errors = [];
    $warnings = [];

    chado_validate_phylotree('insert', $options, $errors, $warnings, $schema);

    // Now set form errors if any errors were detected.
    if (count($errors) > 0) {
      foreach ($errors as $field => $message) {
        if ($field == 'name') {
          $field = 'tree_name';
        }
        $form_state->setErrorByName($field, $message);
      }
    }
    // Add any warnings if any were detected
    // n.b. chado_validate_phylotree() does not currently return any warnings.
    if (count($warnings) > 0) {
      foreach ($warnings as $field => $message) {
        $form_state->setErrorByName($field, $message);
      }
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $schema = $arguments['schema_name'];
    $options = [
      'name' => $arguments["tree_name"],
      'description' => $arguments["description"],
      'analysis_id' => $arguments["analysis_id"] ?? 0,
      'leaf_type' => $arguments["leaf_type"],
      'tree_file' => $this->arguments['files'][0]['file_path'],
      'format' => 'newick',
      'dbxref' => $arguments["dbxref"],
      'match' => $arguments["match"],
      'name_re' => $arguments["name_re"],
      'load_later' => $arguments["load_later"],
    ];
    // pass through the job, needed for log output to show up on the "jobs page"
    if (property_exists($this, 'job')) {
      $options['job'] = $this->job;
    }
    $errors = [];
    $warnings = [];
    chado_insert_phylotree($options, $errors, $warnings, $schema);
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {

  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {

  }

}
