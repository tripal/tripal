<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;

/**
 * GFF3 Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_gff3_loader",
 *    label = @Translation("Chado GFF3 File Loader"),
 *    description = @Translation("Import a GFF3 file into Chado"),
 *    file_types = {"gff","gff3", "txt"},
 *    upload_description = @Translation("Please provide a plain text, tab-delimited file following the <a target='_blank' href='https://github.com/The-Sequence-Ontology/Specifications/blob/master/gff3.md'>GFF3 Specification</a>. It is expected that all landmark features are associated with the same organism and that the type (column 3) are sequence ontology terms."),
 *    upload_title = @Translation("GFF3 File"),
 *    use_analysis = True,
 *    require_analysis = True,
 *    button_text = @Translation("Import GFF3 file"),
 *    file_upload = True,
 *    file_remote = True,
 *    file_local = True,
 *    file_required = True,
 *  )
 */
class GFF3Importer extends ChadoImporterBase implements ContainerFactoryPluginInterface {

  /**
   * Used to store the manager so we can create various buddies
   */
  protected object $buddy_manager;

  /**
   * Provide the dbxref buddy instance
   */
  protected object $dbxref_buddy;

  /**
   * Provide the cvterm buddy instance
   */
  protected object $cvterm_buddy;

  /**
   * Provide the property buddy instance
   */
  protected object $property_buddy;

  /**
   * A handle to a temporary file for caching the GFF features. This allows for
   * quick lookup of parsed features without having to store it in RAM.
   */
  private $gff_cache_file = NULL;

  /**
   * The name of the temporary cache file.
   */
  private $gff_cache_file_name = NULL;

  /**
   * The lines from the ##sequence-region at the top of the GFF
   */
  private $seq_region_headers = [];

  /**
   * The path to the GFF3 file.
   */
  private $gff_file = NULL;

  /**
   * The file handle for the GFF3 file.
   */
  private $gff_file_h = NULL;

  /**
   * The organism ID for this GFF file.
   */
  private $organism_id = NULL;

  /**
   * The organism ChadoRecord object that corresponds to the $organism_id value.
   */
  private $organism = NULL;

  /**
   * An array of organism records for quick lookup.
   */
  private $organism_lookup = [];

  /**
   * The analysis ID for this GFF file
   */
  private $analysis_id = NULL;

  /**
   * The analysis results object that corresponds to the $analysis_id value.
   */
  private $analysis = NULL;

  /**
   * A flag indicating if only new items should be added (no updates)
   */
  private $add_only = NULL;

  /**
   * A flag indicting if only existing items should be updated.
   */
  private $update = TRUE;

  /**
   * A list of features to have names updated.
   */
  private $update_names = [];

  /**
   * If the GFF file contains a 'Target' attribute then the feature and the
   * target will have an alignment created, but to find the proper target
   * feature the target organism must also be known.  If different from the
   * organism specified for the GFF file, then use  this argument to specify
   * the target organism.  Only use this argument if all target sequences
   * belong to the same species. If the targets in the GFF file belong to
   * multiple different species then the organism must be specified using the
   * 'target_organism=genus:species' attribute in the GFF file. Default is
   * NULL.
   */
  private $target_organism_id = NULL;

  /**
   * If the GFF file contains a 'Target' attribute then the feature and the
   * target will have an alignment created, but to find the proper target
   * feature the target organism must also be known.  This can be used to
   * specify the target feature type to help with identification of the
   * target feature.  Only use this argument if all target sequences types are
   * the same. If the targets are of different types then the type must be
   * specified using the 'target_type=type' attribute in the GFF file. This
   * must be a valid Sequence Ontology (SO) term. Default is NULL
   */
  private $target_type = NULL;
  private $target_type_id = NULL;

  /**
   * A flag indicating if the target feature should be created. If FALSE
   * then it should already exist.
   */
  private $create_target = FALSE;

  /**
   * Set this to the line in the GFF file where importing should start. This
   * is useful for testing and debugging GFF files that may have problems and
   * you want to start at a particular line to speed testing.  Default = 1
   */
  private $start_line = 1;

  /**
   * During parsing of the GFF file this keeps track of the current line
   * number.
   */
  private $current_line = 0;

  /**
   * A Sequence Ontology term name for the landmark sequences in the GFF
   * file (e.g. 'chromosome'), if the GFF file contains a '##sequence-region'
   * line that describes the landmark sequences. Default = ''
   */
  private $default_landmark_type = '';

  /**
   * $this->landmark_types which is an associative array.
   * The key is the landmark name, and the value is the type name.
   */
  private $landmark_types = [];

  /**
   * $this->landmark_types_type_ids which is an associative array.
   * The key is the type, and the value is the type_id.
   */
  private $landmark_types_type_ids = [];

  /**
   * The cvterm_id for the landmark type cvterm.
   */
  private $landmark_cvterm_id = NULL;

  /**
   * Regular expression to pull out the mRNA name.
   */
  private $re_mrna = '';

  /**
   * Regular expression to pull out the protein name.
   */
  private $re_protein = '';

  /**
   * A flag that indicates if a protein record should be created.
   * @var integer
   */
  private $skip_protein = 0;

  /**
   * Sometimes lines in the GFF file are missing the required ID attribute
   * that specifies the unique name of the feature. If so, you may specify
   * the name of an existing attribute to use for the ID.
   */
  private $alt_id_attr = '';

  /**
   * The Tripal GFF loader supports the "organism" attribute. This allows
   * features of a different organism to be aligned to the landmark sequence
   * of another species. The format of the attribute is
   * "organism=[genus]:[species]", where [genus] is the organism's genus and
   * [species] is the species name. Check this box to automatically add the
   * organism to the database if it does not already exists. Otherwise lines
   * with an organism attribute where the organism is not present in the
   * database will be skipped.
   */
  private $create_organism = FALSE;

  /**
   * Holds mapping of DB names to DB ids.
   */
  private $db_lookup = [];

  /**
   * Holds a mapping of Dbxref names to ids.
   */
  private $dbxref_lookup = [];

  /**
   * Holds a mapping of Dbxref names to cvterm ids.
   */
  private $cvterm_lookup = [];

  /**
   * Holds a mapping of synonymns to ids.
   */
  private $synonym_lookup = [];

  /**
   * Maps parents to their children and contains the ranks of the children.
   */
  private $parent_lookup = [];

  /**
   * An array that stores CVterms that have been looked up so we don't have
   * to do the database query every time.
   */
  private $feature_cvterm_lookup = [];

  /**
   * An array that stores CVterms that have been looked up so we don't have
   * to do the database query every time.
   */
  private $featureprop_cvterm_lookup = [];

  /**
   * Holds the CV term id for the "exact" synonym.
   */
  private $exact_syn_id = NULL;

  /**
   * Holds the object for the null publication record.
   */
  private $null_pub = NULL;

  /**
   * The list of features from the GFF3 file.  Each element is an
   * associative array of the columns from the GFF3 file, with the attribute
   * field being an associative array of key/value pairs.
   */
  private $features = [];

  /**
   * An associative array containing the pointers to the FASTA sequences
   * in the GFF file. We don't want to load these into memory as they
   * may be too big!
   */
  private $residue_index = [];

  /**
   * An array that stores landmarks objects.  Landmarks should be inserted
   * first if they don't already exist.
   */
  private $landmarks = [];


  /**
   * A controlled vocabulary results object. This is the CV that will be
   * used to for feature properties.
   */
  private $feature_prop_cv = NULL;


  /**
   * A controlled vocabulary results object. This is the CV that will be
   * used to for feature properties.
   */
  private $feature_cv = NULL;

