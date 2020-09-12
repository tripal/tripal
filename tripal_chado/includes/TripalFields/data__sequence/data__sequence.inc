<?php

class data__sequence extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Sequence';

  // The default description for this field.
  public static $description = 'A field for managing nucleotide and protein residues.';

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
    'term_name' => 'sequence',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2044',
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
    'TripalNucFASTADownloader',
  ];

  // The default widget for this field.
  public static $default_widget = 'data__sequence_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__sequence_formatter';


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => FALSE,
      ],
    ];
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;

    $feature = chado_expand_var($feature, 'field', 'feature.residues');
    $entity->{$field_name}['und'][0]['value'] = $feature->residues;

    /* // Add in sequences from alignments.
       $options = array(
         'return_array' => 1,
         'include_fk' => array(
           'srcfeature_id' => array(
             'type_id' => 1
           ),
           'feature_id' => array(
             'type_id' => 1
           ),
         ),
       );
       $feature = chado_expand_var($feature, 'table', 'featureloc', $options);
       $featureloc_sequences = $this->get_featureloc_sequences($feature->feature_id, $feature->featureloc->feature_id);
   
       // Add in the coding sequences. It's faster to provide the SQL rather than
       // to use chado_generate_var based on the type.
       $sql = "
         SELECT F.*
         FROM {feature_relationship} FR
           INNER JOIN {feature} F on FR.subject_id = F.feature_id
           INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
           INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
           INNER JOIN {featureloc} FL on FL.feature_id = F.feature_id
         WHERE
           FR.object_id = :feature_id and
           CVT.name = 'CDS' and
           RCVT.name = 'part_of'
         ORDER BY FR.rank ASC
       ";
       $results = chado_query($sql, array(':feature_id' => $feature->feature_id));
       $coding_seq = '';
       while ($CDS = $results->fetchObject()) {
         if ($CDS->residues) {
           $coding_seq .= $CDS->residues;
         }
       }
       if ($coding_seq) {
         $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
           '@type' => 'SO:0000316',
           'type' => 'coding_sequence',
           'label' => 'Coding sequence (CDS)',
           'defline' => chado_get_fasta_defline($feature, 'CDS', NULL, '', strlen($coding_seq)),
           'residues' => $coding_seq,
         );
       }
   
       foreach($featureloc_sequences as $src => $attrs){
         // the $attrs array has the following keys
         //   * id:  a unique identifier combining the feature id with the cvterm id
         //   * type: the type of sequence (e.g. mRNA, etc)
         //   * location:  the alignment location
         //   * defline: the definition line
         //   * formatted_seq: the formatted sequences
         //   * featureloc:  the feature object aligned to
         $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
           'residues' => $attrs['residues'],
           '@type' => 'SO:0000110',
           'type' => 'sequence_feature',
           'defline' => chado_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', strlen($attrs['residues'])),
           'label' => 'Sequence from alignment at ' . $attrs['location'],
         );
   
   
         // check to see if this alignment has any CDS. If so, generate a CDS sequence
         $cds_sequence = chado_get_feature_sequences(
             array(
               'feature_id' => $feature->feature_id,
               'parent_id' => $attrs['featureloc']->srcfeature_id->feature_id,
               'name' => $feature->name,
               'featureloc_id' => $attrs['featureloc']->featureloc_id,
             ),
             array(
               'derive_from_parent' => 1, // CDS are in parent-child relationships so we want to use the sequence from the parent
               'aggregate' => 1, // we want to combine all CDS for this feature into a single sequence
               'sub_feature_types' => array('CDS'), // we're looking for CDS features
               'is_html' => 0
             )
             );
   
         if (count($cds_sequence) > 0) {
           // the chado_get_feature_sequences() function can return multiple sequences
           // if a feature is aligned to multiple places. In the case of CDSs we expect
           // that one mRNA is only aligned to a single location on the assembly so we
           // can access the CDS sequence with index 0.
           if ($cds_sequence[0]['residues']) {
             $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
               'residues' => $cds_sequence[0]['residues'],
               '@type' => 'SO:0000316',
               'type' => 'coding_sequence',
               'defline' => chado_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', $cds_sequence[0]['length']),
               'label' => 'Coding sequence (CDS) from alignment at  ' . $attrs['location'],
             );
           }
         }
       } */
  }
}

