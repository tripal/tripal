<?php

class data__protein_sequence extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Protein Sequence';

  // The default description for this field.
  public static $description = 'polypeptide sequences.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'protein_sequence',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2976',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => TRUE,
  );

  // The default widget for this field.
  public static $default_widget = 'data__protein_sequence_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__protein_sequence_formatter';


  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $field_name = $this->field['field_name'];
    $feature = $details['record'];
    $num_seqs = 0;

    // Look for Protein sequences
    $sql = "
      SELECT F.*
      FROM {feature_relationship} FR
        INNER JOIN {feature} F on FR.subject_id = F.feature_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
      WHERE
        FR.object_id = :feature_id and
        CVT.name = 'polypeptide' and
        RCVT.name = 'derives_from'
      ORDER BY FR.rank ASC
    ";
    $results = chado_query($sql, array(':feature_id' => $feature->feature_id));
    while ($protein = $results->fetchObject()) {
      if ($protein->residues) {
        $entity->{$field_name}['und'][$num_seqs++]['value'] = $protein->residues;
      }
    }
  }
}