<?php

class ogi__location_on_map extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Location on Map';

  // The default description for this field.
  public static $description = 'Map position of a sequence.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'OGI',
    // The name of the term.
    'term_name' => 'location on map',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000021',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'ogi__location_on_map_widget';

  // The default formatter for this field.
  public static $default_formatter = 'ogi__location_on_map_formatter';

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

    $name_term = tripal_get_chado_semweb_term('featuremap', 'name');
    $description_term = tripal_get_chado_semweb_term('featuremap', 'description');
    $mappos_term = tripal_get_chado_semweb_term('featurepos', 'mappos');

    return array(
      $field_term => array(
        'sortable' => FALSE,
        'searchable' => FALSE,
        'elements' => array(
          'data:1274' => array(
            'name' => 'Map',
            'searchable' => FALSE,
            'sortable' => FALSE,
            'elements' => array(
              $name_term => array(
                'name' => 'map_name',
                'searchable' => TRUE,
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => FALSE,
              ),
              $description_term => array(
                'name' => 'map_description',
                'searchable' => TRUE,
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => FALSE,
              ),
            ),
          ),
          $mappos_term => array(
            'name' => 'Map',
            'searchable' => TRUE,
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => FALSE,
          ),
          'entity' => array(
            'searchable' => FALSE,
            'sortable' => FALSE,
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

    $record = $entity->chado_record;
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    $name_term = tripal_get_chado_semweb_term('featuremap', 'name');
    $description_term = tripal_get_chado_semweb_term('featuremap', 'description');
    $mappos_term = tripal_get_chado_semweb_term('featurepos', 'mappos');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    // Add the featurepos records to our $record.
    $options = array(
      'return_array' => TRUE,
      'include_fk' => array(
        'srcfeature_id' => array(
          'type_id' => 1,
        ),
        'feature_id' => array(
          'type_id' => 1
        ),
      )
    );
    $feature = chado_expand_var($record, 'table', 'featurepos', $options);

    // Iterate through the positions and add them to our values.
    $i = 0;
    if (isset($feature->featurepos->feature_id)) {
      foreach ($feature->featurepos->feature_id AS $featurepos) {
        // Get details about the map
        $featuremap = chado_generate_var('featuremap', array('featuremap_id' => $featurepos->featuremap_id));
        $value = array (
          // Map.
          'data:1274' => array(
            $name_term => $featuremap->name,
            $description_term => $featuremap->description,
          ),
          $mappos_term => $featurepos->mappos
        );
        if (property_exists($featuremap, 'entity_id')) {
          $value['data:1274']['entity'] = 'TripalEntity:' . $featuremap->entity_id;
        }
        $entity->{$field_name}['und'][$i]['value'] = $value;
        $i++;
      }
    }
  }

}
