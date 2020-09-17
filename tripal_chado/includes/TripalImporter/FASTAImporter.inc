<?php

class FASTAImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado FASTA Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_fasta_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Load sequences from a multi-FASTA file into Chado';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = [
    'fasta',
    'txt',
    'fa',
    'aa',
    'pep',
    'nuc',
    'faa',
    'fna',
  ];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the FASTA file. The file must have a .fasta extension.';

  /**
   * The title that should appear above the file upload section.
   */
  public static $upload_title = 'FASTA Upload';

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import FASTA file';


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

    // get the list of organisms
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $org_rset = chado_query($sql);
    $organisms = [];
    $organisms[''] = '';
    while ($organism = $org_rset->fetchObject()) {
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
    }
    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#description' => t("Choose the organism to which these sequences are associated"),
      '#required' => TRUE,
      '#options' => $organisms,
    ];

    // get the sequence ontology CV ID
    $values = ['name' => 'sequence'];
    $cv = chado_select_record('cv', ['cv_id'], $values);
    $cv_id = $cv[0]->cv_id;

    $form['seqtype'] = [
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology (SO) term name that describes the sequences in the FASTA file (e.g. gene, mRNA, polypeptide, etc...)'),
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv_id",
    ];

    $form['method'] = [
      '#type' => 'radios',
      '#title' => 'Method',
      '#required' => TRUE,
      '#options' => [
        t('Insert only'),
        t('Update only'),
        t('Insert and update'),
      ],
      '#description' => t('Select how features in the FASTA file are handled.
         Select "Insert only" to insert the new features. If a feature already
         exists with the same name or unique name and type then it is skipped.
         Select "Update only" to only update featues that already exist in the
         database.  Select "Insert and Update" to insert features that do
         not exist and upate those that do.'),
      '#default_value' => 2,
    ];
    $form['match_type'] = [
      '#type' => 'radios',
      '#title' => 'Name Match Type',
      '#required' => TRUE,
      '#options' => [
        t('Name'),
        t('Unique name'),
      ],
      '#description' => t('Used for "updates only" or "insert and update" methods. Not required if method type is "insert".
        Feature data is stored in Chado with both a human-readable
        name and a unique name. If the features in your FASTA file are uniquely identified using
        a human-readable name then select the "Name" button. If your features are
        uniquely identified using the unique name then select the "Unique name" button.  If you
        loaded your features first using the GFF loader then the unique name of each
        features were indicated by the "ID=" attribute and the name by the "Name=" attribute.
        By default, the FASTA loader will use the first word (character string
        before the first space) as  the name for your feature. If
        this does not uniquely identify your feature consider specifying a regular expression in the advanced section below.
        Additionally, you may import both a name and a unique name for each sequence using the advanced options.'),
      '#default_value' => 1,
    ];

    // Additional Options
    $form['additional'] = [
      '#type' => 'fieldset',
      '#title' => t('Additional Options'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $form['additional']['re_help'] = [
      '#type' => 'item',
      '#value' => t('A regular expression is an advanced method for extracting
         information from a string of text. Your FASTA file may contain both a
         human-readable name and a unique name for each sequence. If you want
         to import both the name and unique name for all sequences, then you
         must provide regular expressions so that the loader knows how to
         separate them. Otherwise the name and uniquename will be the same.
         By default, this loader will use the first word in the definition
         lines of the FASTA file
         as the name or unique name of the feature.'),
    ];
    $form['additional']['re_name'] = [
      '#type' => 'textfield',
      '#title' => t('Regular expression for the name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the
         feature name from the FASTA definition line. For example, for a
         defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename),
         the regular expression for the name would be, "^(.*?)\|.*$".  All FASTA
         definition lines begin with the ">" symbol.  You do not need to incldue
         this symbol in your regular expression.'),
    ];
    $form['additional']['re_uname'] = [
      '#type' => 'textfield',
      '#title' => t('Regular expression for the unique name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the
         feature name from the FASTA definition line. For example, for a
         defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename),
         the regular expression for the unique name would be "^.*?\|(.*)$".  All FASTA
         definition lines begin with the ">" symbol.  You do not need to incldue
         this symbol in your regular expression.'),
    ];

    // Advanced database cross reference options.
    $form['additional']['db'] = [
      '#type' => 'fieldset',
      '#title' => t('External Database Reference'),
      '#weight' => 6,
      '#collapsed' => TRUE,
    ];
    $form['additional']['db']['re_accession'] = [
      '#type' => 'textfield',
      '#title' => t('Regular expression for the accession'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the accession for the external database for each feature from the FASTA definition line.'),
      '#weight' => 2,
    ];

    // get the list of databases
    $sql = "SELECT * FROM {db} ORDER BY name";
    $db_rset = chado_query($sql);
    $dbs = [];
    $dbs[''] = '';
    while ($db = $db_rset->fetchObject()) {
      $dbs[$db->db_id] = "$db->name";
    }
    $form['additional']['db']['db_id'] = [
      '#title' => t('External Database'),
      '#type' => t('select'),
      '#description' => t("Plese choose an external database for which these sequences have a cross reference."),
      '#required' => FALSE,
      '#options' => $dbs,
      '#weight' => 1,
    ];

    $form['additional']['relationship'] = [
      '#type' => 'fieldset',
      '#title' => t('Relationships'),
      '#weight' => 6,
      '#collapsed' => TRUE,
    ];
    $rels = [];
    $rels[''] = '';
    $rels['part_of'] = 'part of';
    $rels['derives_from'] = 'produced by (derives from)';

    // Advanced references options
    $form['additional']['relationship']['rel_type'] = [
      '#title' => t('Relationship Type'),
      '#type' => t('select'),
      '#description' => t("Use this option to create associations, or relationships between the
                        features of this FASTA file and existing features in the database. For
                        example, to associate a FASTA file of peptides to existing genes or transcript sequence,
                        select the type 'produced by'. For a CDS sequences select the type 'part of'"),
      '#required' => FALSE,
      '#options' => $rels,
      '#weight' => 5,
    ];
    $form['additional']['relationship']['re_subject'] = [
      '#type' => 'textfield',
      '#title' => t('Regular expression for the parent'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract the unique
                         name needed to identify the existing sequence for which the
                         relationship type selected above will apply.  If no regular
                         expression is provided, the parent unique name must be the
                         same as the loaded feature name.'),
      '#weight' => 6,
    ];
    $form['additional']['relationship']['parent_type'] = [
      '#type' => 'textfield',
      '#title' => t('Parent Type'),
      '#required' => FALSE,
      '#description' => t('Please enter the Sequence Ontology term for the parent.  For example
                         if the FASTA file being loaded is a set of proteins that are
                         products of genes, then use the SO term \'gene\' or \'transcript\' or equivalent. However,
                         this type must match the type for already loaded features.'),
      '#weight' => 7,
    ];
    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $organism_id = $form_state['values']['organism_id'];
    $type = trim($form_state['values']['seqtype']);
    $method = trim($form_state['values']['method']);
    $match_type = trim($form_state['values']['match_type']);
    $re_name = trim($form_state['values']['re_name']);
    $re_uname = trim($form_state['values']['re_uname']);
    $re_accession = trim($form_state['values']['re_accession']);
    $db_id = $form_state['values']['db_id'];
    $rel_type = $form_state['values']['rel_type'];
    $re_subject = trim($form_state['values']['re_subject']);
    $parent_type = trim($form_state['values']['parent_type']);

    if ($method == 0) {
      $method = 'Insert only';
    }
    if ($method == 1) {
      $method = 'Update only';
    }
    if ($method == 2) {
      $method = 'Insert and update';
    }

    if ($match_type == 0) {
      $match_type = 'Name';
    }

    if ($match_type == 1) {
      $match_type = 'Unique name';
    }

    if ($re_name and !$re_uname and strcmp($match_type, 'Unique name') == 0) {
      form_set_error('re_uname', t("You must provide a regular expression to identify the sequence unique name"));
    }

    if (!$re_name and $re_uname and strcmp($match_type, 'Name') == 0) {
      form_set_error('re_name', t("You must provide a regular expression to identify the sequence name"));
    }

    // make sure if a relationship is specified that all fields are provided.

    if (($rel_type or $re_subject) and !$parent_type) {
      form_set_error('parent_type', t("Please provide a SO term for the parent"));
    }
    if (($parent_type or $re_subject) and !$rel_type) {
      form_set_error('rel_type', t("Please select a relationship type"));
    }

    // make sure if a database is specified that all fields are provided
    if ($db_id and !$re_accession) {
      form_set_error('re_accession', t("Please provide a regular expression for the accession"));
    }
    if ($re_accession and !$db_id) {
      form_set_error('db_id', t("Please select a database"));
    }

    // Check to make sure the regexps are valid.
    if ($re_name && @preg_match("/$re_name/", null) === false) {
      form_set_error('re_name', t("please provide a valid regular expression for the feature name."));
    }
    if ($re_uname && @preg_match("/$re_uname/", null) === false) {
      form_set_error('re_uname', t("please provide a valid regular expression for the feature unique name."));
    }
    if ($re_accession && @preg_match("/$re_accession/", null) === false) {
      form_set_error('re_accession', t("please provide a valid regular expression for the external database accession."));
    }
    if ($re_subject && @preg_match("/$re_subject/", null) === false) {
      form_set_error('re_subject', t("please provide a valid regular expression for the relationship parent."));
    }

    // check to make sure the types exists
    $cvtermsql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
    ";
    $cvterm = chado_query($cvtermsql,
      [
        ':cvname' => 'sequence',
        ':name' => $type,
        ':synonym' => $type,
      ])->fetchObject();
    if (!$cvterm) {
      form_set_error('type', t("The Sequence Ontology (SO) term selected for the sequence type is not available in the database. Please check spelling or select another."));
    }
    if ($rel_type) {
      $cvterm = chado_query($cvtermsql, [
        ':cvname' => 'sequence',
        ':name' => $parent_type,
        ':synonym' => $parent_type,
      ])->fetchObject();
      if (!$cvterm) {
        form_set_error('parent_type', t("The Sequence Ontology (SO) term selected for the parent relationship is not available in the database. Please check spelling or select another."));
      }
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {
    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files'][0]['file_path'];

    $organism_id = $arguments['organism_id'];
    $type = $arguments['seqtype'];
    $method = $arguments['method'];
    $match_type = $arguments['match_type'];
    $re_name = $arguments['re_name'];
    $re_uname = $arguments['re_uname'];
    $re_accession = $arguments['re_accession'];
    $db_id = $arguments['db_id'];
    $rel_type = $arguments['rel_type'];
    $re_subject = $arguments['re_subject'];
    $parent_type = $arguments['parent_type'];
    $method = $arguments['method'];
    $analysis_id = $arguments['analysis_id'];
    $match_type = $arguments['match_type'];


    if ($method == 0) {
      $method = 'Insert only';
    }
    if ($method == 1) {
      $method = 'Update only';
    }
    if ($method == 2) {
      $method = 'Insert and update';
    }

    if ($match_type == 0) {
      $match_type = 'Name';
    }

    if ($match_type == 1) {
      $match_type = 'Unique name';
    }

    $this->loadFasta($file_path, $organism_id, $type, $re_name, $re_uname, $re_accession,
      $db_id, $rel_type, $re_subject, $parent_type, $method, $analysis_id,
      $match_type);
  }

  /**
   * Load a fasta file.
   *
   * @param $file_path
   *   The full path to the fasta file to load.
   * @param $organism_id
   *   The organism_id of the organism these features are from.
   * @param $type
   *   The type of features contained in the fasta file.
   * @param $re_name
   *   The regular expression to extract the feature.name from the fasta header.
   * @param $re_uname
   *   The regular expression to extract the feature.uniquename from the fasta
   *   header.
   * @param $re_accession
   *   The regular expression to extract the accession of the feature.dbxref_id.
   * @param $db_id
   *   The database ID of the above accession.
   * @param $rel_type
   *   The type of relationship when creating a feature_relationship between
   *   this feature (object) and an extracted subject.
   * @param $re_subject
   *   The regular expression to extract the uniquename of the feature to be
   *   the subject of the above specified relationship.
   * @param $parent_type
   *   The type of the parent feature.
   * @param $method
   *   The method of feature adding. (ie: 'Insert only', 'Update only',
   *   'Insert and update').
   * @param $analysis_id
   *   The analysis_id to associate the features in this fasta file with.
   * @param $match_type
   *  Whether to match existing features based on the 'Name' or 'Unique name'.
   */
  private function loadFasta($file_path, $organism_id, $type, $re_name, $re_uname, $re_accession,
                             $db_id, $rel_type, $re_subject, $parent_type, $method, $analysis_id, $match_type) {

    // First get the type for this sequence.
    $cvtermsql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
    ";
    $cvterm = chado_query($cvtermsql, [
      ':cvname' => 'sequence',
      ':name' => $type,
      ':synonym' => $type,
    ])->fetchObject();
    if (!$cvterm) {
      $this->logMessage("Cannot find the term type: '!type'", ['!type' => $type], TRIPAL_ERROR);
      return 0;
    }

    // Second, if there is a parent type then get that.
    $parentcvterm = NULL;
    if ($parent_type) {
      $parentcvterm = chado_query($cvtermsql, [
        ':cvname' => 'sequence',
        ':name' => $parent_type,
        ':synonym' => $parent_type,
      ])->fetchObject();
      if (!$parentcvterm) {
        $this->logMessage("Cannot find the parent term type: '!type'",
          ['!type' => $parentcvterm], TRIPAL_ERROR);
        return 0;
      }
    }

    // Third, if there is a relationship type then get that.
    $relcvterm = NULL;
    if ($rel_type) {
      $relcvterm = chado_query($cvtermsql, [
        ':cvname' => 'sequence',
        ':name' => $rel_type,
        ':synonym' => $rel_type,
      ])->fetchObject();
      if (!$relcvterm) {
        $this->logMessage("Cannot find the relationship term type: '!type'",
          ['!type' => $relcvterm], TRIPAL_ERROR);
        return 0;
      }
    }

    // We need to get the table schema to make sure we don't overrun the
    // size of fields with what our regular expressions retrieve
    $feature_tbl = chado_get_schema('feature');
    $dbxref_tbl = chado_get_schema('dbxref');

    $this->logMessage(t("Step 1: Finding sequences..."));
    $filesize = filesize($file_path);
    $fh = fopen($file_path, 'r');
    if (!$fh) {
      throw new Exception(t("Cannot open file: !dfile", ['!dfile' => $file_path]));
    }
    $num_read = 0;

    // Iterate through the lines of the file. Keep a record for
    // where in the file each line is at for later import.
    $seqs = [];
    $num_seqs = 0;
    $prev_pos = 0;
    $set_start = FALSE;
    $i = 0;

    while ($line = fgets($fh)) {
      $i++;
      $num_read += strlen($line);

      // If we encounter a definition line then get the name, uniquename,
      // accession and relationship subject from the definition line.
      if (preg_match('/^>/', $line)) {

        // Remove the > symbol from the defline.
        $defline = preg_replace("/^>/", '', $line);

        // Get the feature name if a regular expression is provided.
        $name = "";
        if ($re_name) {
          if (!preg_match("/$re_name/", $defline, $matches)) {
            $this->logMessage("Regular expression for the feature name finds nothing. Line !line.",
              ['!line' => $i], TRIPAL_ERROR);
          }
          elseif (strlen($matches[1]) > $feature_tbl['fields']['name']['length']) {
            $this->logMessage("Regular expression retrieves a value too long for the feature name. Line !line.",
              ['!line' => $i], TRIPAL_WARNING);
          }
          else {
            $name = trim($matches[1]);
          }
        }

        // If the match_type is name and no regular expression was provided
        // then use the first word as the name, otherwise we don't set the name.
        elseif (strcmp($match_type, 'Name') == 0) {
          if (preg_match("/^\s*(.*?)[\s\|].*$/", $defline, $matches)) {
            if (strlen($matches[1]) > $feature_tbl['fields']['name']['length']) {
              $this->logMessage("Regular expression retrieves a feature name too long for the feature name. Line !line.",
                ['!line' => $i], TRIPAL_WARNING);
            }
            else {
              $name = trim($matches[1]);
            }
          }
          else {
            $this->logMessage("Cannot find a feature name. Line !line.", ['!line' => $i], TRIPAL_WARNING);
          }
        }

        // Get the feature uniquename if a regular expression is provided.
        $uname = "";
        if ($re_uname) {
          if (!preg_match("/$re_uname/", $defline, $matches)) {
            $this->logMessage("Regular expression for the feature unique name finds nothing. Line !line.",
              ['!line' => $i], TRIPAL_ERROR);
          }
          $uname = trim($matches[1]);
        }
        // If the match_type is name and no regular expression was provided
        // then use the first word as the name, otherwise, we don't set the
        // uniquename.
        elseif (strcmp($match_type, 'Unique name') == 0) {
          if (preg_match("/^\s*(.*?)[\s\|].*$/", $defline, $matches)) {
            $uname = trim($matches[1]);
          }
          else {
            $this->logMessage("Cannot find a feature unique name. Line !line.",
              ['!line' => $i], TRIPAL_ERROR);
          }
        }

        // Get the accession if a regular expression is provided.
        $accession = "";
        if (!empty($re_accession)) {
          preg_match("/$re_accession/", $defline, $matches);
          if (strlen($matches[1]) > $dbxref_tbl['fields']['accession']['length']) {
            tripal_report_error('trp-fasta', TRIPAL_WARNING, "WARNING: Regular expression retrieves an accession too long for the feature name. " .
              "Cannot add cross reference. Line %line.", [
              '%line' => $i,
            ]);
          }
          else {
            $accession = trim($matches[1]);
          }
        }

        // Get the relationship subject
        $subject = $uname ? $uname : "";
        if (!empty($re_subject)) {
          preg_match("/$re_subject/", $line, $matches);
          $subject = trim($matches[1]);
        }

        // Add the details to the sequence.
        $seqs[$num_seqs] = [
          'name' => $name,
          'uname' => $uname,
          'accession' => $accession,
          'subject' => $subject,
          'seq_start' => ftell($fh),
        ];
        $set_start = TRUE;
        // If this isn't the first sequence, then we want to specify where
        // the previous sequence ended.
        if ($num_seqs > 0) {
          $seqs[$num_seqs - 1]['seq_end'] = $prev_pos;
        }
        $num_seqs++;
      }
      // Keep the current file position so we can use it to set the sequence
      // ending position
      $prev_pos = ftell($fh);

    }

    // Set the end position for the last sequence.
    $seqs[$num_seqs - 1]['seq_end'] = $num_read - strlen($line);

    // Now that we know where the sequences are in the file we need to add them.
    $this->logMessage("Step 2: Importing sequences...");
    $this->logMessage("Found !num_seqs sequence(s).", ['!num_seqs' => $num_seqs]);
    $this->setTotalItems($num_seqs);
    $this->setItemsHandled(0);
    for ($j = 0; $j < $num_seqs; $j++) {
      $seq = $seqs[$j];
      //$this->logMessage("Importing !seqname.", array('!seqname' => $seq['name']));
      $source = NULL;
      $this->loadFastaFeature($fh, $seq['name'], $seq['uname'], $db_id,
        $seq['accession'], $seq['subject'], $rel_type, $parent_type,
        $analysis_id, $organism_id, $cvterm, $source, $method, $re_name,
        $match_type, $parentcvterm, $relcvterm, $seq['seq_start'],
        $seq['seq_end']);
      $this->setItemsHandled($j);
    }
    fclose($fh);
    $this->setItemsHandled($num_seqs);
  }

  /**
   * A helper function for loadFasta() to load a single feature
   *
   * @ingroup fasta_loader
   */
  private function loadFastaFeature($fh, $name, $uname, $db_id, $accession, $parent,
                                    $rel_type, $parent_type, $analysis_id, $organism_id, $cvterm, $source, $method, $re_name,
                                    $match_type, $parentcvterm, $relcvterm, $seq_start, $seq_end) {

    // Check to see if this feature already exists if the match_type is 'Name'.
    if (strcmp($match_type, 'Name') == 0) {
      $values = [
        'organism_id' => $organism_id,
        'name' => $name,
        'type_id' => $cvterm->cvterm_id,
      ];
      $results = chado_select_record('feature', [
        'feature_id',
      ], $values);
      if (count($results) > 1) {
        $this->logMessage("Multiple features exist with the name '!name' of type '!type' for the organism.  skipping",
          ['!name' => $name, '!type' => $cvterm->name], TRIPAL_ERROR);
        return 0;
      }
      if (count($results) == 1) {
        $feature = $results[0];
      }
    }

    // Check if this feature already exists if the match_type is 'Unique Name'.
    if (strcmp($match_type, 'Unique name') == 0) {
      $values = [
        'organism_id' => $organism_id,
        'uniquename' => $uname,
        'type_id' => $cvterm->cvterm_id,
      ];

      $results = chado_select_record('feature', ['feature_id'], $values);
      if (count($results) > 1) {
        $this->logMessage("Multiple features exist with the name '!name' of type '!type' for the organism.  skipping",
          ['!name' => $name, '!type' => $cvterm->name], TRIPAL_WARNING);
        return 0;
      }
      if (count($results) == 1) {
        $feature = $results[0];
      }

      // If the feature exists but this is an "insert only" then skip.
      if (isset($feature) and (strcmp($method, 'Insert only') == 0)) {
        $this->logMessage("Feature already exists '!name' ('!uname') while matching on !type. Skipping insert.",
          [
            '!name' => $name,
            '!uname' => $uname,
            '!type' => drupal_strtolower($match_type),
          ], TRIPAL_WARNING);
        return 0;
      }
    }

    // If we don't have a feature and we're doing an insert then do the insert.
    $inserted = 0;
    if (!isset($feature) and (strcmp($method, 'Insert only') == 0 or strcmp($method, 'Insert and update') == 0)) {
      // If we have a unique name but not a name then set them to be the same
      if (!$uname) {
        $uname = $name;
      }
      elseif (!$name) {
        $name = $uname;
      }

      // Insert the feature record.
      $values = [
        'organism_id' => $organism_id,
        'name' => $name,
        'uniquename' => $uname,
        'type_id' => $cvterm->cvterm_id,
      ];
      $success = chado_insert_record('feature', $values);
      if (!$success) {
        $this->logMessage("Failed to insert feature '!name (!uname)'", [
          '!name' => $name,
          '!uname' => $uname,
        ], TRIPAL_ERROR);
        return 0;
      }

      // now get the feature we just inserted
      $values = [
        'organism_id' => $organism_id,
        'uniquename' => $uname,
        'type_id' => $cvterm->cvterm_id,
      ];
      $results = chado_select_record('feature', ['feature_id'], $values);
      if (count($results) == 1) {
        $inserted = 1;
        $feature = $results[0];
      }
      else {
        $this->logMessage("Failed to retreive newly inserted feature '!name (!uname)'", [
          '!name' => $name,
          '!uname' => $uname,
        ], TRIPAL_ERRORR);
        return 0;
      }

      // Add the residues for this feature
      $this->loadFastaResidues($fh, $feature->feature_id, $seq_start, $seq_end);
    }

    // if we don't have a feature and the user wants to do an update then fail
    if (!isset($feature) and (strcmp($method, 'Update only') == 0 or strcmp($method, 'Insert and update') == 0)) {
      $this->logMessage("Failed to find feature '!name' ('!uname') while matching on " . drupal_strtolower($match_type) . ".",
        ['!name' => $name, '!uname' => $uname], TRIPAL_ERROR);
      return 0;
    }

    // if we do have a feature and this is an update then proceed with the update
    if (isset($feature) and !$inserted and (strcmp($method, 'Update only') == 0 or strcmp($method, 'Insert and update') == 0)) {

      // if the user wants to match on the Name field
      if (strcmp($match_type, 'Name') == 0) {

        // if we're matching on the name but do not have a unique name then we
        // don't want to update the uniquename.
        $values = [];
        if ($uname) {

          // First check to make sure that by changing the unique name of this
          // feature that we won't conflict with another existing feature of
          // the same name
          $values = [
            'organism_id' => $organism_id,
            'uniquename' => $uname,
            'type_id' => $cvterm->cvterm_id,
          ];
          $results = chado_select_record('feature', ['feature_id'], $values);
          if (count($results) > 0) {
            $this->logMessage("Cannot update the feature '!name' with a uniquename of '!uname' and type of '!type' as it " .
              "conflicts with an existing feature with the same uniquename and type.",
              [
                '!name' => $name,
                '!uname' => $uname,
                '!type' => $cvterm->name,
              ], TRIPAL_ERROR);
            return 0;
          }

          // the changes to the uniquename don't conflict so proceed with the update
          $values = ['uniquename' => $uname];
          $match = [
            'name' => $name,
            'organism_id' => $organism_id,
            'type_id' => $cvterm->cvterm_id,
          ];

          // perform the update
          $success = chado_update_record('feature', $match, $values);
          if (!$success) {
            $this->logMessage("Failed to update feature '!name' ('!name')",
              ['!name' => $name, '!uiname' => $uname], TRIPAL_ERROR);
            return 0;
          }
        }
      }

      // If the user wants to match on the unique name field.
      if (strcmp($match_type, 'Unique name') == 0) {
        // If we're matching on the uniquename and have a new name then
        // we want to update the name.
        $values = [];
        if ($name) {
          $values = ['name' => $name];
          $match = [
            'uniquename' => $uname,
            'organism_id' => $organism_id,
            'type_id' => $cvterm->cvterm_id,
          ];
          $success = chado_update_record('feature', $match, $values);
          if (!$success) {
            $this->logMessage("Failed to update feature '!name' ('!name')",
              ['!name' => $name, '!uiname' => $uname], TRIPAL_ERROR);
            return 0;
          }
        }
      }
    }

    // Update the residues for this feature
    $this->loadFastaResidues($fh, $feature->feature_id, $seq_start, $seq_end);

    // add in the analysis link
    if ($analysis_id) {
      // if the association doens't already exist then add one
      $values = [
        'analysis_id' => $analysis_id,
        'feature_id' => $feature->feature_id,
      ];
      $results = chado_select_record('analysisfeature', ['analysisfeature_id'], $values);
      if (count($results) == 0) {
        $success = chado_insert_record('analysisfeature', $values);
        if (!$success) {
          $this->logMessage("Failed to associate analysis and feature '!name' ('!name')",
            ['!name' => $name, '!uname' => $uname], TRIPAL_ERROR);
          return 0;
        }
      }
    }

    // now add the database cross reference
    if ($db_id) {
      // check to see if this accession reference exists, if not add it
      $values = [
        'db_id' => $db_id,
        'accession' => $accession,
      ];
      $results = chado_select_record('dbxref', ['dbxref_id'], $values);

      // if the accession doesn't exist then add it
      if (count($results) == 0) {
        $results = chado_insert_record('dbxref', $values);
        if (!$results) {
          $this->logMessage("Failed to add database accession '!accession'",
            ['!accession' => $accession], TRIPAL_ERROR);
          return 0;
        }
        $results = chado_select_record('dbxref', ['dbxref_id'], $values);
        if (count($results) == 1) {
          $dbxref = $results[0];
        }
        else {
          $this->logMessage("Failed to retreive newly inserted dbxref '!name (!uname)'",
            ['!name' => $name, '!uname' => $uname], TRIPAL_ERROR);
          return 0;
        }
      }
      else {
        $dbxref = $results[0];
      }

      // check to see if the feature dbxref record exists if not, then add it
      $values = [
        'feature_id' => $feature->feature_id,
        'dbxref_id' => $dbxref->dbxref_id,
      ];
      $results = chado_select_record('feature_dbxref', ['feature_dbxref_id'], $values);
      if (count($results) == 0) {
        $success = chado_insert_record('feature_dbxref', $values);
        if (!$success) {
          $this->logMessage("Failed to add associate database accession '!accession' with feature",
            ['!accession' => $accession], TRIPAL_ERROR);
          return 0;
        }
      }
    }

    // now add in the relationship if one exists. If not, then add it
    if ($rel_type) {
      $values = [
        'organism_id' => $organism_id,
        'uniquename' => $parent,
        'type_id' => $parentcvterm->cvterm_id,
      ];
      $results = chado_select_record('feature', ['feature_id'], $values);
      if (count($results) != 1) {
        $this->logMessage("Cannot find a unique feature for the parent '!parent' of type '!type' for the feature.",
          ['!parent' => $parent, '!type' => $parent_type], TRIPAL_ERROR);
        return 0;
      }
      $parent_feature = $results[0];

      // check to see if the relationship already exists if not then add it
      $values = [
        'subject_id' => $feature->feature_id,
        'object_id' => $parent_feature->feature_id,
        'type_id' => $relcvterm->cvterm_id,
      ];
      $results = chado_select_record('feature_relationship', ['feature_relationship_id'], $values);
      if (count($results) == 0) {
        $success = chado_insert_record('feature_relationship', $values);
        if (!$success) {
          $this->logMessage("Failed to add associate database accession '!accession' with feature",
            ['!accession' => $accession], TRIPAL_ERROR);
          return 0;
        }
      }
    }
  }

  /**
   * Adds the residues column to the feature.
   *
   * This function seeks to the proper location in the file for the sequence
   * and reads in chunks of sequence and appends them to the feature.residues
   * column in the database.
   *
   * @param $fh
   * @param $feature_id
   * @param $seq_start
   * @param $seq_end
   */
  private function loadFastaResidues($fh, $feature_id, $seq_start, $seq_end) {

    // First position the file at the beginning of the sequence
    fseek($fh, $seq_start, SEEK_SET);
    $chunk_size = 100000000;
    $chunk = '';
    $seqlen = ($seq_end - $seq_start);

    $num_read = 0;
    $total_seq_size = 0;

    // First, make sure we don't have a null in the residues
    $sql = "UPDATE {feature} SET residues = '' WHERE feature_id = :feature_id";
    chado_query($sql, [':feature_id' => $feature_id]);

    // Read in the lines until we reach the end of the sequence. Once we
    // get a specific bytes read then append the sequence to the one in the
    // database.
    $partial_seq_size = 0;
    $chunk_intv_read = 0;
    while ($line = fgets($fh)) {
      $num_read += strlen($line) + 1;
      $chunk_intv_read += strlen($line) + 1;
      $partial_seq_size += strlen($line);
      $chunk .= trim($line);

      // If we've read in enough of the sequence then append it to the database.
      if ($chunk_intv_read >= $chunk_size) {
        $sql = "
          UPDATE {feature}
          SET residues = residues || :chunk
          WHERE feature_id = :feature_id
        ";
        $success = chado_query($sql, [
          ':feature_id' => $feature_id,
          ':chunk' => $chunk,
        ]);
        if (!$success) {
          return FALSE;
        }
        $total_seq_size += strlen($chunk);
        $chunk = '';
        $chunk_intv_read = 0;
      }

      // If we've reached the end of the sequence then break out of the loop
      if (ftell($fh) == $seq_end) {
        break;
      }
    }

    // write the last bit of sequence if it remains
    if (strlen($chunk) > 0) {
      $sql = "
          UPDATE {feature}
          SET residues = residues || :chunk
          WHERE feature_id = :feature_id
        ";
      $success = chado_query($sql, [
        ':feature_id' => $feature_id,
        ':chunk' => $chunk,
      ]);
      if (!$success) {
        return FALSE;
      }
      $total_seq_size += $partial_seq_size;
      $partial_seq_size = 0;
      $chunk = '';
      $chunk_intv_read = 0;
    }

    // Now update the seqlen and md5checksum fields
    $sql = "UPDATE {feature} SET seqlen = char_length(residues),  md5checksum = md5(residues) WHERE feature_id = :feature_id";
    chado_query($sql, [':feature_id' => $feature_id]);

  }
}
