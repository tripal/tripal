<?php

class data__sequence_coordinates extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Sequence Coordinates';

  // The default description for this field.
  public static $description = 'Locations on reference sequences where the feature is located.';

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
    'term_name' => 'sequence_coordinates',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2012',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

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
  protected function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'operations' => array(),
        'sortable' => FALSE,
        'searchable' => FALSE,
        'elements' => array(
          'data:3002' => array(
            'searchable' => TRUE,
            'name' => 'source_feature',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          'local:fmin' => array(
            'searchable' => TRUE,
            'name' => 'feature min',
            'type' => 'numeric',
            'operations' => array('eq', 'gt', 'lt', 'gte' ,'lte'),
            'sortable' => TRUE,
          ),
          'local:fmax' => array(
            'searchable' => TRUE,
            'name' => 'feature max',
            'type' => 'numeric',
            'operations' => array('eq', 'gt', 'lt', 'gte' ,'lte'),
            'sortable' => TRUE,
          ),
          'data:2336' => array(
            'searchable' => TRUE,
            'name' => 'phase',
            'type' => 'numeric',
            'operations' => array('eq', 'gt', 'lt', 'gte' ,'lte'),
            'sortable' => TRUE,
          ),
          'data:0853' => array(
            'searchable' => TRUE,
            'name' => 'strand',
            'type' => 'numeric',
            'operations' => array('eq', 'gt', 'lt', 'gte' ,'lte'),
            'sortable' => TRUE,
          ),
        ),
      ),
    );
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;
    $num_seqs = 0;

    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC'),
    );
    $feature = chado_expand_var($feature, 'table', 'featureloc', $options);

    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        /*
        // Types of elements that will appear in the value array.
        // Annotation track
        'data:3002' => '',
        'local:fmin' => '',
        'local:fmax' => '',
        // phase
        'data:2336' => '',
        // strand
        'data:0853' => '',
        */
      ),
    );

    // Get the featureloc records that this feature is aligned to.
    $aligned = $feature->featureloc->feature_id;
    if ($aligned) {
      foreach ($aligned as $index => $featureloc) {
        $srcfeature = $featureloc->srcfeature_id->name;
        $entity->{$field_name}['und'][0] = array(
          'value' => array(
            'data:3002' => $srcfeature,
            'local:fmin' => $featureloc->fmin + 1,
            'local:fmax' => $featureloc->fmax,
            'data:2336' => $featureloc->phase,
            'data:0853' => $featureloc->strand,
          ),
        );
        $sentity_id = chado_get_record_entity_by_table('feature_id', $featureloc->srcfeature_id->feature_id);
        if ($sentity_id) {
          $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $sentity_id;
        }
      }
    }
  }
}