<?php

class data__sequence_record extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Sequences';

  // The default description for this field.
  public static $description = 'A field for displaying all sequences associated with a feature along with their metadata.';

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
    'term_name' => 'sequence_record',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0849',
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
  public static $default_widget = 'data__sequence_record_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__sequence_record_formatter';


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
        'type' => 'xs:complex',
        'readonly' => TRUE,
      ],
    ];
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;

    // Intialize the field items array
    $entity->{$field_name}['und'] = [];

    // Add the primary sequence from the Chada feature table, residues column.
    $feature = chado_expand_var($feature, 'field', 'feature.residues');

    // Always add the primary sequence.
    $this->addPrimary($entity, $feature, $field_name);

    // If this is an mRNA feature then add the gene parent, full length
    // mRNA, CDS and protein.
    if ($feature->type_id->name == 'mRNA') {
      $featurelocs = $this->addFLmRNA($entity, $feature, $field_name);
      $this->addGeneParent($entity, $feature, $field_name);
      if (count($featurelocs) > 0) {
        $this->addCDS($entity, $feature, $field_name, $featurelocs);
        $this->addProtein($entity, $feature, $field_name);
      }
    }
    // For all others get the sequence from the reference.
    else {
      $this->addGenericReference($entity, $feature, $field_name);
    }

    // The field should always return a 'value' key so even if we don't
    // have a sequence we still need to add it
    if (count(array_keys($entity->{$field_name}['und'])) == 0) {
      $entity->{$field_name}['und'][0]['value'] = '';
    }
  }

  /**
   *
   * @param unknown $entity
   * @param unknown $feature
   * @param unknown $field_name
   */
  private function addGenericReference(&$entity, $feature, $field_name) {
    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_coords_term = 'data:2012';
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    $options = [
      'derive_from_parent' => 1,
    ];
    $seqs = chado_get_feature_sequences(['feature_id' => $feature->feature_id], $options);
    $featurelocs = [];
    foreach ($seqs as $seq) {
      $featureloc = $this->getFeatureLoc($seq['featureloc_id']);
      $coords = $this->getSequenceCoords($featureloc);

      $entity->{$field_name}['und'][]['value'] = [
        $sequence_term => $seq['residues'],
        $label_term =>  'Derived ' . ucfirst(preg_replace('/_/', ' ', $feature->type_id->name)) . ' Sequence (' . number_format(strlen($seq['residues'])) . 'bp)',
        $description_term => 'This sequence was derived by extracting bases from the reference sequence location at ' . $coords['schema:description'] . '.',
        $seq_coords_term => $coords,
        $seq_length_term => strlen($seq['residues']),
        $seq_md5sum_term => md5($seq['residues']),
        $type_term => $feature->type_id->name,
        $fasta_defline => $seq['defline'],
      ];

      $featurelocs[] = $featureloc;
    }
    return $featurelocs;
  }

  /**
   * Adds the primary sequence from the feature.residues column.
   */
  private function addPrimary(&$entity, $feature, $field_name) {

    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    if ($feature->residues) {
      $entity->{$field_name}['und'][]['value'] = [
        $label_term => 'Primary ' . preg_replace('/_/', ' ', $feature->type_id->name) . ' Sequence (' . number_format($feature->seqlen) . 'bp)',
        $description_term => 'This is the primary representative sequence for this feature.',
        $sequence_term => $feature->residues,
        $seq_length_term => $feature->seqlen,
        $seq_md5sum_term => $feature->md5checksum,
        $type_term => $feature->type_id->name,
        $fasta_defline => chado_get_fasta_defline($feature)
      ];
    }
  }


  /**
   * Adds the full length mRNA sequence (only for gene features).
   */
  private function addFLmRNA(&$entity, $feature, $field_name) {
    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_coords_term = 'data:2012';
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    // Sometimes an mRNA may have only exons, only CDS or both exons and
    // CDS.  We need to know which.
    $sql = "
      SELECT DISTINCT CVT.name
      FROM {feature_relationship} FR
        INNER JOIN {feature} SF on FR.subject_id = SF.feature_id
        INNER JOIN {feature} OF on FR.object_id = OF.feature_id
        INNER JOIN {cvterm} CVT on SF.type_id = CVT.cvterm_id
      WHERE FR.object_id = :feature_id
    ";
    $subtypes = chado_query($sql, [':feature_id' => $feature->feature_id])->fetchCol('name');

    $exon = 'exon';
    if (!in_array('exon', $subtypes) and in_array('CDS', $subtypes)) {
      $exon = 'CDS';
    }

    $options = [
      'derive_from_parent' => 1,
      'aggregate' => 1,
      'is_html' => 0,
      'sub_feature_types' => ['three_prime_UTR', $exon, 'five_prime_UTR'],
    ];
    $seqs = chado_get_feature_sequences(['feature_id' => $feature->feature_id], $options);
    $featurelocs = [];
    foreach ($seqs as $seq) {

      $featureloc = $this->getFeatureLoc($seq['featureloc_id']);
      $coords = $this->getSequenceCoords($featureloc);
      $types = preg_replace('/_/', ' ', $seq['types']);

      $entity->{$field_name}['und'][]['value'] = [
        $sequence_term => $seq['residues'],
        $label_term => 'Derived mRNA Sequence (' . number_format(strlen($seq['residues'])) . 'bp)',
        $description_term => 'This full length mRNA sequence was derived by extracting bases from the reference sequence location at ' . $coords['schema:description'] . ' and contains: ' . implode(', ', $types) ,
        $seq_coords_term => $coords,
        $seq_length_term => strlen($seq['residues']),
        $seq_md5sum_term => md5($seq['residues']),
        $type_term => 'mRNA',
        $fasta_defline => $seq['defline']
      ];

      $featurelocs[] = $featureloc;
    }
    return $featurelocs;
  }

  /**
   * Retrieves the feature location information.
   */
  private function getFeatureLoc($featureloc_id) {
    $featurelocs_sql = "
        SELECT SRCF.name, FL.srcfeature_id, FL.strand, FL.fmin, FL.fmax, FL,phase, FL.featureloc_id
        FROM {featureloc} FL
          INNER JOIN {feature} SRCF on SRCF.feature_id = FL.srcfeature_id
        WHERE FL.featureloc_id = :featureloc_id
     ";
    return chado_query($featurelocs_sql, [':featureloc_id' => $featureloc_id])->fetchObject();
  }

  /**
   * Gets the sequence location string for a featureloc record.
   */
  private function getSequenceCoords($featureloc) {
    $description_term = 'schema:description';
    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');

    $srcfeature = $featureloc->name;
    $strand = '';
    if ($featureloc->strand == 1) {
      $strand = '+';
    }
    elseif ($featureloc->strand == -1) {
      $strand = '-';
    }
    else {
      $strand = '';
    }
    $fmin = $featureloc->fmin + 1;
    $fmax = $featureloc->fmax;
    $location = $srcfeature . ':' . $fmin . '-' . $fmax . $strand;

    return [
      $description_term => $location,
      $reference_term => $srcfeature,
      $fmin_term => $fmin,
      $fmax_term => $fmax,
      $strand_term => $strand,
    ];
  }

  /**
   * Adds the CDS sequence (only for an mRNA feature)
   */
  private function addCDS(&$entity, $feature, $field_name, $featurelocs) {
    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    foreach ($featurelocs as $featureloc) {
      $cds_feature = [
        'feature_id' => $feature->feature_id,
        'parent_id' => $featureloc->srcfeature_id,
        'name' => $feature->name,
        'featureloc_id' => $featureloc->featureloc_id,
      ];
      $options = [
        'derive_from_parent' => 1,
        'aggregate' => 1,
        'sub_feature_types' => ['CDS'],
        'is_html' => 0,
      ];

      $cds_sequence = chado_get_feature_sequences($cds_feature, $options);
      $coords = $this->getSequenceCoords($featureloc);

      if (count($cds_sequence) > 0) {
        $entity->{$field_name}['und'][]['value'] = [
          $label_term => 'Coding Sequence (' . number_format($cds_sequence[0]['length']) . 'bp)',
          $sequence_term => $cds_sequence[0]['residues'],
          $description_term => 'This CDS was derived by extracting bases from the reference sequence location at ' . $coords['schema:description'] . '.' ,
          $seq_length_term => $cds_sequence[0]['length'],
          $seq_md5sum_term => md5($cds_sequence[0]['residues']),
          $type_term => 'CDS',
          $fasta_defline => $cds_sequence[0]['defline']
        ];
      }
    }
  }

  /**
   * Adds the sequecne for a gene parent (only for gene children).
   */
  private function addGeneParent(&$entity, $feature, $field_name) {
    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $seq_coords_term = 'data:2012';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    $sql = "
      SELECT FO.*
      FROM {feature_relationship} FREL
        INNER JOIN {feature} FO on FO.feature_id = FREL.object_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = FO.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FREL.type_id
      WHERE
        FREL.subject_id = :feature_id and
        CVT.name = 'gene' and
        RCVT.name IN ('part_of')
    ";
    $genes = chado_query($sql, [':feature_id' => $feature->feature_id]);
    while ($gene = $genes->fetchObject()) {
      if (!empty($gene->residues)) {
        $entity->{$field_name}['und'][]['value'] = [
          $label_term => 'Gene Sequence (primary)',
          $sequence_term => $gene->residues,
          $label_term => 'The gene sequence.',
          $seq_length_term => strlen($gene->residues),
          $seq_md5sum_term => md5($gene->residues),
          $type_term => 'gene',
          $fasta_defline => chado_get_fasta_defline($gene),
        ];
      }
      else {
        $seqs = chado_get_feature_sequences(['feature_id' => $gene->feature_id], ['derive_from_parent' => 1]);
        foreach ($seqs as $seq) {
          $featureloc = $this->getFeatureLoc($seq['featureloc_id']);
          $coords = $this->getSequenceCoords($featureloc);
          $entity->{$field_name}['und'][]['value'] = [
            $label_term => 'Derived Gene Sequence (' . number_format(strlen($seq['residues'])) . 'bp)',
            $sequence_term => $seq['residues'],
            $description_term => 'This gene sequence was derived by extracting bases from the reference sequence location at ' . $coords['schema:description'] . '.' ,
            $seq_coords_term => $coords,
            $seq_length_term => strlen($seq['residues']),
            $seq_md5sum_term => md5($seq['residues']),
            $type_term => 'gene',
            $fasta_defline => $seq['defline'],
          ];
        }
      }
    }
  }

  /**
   * Adds the protein sequence (only for mRNA features).
   */
  private function addProtein(&$entity, $feature, $field_name) {
    $label_term = 'rdfs:label';
    $type_term = 'rdfs:type';
    $description_term = 'schema:description';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $fasta_defline = 'local:fasta_definition';

    $sql = "
      SELECT F.*
      FROM {feature_relationship} FR
        INNER JOIN {feature} F on FR.subject_id = F.feature_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
      WHERE
        FR.object_id = :feature_id and
        CVT.name = 'polypeptide' and
        RCVT.name IN ('derives_from', 'part_of')
      ORDER BY FR.rank ASC
    ";
    $proteins = chado_query($sql, [':feature_id' => $feature->feature_id]);
    while ($protein = $proteins->fetchObject()) {
      if (!empty($protein->residues)) {
        $entity->{$field_name}['und'][]['value'] = [
          $label_term => 'Protein Sequence (' . number_format(strlen($protein->residues)) . 'aa)',
          $sequence_term => $protein->residues,
          $description_term => 'The protein sequence.',
          $seq_length_term => strlen($protein->residues),
          $seq_md5sum_term => md5($protein->residues),
          $type_term => 'polypeptide',
          $fasta_defline => chado_get_fasta_defline($protein),
        ];
      }
    }
  }
}

