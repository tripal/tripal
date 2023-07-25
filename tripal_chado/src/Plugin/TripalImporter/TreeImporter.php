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
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $form = $this->newLoaderForm($form, $form_state);

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

dpm('tree parent formValidate called'); //@@@
if (0) { //@@@
    $values = $form_state->getValues();
    $schema = $values['schema_name'];
    $options = [
      'name' => trim($values["tree_name"]),
      'description' => trim($values["description"]),
      'analysis_id' => $values["analysis_id"],
      'leaf_type' => $values["leaf_type"],
      'format' => 'newick',
      'dbxref' => trim($values["dbxref"]),
      'match' => $values["match"],
      'name_re' => $values["name_re"],
      'load_later' => $values["load_later"],
    ];

    // When leaf_type is not specified on the form, default to 'taxonomy'
    // for taxonomic (species) trees. In Tripal3 this had to be typed in.
    if (!$options['leaf_type']) {
      $options['leaf_type'] = 'taxonomy';
    }

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
} //@@@
  }

  /**
   * Form for loading a tree file.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function newLoaderForm($form, &$form_state) {

    // Retrieve a sorted list of available tree parser plugins.
    $tree_parser_manager = \Drupal::service('tripal.tree_parser');
    $tree_parser_defs = $tree_parser_manager->getDefinitions();
    $plugins = [];
    foreach ($tree_parser_defs as $plugin_id => $def) {
      $plugin_key = $def['id'];
      $plugin_value = $def['label']->render();
      $plugins[$plugin_key] = $plugin_value;
    }
    asort($plugins);

    // We want the file type selector right underneath the file element
    // which has weight -15. Analysis does not have a weight. Our
    // placeholder has weight +1. Advanced has weight +9.
    $form['plugin_id'] = [
      '#weight' => -14,
      '#title' => t('Select the type of tree file to load'),
      '#type' => 'radios',
      '#description' => t("Choose one of the formats above for loading the tree file. Currently only Newick format is supported"),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => NULL,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'],
        'wrapper' => 'edit-parser',
      ]
    ];

    // A placeholder for the form elements for the selected plugin,
    // to be populated by the AJAX callback.
    $form['tree_parser'] = [
      '#weight' => 1,
      '#prefix' => '<span id="edit-tree_parser">',
      '#suffix' => '</span>',
    ];

    // The placeholder will only be populated if a plugin, i.e.
    // $form['plugin_id'], has been selected. Both the plugin base
    // class and the selected plugin can each add form elements.
    $form = $this->formPlugin($form, $form_state);

    return $form;
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
      'analysis_id' => $arguments["analysis_id"],
      'leaf_type' => $arguments["leaf_type"],
      'tree_file' => $this->arguments['files'][0]['file_path'],
      'format' => 'newick',
      'dbxref' => $arguments["dbxref"],
      'match' => $arguments["match"],
      'name_re' => $arguments["name_re"],
      'load_later' => $arguments["load_later"],
    ];

    // When leaf_type is not specified on the form, default to 'taxonomy'
    // for taxonomic (species) trees. In Tripal3 this had to be typed in.
    if (!$options['leaf_type']) {
      $options['leaf_type'] = 'taxonomy';
    }

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
    // Disable the parent submit
    $form_state->setRebuild(True);
  }

  /**
   * Retrieves form elements from a plugin.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  private function formPlugin($form, &$form_state) {

    // Add elements only after a plugin has been selected.
    $plugin_id = $form_state->getValue(['plugin_id']);
    if ($plugin_id) {

      // Instantiate the selected plugin
      $tree_parser_manager = \Drupal::service('tripal.tree_parser');
      $plugin = $tree_parser_manager->createInstance($plugin_id, []);

      // The plugin manager defines form elements used by
      // all pub_parser plugins.
      $form = $tree_parser_manager->form($form, $form_state);

      // The selected plugin defines form elements specific
      // to itself.
      $form = $plugin->form($form, $form_state);
    }

    return $form;
  }

  /**
   * Ajax callback for the ChadoTreeImporter::form() function.
   * This adds form elements appropriate for the selected parser plugin.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function formAjaxCallback($form, &$form_state) {

    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-tree_parser', $form['tree_parser']));

    return $response;
  }

}
