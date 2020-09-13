<?php

class data__protein_sequence extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Protein Sequence';

  // The default description for this field.
  public static $description = 'polypeptide sequences.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'protein_sequence',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2976',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = [
    'TripalTabDownloader',
    'TripalCSVDownloader',
    'TripalProteinFASTADownloader',
  ];

  // The default widget for this field.
  public static $default_widget = 'data__protein_sequence_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__protein_sequence_formatter';


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    $info = [
      $field_term => [
        'label' => 'Protein sequence',
        'help' => 'The polypeptide sequence derived from mRNA',
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => FALSE,
      ],
    ];
    return $info;
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;
    $num_seqs = 0;

    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    // Look for protein sequences based on the relationship of this field.
    $sql = "
      SELECT F.*
      FROM {feature_relationship} FR
        INNER JOIN {feature} F on FR.subject_id = F.feature_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
      WHERE
        FR.object_id = :feature_id and
        CVT.name = 'polypeptide' and
        RCVT.name  IN ('derives_from', 'part_of')
      ORDER BY FR.rank ASC
    ";
    $proteins = chado_query($sql, [':feature_id' => $feature->feature_id]);
    while ($protein = $proteins->fetchObject()) {
      $entity->{$field_name}['und'][$num_seqs]['value'] = $protein->residues;
      // Because we'll be saving a feature we need to maintain all of it's
      // columns in the feature table. The following will add them all.
      $columns = get_object_vars($protein);
      foreach ($columns as $colname => $value) {
        $entity->{$field_name}['und'][$num_seqs]['chado-feature__' . $colname] = $value;
      }
      $num_seqs++;
    }
  }
}
