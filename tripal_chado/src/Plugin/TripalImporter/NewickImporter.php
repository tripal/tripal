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
 * Taxonomy Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_newick_tree_loader",
 *    label = @Translation("Newick Tree Loader"),
 *    description = @Translation("Import Newick Tree into Chado"),
 *    file_types = {"tree","txt","newick"},
 *    upload_description = @Translation("Please provide the Newick formatted tree file (one tree per file only)."),
 *    upload_title = @Translation("Newick Tree File"),
 *    use_analysis = True,
 *    require_analysis = True,
 *    button_text = @Translation("Import Newick Tree file"),
 *    file_upload = True,
 *    file_remote = False,
 *    file_required = False,
 *  )
 */
class NewickImporter extends ChadoImporterBase {

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $chado = $this->getChadoConnection();
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
    // $load_later = FALSE;  // Default is to combine tree import with current job

    // get the sequence ontology CV ID
    $cv_results = $chado->select('1:cv', 'cv')
      ->fields('cv')
      ->condition('name', 'sequence')
      ->execute();
    $cv_id = $cv_results->fetchObject()->cv_id;

    $form_state_values = $form_state->getValues();
    $form_state_input = $form_state->getUserInput();
    // If we are re constructing the form from a failed validation or ajax callback
    // then use the $form_state['values'] values.
    if (isset($form_state_values['tree_name'])) {
      $tree_name = $form_state_values['tree_name'];
      $leaf_type = $form_state_values['leaf_type'];
      $analysis_id = $form_state_values['analysis_id'];
      $dbxref = $form_state_values['dbxref'];
      $comment = $form_state_values['description'];
    }
    // If we are re building the form from after submission (from ajax call) then
    // the values are in the $form_state['input'] array.
    if (!empty($form_state_input)) {
      $tree_name = $form_state_input['tree_name'];
      $leaf_type = $form_state_input['leaf_type'];
      $analysis_id = $form_state_input['analysis_id'];
      $comment = $form_state_input['description'];
      $dbxref = $form_state_input['dbxref'];
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
      \Drupal::messenger()->addError(t("The Sequence Ontology does not appear to be imported.
         Please import the Sequence Ontology before adding a tree."));
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
      '#autocomplete_route_name' => 'tripal_chado.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5, 'cv_id' => $cv_id]
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

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $chado = $this->getChadoConnection();
    // $values = $form_state['values'];
    $values = $form_state->getValues();

    // TRIPAL 4 - The type option is from an autocomplete which seems to include (SO:*) part
    // Temporarily, remove this part
    $leaf_type = $values["leaf_type"];
    $leaf_type = explode(' ', $leaf_type)[0]; // splits the string by space and takes the first part

    $options = [
      'name' => trim($values["tree_name"]),
      'description' => trim($values["description"]),
      'analysis_id' => $values["analysis_id"],
      'leaf_type' => $leaf_type,
      'format' => 'newick',
      'dbxref' => trim($values["dbxref"]),
      'match' => $values["match"],
      'name_re' => $values["name_re"],
      // 'load_later' => $values["load_later"],
    ];

    $errors = [];
    $warnings = [];

    // The parent class will validate that a file has been specified and is valid.

    // Validate DBXREF
    if ($options['dbxref'] and ($options['dbxref'] != "null:local:null")) {
      // The db in a dbxref must already exist, the accession can be new.
      $dbxref_parts = explode(':', $options['dbxref'], 2);
      $db = $dbxref_parts[0];
      if ((count($dbxref_parts) < 2) or (strlen($db) < 1) or (strlen($dbxref_parts[1]) < 1)) {
        $form_state->setErrorByName('dbxref', "The dbxref must consist of a DB and an accession separated by a colon, specify a valid dbxref value.");
        return;
      }
      // Lookup
      $results = $chado->select('1:db', 'db')
        ->fields('db')
        ->condition('name', $db)
        ->execute()
        ->fetchAll();
      $count = count($results);
      if ($count < 1) {
        $form_state->setErrorByName('dbxref', "The DB \"$db\" in the dbxref value does not exist, specify a valid dbxref value.");
        return;
      }
    }

    // Perform API validation.
    chado_validate_phylotree('insert', $options, $errors, $warnings, $chado->getSchemaName());

    // Now set form errors if any errors were detected.
    if (count($errors) > 0) {
      foreach ($errors as $field => $message) {
        if ($field == 'name') {
          $field = 'tree_name';
        }
        $form_state->setErrorByName($field, $message);
      }
      $form_state->setError($form, "Please fix these errors to continue creating a job");
    }
    // Add any warnings if any were detected
    if (count($warnings) > 0) {
      foreach ($warnings as $field => $message) {
        \Drupal::messenger()->addWarning(t("$message"));
      }
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {
    $chado = $this->getChadoConnection();
    $arguments = $this->arguments['run_args'];

    // TRIPAL 4 - The type option is from an autocomplete which seems to include (SO:*) part
    // Temporarily, remove this part
    $leaf_type = $arguments["leaf_type"];
    $leaf_type = explode(' ', $leaf_type)[0]; // splits the string by space and takes the first part

    $options = [
      'name' => $arguments["tree_name"],
      'description' => $arguments["description"],
      'analysis_id' => $arguments["analysis_id"],
      'leaf_type' => $leaf_type,
      'tree_file' => $this->arguments['files'][0]['file_path'],
      'format' => 'newick',
      'dbxref' => $arguments["dbxref"],
      'match' => $arguments["match"],
      'name_re' => $arguments["name_re"],
      // 'load_later' => $arguments["load_later"],
    ];
    // pass through the job, needed for log output to show up on the "jobs page"
    if (property_exists($this, 'job')) {
      $options['job'] = $this->job;
    }
    $errors = [];
    $warnings = [];
    chado_insert_phylotree($options, $errors, $warnings, $chado->getSchemaName());
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
