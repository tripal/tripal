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
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    // Call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    // If chado was not prepared, alert user and return.
    $so_cv = chado_get_cv(['name' => 'sequence']);
    if (!$so_cv) {
      $message = t('The Sequence Ontology does not appear to be present.'
               . ' This loader will not function correctly.'
               . ' Please import the Sequence Ontology before adding a tree.');
      $form = [
        'error' => [
          '#markup' => "<h1>$message</h1>",
          '#weight' => -100,
        ]
      ] + $form;
      return $form;
    }

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

    // We want the file format type selector right underneath the file
    // element which has weight -15. Analysis does not have a weight.
    // Our placeholder has weight +1. Advanced has weight +9.
    $form['plugin_id'] = [
      '#weight' => -14,
      '#title' => t('Select the file format of the tree file'),
      '#type' => 'radios',
      '#required' => TRUE,
      '#options' => $plugins,
      // If a second tree parser is created, elements here can be changed to the
      // commented-out versions. The ajax callback can be enabled if there are
      // any field elements specific to just one parser.
      '#description' => t('Currently only Newick format is supported'),
      '#default_value' => 'tripal_tree_parser_newick',  // Newick tree format
      // '#description' => t('Choose one of the formats above for loading the tree file.'),
      // '#default_value' => NULL,
      // '#ajax' => [
      //   'callback' => [$this, 'formAjaxCallback'],
      //   'wrapper' => 'edit-tree_parser',
      // ],
    ];

    // A placeholder for the form elements for the selected tree
    // parser plugin, to be populated by formAjaxCallback().
    $form['tree_parser'] = [
      '#weight' => 1,
      '#prefix' => '<span id="edit-tree_parser">',
      '#suffix' => '</span>',
    ];

    // The placeholder above will only be populated when a plugin, i.e.
    // $form['plugin_id'], has been selected. Both the plugin base
    // class and the selected plugin can each add form elements.
    $form = $this->formPlugin($form, $form_state);

    // Form elements common to all tree importers
    $form = $this->formCommon($form, $form_state);

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    $form_state_values = $form_state->getValues();
    $schema = $form_state_values['schema_name'];
    $options = [
      'name' => trim($form_state_values['tree_name'] ?? ''),
      'description' => trim($form_state_values['description'] ?? ''),
      'leaf_type' => $form_state_values['leaf_type'] ?? '',
      'format' => $form_state_values['plugin_id'] ?? '',
      'dbxref' => trim($form_state_values['dbxref'] ?? ''),
      'match' => $form_state_values['match'] ?? '',
      'name_re' => $form_state_values['name_re'] ?? '',
      'load_later' => $form_state_values['load_later'] ?? '',
    ];

    // dbxref validation, make sure a colon is present and the db exists
    if ($options['dbxref']) {
      if (!preg_match('/.:./', $options['dbxref'])) {
        $form_state->setErrorByName('dbxref',
            t('The Database Cross-Reference must be of the format %format', ['%format' => 'DB name:accession']));
      }
      else {
        $dbname = preg_replace('/:.*$/', '', $options['dbxref']);
        $db = chado_get_db(['name' => $dbname]);
        if (!$db) {
          $form_state->setErrorByName('dbxref',
              t('The database %dbname does not exist in this site', ['%dbname' => $dbname]));
        }
      }
    }

    // check the regular expression to make sure it is valid
    if ($options['name_re']) {
      @ $result_re = preg_match('/' . $options['name_re'] . '/', NULL);
      if (!$result_re) {
        $form_state->setErrorByName('name_re',
            t('The entered regular expression %re is not valid', ['%re' => $options['name_re']]));
      }
    }

    // Call plugin validation if a plugin has been selected.
    $plugin_id = $form_state->getValue(['plugin_id']);
    if ($plugin_id) {
      // Instantiate the selected plugin
      $tree_parser_manager = \Drupal::service('tripal.tree_parser');
      $plugin = $tree_parser_manager->createInstance($plugin_id, []);

      // The selected plugin has a formValidate() for just
      // the form elements specific to itself.
      $plugin->formValidate($form, $form_state);
    }

    // Prepare to call API validation
    // When leaf_type is not specified on the form, default to 'taxonomy'
    // for taxonomic (species) trees. In Tripal3 this had to be typed in.
    if (!$options['leaf_type']) {
      $options['leaf_type'] = 'taxonomy';
    }
    // API functions currently cannot handle the (DB:accession) suffix on the leaf_type
    $options['leaf_type'] = preg_replace('/ \(.*\)/', '', $options['leaf_type']);
    $options['format'] = preg_replace('/tripal_tree_parser_/', '', $options['format']);

    // perform API validations
    $errors = [];
    $warnings = [];
    chado_validate_phylotree('insert', $options, $errors, $warnings, $schema);

    // Now set form errors if any errors were detected in the API validations
    if (count($errors) > 0) {
      foreach ($errors as $field => $message) {
        if ($field == 'name') {
          $field = 'tree_name';
        }
        $form_state->setErrorByName($field, $message);
      }
    }
    // Add any warnings if any were detected in the API validations
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
      'name' => $arguments['tree_name'],
      'description' => $arguments['description'],
      'analysis_id' => $arguments['analysis_id'],
      'leaf_type' => $arguments['leaf_type'],
      'tree_file' => $this->arguments['files'][0]['file_path'],
      'format' => $arguments['plugin_id'] ?? '',
      'dbxref' => $arguments['dbxref'],
      'match' => $arguments['match'],
      'name_re' => $arguments['name_re'],
      'load_later' => $arguments['load_later'],
    ];

    // When Tree Type on the form is left blank, we assume a taxonomic tree.
    // Tripal 3 would assume 'polypeptide'.
    if (!$options['leaf_type']) {
      $leaf_type = 'taxonomy';
    }
    // API functions currently cannot handle the (DB:accession) suffix on the leaf_type
    $options['leaf_type'] = preg_replace('/ \(.*\)/', '', $options['leaf_type']);
    $options['format'] = preg_replace('/tripal_tree_parser_/', '', $options['format']);

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

  /**
   * Form elements common to all tree importers.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  private function formCommon($form, &$form_state) {

    $form_state_values = $form_state->getValues();
    $tree_name = $form_state_values['tree_name'] ?? '';
    $leaf_type = $form_state_values['leaf_type'] ?? '';
    $comment = $form_state_values['description'] ?? '';
    $name_re = $form_state_values['name_re'] ?? '';
    $match = $form_state_values['match'] ?? '';
    $dbxref = $form_state_values['dbxref'] ?? '';
    $load_later = $form_state_values['load_later'] ?? FALSE;  // Default is to combine tree import with current job

    $form['tree_name'] = [
      '#weight' => 2,
      '#type' => 'textfield',
      '#title' => t('Tree Name'),
      '#required' => TRUE,
      '#default_value' => $tree_name,
      '#description' => t('Enter the name used to refer to this phylogenetic tree.'),
      '#maxlength' => 255,
    ];

    $form['leaf_type'] = [
      '#weight' => 3,
      '#title' => t('Tree Type (optional)'),
      '#type' => 'textfield',
      '#required' => FALSE,
      '#default_value' => $leaf_type,
      '#description' => t("Choose the tree type. The type should be
        a valid Sequence Ontology (SO) term. For example, trees derived
        from protein sequences should use the SO term 'polypeptide'.
        When left blank, the tree is assumed to represent a taxonomic tree."),
      '#autocomplete_route_name' => 'tripal_chado.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5],
// To-Do: Change line above to this when pull #1585 is merged
//      '#autocomplete_route_parameters' => ['cv_id' => $cv_id, 'count' => 5],
    ];

    $form['description'] = [
      '#weight' => 4,
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
          a prefix ABC_ with %example', ['%example' => '^ABC_(.*)$']),
      '#default_value' => $name_re,
    ];
    $form['match'] = [
      '#title' => t('Use Unique Feature Name'),
      '#type' => 'checkbox',
      '#description' => t('If this is a phylogenetic (non taxonomic) tree and the nodes ' .
        'should match the unique name of the feature rather than the name of the feature, ' .
        'then check this box. If unchecked, the loader will try to match the feature ' .
        'using the feature name.'),
      '#default_value' => $match,
    ];

    $form['dbxref'] = [
      '#weight' => 5,
      '#title' => t('Database Cross-Reference'),
      '#type' => 'textfield',
      '#required' => FALSE,
      '#default_value' => $dbxref,
      '#description' => t("Enter a database cross-reference of the form %form.
        The database name must already exist in your site's database.
        If the accession does not exist it is automatically added.",
        ['%form' => 'DB name:accession']),
    ];

    $form['load_later'] = [
      '#weight' => 6,
      '#title' => t('Run Tree Import as a Separate Job'),
      '#type' => 'checkbox',
      '#default_value' => $load_later,
      '#description' => t('Check if tree loading should be performed as a separate job. ' .
        'If not checked, tree loading will be combined with this job.'),
    ];

    return $form;
  }

  /**
   * Form elements from a tree parser plugin specific to itself.
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

      // This plugin base class does not define any form elements.

      // The selected plugin defines form elements specific to itself.
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
