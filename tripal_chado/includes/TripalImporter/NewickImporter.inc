<?php

class NewickImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Newick Tree Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_newick_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Load Newick formatted phylogenetic trees.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['tree', 'txt'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the Newick formatted tree file (one tree per file only).  The file must have a .txt or .tree extension.';

  /**
   * The title that should appear above the file upload section.
   */
  public static $upload_title = 'Newick Upload';

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import Newick file';


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
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    // Default values can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing phylotree
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks from non submit form elements
    // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
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

    // If we are re constructing the form from a failed validation or ajax callback
    // then use the $form_state['values'] values.
    if (array_key_exists('values', $form_state) and isset($form_state['values']['tree_name'])) {
      $tree_name = $form_state['values']['tree_name'];
      $leaf_type = $form_state['values']['leaf_type'];
      $analysis_id = $form_state['values']['analysis_id'];
      $dbxref = $form_state['values']['dbxref'];
      $comment = $form_state['values']['description'];
    }
    // If we are re building the form from after submission (from ajax call) then
    // the values are in the $form_state['input'] array.
    if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
      $tree_name = $form_state['input']['tree_name'];
      $leaf_type = $form_state['input']['leaf_type'];
      $analysis_id = $form_state['input']['analysis_id'];
      $comment = $form_state['input']['description'];
      $dbxref = $form_state['input']['dbxref'];
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
      '#description' => t('If this is a phylogenetic (non taxonomic) tree, then
          the tree nodes will be automatically associated with features. However,
          if the nodes in the tree file are not exactly as the names of features
          but have enough information to uniquely identify the feature then you
          may provide a regular expression that the importer will use to extract
          the feature names from the node names.'),
      '#default_value' => $name_re,
    ];
    $form['match'] = [
      '#title' => t('Use Unique Feature Name'),
      '#type' => 'checkbox',
      '#description' => t('If this is a phylogenetic (non taxonomic tree) and the nodes ' . 'should match the unique name of the feature rather than the name of the feature ' . 'then select this box. If unselected the loader will try to match the feature ' . 'using the feature name.'),
      '#default_value' => $match,
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    $values = $form_state['values'];
    $options = [
      'name' => trim($values["tree_name"]),
      'description' => trim($values["description"]),
      'analysis_id' => $values["analysis_id"],
      'leaf_type' => $values["leaf_type"],
      'format' => 'newick',
      'dbxref' => trim($values["dbxref"]),
      'match' => $values["match"],
      'name_re' => $values["name_re"],
    ];

    $errors = [];
    $warnings = [];

    chado_validate_phylotree('insert', $options, $errors, $warnings);

    // Now set form errors if any errors were detected.
    if (count($errors) > 0) {
      foreach ($errors as $field => $message) {
        if ($field == 'name') {
          $field = 'tree_name';
        }
        form_set_error($field, $message);
      }
    }
    // Add any warnings if any were detected
    if (count($warnings) > 0) {
      foreach ($warnings as $field => $message) {
        drupal_set_message($message, 'warning');
      }
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];

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
    ];
    $errors = [];
    $warnings = [];
    chado_insert_phylotree($options, $errors, $warnings);
  }
}
