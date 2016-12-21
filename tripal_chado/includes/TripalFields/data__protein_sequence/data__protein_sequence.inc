<?php

class data__protein_sequence extends TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The term that this field maps to.  The format for the term should be:
  // [vocab]:[accession] where [vocab] is the short name of the vocabulary
  // and [acession] is the unique accession number for the term.  This term
  // must already exist in the vocabulary storage backend. This
  // value should never be changed once fields exist for this type.
  public static $term = 'data:2976';

  // The default lable for this field.
  public static $label = 'Protein Sequence';

  // The default description for this field.
  public static $description = 'polypeptide sequences.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $instance_settings  = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'tripal_no_storage';

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