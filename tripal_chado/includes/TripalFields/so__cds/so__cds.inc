<?php

class so__cds extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Coding Sequence';

  // The default description for this field.
  public static $description = 'A contiguous sequence which begins with, and includes, a start codon and ends with, and includes, a stop codon.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SO',
    // The name of the term.
    'term_name' => 'CDS',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000316',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => TRUE,
  );

  // The default widget for this field.
  public static $default_widget = 'so__cds_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__cds_formatter';

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $field_name = $this->field['field_name'];
    $feature = $details['record'];
    $num_seqs = 0;

    $feature = chado_expand_var($feature, 'table', 'featureloc', $options);
    $featureloc_sequences = chado_get_featureloc_sequences($feature->feature_id, $feature->featureloc->feature_id);
    foreach($featureloc_sequences as $src => $attrs){
      // Generate a CDS sequence if one exsits for this feature alignment.
      $cds_sequence = tripal_get_feature_sequences(
        array(
          'feature_id' => $feature->feature_id,
          'parent_id' => $attrs['featureloc']->srcfeature_id->feature_id,
          'name' => $feature->name,
          'featureloc_id' => $attrs['featureloc']->featureloc_id,
        ),
        array(
          // CDS are in parent-child relationships so we want to use the
          // sequence from the parent
          'derive_from_parent' => 1,
          // we want to combine all CDS for this feature into a single sequence
          'aggregate' => 1,
          // we're looking for CDS features
          'sub_feature_types' => array('CDS'),
          'is_html' => 0
        )
      );

      if (count($cds_sequence) > 0) {
        $defline = tripal_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', strlen($attrs['residues']));

        // the tripal_get_feature_sequences() function can return multiple sequences
        // if a feature is aligned to multiple places. In the case of CDSs we expect
        // that one mRNA is only aligned to a single location on the assembly so we
        // can access the CDS sequence with index 0.
        if ($cds_sequence[0]['residues']) {
          $entity->{$field_name}['und'][$num_seqs++]['value'] = $defline . "\n" .  $cds_sequence[0]['residues'];
        }
      }
    }
  }
}