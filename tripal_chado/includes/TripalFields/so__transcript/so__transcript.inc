<?php

class so__transcript extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Transcripts';

  // The default description for this field.
  public static $description = 'An RNA synthesized on a DNA or RNA template by an RNA polymerase.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SO',
    // The name of the term.
    'term_name' => 'transcript',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000673',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'so__transcript_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__transcript_formatter';

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
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          'rdfs:type' => [
            'name' => 'transcript_type',
            'label' => 'Transcript Type',
            'help' => 'The type of a transcript.',
            'searchable' => FALSE,
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'schema:name' => [
            'name' => 'transcript_name',
            'label' => 'Transcript Name',
            'help' => 'The name of a transcript.',
            'searchable' => FALSE,
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'data:0842' => [
            'name' => 'transcript_uniquename',
            'label' => 'Transcript Identifier',
            'help' => 'The unique identifier of a transcript.',
            'searchable' => FALSE,
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'SO:0000735' => [
            'name' => 'loc',
            'label' => 'Transcript Location',
            'help' => 'The location of the transcript on a reference feature.',
            'searchable' => FALSE,
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
            'sortable' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {

    $record = $entity->chado_record;
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    if (!$record) {
      return;
    }

    // Get the mRNA features for this gene.
    $sql = "
      SELECT FS.name, FS.uniquename, FS.feature_id, FCVT.name as type
      FROM {feature_relationship} FR
        INNER JOIN {feature} FS on FS.feature_id = FR.subject_id
        INNER JOIN {cvterm} FCVT on FCVT.cvterm_id = FS.type_id
        INNER JOIN {cv} CV on CV.cv_id = FCVT.cv_id
      WHERE
        FR.object_id = :feature_id and
        FCVT.name = 'mRNA' and
        CV.name = 'sequence'
    ";
    $results = chado_query($sql, [':feature_id' => $record->feature_id]);
    $i = 0;
    while ($transcript = $results->fetchObject()) {
      // Get the location of this mRNA.
      $sql = "
        SELECT FL.*, F.name as srcfeature_name
        FROM {featureloc} FL
          INNER JOIN {feature} F on F.feature_id = FL.srcfeature_id
        WHERE FL.feature_id = :object_id
      ";
      $floc_results = chado_query($sql, [':object_id' => $transcript->feature_id]);
      $loc = "";
      while ($location = $floc_results->fetchObject()) {
        $loc .= $location->srcfeature_name . ":" . $location->fmin . ".." . $location->fmax;
      }
      $entity->{$field_name}['und'][$i]['value'] = [
        'rdfs:type' => $transcript->type,
        'schema:name' => $transcript->name,
        'data:0842' => $transcript->uniquename,
        'SO:0000735' => $loc,
      ];
      $feature = chado_generate_var('feature', ['feature_id' => $transcript->feature_id]);
      if (property_exists($feature, 'entity_id')) {
        $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $feature->entity_id;
      }
      $i++;
    }
  }

}