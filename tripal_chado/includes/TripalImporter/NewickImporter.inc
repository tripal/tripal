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
  public static $upload_description = 'Please provide the Newick formatted tree file.  The file must have a .txt or .tree extension.';

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

    $type_cv = tripal_get_default_cv('phylotree', 'type_id');
    $so_cv = tripal_get_cv(['name' => 'sequence']);
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
      '#autocomplete_path' => "admin/tripal/legacy/tripal_cv/cvterm/auto_name/$cv_id",
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
      // 'tree_file'    => $values["tree_file"],
      'format' => 'newick',
      'dbxref' => trim($values["dbxref"]),
      'match' => $values["match"],
      'name_re' => $values["name_re"],
    ];

    $errors = [];
    $warnings = [];

    tripal_validate_phylotree('insert', $options, $errors, $warnings);

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
    $file_path = $this->arguments['files'][0]['file_path'];
    $name = $this->arguments["tree_name"];
    $description = $this->arguments["description"];
    $analysis_id = $this->arguments["analysis_id"];
    $leaf_type = $this->arguments["leaf_type"];
    $format = 'newick';
    $dbxref = $this->arguments["dbxref"];
    $match = $this->arguments["match"];
    $name_re = $this->arguments["name_re"];


    $options = array(
      'name'         => $node->tree_name,
      'description'  => $node->description,
      'analysis_id'  => $node->analysis_id,
      'leaf_type'    => $node->leaf_type,
      'tree_file'    => $node->tree_file,
      'format'       => 'newick',
      'dbxref'       => $node->dbxref,
      'match'        => $node->match,
      'name_re'      => $node->name_re,
    );
    $errors = array();
    $warnings = array();
    if (tripal_insert_phylotree($options, $errors, $warnings))

  }


  /**
   * This code parses the grammar for the Newick format as per the following
   * grammar:
   *
   *  Tree --> Subtree ";" | Branch ";"
   *  Subtree --> Leaf | Internal
   *  Leaf --> Name
   *  Internal --> "(" BranchSet ")" Name
   *  BranchSet --> Branch | BranchSet "," Branch
   *  Branch --> Subtree Length
   *  Name --> empty | string
   *  Length --> empty | ":" number
   *
   */


  function parse_newick_file($file_name) {

    // Initialize the bootstrap value and index
    global $tripal_phylogeny_bootstrap;

    $tripal_phylogeny_bootstrap = 1;
    $tripal_phylogeny_index = 1;

    $tree = [];

    $fp = fopen($file_name, 'r');
    if ($fp) {
      $tree = $this->parse_newick_tree($fp);
    }
    else {
      // ERROR
    }
    return $tree;
  }

  /**
   *
   * @param unknown $fp
   * @param number $depth
   *
   * @return boolean
   */
  function parse_newick_tree($fp, $depth = 0) {

    $subtree = $this->parse_newick_subtree($fp, $depth);
    $subtree['is_root'] = 1;

    // this subtree may also be a branch. A branch is a subtree with a length,
    // so see if there is a length
    $token = $this->parse_newick_get_token($fp);
    if ($token == ";") {
      // we're done!
      return $subtree;
    }
    $this->parse_newick_replace_token($fp);

    // Get the length.
    $length = $this->parse_newick_length($fp, $depth);
    $subtree['length'] = $length;

    // Now if we're missing the semicolon we have a syntax error.
    $token = $this->parse_newick_get_token($fp);
    if ($token != ';') {
      print "Syntax Error: missing trailing semicolon.\n";
      exit;
    }

    return $subtree;
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $depth
   *
   * @return Ambigous|unknown
   */
  function parse_newick_subtree($fp, $depth) {

    $internal = $this->parse_newick_internal($fp, $depth + 1);
    if (!is_array($internal)) {
      $leaf_node = $this->parse_newick_leaf($fp, $depth);
      return [
        'name' => $leaf_node,
        'depth' => $depth,
        'is_leaf' => TRUE,
        'descendents' => 0,
      ];
    }
    else {
      $internal['depth'] = $depth;
    }
    return $internal;
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $depth
   *
   * @return boolean|multitype:unknown Ambigous <Ambigous, unknown>
   */
  function parse_newick_branch($fp, $depth) {

    $subtree = $this->parse_newick_subtree($fp, $depth);
    $length = $this->parse_newick_length($fp, $depth);

    $subtree['length'] = $length;
    return $subtree;
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $parent
   * @param unknown $depth
   */
  function parse_newick_internal($fp, $depth) {

    // If the next character is not an open paren then this is an internal node
    if ($this->parse_newick_get_token($fp) != '(') {
      $this->parse_newick_replace_token($fp);
      return FALSE;
    }

    $branches = $this->parse_newick_branchset($fp, $depth);
    if (!is_array($branches)) {
      return FALSE;
    }
    // If we don't have a closing parent then this is a syntax error.
    if ($this->parse_newick_get_token($fp) != ')') {
      $this->parse_newick_replace_token($fp);
      return FALSE;
    }
    $internal_node = $this->parse_newick_name($fp, $depth);
    $descendent_count = 0;
    for ($i = 0; $i < count($branches); $i++) {
      $branches[$i]['parent'] = $internal_node;
      $descendent_count += 1 + $branches[$i]['descendents'];
    }

    return [
      'name' => $internal_node,
      'depth' => $depth,
      'branch_set' => $branches,
      'is_internal' => TRUE,
      'descendents' => $descendent_count,
    ];
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $parent
   * @param unknown $depth
   */
  function parse_newick_branchset($fp, $depth) {
    $branches = [];

    $num_read = 0;
    $branch = $this->parse_newick_branch($fp, $depth);
    $branches[] = $branch;

    // If it's not a branch then return false, a branchset will
    // always appear as a branch.
    if (!is_array($branch)) {
      return FALSE;
    }

    // If we have a comma as the next token then this is
    // a branchset and we should recurse.
    $token = $this->parse_newick_get_token($fp);
    if ($token == ',') {
      $rbranches = $this->parse_newick_branchset($fp, $depth);
      foreach ($rbranches as $branch) {
        $branches[] = $branch;
      }
    }
    else {
      $this->newick_replace_token($fp);
    }

    return $branches;
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $depth
   *
   * @return Ambigous <string, boolean, unknown>
   */
  function parse_newick_leaf($fp, $depth) {
    return $this->parse_newick_name($fp, $depth);
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $depth
   *
   * @return string|boolean|Ambigous <string, unknown>
   */
  function parse_newick_name($fp, $depth) {
    global $tripal_phylogeny_bootstrap;

    $token = $this->parse_newick_get_token($fp);

    // If the next token is a colon, semicolon, close paren, or comma
    // then the name is empty.
    if ($token == ':' or $token == ',' or $token == ';' or $token == ')') {
      $this->parse_newick_replace_token($fp);
      // create a bootstrap value
      return $tripal_phylogeny_bootstrap++;
    }

    // If the next token is an open paren then this is a syntax error:
    if ($token == '(') {
      $this->parse_newick_replace_token($fp);
      return FALSE;
    }
    return $token;
  }

  /**
   *
   * @param unknown $fp
   * @param unknown $depth
   *
   * @return string|boolean|unknown
   */
  function parse_newick_length($fp, $depth) {
    $length = '';

    $token = $this->parse_newick_get_token($fp);

    // If the next token is a semicolon, close paren, or comma
    // then the length is empty.
    if ($token == ',' or $token == ';' or $token == ')') {
      $this->newick_replace_token($fp);
      return '';
    }

    // If the next token is a colon then we are parsing the length.
    // Otherwise we are not.
    if ($token != ':') {
      $this->parse_newick_replace_token($fp);
      return FALSE;
    }

    // Now get the length.
    $token = $this->parse_newick_get_token($fp);

    // If the next token is an open paren then this is a syntax error:
    if ($token == '(') {
      exit();
    }

    return $token;
  }

  /**
   *
   * @param unknown $fp
   *
   * @return string
   */
  function parse_newick_get_token($fp) {

    // Keep track of the file position that we start with
    global $tripal_phylogeny_fp_pos;
    $tripal_phylogeny_fp_pos = ftell($fp);

    $token = '';
    $in_quote = FALSE;
    $num_read = 0;

    $c = fgetc($fp);
    while (!feof($fp)) {
      $num_read++;

      switch ($c) {
        // If the first character is a reserved character and we
        // we have not encountered any other charcters then return
        // it as the token. Otherwise, return the collected token.
        case ';':
        case '(':
        case ')':
        case ',':
        case ':':
          if (!$token) {
            return $c;
          }
          else {
            // put the character back and return the token
            fseek($fp, $tripal_phylogeny_fp_pos + $num_read - 1);
            return $token;
          }

          break;
        // Quotes are allowed around names and if a name is in
        // quotes then allow spaces. Otherwise, spaces are ignored.
        case '\'':
        case '"':
          if (!$in_quote) {
            $in_quote = TRUE;
          }
          else {
            $in_quote = FALSE;
          }
          break;
        case " ":
        case "\t":
        case "\r":
        case "\n":
          if ($in_quote) {
            $token .= $c;
          }
          break;
        // All other characters get saved as the token
        default:
          $token .= $c;
      }
      $c = fgetc($fp);
    }
    return $token;
  }

  /**
   *
   * @param unknown $fp
   */
  function parse_newick_replace_token($fp) {
    global $tripal_phylogeny_fp_pos;

    fseek($fp, $tripal_phylogeny_fp_pos);
    $tripal_phylogeny_fp_pos = ftell($fp);
  }

}
