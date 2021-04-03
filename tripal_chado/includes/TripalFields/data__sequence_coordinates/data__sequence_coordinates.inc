<?php

class data__sequence_coordinates extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Sequence Coordinates';

  // The default description for this field.
  public static $description = 'Locations on reference sequences where the feature is located.';

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
    'term_name' => 'sequence_coordinates',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2012',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'data__sequence_coordinates_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__sequence_coordinates_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;

  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');
    $phase_term = chado_get_semweb_term('featureloc', 'phase');

    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'label' => 'Location Coordinates',
        'help' => 'The locations on other genomic sequences where this record has been aligned.',
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          $reference_term => [
            'searchable' => TRUE,
            'label' => 'Location Reference Name',
            'help' => 'The genomic feature on which this feature is localized.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $fmin_term => [
            'searchable' => TRUE,
            'label' => 'Location Start Position',
            'help' => 'The start position',
            'type' => 'numeric',
            'operations' => ['eq', 'gt', 'lt', 'gte', 'lte'],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $fmax_term => [
            'searchable' => TRUE,
            'label' => 'Location End Position',
            'help' => 'The end position',
            'type' => 'numeric',
            'operations' => ['eq', 'gt', 'lt', 'gte', 'lte'],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $phase_term => [
            'searchable' => TRUE,
            'type' => 'numeric',
            'label' => 'Location Phase',
            'help' => 'The phase of the feature (applicable only to coding sequences).',
            'operations' => ['eq', 'gt', 'lt', 'gte', 'lte'],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $strand_term => [
            'searchable' => TRUE,
            'label' => 'Location Strand',
            'help' => 'The orientation of this feature where it is localized',
            'operations' => ['eq', 'gt', 'lt', 'gte', 'lte'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $reference_term = $field_term_id . ',' . 'data:3002';
    $fmin_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'strand');
    $phase_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'phase');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'featureloc', $alias, "base.feature_id = $alias.feature_id");
    if ($condition['column'] == $reference_term) {
      $salias = $alias . '_src';
      $this->queryJoinOnce($query, 'feature', $salias, "$alias.srcfeature_id = $salias.feature_id");
      $query->condition("$salias.name", $condition['value'], $operator);
    }
    if ($condition['column'] == $strand_term) {
      $strand = '';
      if ($condition['value'] == '-') {
        $strand = -1;
      }
      if ($condition['value'] == '+') {
        $strand = 1;
      }
      $query->condition("$alias.strand", $strand, $operator);
    }
    if ($condition['column'] == $fmin_term) {
      $query->condition("$alias.fmin", $condition['value'] - 1, $operator);
    }
    if ($condition['column'] == $fmax_term) {
      $query->condition("$alias.fmax", $condition['value'], $operator);
    }
    if ($condition['column'] == $phase_term) {
      $query->condition("$alias.phase", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $reference_term = $field_term_id . ',' . 'data:3002';
    $fmin_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'strand');
    $phase_term = $field_term_id . ',' . chado_get_semweb_term('featureloc', 'phase');

    $this->queryJoinOnce($query, 'featureloc', $alias, "base.feature_id = $alias.feature_id", "LEFT OUTER");
    if ($order['column'] == $reference_term) {
      $salias = $alias . '_src';
      $this->queryJoinOnce($query, 'feature', $salias, "$alias.srcfeature_id = $salias.feature_id", "LEFT OUTER");
      $query->orderBy("$salias.name", $order['direction']);
    }
    if ($order['column'] == $fmin_term) {
      $query->orderBy("$alias.fmin", $order['direction']);
    }
    if ($order['column'] == $fmax_term) {
      $query->orderBy("$alias.fmax", $order['direction']);
    }
    if ($order['column'] == $phase_term) {
      $query->orderBy("$alias.phase", $order['direction']);
    }
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;
    $num_seqs = 0;

    $description = 'schema:description';
    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');
    $phase_term = chado_get_semweb_term('featureloc', 'phase');

    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    // Get the featureloc records that this feature is aligned to. We use
    // this SQL rather than the chado_expand_var function because we don't
    // want the residues included from the srcfeature_id which may be huge
    // and overrun memory.
    $featurelocs_sql = "
      SELECT SRCF.name, FL.srcfeature_id, FL.strand, FL.fmin, FL.fmax, FL,phase
      FROM {featureloc} FL
        INNER JOIN {feature} SRCF on SRCF.feature_id = FL.srcfeature_id
      WHERE FL.feature_id = :feature_id
      ORDER BY rank ASC
    ";
    $aligned = chado_query($featurelocs_sql, [':feature_id' => $feature->feature_id]);
    $index = 0;
    while ($featureloc = $aligned->fetchObject()) {
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
      $entity->{$field_name}['und'][$index] = [
        'value' => [
          $description => $srcfeature . ':' . $fmin . '-' . $fmax . $strand,
          $reference_term => $srcfeature,
          $fmin_term => $fmin,
          $fmax_term => $fmax,
          $strand_term => $strand,
          $phase_term => $featureloc->phase,
        ],
      ];
      $sentity_id = chado_get_record_entity_by_table('feature_id', $featureloc->srcfeature_id);
      if ($sentity_id) {
        $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $sentity_id;
      }
      $index++;
    }
  }
}