  /**
   * Stores proteins
   */
  private $proteins = [];


  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager.
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database'),
      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition,
                              ChadoConnection $connection, ChadoBuddyPluginManager $buddy_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $connection);
    $this->buddy_manager = $buddy_manager;
    $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
    $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
    $this->property_buddy = $this->buddy_manager->createInstance('chado_property_buddy', []);
  }

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    // get the list of organisms
    $organisms = chado_get_organism_select_options(FALSE, TRUE);

    // get the sequence ontology CV id
    $conditions = ['cv.name' => 'sequence'];
    $cv_record = $this->cvterm_buddy->getCv($conditions, []);
    $sequence_cv_id = $cv_record->getValue('cv.cv_id');

    $form['organism_id'] = [
      '#title' => t('Existing Organism'),
      '#type' => 'select',
      '#description' => t("Choose an existing organism to which the entries in the GFF file will be associated."),
      '#required' => TRUE,
      '#options' => $organisms,
      '#empty_option' => t('- Select -'),
    ];

    $form['landmark_type'] = [
      '#title' => t('Default Landmark Type'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#description' => t("Use this field to specify a Sequence Ontology type
       for the default landmark sequences in the GFF fie (e.g. 'chromosome'). This is only needed if
       the landmark features (first column of the GFF3 file) are not already in the database."),
      '#autocomplete_route_name' => 'tripal_chado.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5, 'cv_id' => $sequence_cv_id],
    ];

    $form['proteins'] = [
      '#type' => 'fieldset',
      '#title' => t('Proteins'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $form['proteins']['skip_protein'] = [
      '#type' => 'checkbox',
      '#title' => t('Skip automatic protein creation'),
      '#required' => FALSE,
      '#description' => t('The GFF loader will automatically create a protein feature for each transcript in the GFF file if a protein feature is missing in the GFF file. Check this box to disable this functionality. Protein features that are specifically present in the GFF will always be created.'),
      '#empty_option' => t('- Select -'),
    ];

    $form['proteins']['re_mrna'] = [
      '#type' => 'textfield',
      '#title' => t('Optional. Regular expression for the mRNA name'),
      '#required' => FALSE,
      '#description' => t('If automatic protein creation is enabled, then by default the loader will give each protein a name based on the name of the corresponding mRNA followed by the "-protein" suffix.
       If you want to customize the name of the created protein, you can enter a regular expression that will extract portions of
       the mRNA unique name. For example, for a
       mRNA with a unique name finishing by -RX (e.g. SPECIES0000001-RA),
       the regular expression would be, "^(.*?)-R([A-Z]+)$". Elements surrounded by parentheses are captured as backreferences and can be used for replacement.' ),
    ];

    $form['proteins']['re_protein'] = [
      '#type' => 'textfield',
      '#title' => t('Optional. Replacement string for the protein name'),
      '#required' => FALSE,
      '#description' => t('If a regular expression is used to specify a protein name you can use the backreference tokens to extract the portion of the mRNA name that you want to use for a protein.
       You use a dollar sign followed by a number to indicate the backreferences. For example: "$1-P$2".'),
    ];

    $form['targets'] = [
      '#type' => 'fieldset',
      '#title' => t('Targets'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $form['targets']['adesc'] = [
      '#markup' => t("When alignments are represented in the GFF file (e.g. such as
       alignments of cDNA sequences to a whole genome, or blast matches), they are
       represented using the term 'match' or more specific match types: 'cDNA_match', 'EST_match', etc.
       These features may also have a 'Target' attribute to
       specify the sequence that is being aligned and the alignment coordinates on that sequence.
       However, the organism to which the aligned sequence belongs may not be present in the
       GFF file.  Here you can specify the organism and feature type of the target sequences.
       The options here will apply to all targets unless the organism and type are explicity
       set in the GFF file using the 'target_organism' and 'target_type' attributes, or for the
       type if a more specific type name is given (e.g. cDNA_match or EST_match)."),
    ];

    $form['targets']['target_organism_id'] = [
      '#title' => t('Target Organism'),
      '#type' => 'select',
      '#description' => t("Optional. Choose the organism to which target sequences belong.
        Select this only if target sequences belong to a different organism than the
        one specified above. And only choose an organism here if all of the target sequences
        belong to the same species.  If the targets in the GFF file belong to multiple
        different species then the organism must be specified using the 'target_organism=genus:species'
        attribute in the GFF file."),
      '#options' => $organisms,
      '#empty_option' => t('- Select -'),
    ];

    $form['targets']['target_type'] = [
      '#title' => t('Target Type'),
      '#type' => 'textfield',
      '#description' => t("Optional. If the unique name for a target sequence is not unique (e.g. a protein
       and an mRNA have the same name) then you must specify the type for all targets in the GFF file. If
       the targets are of different types then the type must be specified using the 'target_type=type' attribute
       in the GFF file. This must be a valid Sequence Ontology (SO) term. If the matches in the GFF3 file
       use specific match types (e.g. cDNA_match, EST_match, etc.) then this can be left blank. "),
      '#autocomplete_route_name' => 'tripal_chado.cvterm_autocomplete',
      '#autocomplete_route_parameters' => ['count' => 5, 'cv_id' => $sequence_cv_id],
    ];

    $form['targets']['create_target'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Target'),
      '#required' => FALSE,
      '#description' => t("If the target feature cannot be found, create one using the organism and type specified above, or
       using the 'target_organism' and 'target_type' fields specified in the GFF file.  Values specified in the
       GFF file take precedence over those specified above."),
    ];

    // Advanced Options
    $form['additional_options'] = [
      '#type' => 'fieldset',
      '#title' => t('Additional Options'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $form['additional_options']['create_organism'] = [
      '#type' => 'checkbox',
      '#title' => t('Create organism'),
      '#required' => FALSE,
      '#description' => t('The Tripal GFF loader supports the "organism" attribute. This allows features of a
       different organism to be aligned to the landmark sequence.  The format of the
       attribute is "organism=[genus]:[species]", where [genus] is the organism\'s genus and [species] is the
       species name. Check this box to automatically add the organism to the database if it does not already exists.
       Otherwise lines with an organism attribute where the organism is not present in the database will be skipped.'),
    ];

    $form['additional_options']['line_number'] = [
      '#type' => 'textfield',
      '#title' => t('Starting Line Number'),
      '#description' => t('Enter the line number in the GFF file where you would like to begin processing.  The
      first line is line number 1.  This option is useful for examining loading problems with large GFF files.'),
      '#size' => 10,
      '#default_value' => 1,
      '#required' => TRUE,
    ];

    $form['additional_options']['alt_id_attr'] = [
      '#title' => t('ID Attribute'),
      '#type' => 'textfield',
      '#description' => t("Optional. Sometimes lines in the GFF file are missing the
      required ID attribute that specifies the unique name of the feature, but there
      may be another attribute that can uniquely identify the feature.  If so,
      you may specify the name of the attribute to use for the name."),
    ];

    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function formValidate($form, &$form_state) {

    $form_state_values = $form_state->getValues();

    // These form inputs are not yet being validated:
    // $organism_id = $form_state_values['organism_id'];
    // $target_organism_id = $form_state_values['target_organism_id'];
    $target_type = trim($form_state_values['target_type']);
    // $create_target = $form_state_values['create_target'];
    // $create_organism = $form_state_values['create_organism'];
    // $refresh = 0; //$form_state['values']['refresh'];
    // $remove = 0; //$form_state['values']['remove'];
    $landmark_type = trim($form_state_values['landmark_type']);
    // $alt_id_attr = trim($form_state_values['alt_id_attr']);

    $line_number = trim($form_state_values['line_number']);

    $re_mrna = trim($form_state_values['re_mrna']);
    $re_protein = trim($form_state_values['re_protein']);

    // The parent class will validate that a file has been specified and is valid.

    if ($line_number and !is_numeric($line_number) or $line_number < 0) {
      $form_state->setErrorByName('line_number', t('Please provide an integer line number greater than zero'));
    }

    if (!($re_mrna and $re_protein) and ($re_mrna or $re_protein)) {
      $form_state->setErrorByName('re_mrna', t('You must provide both a regular expression for mRNA and a replacement string for protein'));
    }

    // check the regular expression to make sure it is valid
    set_error_handler(function () {}, E_WARNING);
    $result_re = preg_match("/" . $re_mrna . "/", "");
    $result = preg_replace("/" . $re_mrna . "/", $re_protein, "");
    restore_error_handler();
    if ($result_re === FALSE) {
      $form_state->setErrorByName('re_mrna', t('Invalid regular expression'));
    }
    elseif ($result === FALSE) {
      $form_state->setErrorByName('re_protein', t('Invalid replacement string'));
    }

    // check to make sure the types exists
    $cv_autocomplete = new ChadoCVTermAutocompleteController();
    $landmark_cvterm_id = $cv_autocomplete->getCVtermId($landmark_type, 'sequence');
    if (!$landmark_cvterm_id) {
      $form_state->setErrorByName('landmark_type', t('The Sequence Ontology (SO) term selected for the landmark type is not'
                                                   . ' available in the database. Please check the spelling or select another'));
    }
    if ($target_type) {
      $target_type_id = $cv_autocomplete->getCVtermId($target_type, 'sequence');
      if (!$target_type_id) {
        $form_state->setErrorByName('target_type', t('The Sequence Ontology (SO) term selected for the target type is not'
                                                   . ' available in the database. Please check the spelling or select another'));
      }
    }

  }

  /**
   * {@inheritDoc}
   */
  public function run() {
    $arguments = $this->arguments['run_args'];

    $chado = $this->getChadoConnection();

    $this->gff_file = $this->arguments['files'][0]['file_path'];

    // Set the private member variables of this class using the loader inputs.
    $this->organism_id = $arguments['organism_id'];
    $this->analysis_id = $arguments['analysis_id'];
    $this->add_only = $arguments['add_only'] ?? 0;
    $this->update = $arguments['update'] ?? 0;
    $this->target_organism_id = $arguments['target_organism_id'];
    $this->target_type = $arguments['target_type'];
    $this->create_target = $arguments['create_target'];
    $this->start_line = $arguments['line_number'];
    $this->default_landmark_type = $arguments['landmark_type'];
    $this->alt_id_attr = $arguments['alt_id_attr'];
    $this->create_organism = $arguments['create_organism'];
    $this->re_mrna = $arguments['re_mrna'];
    $this->re_protein = $arguments['re_protein'];
    $this->skip_protein = $arguments['skip_protein'];

    // Check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $this->gff_file;
    if (file_exists($dfile)) {
      $this->gff_file = $dfile;
    }
    // If the file is not local to Drupal check if it exists on the system.
    else if (!file_exists($this->gff_file)) {
      throw new \Exception(t("Cannot find the file: %file", ['%file' => $this->gff_file]));
    }

    // Open the GFF3 file.
    $this->logger->notice("Opening @gff_file", ['@gff_file' => $this->gff_file]);
    $this->gff_file_h = fopen($this->gff_file, 'r');
    if (!$this->gff_file_h) {
      throw new \Exception(t("Cannot open file: %file", ['%file' => $this->gff_file]));
    }

    // Get the feature property CV object
    $conditions = ['cv.name' => 'local'];
    $cv_record = $this->cvterm_buddy->getCv($conditions, []);
    if ($this->cvterm_buddy->countBuddies($cv_record) != 1) {
      throw new \Exception(t("Cannot find the 'local' ontology (feature property CV)"));
    }
    $this->feature_prop_cv = $cv_record;

    // Get the sequence CV object
    $conditions = ['cv.name' => 'sequence'];
    $cv_record = $this->cvterm_buddy->getCv($conditions, []);
    if ($this->cvterm_buddy->countBuddies($cv_record) != 1) {
      throw new \Exception(t("Cannot find the 'sequence' ontology (feature CV)"));
    }
    $this->feature_cv = $cv_record;

    // Get the organism object.
    $this->organism = $chado->select('1:organism','o')
    ->fields('o')
    ->condition('organism_id', $this->organism_id)
    ->execute()
    ->fetchObject();

    $num_found = $chado->select('1:organism','o')
    ->fields('o')
    ->condition('organism_id', $this->organism_id)
    ->countQuery()
    ->execute()
    ->fetchField();

    if ($num_found == 0) {
      throw new \Exception(t("Cannot find the specified organism for this GFF3 file."));
    }

    // Get the analysis object.
    $this->analysis = $chado->select('1:analysis','a')
    ->fields('a')
    ->condition('analysis_id', $this->analysis_id)
    ->execute()
    ->fetchObject();

    $num_found = $chado->select('1:analysis','a')
    ->fields('a')
    ->condition('analysis_id', $this->analysis_id)
    ->countQuery()
    ->execute()
    ->fetchField();

    if ($num_found == 0) {
      throw new \Exception(t("Cannot find the specified organism for this GFF3 file."));
    }

    // If a landmark type was provided then get the ID.
    if ($this->default_landmark_type) {
      $cv_autocomplete = new ChadoCVTermAutocompleteController();
      $this->landmark_cvterm_id = $cv_autocomplete->getCVtermId($this->default_landmark_type, 'sequence');
      if (!$this->landmark_cvterm_id) {
        throw new \Exception(t('Cannot find landmark feature type \'%landmark_type\'.',
          ['%landmark_type' => $this->default_landmark_type]));
      }
    }

    // If a target type is provided then get the ID.
    if ($this->target_type) {
      $cv_autocomplete = new ChadoCVTermAutocompleteController();
      $this->target_type_id = $cv_autocomplete->getCVtermId($this->target_type, 'sequence');
      if (!$this->target_type_id) {
        throw new \Exception(t("Cannot find the specified target type, %type.", ['%type' => $this->target_type]));
      }
    }

    // Create the cache file for storing parsed GFF entries.
    $this->openCacheFile();
    // Load the GFF3.
    try {

      $this->logger->notice("Step  1 of 27: Caching GFF3 file...                                ");
      $this->parseGFF3();

      // Prep the database for necessary records.
      $this->prepSynonyms();
      $this->prepNullPub();
      $this->prepDBs();

      $this->logger->notice("Step  2 of 27: Find existing landmarks...                          ");
      $this->findLandmarks();

      $this->logger->notice("Step  3 of 27: Insert new landmarks (if needed)...                 ");
      $this->insertLandmarks();

      if (!$this->skip_protein) {
        $this->logger->notice("Step  4 of 27: Find missing proteins...                            ");
        $this->findMissingProteins();

        $this->logger->notice("Step  5 of 27: Add missing proteins to list of features...         ");
        $this->addMissingProteins();
      }
      else {
        $this->logger->notice("Step  4 of 27: Find missing proteins (Skipped)                     ");
        $this->logger->notice("Step  5 of 27: Add missing proteins to list of features (Skipped)  ");
      }

      $this->logger->notice("Step  6 of 27: Find existing features...                           ");
      $this->findFeatures();

      $this->logger->notice("Step  7 of 27: Clear attributes of existing features...            ");
      $this->deleteFeatureData();

      $this->logger->notice("Step  8 of 27: Processing @num_features features...                ",
        ['@num_features' => number_format(count(array_keys($this->features)))]
      );

      $this->insertFeatures();
      $this->logger->notice("Step  9 of 27: Processing @num_features feature Names to update...               ",
        ['@num_features' =>  number_format(count(array_keys($this->update_names)))]
      );

      $this->updateFeatureNames();

      $this->logger->notice("Step 10 of 27: Get new feature IDs...                              ");
      $this->findFeatures();

      $this->logger->notice("Step 11 of 27: Insert locations...                                 ");
      $this->insertFeatureLocs();

      $this->logger->notice("Step 12 of 27: Associate parents and children...                   ");
      $this->associateChildren();

      $this->logger->notice("Step 13 of 27: Calculate child ranks...                            ");
      $this->calculateChildRanks();

      $this->logger->notice("Step 14 of 27: Add child-parent relationships...                   ");
      $this->insertFeatureParents();

      $this->logger->notice("Step 15 of 27: Insert properties...                                ");
      $this->insertFeatureProps();

      $this->logger->notice("Step 16 of 27: Find synonyms (aliases)...                          ");
      $this->findSynonyms();

      $this->logger->notice("Step 17 of 27: Insert new synonyms (aliases)...                    ");
      $this->insertSynonyms();

      $this->logger->notice("Step 18 of 27: Insert feature synonyms (aliases)...                ");
      $this->insertFeatureSynonyms();

      $this->logger->notice("Step 19 of 27: Find cross references...                            ");
      $this->findDbxrefs();

      $this->logger->notice("Step 20 of 27: Insert new cross references...                      ");
      $this->insertDbxrefs();

      $this->logger->notice("Step 21 of 27: Get new cross references IDs...                     ");
      $this->findDbxrefs();

      $this->logger->notice("Step 22 of 27: Insert feature cross references...                  ");
      $this->insertFeatureDbxrefs();

      $this->logger->notice("Step 23 of 27: Insert feature ontology terms...                    ");
      $this->insertFeatureCVterms();

      $this->logger->notice("Step 24 of 27: Insert 'derives_from' relationships...              ");
      $this->insertFeatureDerivesFrom();

      $this->logger->notice("Step 25 of 27: Insert Targets...                                   ");
      $this->insertFeatureTargets();

      $this->logger->notice("Step 26 of 27: Associate features with analysis...                 ");
      $this->insertFeatureAnalysis();

      if (!empty($this->residue_index)) {
        $this->logger->notice("Step 27 of 27: Adding sequences data...                            ");
        $this->insertFeatureSeqs();
      }
      else {
        $this->logger->notice("Step 27 of 27: Adding sequences data (Skipped: none available)     ");
      }
    }
    // On exception, catch the error, clean up the cache file and rethrow
    catch (\Exception $e) {
      $this->closeCacheFile();
      throw $e;
    }

  }

  /**
   * Load a controlled vocabulary term.
   *
   * This method first checks if the term has already been loaded in the
   * feature_cvterm_lookup array, which helps a lot with performance.
   *
   * @param $type
   * @param $cv_id
   *
   * @ingroup gff3_loader
   */
  private function getTypeID($type, $is_prop_type) {

    // Retrieve the appropriate cv buddy record for this property type
    $cv_buddy_record = $this->feature_cv;
    if ($is_prop_type) {
      $cv_buddy_record = $this->feature_prop_cv;
    }

    if ($is_prop_type) {
      if(array_key_exists(strtolower($type), $this->featureprop_cvterm_lookup)) {
        return $this->featureprop_cvterm_lookup[strtolower($type)];
      }
    }
    elseif (array_key_exists(strtolower($type), $this->feature_cvterm_lookup)) {
      return $this->feature_cvterm_lookup[strtolower($type)];
    }

    $conditions = [
      'cvterm.cv_id' => $cv_buddy_record->getValue('cv.cv_id'),
      'cvterm.name' => $type,
    ];
    // The GFF3 importer has had case insensitivity incorporated
    // since Tripal 3 for this query.
    $options = ['case_insensitive' => 'cvterm.name'];
    $cvterm_record = $this->cvterm_buddy->getCvterm($conditions, $options);
    if (!$cvterm_record) {
      $conditions = [
        'cvterm.cv_id' => $cv_buddy_record->getValue('cv.cv_id'),
        'cvtermsynonym.synonym' => $type,
      ];
      $options = ['case_insensitive' => 'cvtermsynonym.synonym'];
      $cvterm_record = $this->cvterm_buddy->getCvtermSynonym($conditions, $options);
    }

    if ($this->cvterm_buddy->countBuddies($cvterm_record) == 1) {
      $cvterm_id = $cvterm_record->getValue('cvterm.cvterm_id');
    }
    elseif ($this->cvterm_buddy->countBuddies($cvterm_record) > 1) {
      throw new \Exception(t('Error, more than one cvterm record matched %type', ['%type' => $type]));
    }
    else {
      // If the term couldn't be found and it's a property term, then
      // insert it as a local term.
      $this->logger->notice("Inserting the term \"@term\" in vocabulary \"@vocab\".",
                            ['@term' => $type, '@vocab' => $cv_buddy_record->getValue('cv.name')]);
      $values = [
        'db.name' => 'local',
        'dbxref.accession' => $type,
        'cv.name' => $cv_buddy_record->getValue('cv.name'),
        'cvterm.name' => $type,
        'cvterm.is_obsolete' => 0,
        'cvterm.is_relationshiptype' => 0,
      ];
      $options = ['create_dbxref' => TRUE];
      $cvterm_record = $this->cvterm_buddy->insertCvterm($values, $options);
      $cvterm_id = $cvterm_record->getValue('cvterm.cvterm_id');
    }

    // Cache the result for future use, under both the canonical name ($cvterm_name)
    // and also by synonym ($type). These will be the same if not a synonym.
    $cvterm_name = $cvterm_record->getValue('cvterm.name');
    if ($is_prop_type) {
      $this->featureprop_cvterm_lookup[strtolower($cvterm_name)] = $cvterm_id;
      $this->featureprop_cvterm_lookup[strtolower($type)] = $cvterm_id;
    }
    else {
      $this->feature_cvterm_lookup[strtolower($cvterm_name)] = $cvterm_id;
      $this->feature_cvterm_lookup[strtolower($type)] = $cvterm_id;
    }
    return $cvterm_id;
  }

  /**
   * Makes sure Chado is ready with the necessary synonym type records.
   */
  private function prepSynonyms() {

    // make sure we have a 'synonym_type' vocabulary
    $values = [
      'cv.name' => 'synonym_type',
    ];
    $synonym_type_record = $this->cvterm_buddy->upsertCv($values, []);
    if ($this->cvterm_buddy->countBuddies($synonym_type_record) != 1) {
      $this->logger->error('Failed to add the "synonym_type" vocabulary.');
      return 0;
    }
    $syncv = $synonym_type_record->getValue('cv.cv_id');

    // get or insert the 'exact' cvterm, which is the type of synonym we're adding
    $values = [
      'db.name' => 'synonym_type',
      'dbxref.accession' => 'exact',
      'cv.name' => 'synonym_type',
      'cvterm.name' => 'exact',
      'cvterm.is_obsolete' => 0,
      'cvterm.is_relationshiptype' => 0,
    ];
    $exact_term_record = $this->cvterm_buddy->upsertCvterm($values, []);
    if ($this->cvterm_buddy->countBuddies($exact_term_record) != 1) {
      $this->logger->error('Failed to add the "synonym_type:exact" term.');
      return 0;
    }
    $this->exact_syn_id = $exact_term_record->getValue('cvterm.cvterm_id');
  }

  /**
   * Makes sure there is a null publication in the database.
   */
  private function prepNullPub(){
    $chado = $this->getChadoConnection();
    // Check to see if we have a NULL publication in the pub table.  If not,
    // then add one.
    $select = ['uniquename' => 'null'];
    $result_query = $chado->select('1:pub', 'pub')
      ->fields('pub')
      ->condition('uniquename', 'null');
    $result_count = $result_query->countQuery()->execute()->fetchField();

    if ($result_count == 0) {
      $this->logger->notice("Inserting the null publication.");
      $pub_sql = "
        INSERT INTO {1:pub} (uniquename,type_id)
        VALUES (:uname,
          (SELECT cvterm_id
           FROM {1:cvterm} CVT
             INNER JOIN {1:dbxref} DBX ON DBX.dbxref_id = CVT.dbxref_id
             INNER JOIN {1:db} DB      ON DB.db_id      = DBX.db_id
           WHERE CVT.name = :type_id))
      ";
      $status = $chado->query($pub_sql, [
        ':uname' => 'null',
        ':type_id' => 'null',
      ]);
      if (!$status) {
        $this->logger->error("Cannot add null publication needed for setup of alias.");
        return 0;
      }

      $result_query = $chado->select('1:pub','pub')
        ->fields('pub')
        ->condition('uniquename','null');
      $result = $result_query->execute()->fetchObject();
      $pub = $result;
    }
    else {
      $result = $result_query->execute()->fetchObject();
      $pub = $result;
    }
    $this->null_pub = $pub;
  }

  /**
   * Makes sure Chado is ready with the necessary DB records.
   */
  private function prepDBs() {
    // Get the list of database records that are needed by this GFF file. If
    // they do not exist, then add them.
    foreach (array_keys($this->db_lookup) as $dbname) {
      // First check for the fully qualified URI (e.g. DB:<dbname>. If that
      // can't be found, then look for the name as is. If it still can't be
      // found, then create the database.
      $db_record = $this->dbxref_buddy->getDb(['db.name' => 'DB:' . $dbname], []);
      if (!$db_record) {
        $db_record = $this->dbxref_buddy->getDb(['db.name' => $dbname], []);
      }
      if (!$db_record) {
        $this->logger->notice("Inserting the database \"@dbname\".",
                              ['@dbname' => $dbname]);
        $values = [
          'db.name' => $dbname,
          'db.description' => 'Added automatically by the Tripal GFF loader.',
        ];
        $db_record = $this->dbxref_buddy->insertDb($values, []);
      }
      if ($this->dbxref_buddy->countBuddies($db_record) != 1) {
        $this->logger->error('Cannot find or add the database "@dbname".',
          ['@dbname' => $dbname]
        );
        return 0;
      }
      $db_id = $db_record->getValue('db.db_id');
      $this->db_lookup[$dbname] = $db_id;
    }
  }

  /**
   * Parses the current line of the GFF3 file for a feature.
   *
   * @return array
   *  An associative array containing the 9 elements othe GFF3 file. The
   *  9th element is an associative array of the attributes.
   */
  private function parseGFF3Line($line) {

    // get the columns
    $cols = explode("\t", $line);
    if (sizeof($cols) != 9) {
      throw new \Exception(t('Improper number of columns on line %line_num: %line',
        ['%line_num' => $this->current_line, '%line' => $line]));
    }

    $ret = [
      'line' => $this->current_line,
      'landmark' => $cols[0],
      'source' => $cols[1],
      'type' => strtolower($cols[2]),
      'start' => $cols[3],
      'stop' => $cols[4],
      'score' => $cols[5],
      'strand' => $cols[6],
      'phase' => $cols[7],
      'attrs' => [],
    ];

    // Ready the start and stop for chado.  Chado expects these positions
    // to be zero-based, so we substract 1 from the fmin. Also, in case
    // they are backwards, put them in the right order.
    $fmin = $ret['start'] - 1;
    $fmax = $ret['stop'];
    if ($ret['stop'] < $ret['start']) {
      $fmin = $ret['stop'] - 1;
      $fmax = $ret['start'];
    }
    $ret['start'] = $fmin;
    $ret['stop'] = $fmax;

    // Landmark (seqid) validation checks based on GFF3 specifications
    $matches = [];
    preg_match('/[a-zA-Z0-9\.:\^\*\$@!\+_\?\-\|]*/', $ret['landmark'], $matches);
    if ($matches[0] != $ret['landmark']) {
      throw new \Exception(t("Landmark/seqid !landmark contains invalid
        characters. Only characters included in this regular expression are
        allowed [a-zA-Z0-9.:^*$@!+_?-|]",
        ['!landmark' => $ret['landmark']]));
    }

    // Check to make sure strand has a valid character
    if (preg_match('/[\+-\?\.]/',$ret['strand']) == false) {
      throw new \Exception(t('Invalid strand detected on line %line,
        strand can only be +-?.', ['%line' => $line]));
    }

    // Format the strand for chado
    if (strcmp($ret['strand'], '.') == 0) {
      $ret['strand'] = 0;
    }
    elseif (strcmp($ret['strand'], '?') == 0) {
      $ret['strand'] = 0;
    }
    elseif (strcmp($ret['strand'], '+') == 0) {
      $ret['strand'] = 1;
    }
    elseif (strcmp($ret['strand'], '-') == 0) {
      $ret['strand'] = -1;
    }


    if (preg_match('/[012\.]/',$ret['phase']) == false) {
      throw new \Exception(t('Invalid phase detected on line %line,
        phase can only be 0,1,2 or . (period)', ['%line' => $line]));
    }


    if (strcmp($ret['phase'], '.') == 0) {
      if ($ret['type'] == 'cds') {
        $ret['phase'] = '0';
      }
      else {
        $ret['phase'] = '';
      }
    }

    $tags = [];
    $attr_name = '';
    $attr_uniquename = '';
    $attrs = explode(";", $cols[8]);
    $attr_organism = $this->organism_id;
    $attr_parent = '';
    $attr_others = [];
    $attr_aliases = [];
    $attr_dbxref = [];
    $attr_derives = [];
    $attr_terms = [];
    $attr_target = [];
    foreach ($attrs as $attr) {
      $attr = rtrim($attr);
      $attr = ltrim($attr);
      if (strcmp($attr, '') == 0) {
        continue;
      }
      if (!preg_match('/^[^\=]+\=.+$/', $attr)) {
        throw new \Exception(t('Attribute is not correctly formatted on line %line_num: %attr',
          ['%line_num' => $this->current_line, '%attr' => $attr]));
      }

      // Break apart each attribute into key/value pairs.
      $tag = preg_split("/=/", $attr, 2);

      // Multiple values of an attribute are separated by commas
      $tag_name = $tag[0];
      if (!array_key_exists($tag_name, $tags)) {
        $tags[$tag_name] = [];
      }
      $tags[$tag_name] = array_merge($tags[$tag_name], explode(",", $tag[1]));

      // Replace the URL escape codes for each tag
      for ($i = 0; $i < count($tags[$tag_name]); $i++) {
        $tags[$tag_name][$i] = urldecode($tags[$tag_name][$i]);
      }

      if (strcmp($tag_name, 'Alias') == 0) {
        $attr_aliases = array_merge($attr_aliases, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Parent') == 0) {
        $attr_parent = $tag[1];
      }
      elseif (strcmp($tag_name, 'Dbxref') == 0) {
        $attr_dbxref = array_merge($attr_dbxref, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Derives_from') == 0) {
        $attr_derives = array_merge($attr_derives, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Ontology_term') == 0) {
        $attr_terms = array_merge($attr_terms, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'organism') == 0) {
        if (count($tags[$tag_name]) > 1) {
          throw new \Exception(t('Each feature can only have one "organism" attribute.
            The feature %uniquename has more than one: %organism',
            ['%uniquename' => $ret['uniquename'], '%organism' => $ret['organism']]));
        }
        $attr_organism = $this->findOrganism($tags[$tag_name][0], $this->current_line);
      }
      elseif (strcmp($tag_name, 'Target') == 0) {
        if (count($tags[$tag_name]) > 1) {
          throw new \Exception(t('Each feature can only have one "Target" attribute.
            The feature %uniquename has more than one.',
            ['%uniquename' => $ret['uniquename']]));
        }
        // Get the elements of the target.
        $matches = [];
        if (preg_match('/^(.*?)\s+(\d+)\s+(\d+)(\s+[\+|\-])*$/', trim($tags[$tag_name][0]), $matches)) {
          $attr_target['name'] = $matches[1];
          $attr_target['start'] = $matches[2];
          $attr_target['stop'] = $matches[3];
          $tfmin = $attr_target['start'] - 1;
          $tfmax = $attr_target['stop'];
          if ($attr_target['stop'] < $attr_target['start']) {
            $tfmin = $attr_target['stop'] - 1;
            $tfmax = $attr_target['start'];
          }
          $attr_target['start'] = $tfmin;
          $attr_target['stop'] = $tfmax;

          $attr_target['phase'] = '';
          $attr_target['strand'] = 0;
          if (!empty($matches[4])) {
            if (preg_match('/^\+$/', trim($matches[4]))) {
              $attr_target['strand'] = 1;
            }
            elseif (preg_match('/^\-$/', trim($matches[4]))) {
              $attr_target['strand'] = -1;
            }
          }
          $attr_target['organism_id'] = $this->target_organism_id ? $this->target_organism_id : $this->organism_id;
          $attr_target['type_id'] = $this->target_type_id ? $this->target_type_id : NULL;
          $attr_target['type'] = $this->target_type ? $this->target_type : NULL;

          // If this Target aligns to a feature where the match type is specified
          // (e.g. cDNA_match, EST_match, etc.) then we can pull the type for
          // the target feature from the feature type.
          if (preg_match('/(.+)_match/', $ret['type'], $matches)) {
            $attr_target['type'] = $matches[1];
            $attr_target['type_id'] = $this->getTypeID($matches[1], FALSE);
          }
        }
        else {
          throw new \Exception(t('The "Target" attribute "%attribute" is incorrectly formatted for the
            feature "%feature"', ['%attribute' => $tags[$tag_name][0], '%feature' => $ret['uniquename']]));
        }
      }
      elseif (strcmp($tag_name, 'target_organism') == 0) {
        $attr_target['organism_id'] = $this->findOrganism($tags[$tag_name][0], $this->current_line);
      }
      elseif (strcmp($tag_name, 'target_type') == 0) {
        $attr_target['type'] = $tags[$tag_name][0];
        $attr_target['type_id'] = $this->getTypeID($tags[$tag_name][0], FALSE);
      }
      // Get the list of non-reserved attributes these will get added
      // as properties to the featureprop table.  The 'Note', 'Gap', 'Is_Circular',
      // attributes will go in as a property so those are not in the list
      // checked below.
      elseif (strcmp($tag_name, 'Name') !=0 and strcmp($tag_name, 'ID') !=0 and
              strcmp($tag_name, 'Alias') != 0 and strcmp($tag_name, 'Parent') != 0 and
              strcmp($tag_name, 'Target') != 0 and strcmp($tag_name, 'Derives_from') != 0 and
              strcmp($tag_name, 'Dbxref') != 0 and strcmp($tag_name, 'Ontology_term') != 0 and
              strcmp($tag_name, 'target_organism') != 0 and strcmp($tag_name, 'target_type') != 0 and
              strcmp($tag_name, 'organism' != 0)) {
        foreach ($tags[$tag_name] as $value) {
          if (!array_key_exists($tag_name, $attr_others)) {
            $attr_others[$tag_name] = [];
          }
          $attr_others[$tag_name][] = $value;
        }
      }
    }

    // A feature may get ignored. But let's default this to FALSE.
    $ret['skipped'] = FALSE;

    // A line may have more than one feature (e.g. match, EST_match, etc).
    // This flag, when TRUE, tells the parseGFF3 function to repeat this line.
    $ret['repeat'] = FALSE;

    // If neither name nor uniquename are provided then generate one.
    $names = $this->getFeatureNames($tags, $ret['type'], $ret['landmark'], $ret['start'], $ret['stop']);
    $attr_uniquename = $names['uniquename'];
    $attr_name = $names['name'];

    // If this is a match feature (match, EST_match, cDNA_match, etc), then
    // we need to handle this line specially.
    if (preg_match('/match$/i', $ret['type'])) {

      // If the feature already exists that means we need to add a match_part
      // feature.  If not, then we will add a flag to the results to tell
      // the parseGFF3 function to repeat this line, as it has two features:
      // the match and the match_part.  All other match feature with the same
      // ID in the GFF3 will just be match_part features.
      $parent_check = preg_replace('/_part_\d+/', '', $attr_uniquename);
      if (array_key_exists($parent_check, $this->features)) {
         // Set the match_part parent
         // remove the "_part_X" suffix added by the getFeatureNames to find
         // the parent.
         $attr_parent = $parent_check;
         $ret['type'] = 'match_part';
      }
      else {
        // Unset all attributes as these belong on the match_part
        $attr_dbxref = [];
        $attr_aliases = [];
        $attr_terms = [];
        $attr_derives = [];
        $attr_others = [];
        $ret['repeat'] = TRUE;
      }
    }

    $ret['name'] = $attr_name;
    $ret['uniquename'] = $attr_uniquename;
    $ret['synonyms'] = $attr_aliases;

    // Add in the dbxref record.
    $ret['dbxrefs'] = [];
    foreach ($attr_dbxref as $key => $dbx) {
      $parts = explode(':', $dbx, 2);
      $ret['dbxrefs']["{$parts[0]}:{$parts[1]}"] = array(
        'db' => $parts[0],
        'accession' => $parts[1],
      );
    }

    // Add in the GFF source dbxref. This is needed for GBrowse.
    $ret['dbxrefs']["GFF_source:{$ret['source']}"] = array(
      'db' => 'GFF_source',
      'accession' => $ret['source'],
    );

    // Add in the ontology terms
    $ret['terms'] = [];
    foreach ($attr_terms as $key => $dbx) {
      $parts = explode(':', $dbx, 2);
      $ret['terms']["{$parts[0]}:{$parts[1]}"] = array(
        'db' => $parts[0],
        'accession' => $parts[1],
      );
    }

    // Add the derives from entry.
    $ret['derives_from'] = '';
    if (count($attr_derives) == 1) {
      $ret['derives_from'] = $attr_derives[0];
    }
    if (count($attr_derives) > 1) {
      throw new \Exception(t('Each feature can only have one "Derives_from" attribute.
        The feature %uniquename has more than one: %derives',
        [
          '%uniquename' => $ret['uniquename'],
          '%derives' => $ret['derives_from'],
        ]));
    }

    // Now add all of the attributes into the return array.
    foreach ($tags as $key => $value) {
      $ret['attrs'][$key] = $value;
    }

    // Add the organism entry, but if we don't have one for this feature
    // (in the case where the target_organism attribute doesn't match
    // an organism in the databse) then skip this feature.
    $ret['organism'] = $attr_organism;
    if (!$ret['organism']) {
      $ret['skipped'] = TRUE;
    }

    // Add the target. If the type_id is missing then remove the target
    // and we'll skip it.
    $ret['target'] = $attr_target;
    if (!array_key_exists('type', $ret['target']) or empty($ret['target'])) {
      $ret['target'] = [];
    }

    // Make sure we only have one Gap if it exists
    if (array_key_exists('Gap', $attr_others) and count($attr_others['Gap']) > 1) {
      throw new \Exception(t('Each feature can only have one "Gap" attribute.
        The feature %uniquename has more than one.',
        ['%uniquename' => $ret['uniquename']]));
    }

    // Add the properties and parent.
    $ret['properties'] = $attr_others;
    $ret['parent'] = $attr_parent;
    return $ret;
  }

  /**
   * Indexes the FASTA section of the file for quick lookup.
   */
  private function indexFASTA() {

    // Iterate through the remaining lines of the file
    while ($line = fgets($this->gff_file_h)) {

      $this->current_line++;
      $this->addItemsHandled(mb_strlen($line));

      // Get the ID and the current file pointer and store that for later.
      if (preg_match('/^>/', $line)) {
        $id = preg_replace('/^>([^\s]+).*$/', '\1', $line);
        $this->residue_index[trim($id)] = ftell($this->gff_file_h);
      }
    }
  }

  /**
   * Loads the actual residue information from the FASTA section of the file.
   */
  private function insertFeatureSeqs() {

    $num_residues = count(array_keys($this->residue_index));

    $this->setItemsHandled(0);
    $this->setTotalItems($num_residues);

    $count = 0;

    foreach ($this->residue_index as $uniquename => $offset) {

      // Skip this sequence if we can't match the name with a known feature
      // or landmark name.
      if (!(array_key_exists($uniquename, $this->features) and $this->features[$uniquename]) and
          !(array_key_exists($uniquename, $this->landmarks) and $this->landmarks[$uniquename])) {
        $this->logger->warning('Assigning Sequence: cannot find a feature with a unique name of: "@uname".'
                             . ' Please ensure the sequence names in the ##FASTA section use the same name'
                             . ' as the ID in the feature in the GFF file.',
          ['@uname' => $uniquename]
        );
        $count++;
        continue;
      }

      // Get the feature that this sequence belongs.
      $feature_id = NULL;
      if (array_key_exists($uniquename, $this->features)) {
        $findex = $this->features[$uniquename]['findex'];
        $feature = $this->getCachedFeature($findex);
        $feature_id = $feature['feature_id'] ?? $this->features[$uniquename]['feature_id'];
      }
      else {
        $feature_id = $this->landmarks[$uniquename];
      }


      // Seek to the position in the GFF file where this sequence is housed.
      // Iterate through the lines and get and then store the value.
      $residues = [];
      fseek($this->gff_file_h, $offset);
      while ($line = fgets($this->gff_file_h)) {
        if (preg_match('/^>/', $line)) {
          break;
        }
        $residues[] = trim($line);
      }
      $residues = implode('', $residues);

      $values = [
        'residues' => $residues,
        'seqlen' => strlen($residues),
        'md5checksum' => md5($residues),
      ];
      chado_update_record('feature', ['feature_id' => $feature_id], $values, NULL, $this->chado_schema_main);
      $count++;
      $this->setItemsHandled($count);
    }
  }

  /**
   * Retrieves a ChadoRecord object for the landmark feature.
   *
   * @param string $landmark_name
   *   The name of the landmark to get
   *
   * @return object
   *   A feature ChadoRecord object or NULL if the landmark is missing and
   *   $skip_on_missing is TRUE.
   */
  private function findLandmark($landmark_name) {
    $chado = $this->getChadoConnection();
    $landmark_type = $this->default_landmark_type;

    // Before performing a database query check to see if
    // this landmark is already in our lookup list.
    if (array_key_exists($landmark_name, $this->landmarks)) {
      return $this->landmarks[$landmark_name];
    }

    $landmark_select = $chado->select('1:feature')
      ->fields('feature')
      ->condition('organism_id', $this->organism_id)
      ->condition('uniquename', $landmark_name);

    if($landmark_type) {
      $landmark_select->condition('type_id', $this->landmark_cvterm_id);
    }

    // Make sure we only match on one landmark.
    $num_found = $landmark_select->countQuery()->execute()->fetchField();
    if ($num_found == 0) {
      return NULL;
    }
    if ($num_found > 1) {
      throw new \Exception(t("The landmark '%landmark' has more than one entry for
      this organism (%species). Did you provide a landmark type? If not, try resubmitting and providing a type.",
        [
          '%landmark' => $landmark_name,
          '%species' => chado_get_organism_scientific_name($this->organism),
        ]));
    }

    // The landmark was found, remember it
    $landmark = $landmark_select->execute()->fetchObject();
    $this->landmarks[$landmark_name] = $landmark->feature_id;

    return $landmark;
  }
  /**
   * Loads into the database any landmark sequences.
   *
   * @param string $line
   *   The line from the GFF file that is the ##sequence-region comment.
   */
  private function insertHeaderLandmark($line) {
    $region_matches = [];
    if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $region_matches)) {
      $rid = $region_matches[1];
      $landmark = $this->findLandmark($rid);
      if (!$landmark) {
        if (!$this->default_landmark_type) {
          throw new \Exception(t('The landmark, %landmark, cannot be added because no landmark ' .
              'type was provided. Please redo the importer job and specify a landmark type.',
              ['%landmark' => $rid]));
        }
        $this->insertLandmark($rid);
      }
    }
  }

  /**
   * Loads a single landmark by name.
   */
  private function insertLandmark($name) {
    $chado = $this->getChadoConnection();
    $residues = '';
    $insert_id = $chado->insert('1:feature')
    ->fields([
      'organism_id' => $this->organism->organism_id,
      'uniquename' => $name,
      'name' => $name,
      // ORIGINAL CODE FROM STEPHEN
      // 'type_id' => $this->landmark_cvterm_id,
      'type_id' => $this->getLandmarkTypeID($name),
      'md5checksum' => md5($residues),
      'is_analysis' => 0,
      'is_obsolete' => 0,
    ])
    ->execute();
    $this->landmarks[$name] = $insert_id;
  }

  /**
   *
   */
  private function parseGFF3() {
    $filesize = filesize($this->gff_file);
    $this->setTotalItems($filesize);

    // Holds a unique list of cvterms for later lookup.
    $feature_cvterms = [];
    $featureprop_cvterms = [];

    while ($line = fgets($this->gff_file_h)) {
      $this->current_line++;
      $this->addItemsHandled(mb_strlen($line));

      $line = trim($line);

      if ($this->current_line < $this->start_line) {
        continue;
      }

      // If we're in the FASTA file we're at the end of the features so return.
      if (preg_match('/^##FASTA/i', $line)) {
        $this->indexFASTA();
        continue;
      }

      // if at the ##sequence-region line handle it.
      $matches = [];
      if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $matches)) {
        $this->seq_region_headers[$matches[1]] = $line;
        continue;
      }

      // skip comments
      if (preg_match('/^#/', $line)) {
        continue;
      }

      // skip empty lines
      if (preg_match('/^\s*$/', $line)) {
        continue;
      }

      // Parse this feature from this line of the GFF3 file.
      $gff_feature = $this->parseGFF3Line($line);
      $this->prepareFeature($gff_feature, $feature_cvterms, $featureprop_cvterms);

      // If there is a second feature (in the case of a match) then
      // repeat this line (to get the match_part).
      if ($gff_feature['repeat'] === TRUE) {
        $gff_feature = $this->parseGFF3Line($line);
        $this->prepareFeature($gff_feature, $feature_cvterms, $featureprop_cvterms);
      }
    }

    // Make sure we have the protein term in our list.
    if (!array_key_exists('protein', $feature_cvterms) and
        !array_key_exists('polypeptide', $feature_cvterms)) {
      $feature_cvterms['polypeptide'] = 0;
    }

    // Iterate through the feature type terms and get a chado object for each.
    foreach (array_keys($feature_cvterms) as $name) {
      $this->getTypeID($name, FALSE);
    }

    // Iterate through the featureprop type terms and get a cvterm_id for
    // each. If it doesn't exist then add a new record.
    foreach (array_keys($featureprop_cvterms) as $name) {
      $this->getTypeID($name, TRUE);
    }
  }

  /**
   * Prepare the database prior to working with the feature.
   */
  private function prepareFeature($gff_feature, &$feature_cvterms, &$featureprop_cvterms) {
    // Add the landmark if it doesn't exist in the landmark list.
    if (!array_key_exists($gff_feature['landmark'], $this->landmarks)) {
      $this->landmarks[$gff_feature['landmark']] = FALSE;

      // Check whether landmark_id matches the feature type
      // Keep track of the landmark types using the class variable
      // landmark_types
      $landmark_id = $gff_feature['attrs']['ID'][0];
      if ($landmark_id == $gff_feature['landmark']) {
        $this->landmark_types[$landmark_id] = $gff_feature['type'];
      }
      else {
        $this->landmark_types[$landmark_id] = $this->default_landmark_type;
      }

    }

    // Organize DBs and DBXrefs for faster access later on.
    foreach ($gff_feature['dbxrefs'] as $index => $info) {
      if (!array_key_exists($info['db'], $this->db_lookup)) {
        $this->db_lookup[$info['db']] = FALSE;
      }
      if (!array_key_exists($index, $this->dbxref_lookup)) {
        $this->dbxref_lookup[$index] = $info;
      }
    }

    // We want to make sure the Ontology_term attribute dbxrefs are
    // also easily looked up... but we do not want to create them
    // if they do not exist. the presence of the 'cvterm' key will
    // tell the loadDbxrefs() function to not create the term.
    foreach ($gff_feature['terms'] as $index => $info) {
      if (!array_key_exists($info['db'], $this->db_lookup)) {
        $this->db_lookup[$info['db']] = FALSE;
      }

      if (!array_key_exists($index, $this->dbxref_lookup)) {
        $this->dbxref_lookup[$index] = $info;
        $this->dbxref_lookup[$index]['cvterm_id'] = NULL;
      }
    }

    // Organize the CVterms for faster access later on.
    if (!array_key_exists($gff_feature['type'], $feature_cvterms)) {
      $feature_cvterms[$gff_feature['type']] = 0;
    }
    $feature_cvterms[$gff_feature['type']]++;

    // Add any target feature types to the list as well.
    if (array_key_exists('name', $gff_feature['target'])) {
      if (!array_key_exists($gff_feature['target']['type'], $feature_cvterms)) {
        $feature_cvterms[$gff_feature['target']['type']] = 0;
      }
      $feature_cvterms[$gff_feature['target']['type']]++;
    }

    // Organize the feature property types for faster access later on.
    foreach ($gff_feature['properties'] as $prop_name => $value) {
      if (!array_key_exists($prop_name, $featureprop_cvterms)) {
        $featureprop_cvterms[$prop_name] = NULL;
      }
      $featureprop_cvterms[$prop_name]++;
    }

    // Cache the GFF feature details for later lookup.
    $this->cacheFeature($gff_feature);

    // If this feature has a target then we need to add the target as
    // new feature for insertion.
    if (array_key_exists('name', $gff_feature['target'])) {
      $this->addTargetFeature($gff_feature);
    }
  }

  /**
   *
   */
  private function findMissingProteins() {
    $this->setItemsHandled(0);
    $this->setTotalItems(count(array_keys($this->features)));

    // Don't do anything if the user wants to skip creation of non listed
    // proteins. Proteins that have actual lines in the GFF will still be
    // created.
    if ($this->skip_protein) {
      $this->logger->notice('  Skipping creation of non-specified proteins...            ');
      return;
    }

    // First, store records for which proteins need to exist. These
    // will be for any parent that has a 'CDS' or 'protein' child.
    $i = 0;
    foreach ($this->features as $info) {
      $i++;
      $this->setItemsHandled($i);
      $findex = $info['findex'];
      $feature = $this->getCachedFeature($findex);
      $type = $feature['type'];
      if ($type == 'cds' or $type == 'protein' or $type == 'polypeptide') {
        if (isset($feature['parent'])) {
          foreach (explode(',', $feature['parent']) as $parent_name) {
            if (!array_key_exists($parent_name, $this->proteins)) {
              $this->proteins[$parent_name] = [];
            }
            if ($type == 'cds') {
              $this->proteins[$parent_name]['cds'][] = $findex;
            }
            if ($type == 'protein' or $type == 'polypeptide') {
              $this->proteins[$parent_name]['protein'] = $findex;
            }
          }
        }
      }
    }
  }

  /**
   * Checks the features and finds those that need proteins added.
   */
  private function addMissingProteins() {
    $this->setItemsHandled(0);
    $this->setTotalItems(count(array_keys($this->proteins)));

    // Second, iterate through the protein list and for any parents that
    // don't already have a protein we need to create one.
    $i = 0;
    foreach ($this->proteins as $parent_name => $info) {
      $i++;
      $this->setItemsHandled($i);

      // Skip addition of any proteins that are already in the GFF file.
      if (array_key_exists('protein', $info)) {
        continue;
      }

      // If we don't have a protein
      if (array_key_exists('cds', $info)) {
        $start = INF;
        $stop = -INF;
        $start_phase = 0;
        $stop_phase = 0;
        // Find the starting and end CDS.
        foreach ($info['cds'] as $findex) {
          $cds = $this->getCachedFeature($findex);
          if ($cds['start'] < $start) {
            $start = $cds['start'];
            $start_phase = $cds['phase'];
          }
          if ($cds['stop'] > $stop) {
            $stop = $cds['stop'];
            $stop_phase = $cds['phase'];
          }
        }

        // Set the start of the protein to be the start of the coding
        // sequence minus the phase.
        if ($cds['strand'] == '-1') {
          $stop -= $stop_phase;
        }
        else {
          $start += $start_phase;
        }

        // Get the name for the protein
        $name = $parent_name;
        $uname = $parent_name . '-protein';
        // If regexes are provided then use those to create the protein name.
        if ($this->re_mrna and $this->re_protein) {
          $uname = preg_replace("/" . $this->re_mrna . "/", $this->re_protein, $parent_name);
        }

        $cds_organism_id = array_key_exists('organism_id', $cds)?$cds['organism_id']:NULL;

        // Now create the protein feature.
        $feature = [
          'line' => $cds['line'],
          'landmark' => $cds['landmark'],
          'source' => $cds['source'],
          'type' => 'polypeptide',
          'start' => $start,
          'stop' => $stop,
          'strand' => $cds['strand'],
          'phase' => '',
          'attr' => [],
          'skipped' => FALSE,
          'name' => $name,
          'uniquename' => $uname,
          'synonyms' => [],
          'dbxrefs' => [],
          'terms' => [],
          'derives_from' => NULL,
          'organism' => $cds_organism_id,
          'target' => [],
          'properties' => [],
          'parent' => $cds['parent'],
        ];
        $this->cacheFeature($feature);
      }
    }
  }

  /**
   * Adds a new target feature to the feature list.
   *
   * @param array $gff_feature
   *   The feature array created by the parseFeature function.
   */
  private function addTargetFeature($gff_feature) {
    if (!array_key_exists($gff_feature['target']['name'], $this->features)) {
      $feature = [
        'is_target' => TRUE,
        'line' => $this->current_line,
        'landmark' => NULL,
        'source' => $gff_feature['source'],
        'type' => $gff_feature['target']['type'],
        'start' => NULL,
        'stop' => NULL,
        'strand' => NULL,
        'phase' => NULL,
        'attr' => [],
        'skipped' => FALSE,
        'name' => $gff_feature['target']['name'],
        'uniquename' => $gff_feature['target']['name'],
        'synonyms' => [],
        'dbxrefs' => [],
        'terms' => [],
        'derives_from' => NULL,
        'organism' => $gff_feature['target']['organism_id'],
        'target' => [],
        'properties' => [],
        'parent' => '',
      ];
      $this->cacheFeature($feature);
    }
  }

  /**
   * Opens the cache file for read/write access.
   */
  private function openCacheFile() {
    $temp_file = \Drupal::service('file_system')->tempnam('temporary://', "TripalGFF3Import_");
    $this->gff_cache_file_name = \Drupal::service('file_system')->realpath($temp_file);

    $this->logger->notice("Opening temporary cache file: @cfile",
      ['@cfile' => $this->gff_cache_file_name]
    );
    $this->gff_cache_file = fopen($this->gff_cache_file_name, "r+");
  }

  /**
   * Closes and cleans up the cache file.
   */
  private function closeCacheFile() {
    fclose($this->gff_cache_file);
    $this->logger->notice("Removing temporary cache file: @cfile",
      ['@cfile' => $this->gff_cache_file_name]
    );
    unlink($this->gff_cache_file_name);
  }

  /**
   * Caches the processed feature from a GFF3 file
   */
  private function cacheFeature($gff_feature) {
    // Make sure we're at the end of the file.
    fseek($this->gff_cache_file, 0, SEEK_END);

    // Get the index of this location
    $findex = ftell($this->gff_cache_file);

    // Write the serialied array for this feature to the cache file
    // and save the index into the member variable.
    fwrite($this->gff_cache_file, serialize($gff_feature) . "\n");
    $this->features[$gff_feature['uniquename']]['findex'] = $findex;
    $this->features[$gff_feature['uniquename']]['feature_id'] = NULL;
  }

  /**
   * Retrieves a feature using its index from the cache file.
   */
  private function getCachedFeature($findex) {
    $retval = fseek($this->gff_cache_file, $findex);
    if ($retval == -1) {
      throw new \Exception(t('Cannot seek to file location, %findex, in cache file %file.',
        ['%findex' => $findex, '%file' -> $this->gff_cache_file]));
    }
    $feature = fgets($this->gff_cache_file);
    $feature = rtrim($feature, "\n");
    $feature = unserialize($feature);
    return $feature;
  }

  /**
   * Imports the landmark features into Chado.
   */
  private function insertLandmarks() {
    foreach ($this->landmarks as $uniquename => $feature_id) {
      // If the landmark does not have an entry in the GFF lines, try to
      // find or add it.
      if ($feature_id === FALSE) {
        // First see if there is a definition in the headers region.
        if (array_key_exists($uniquename, $this->seq_region_headers)) {
          $this->insertHeaderLandmark($this->seq_region_headers[$uniquename]);
        }
        // Second, if a landmark_type is provided from the GFF file
        else if (array_key_exists($uniquename, $this->landmark_types)) {
          $this->insertLandmark($uniquename);
        }
        // Third, if a landmark_type is provided then just add the landmark feature.
        else if ($this->default_landmark_type) {
          $this->insertLandmark($uniquename);
        }
        else {
          throw new \Exception(t('The landmark (reference) sequence, %landmark,
            is not in the database and not specified in the GFF3 file.
            Please either pre-load the landmark sequences or set a "Landmark Type"
            in the GFF importer',
            ['%landmark' => $uniquename]));
        }
      }
    }
  }

  /**
   * Imports the feature records into Chado.
   */
  private function insertFeatures() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {1:feature}
        (uniquename, name, type_id, organism_id, residues, md5checksum,
         seqlen, is_analysis, is_obsolete)
      VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;
      $i++;

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$feature_id and !$feature['skipped']) {
        $residues = '';

        $type_id = $this->feature_cvterm_lookup[$feature['type']];
        $sql .= "(:uniquename_$i, :name_$i, :type_id_$i, :organism_id_$i, :residues_$i, " .
               " :md5checksum_$i, :seqlen_$i, FALSE, FALSE),\n";
        $args[":uniquename_$i"] = $uniquename;
        $args[":name_$i"] = $feature['name'];
        $args[":type_id_$i"] = $type_id;
        $args[":organism_id_$i"] = $feature['organism'] ? $feature['organism'] : $this->organism->organism_id;
        $args[":residues_$i"] = $residues;
        $args[":md5checksum_$i"] = $residues ? md5($residues) : '';
        $args[":seqlen_$i"] = strlen($residues);
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }


  /**
   * UPDATES the name of feature records in Chado.
   */
  private function updateFeatureNames() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->update_names));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    // Batch update: https://www.alibabacloud.com/blog/how-does-postgresql-implement-batch-update-deletion-and-insertion_596030
    $init_sql = "UPDATE {1:feature}
        SET name=tmp.name from (values\n";

    $fin_sql = ") as tmp (name,feature_id) where {1:feature}.feature_id::text=tmp.feature_id\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->update_names as $feature_id => $new_name){

      $total++;
      $i++;
      // Only do an update if this feature already exist in the database and is flagged for update.
      // TO DO: make is_obsolete updatable. Make sure to add is_obsolute collection to cached feature
      $sql .= "(:name_$i, :feature_id_$i),\n";
      $args[":name_$i"] = $new_name;
      $args[":feature_id_$i"] = $feature_id;

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql . $fin_sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Check if the features exist in the database.
   */
  private function findFeatures() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $sql = "SELECT uniquename, name, type_id, organism_id, feature_id FROM {1:feature} WHERE uniquename in (:uniquenames[])";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $names = [];
    foreach ($this->features as $uniquename => $info) {
      $feature_id = $info['feature_id'];
      $total++;

      if (!$feature_id) {
        $i++;
        $names[] = $uniquename;
      }

      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_features) {
        if (count($names) > 0) {
          $args = [':uniquenames[]' => $names];
          $results = $chado->query($sql, $args);
          while ($f = $results->fetchObject()) {
            if (array_key_exists($f->uniquename, $this->features)) {
              $matched_findex = $this->features[$f->uniquename]['findex'];
              $matched_feature = $this->getCachedFeature($matched_findex);
              $matched_type_id = $this->feature_cvterm_lookup[$matched_feature['type']];
              $matched_organism_id = $this->organism->organism_id;
              if ($matched_feature['organism']) {
                $matched_organism_id = $matched_feature['organism'];
              }
              if ($matched_type_id == $f->type_id and $matched_organism_id == $f->organism_id) {
                $this->features[$f->uniquename]['feature_id'] = $f->feature_id;
                $this->features[$f->uniquename]['name'] = $f->name;
                // Checking to see if the name has changed and therefore needs updating
                if ($f->name != $matched_feature['name']) {
                  // Yes. we need to update name of this feature.
                  // Adding flag to cached feature that indicates updated needed.
                  $this->update_names[$f->feature_id] = $matched_feature['name'];
                }
              }
            }
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $i = 0;
        $names = [];
      }
    }
  }

  /**
   * Deletes all ancillary data about a feature so we can re-insert it.
   */
  private function deleteFeatureData() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $sql1 = "DELETE from {1:featureprop} WHERE feature_id IN (:feature_ids[])";
    $sql2 = "DELETE from {1:featureloc} WHERE feature_id IN (:feature_ids[])";
    $sql3 = "DELETE from {1:feature_cvterm} WHERE feature_id IN (:feature_ids[])";
    $sql4 = "DELETE from {1:feature_dbxref} WHERE feature_id IN (:feature_ids[])";
    $sql5 = "DELETE from {1:feature_synonym} WHERE feature_id IN (:feature_ids[])";
    $sql6 = "DELETE from {1:feature_relationship} WHERE subject_id IN (:feature_ids[])";
    $sql7 = "DELETE from {1:analysisfeature} WHERE feature_id IN (:feature_ids[])";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $feature_ids = [];
    foreach ($this->features as $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;
      $i++;

      if ($feature_id and !$feature['skipped']) {
        $feature_ids[] = $feature_id;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($feature_ids) > 0) {
          $args = [':feature_ids[]' => $feature_ids];
          $chado->query($sql1, $args);
          $chado->query($sql2, $args);
          $chado->query($sql3, $args);
          $chado->query($sql4, $args);
          $chado->query($sql5, $args);
          $chado->query($sql6, $args);
          $chado->query($sql7, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $i = 0;
        $feature_ids = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureProps(){
    $chado = $this->getChadoConnection();
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {1:featureprop} (feature_id, type_id, value, rank) VALUES\n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the properties of this feature.
        foreach ($feature['properties'] as $prop_name => $values) {
          foreach ($values as $rank => $value) {
            $j++;
            $type_id = $this->featureprop_cvterm_lookup[strtolower($prop_name)];
            $sql .= "(:feature_id_$j, :type_id_$j, :value_$j, :rank_$j),\n";
            $args[":feature_id_$j"] = $feature_id;
            $args[":type_id_$j"] = $type_id;
            $args[":value_$j"] = $value;
            $args[":rank_$j"] = $rank;
          }
        }
      }
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureParents(){
    $chado = $this->getChadoConnection();
    $batch_size = 100;
    $num_parents = count(array_keys($this->parent_lookup));
    $num_batches = (int) ($num_parents / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Get the 'part_of' and 'derives_from cvterm.
    $part_of = $this->getTypeID('part_of', FALSE);
    $derives_from = $this->getTypeID('derives_from', FALSE);

    $init_sql = "INSERT INTO {1:feature_relationship} (subject_id, object_id, type_id, rank) VALUES\n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->parent_lookup as $parent => $starts) {
      $total++;
      $i++;

      $parent_feature = $this->getCachedFeature($this->features[$parent]['findex']);
      $parent_uniquename = $parent_feature['uniquename'];
      $parent_feature_id = $this->features[$parent_uniquename]['feature_id'];
      if (!$parent_feature['skipped']) {
        foreach ($starts as $start => $children) {
          foreach ($children as $child_findex) {
            $j++;
            $child_feature = $this->getCachedFeature($child_findex);
            $child_uniquename = $child_feature['uniquename'];
            $child_feature_id = $this->features[$child_uniquename]['feature_id'];
            $type_id = $part_of;
            if ($child_feature['type'] == 'polypeptide' or $child_feature['type'] == 'protein') {
              $type_id = $derives_from;
            }
            $sql .= "(:subject_id_$j, :object_id_$j, :type_id_$j, :rank_$j),\n";
            $args[":subject_id_$j"] = $child_feature_id;
            $args[":object_id_$j"] = $parent_feature_id;
            $args[":type_id_$j"] = $type_id;
            $args[":rank_$j"] = $this->features[$child_uniquename]['rank'];
          }
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_parents) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function findDbxrefs() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_dbxrefs = count(array_keys($this->dbxref_lookup));
    $num_batches = (int) ($num_dbxrefs / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // DBXrefs may be already present so we'll do an initial round of
    // looking for them and then insert those that don't exist.
    $init_sql = "
      SELECT DB.name, DBX.db_id, DBX.accession, DBX.dbxref_id, CVT.cvterm_id
      FROM {1:dbxref} DBX
        INNER JOIN {1:db} DB on DB.db_id = DBX.db_id
        LEFT JOIN {1:cvterm} CVT on DBX.dbxref_id = CVT.dbxref_id
      WHERE
    ";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->dbxref_lookup as $xref => $info) {
      $i++;
      $total++;
      $sql .= "(DBX.accession = :accession_$i and DBX.db_id = :db_id_$i) OR\n";
      $args[":accession_$i"] = $info['accession'];
      $args[":db_id_$i"] = $this->db_lookup[$info['db']];

      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_dbxrefs) {
        $sql = rtrim($sql, " OR\n");
        $sql = $init_sql . $sql;
        $results = $chado->query($sql, $args);
        while ($dbxref = $results->fetchObject()) {
          $index = $dbxref->name . ':' . $dbxref->accession;
          $this->dbxref_lookup[$index]['dbxref_id'] = $dbxref->dbxref_id;
          if ($dbxref->cvterm_id) {
            $this->cvterm_lookup[$index] = $dbxref->cvterm_id;
            $this->dbxref_lookup[$index]['cvterm_id'] = $dbxref->cvterm_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function associateChildren() {
    $this->setItemsHandled(0);
    $this->setTotalItems(count(array_keys($this->features)));

    // Iterate through parent-child relationships and set the ranks.
    $i = 0;
    foreach ($this->features as $info) {
      $i++;
      $feature = $this->getCachedFeature($info['findex']);
      if ($feature['parent']) {
        // Place features in order that they appear by their start coordinates.
        $parent = $feature['parent'];
        $start = $feature['start'];
        // We can have multiple children that start at the same location
        // so we'll store children in an array indexed by start position.
        if (!array_key_exists($parent, $this->parent_lookup)) {
          $this->parent_lookup[$parent] = [];
        }
        if (!array_key_exists($start, $this->parent_lookup[$parent])) {
          $this->parent_lookup[$parent][$start] = [];
        }
        $this->parent_lookup[$parent][$start][] = $info['findex'];
      }
      $this->setItemsHandled($i);
    }
  }

  /**
   * Calculates ranks for all of the children of each feature.
   *
   * This function should not be executed until after features are loaded
   * into the database and we have feature_ids for all of them.
   */
  private function calculateChildRanks() {

    $this->setItemsHandled(0);
    $this->setTotalItems(count(array_keys($this->parent_lookup)));
    foreach ($this->parent_lookup as $parent => $starts) {
      $starts = array_keys($starts);
      sort($starts);
      $j = 0;
      foreach ($starts as $start) {
        foreach ($this->parent_lookup[$parent][$start] as $child_findex) {
          $child = $this->getCachedFeature($child_findex);
          $this->features[$child['uniquename']]['rank'] = $j;
          $j++;
        }
      }
      $this->setItemsHandled($j);
    }
  }

  /**
   * This function goes through each landmark found, looks up the landmark_types
   * and then gets the type_id for the landmark if it's not already cached.
   * If it is already cached, it does not perform a lookup.
   */
  private function findLandmarksTypeIDs() {
    $chado = $this->getChadoConnection();
    // Get landmark type_ids and cache them
    foreach ($this->landmarks as $landmark_name => $feature_id) {
      // If there is no cached type_id for this landmark via landmark_types_type_ids

      // Determine the cvterm_name for the landmark
      $type = NULL;
      // If there is a type from the landmark_types that match, use this
      if (isset($this->landmark_types[$landmark_name])) {
        $type = $this->landmark_types[$landmark_name];
      }
      // Else use the default landmark_type
      else {
        $type = $this->default_landmark_type;
      }
      if ($type == NULL) {
        $error_msg = 'Could not determine a type for landmark name: %landmark_name';
        $error_msg .= '. There was no default landmark type to force either.';
        throw new \Exception(t($error_msg, ['%landmark_name' => $landmark_name]));
      }

      // If there is no cached type_id for this landmark type, try to lookup and cache
      if (!isset($this->landmark_types_type_ids[$type])) {
        $sql_landmark_type_id = "SELECT cvterm_id FROM {1:cvterm} WHERE name = :name";
        $args_type = array(':name' => $type);
        $results_type_ids = $chado->query($sql_landmark_type_id, $args_type);
        $rowsCount = 0;
        foreach ($results_type_ids as $row) {
          $rowsCount++;
          $this->landmark_types_type_ids[$type] = $row->cvterm_id;
        }
        // If the database lookup was not successful
        if ($rowsCount == 0) {
          // Try to default to the default landmark type cvterm
          if ($this->landmark_cvterm_id) {
            $this->landmark_types_type_ids[$type] = $this->landmark_cvterm_id;
          }
          // Else if the default could not be found (if default landmark is empty in the form)
          else {
            $error_msg = 'Could not lookup cvterm / type id for landmark type: %type.';
            $error_msg .= ' Also since there is no default landmark type specified, could not force a default landmark type_id.';
            throw new \Exception(t($error_msg, ['%type' => $type]));
          }
        }
      }
    }
  }

  /**
   * This looks up the landmark type_id by utilizing the landmark_types associate
   * array cached values and then checks the landmark_types_type_ids associate
   * array cached values.
   */
  function getLandmarkTypeID($landmark_name) {
    // If there is a type from the landmark_types that match, use this
    if (isset($this->landmark_types[$landmark_name])) {
      $type = $this->landmark_types[$landmark_name];
    }
    // Else use the default landmark_type
    else {
      $type = $this->default_landmark_type;
    }

    return $this->landmark_types_type_ids[$type];
  }

  /**
   *
   */
  private function findLandmarks() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_landmarks = count(array_keys($this->landmarks));
    $num_batches = (int) ($num_landmarks / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Perform cache of Landmarks Type IDs
    $this->findLandmarksTypeIDs();

    // NEW IMPROVED CODE WITH BATCH PROCESSING
    $init_sql = "SELECT name, uniquename, feature_id FROM {1:feature} WHERE \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $names = [];
    foreach ($this->landmarks as $landmark_name => $feature_id) {
      $i++;
      $total++;

      // Only do an insert if this dbxref doesn't already exist in the databse.
      // and this dbxref is from a Dbxref attribute not an Ontology_term attr.
      if (!$feature_id) {
        $names[] = $landmark_name;
      }

      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_landmarks) {
        foreach ($names as $name) {
          $j++;
          $sql .= "(uniquename = :landmark_$j AND type_id = :type_id_$j AND organism_id = :organism_id_$j) OR\n";
          $type_id = $this->getLandmarkTypeID($name);
          $args[":landmark_$j"] = $name;
          $args[":type_id_$j"] = $type_id;
          $args[":organism_id_$j"] = $this->organism_id;
        }
        if (count($names) > 0) {
          $sql = rtrim($sql, " OR\n");
          $sql = $init_sql . $sql;
          $results = $chado->query($sql, $args);
          while ($f = $results->fetchObject()) {
            $this->landmarks[$f->uniquename] = $f->feature_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
        $names = [];
      }
    }

  }

  /**
   *
   */
  private function insertDbxrefs() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_dbxrefs = count(array_keys($this->dbxref_lookup));
    $num_batches = (int) ($num_dbxrefs / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {1:dbxref} (db_id, accession) VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->dbxref_lookup as $info) {
      $i++;
      $total++;

      // Only do an insert if this dbxref doesn't already exist in the databse.
      // and this dbxref is from a Dbxref attribute not an Ontology_term attr.
      if (!array_key_exists('dbxref_id', $info) and
          !array_key_exists('cvterm_id', $info)) {
        $sql .= "(:db_id_$i, :accession_$i),\n";
        $args[":db_id_$i"] = $this->db_lookup[$info['db']];
        $args[":accession_$i"] = $info['accession'];
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_dbxrefs) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureDbxrefs() {
    $chado = $this->getChadoConnection();
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Don't need to use placeholders for this insert since we are only using integers.
    $init_sql = "INSERT INTO {1:feature_dbxref} (feature_id, dbxref_id) VALUES \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the dbxrefs of this feature.
        foreach ($feature['dbxrefs'] as $index => $details) {
          $j++;
          $sql .= "(:feature_id_$j, :dbxref_id_$j),\n";
          $args[":feature_id_$j"] = $feature_id;
          $args[":dbxref_id_$j"] = $this->dbxref_lookup[$index]['dbxref_id'];
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureCVterms() {
    $chado = $this->getChadoConnection();
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Don't need to use placeholders for this insert since we are only using integers.

    $init_sql = "INSERT INTO {1:feature_cvterm} (feature_id, cvterm_id, pub_id) VALUES \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the dbxrefs of this feature.
        foreach ($feature['terms'] as $index => $info) {
          $j++;
          $sql .= "(:feature_id_$j, :cvterm_id_$j, :pub_id_$j),\n";
          $args[":feature_id_$j"] = $feature_id;
          $args[":cvterm_id_$j"] = $this->cvterm_lookup[$index];
          $args[":pub_id_$j"] = $this->null_pub->pub_id;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   * Features that represent alignments have a second featureloc.
   *
   * The second featureloc entry belongs on the target sequence which
   * should either exist or was added if desired by the end-user.
   */
  private function insertFeatureTargets() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {1:featureloc}
        (srcfeature_id, feature_id, fmin, fmax, strand, phase, rank)
      VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;
      $i++;

      // If the feature is not skipped and has a target then insert the
      // target alignment.
      if (!$feature['skipped'] and array_key_exists('name', $feature['target'])) {
        $tname = $feature['target']['name'];
        $tfindex = $this->features[$tname]['findex'];
        $tfeature_id = $this->features[$tname]['feature_id'];
        $target = $this->getCachedFeature($tfindex);

        // According to the Chado instructions for rank, the feature aligned
        // to the landmark will have a rank of 0.  The feature aligned to the
        // target match will have a rank of 1.
        $rank = 1;

        $sql .= "(:srcfeature_id_$i, :feature_id_$i, :fmin_$i, :fmax_$i," .
          " :strand_$i, :phase_$i, :rank_$i),\n";
        $args[":srcfeature_id_$i"] = $tfeature_id;
        $args[":feature_id_$i"] = $feature_id;
        $args[":fmin_$i"] = $target['start'];
        $args[":fmax_$i"] = $target['stop'];
        $args[":strand_$i"] = $target['strand'];
        $args[":phase_$i"] = $target['phase'] ? $target['phase'] : NULL;
        $args[":rank_$i"] = $rank;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureDerivesFrom() {
    $chado = $this->getChadoConnection();
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Get the 'derives_from' cvterm
    $type_id = $this->getTypeID('derives_from', FALSE);

    $init_sql = "INSERT INTO {1:feature_relationship} (subject_id, object_id, type_id, rank) VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;
      $i++;

      // If the feature is not skipped
      if (!$feature['skipped'] and $feature['derives_from']) {
        $object_id = $this->features[$feature['derives_from']]['feature_id'];
        if (!$object_id) {
          $this->logger->warning("Skipping 'derives_from' relationship for feature @feature_name. "
                               . "Could not find the derives_from feature: @derives_from.",
            ['@feature_name' => $feature['uniquename'], '@derives_from' => $feature['derives_from']]
          );
          continue;
        }
        $sql .= "(:subject_id_$i, :object_id_$i, :type_id_$i, 0),\n";
        $args[":subject_id_$i"] = $feature_id;
        $args[":object_id_$i"] = $object_id;
        $args[":type_id_$i"] = $type_id;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureLocs() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {1:featureloc}
        (srcfeature_id, feature_id, fmin, fmax, strand, phase, rank)
      VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;
      $i++;

      // If the feature is not skipped and is not a match "target".
      if (!$feature['skipped'] and !array_key_exists('is_target', $feature)) {

        $sql .= "(:srcfeature_id_$i, :feature_id_$i, :fmin_$i, :fmax_$i," .
                " :strand_$i, :phase_$i, :rank_$i),\n";
        $args[":srcfeature_id_$i"] = $this->landmarks[$feature['landmark']];
        $args[":feature_id_$i"] = $feature_id;
        $args[":fmin_$i"] = $feature['start'];
        $args[":fmax_$i"] = $feature['stop'];
        $args[":strand_$i"] = $feature['strand'];
        $args[":phase_$i"] = $feature['phase'] ? $feature['phase'] : NULL;
        $args[":rank_$i"] = 0;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Finds an organism from an organism attribute value.
   */
  private function findOrganism($organism_attr, $line_num) {
    $chado = $this->getChadoConnection();

    if (array_key_exists($organism_attr, $this->organism_lookup)) {
      return $this->organism_lookup[$organism_attr];
    }

    // Get the organism object.
    list($genus, $species) = explode(':', $organism_attr, 2);
    $organism_select = $chado->select('1:organism','o');
    $organism_select->fields('o');
    $organism_select->condition('genus', $genus);
    $organism_select->condition('species', $species);

    $organism_count = $organism_select->countQuery();
    $num_found = $organism_count->execute()->fetchField();

    if ($num_found == 1){
      $organism = $organism_select->execute()->fetchObject();
      $this->organism_lookup[$organism_attr] = $organism->organism_id;
      return $organism->organism_id;
    }

    if ($num_found > 1) {
      throw new \Exception(t('Multiple organisms were found for the "organism" attribute, %organism, on line %line_num',
        ['%organism' => $organism_attr, '%line_num' => $line_num]));
    }

    if ($this->create_organism) {
      $organism_insert = $chado->insert('1:organism');
      $organism_insert->fields([
        'genus' => $genus,
        'species' => $species
      ]);
      $organism_id = $organism_insert->execute();
      $this->organism_lookup[$organism_attr] = $organism_id;
      return $organism_id;
    }
    return NULL;
  }

  /**
   *
   */
  private function findSynonyms() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "SELECT synonym_id, name FROM {1:synonym} WHERE \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $batch_synonyms = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature = $this->getCachedFeature($findex);

      $i++;
      $total++;

      // Get all of the synonyms for this batch.
      if (array_key_exists('synonyms', $feature)) {
        foreach ($feature['synonyms'] as $index => $synonym) {
          $batch_synonyms[] = $synonym;
        }
      }

      // If we've reached the size of the batch then let's do the select
      if ($i == $batch_size or $total == $num_features) {

        $batch_synonyms = array_unique($batch_synonyms);
        foreach ($batch_synonyms as $synonym) {
          $j++;
          if (!array_key_exists($synonym, $this->synonym_lookup)) {
            $this->synonym_lookup[$synonym] = NULL;
          }
          if (!$this->synonym_lookup[$synonym]) {
            $sql .= "(type_id = :type_id_$j AND name = :name_$j) OR\n";
            $args[":type_id_$j"] = $this->exact_syn_id;
            $args[":name_$j"] = $synonym;
          }
        }
        if (count($args) > 0) {
          $sql = rtrim($sql, " OR\n");
          $sql = $init_sql . $sql;
          $results = $chado->query($sql, $args);
          while ($synonym = $results->fetchObject()) {
            $this->synonym_lookup[$synonym->name] = $synonym->synonym_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
        $batch_synonyms = [];
      }
    }
  }

  /**
   *
   */
  private function insertSynonyms() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_synonyms = count(array_keys($this->synonym_lookup));
    $num_batches = (int) ($num_synonyms / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {1:synonym} (type_id, name, synonym_sgml) VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->synonym_lookup as $synonym => $synonym_id) {
      $i++;
      $total++;

      // Only do an insert if this dbxref doesn't already exist in the databse.
      if (!$synonym_id) {
        $sql .= "(:type_id_$i,:name_$i, ''),\n";
        $args[":type_id_$i"] = $this->exact_syn_id;
        $args[":name_$i"] = $synonym;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_synonyms) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }

    // Now we need to retrieve the synonyms IDs.
    $this->findSynonyms();
  }

  /**
   *
   */
  private function insertFeatureSynonyms(){
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {1:feature_synonym} (synonym_id, feature_id, pub_id) VALUES \n";
    $sql = "";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    foreach ($this->features as $uniquename => $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Handle all of the synonyms for this feature.
        foreach (array_unique($feature['synonyms']) as $synonym) {
          $j++;
          $sql .= "(:synonym_id_$j, :feature_id_$j, :pub_id_$j),\n";
          $args[":synonym_id_$j"] = $this->synonym_lookup[$synonym];
          $args[":feature_id_$j"] = $feature_id;
          $args[":pub_id_$j"] = $this->null_pub->pub_id;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
      }
    }
  }

  /**
   * Determines the names for a feature using the ID and name attributes.
   *
   * @param array $feature_attrs
   *   The associative array of attributes for the feature.
   *
   * @param string $type
   *   The type of feature.
   *
   * @return array
   *   An associative array with 'uniquename' and 'name' keys.
   */
  private function getFeatureNames($attrs, $type, $landmark_name, $fmin, $fmax) {
    $uniquename = '';
    $name = '';

    if (!array_key_exists('ID', $attrs) and !array_key_exists('Name', $attrs)) {

      // Check if an alternate ID field is suggested, if so, then use
      // that for the name.
      if (array_key_exists($this->alt_id_attr, $attrs)) {
        $uniquename = $attrs[$this->alt_id_attr][0];
        $name = $uniquename;
      }

      // If the row has a parent then generate a unique ID
      elseif (array_key_exists('Parent', $attrs)) {
        $uniquename = $attrs['Parent'][0] . "-" . $type . "-" .
          $landmark_name . ":" . ($fmin + 1) . ".." . $fmax;
        $name = $attrs['Parent'][0] . "-" . $type;
      }

      // Generate a unique name based on the type and location
      // and set the name to simply be the type.
      else {
        $uniquename = $type . "-" . $landmark_name . ":" . ($fmin + 1) . ".." . $fmax;
        $name = $type . "-" . $landmark_name;
      }
    }
    elseif (!array_key_exists('Name', $attrs)) {
      $uniquename = $attrs['ID'][0];
      $name = $attrs['ID'][0];
    }
    elseif (!array_key_exists('ID', $attrs)) {
      $uniquename = $attrs['Name'][0];
      $name = $attrs['Name'][0];
    }
    else {
      $uniquename = $attrs['ID'][0];
      $name = $attrs['Name'][0];
    }

    // Does this uniquename already exist?
    if (array_key_exists($uniquename, $this->features)) {
      $prev_feature = $this->getCachedFeature($this->features[$uniquename]['findex']);
      // A name can be duplicated for subfeatures (e.g. CDS features)
      // that have the same parent but are really all the same thing.
      if (array_key_exists('Parent', $attrs)) {
        // Iterate through the list of similar IDs and see how many we have
        // then add a numeric suffix.
        $i = 2;
        while (array_key_exists($uniquename . "_" . $i, $this->features)) {
          $i++;
        }
        $uniquename = $uniquename . "_" . $i;
      }
      // If this is a match feature (e.g. match, EST_match, cDNA_match, etc).
      // then we can accept a duplicated ID in the GFF3 file.  But we
      // must rename it before going into Chado.  For this, we will allow
      // the match feature to keep the original ID and we will create a new
      // name for the match_part.
      elseif (preg_match('/match$/', $type)) {
        $i = 1;
        $temp_uname = $uniquename;
        do {
          $temp_uname = $uniquename . "_part_" . $i;
          $i++;
        }
        while (array_key_exists($temp_uname, $this->features));
        $uniquename = $temp_uname;
      }
      // If this feature has already been added but as a target of a previous
      // feature then we'll let it go through and replace the target feature.
      elseif ($prev_feature['is_target'] == TRUE) {
        // Do nothing.
      }
      else {
        throw new \Exception(t("A feature with the same ID exists multiple times: %uname",
          ['%uname' => $uniquename]));
      }
    }
    return [
      'name' => $name,
      'uniquename' => $uniquename,
    ];
  }

  /**
   *
   */
  private function insertFeatureAnalysis() {
    $chado = $this->getChadoConnection();
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {1:analysisfeature} (feature_id, analysis_id, significance) VALUES \n";
    $sql = "";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    foreach ($this->features as $info) {
      $findex = $info['findex'];
      $feature_id = $info['feature_id'];
      $feature = $this->getCachedFeature($findex);

      $i++;
      $total++;

      // If the feature is not skipped then add it to the table
      if (!$feature['skipped']) {
        $sql .= "(:feature_id_$i, :analysis_id_$i, :significance_$i),\n";
        $args[":feature_id_$i"] = $feature_id;
        $args[":analysis_id_$i"] = $this->analysis->analysis_id;
        if (array_key_exists('score', $feature) and (strcmp($feature['score'], '.') != 0)) {
          $args[":significance_$i"] = $feature['score'];
        }
        else {
          $args[":significance_$i"] = NULL;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $chado->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
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
