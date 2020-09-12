<?php

class ogi__location_on_map extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Location on Map';

  // The default description for this field.
  public static $description = 'Map position of a sequence.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'OGI',
    // The name of the term.
    'term_name' => 'location on map',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000021',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

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
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $map_term = 'data:1274';
    $name_term = chado_get_semweb_term('featuremap', 'name');
    $description_term = chado_get_semweb_term('featuremap', 'description');
    $mappos_term = chado_get_semweb_term('featurepos', 'mappos');
    $ref_feature_term = chado_get_semweb_term('featurepos', 'map_feature_id');
    $ref_feature_name = chado_get_semweb_term('feature', 'name');
    $ref_feature_id = chado_get_semweb_term('feature', 'uniquename');
    $ref_feature_type = 'rdfs:type';

    return [
      $field_term => [
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          $map_term => [
            'searchable' => FALSE,
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => TRUE,
            'required' => FALSE,
            'elements' => [
              $name_term => [
                'label' => 'Map Name',
                'help' => 'The name of the map.',
                'searchable' => TRUE,
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => TRUE,
                'required' => FALSE,
              ],
              $description_term => [
                'label' => 'Map Description',
                'help' => 'A description of the map.',
                'searchable' => TRUE,
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
          $ref_feature_term => [
            'searchable' => FALSE,
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => TRUE,
            'required' => FALSE,
            'elements' => [
              $ref_feature_name => [
                'label' => 'Map Reference Feature Name',
                'help' => 'The genomic or genetic feature Nameof the map on which this feature is mapped.',
                'searchable' => TRUE,
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => TRUE,
                'required' => FALSE,
              ],
              $ref_feature_id => [
                'label' => 'Map Reference Feature Identifier',
                'help' => 'The genomic or genetic feature of the map on which this feature is mapped.',
                'searchable' => TRUE,
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => TRUE,
                'required' => FALSE,
              ],
              $ref_feature_type => [
                'label' => 'Map Reference Feature Type',
                'help' => 'The type of genomic or genetic feature of the map on which this feature is mapped.',
                'searchable' => TRUE,
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => TRUE,
                'required' => FALSE,
              ],
            ],
          ],
          $mappos_term => [
            'label' => 'Map Reference Position',
            'help' => 'Maps may use different coordinate systems. This indicates the type of coordinate.',
            'searchable' => TRUE,
            'sortable' => TRUE,
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

    // Map details.
    $field_term_id = $this->getFieldTermID();
    $map_term = 'data:1274';
    $name_term = $field_term_id . ',' . $map_term . ',' . chado_get_semweb_term('featuremap', 'name');
    $description_term = $field_term_id . ',' . $map_term . ',' . chado_get_semweb_term('featuremap', 'description');
    $mappos_term = $field_term_id . ',' . chado_get_semweb_term('featurepos', 'mappos');

    // Reference sequence details.
    $ref_feature_term = chado_get_semweb_term('featurepos', 'map_feature_id');
    $ref_feature_name = $field_term_id . ',' . $ref_feature_term . ',' . chado_get_semweb_term('feature', 'name');
    $ref_feature_id = $field_term_id . ',' . $ref_feature_term . ',' . chado_get_semweb_term('feature', 'uniquename');
    $ref_feature_type = $field_term_id . ',' . $ref_feature_term . ',' . 'rdfs:type';

    $this->queryJoinOnce($query, 'featurepos', $alias, "base.feature_id = $alias.feature_id");

    if ($condition['column'] == $name_term) {
      $this->queryJoinOnce($query, 'featuremap', $alias . '_map', $alias . '_map.featuremap_id = ' . $alias . '.featuremap_id');
      $query->condition($alias . '_map.name', $condition['value'], $operator);
    }
    if ($condition['column'] == $description_term) {
      $this->queryJoinOnce($query, 'featuremap', $alias . '_map', $alias . '_map.featuremap_id = ' . $alias . '.featuremap_id');
      $query->condition($alias . '_map.description', $condition['value'], $operator);
    }
    if ($condition['column'] == $mappos_term) {
      $query->condition($alias . '.mappos', $condition['value'], $operator);
    }

    if ($condition['column'] == $ref_feature_name) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id');
      $query->condition($alias . '_mapref.name', $condition['value'], $operator);
    }
    if ($condition['column'] == $ref_feature_id) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id');
      $query->condition($alias . '_mapref.uniquename', $condition['value'], $operator);
    }
    if ($condition['column'] == $ref_feature_type) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id');
      $this->queryJoinOnce($query, 'cvterm', $alias . '_mapref_cvterm', $alias . '_mapref_cvterm.cvterm_id = ' . $alias . '_mapref.type_id');
      $query->condition($alias . '_mapref_cvterm.name', $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    // Map details.
    $field_term_id = $this->getFieldTermID();
    $map_term = 'data:1274';
    $name_term = $field_term_id . ',' . $map_term . ',' . chado_get_semweb_term('featuremap', 'name');
    $description_term = $field_term_id . ',' . $map_term . ',' . chado_get_semweb_term('featuremap', 'description');
    $mappos_term = $field_term_id . ',' . chado_get_semweb_term('featurepos', 'mappos');

    // Reference sequence details.
    $ref_feature_term = chado_get_semweb_term('featurepos', 'map_feature_id');
    $ref_feature_name = $field_term_id . ',' . $ref_feature_term . ',' . chado_get_semweb_term('feature', 'name');
    $ref_feature_id = $field_term_id . ',' . $ref_feature_term . ',' . chado_get_semweb_term('feature', 'uniquename');
    $ref_feature_type = $field_term_id . ',' . $ref_feature_term . ',' . 'rdfs:type';

    $this->queryJoinOnce($query, 'featurepos', $alias, "base.feature_id = $alias.feature_id", 'LEFT OUTER');

    if ($order['column'] == $name_term) {
      $this->queryJoinOnce($query, 'featuremap', $alias . '_map', $alias . '_map.featuremap_id = ' . $alias . '.featuremap_id', 'LEFT OUTER');
      $query->orderBy($alias . '_map.name', $order['direction']);
    }
    if ($order['column'] == $description_term) {
      $this->queryJoinOnce($query, 'featuremap', $alias . '_map', $alias . '_map.featuremap_id = ' . $alias . '.featuremap_id', 'LEFT OUTER');
      $query->orderBy($alias . '_map.description', $order['direction']);
    }
    if ($order['column'] == $mappos_term) {
      $query->orderBy($alias . '.mappos', $order['direction']);
    }

    if ($order['column'] == $ref_feature_name) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id', 'LEFT OUTER');
      $query->orderBy($alias . '_mapref.name', $order['direction']);
    }
    if ($order['column'] == $ref_feature_id) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id', 'LEFT OUTER');
      $query->orderBy($alias . '_mapref.uniquename', $order['direction']);
    }
    if ($order['column'] == $ref_feature_type) {
      $this->queryJoinOnce($query, 'feature', $alias . '_mapref', $alias . '_mapref.feature_id = ' . $alias . '.map_feature_id', 'LEFT OUTER');
      $this->queryJoinOnce($query, 'cvterm', $alias . '_mapref_cvterm', $alias . '_mapref_cvterm.cvterm_id = ' . $alias . '_mapref.type_id', 'LEFT OUTER');
      $query->orderBy($alias . '_mapref_cvterm.name', $order['direction']);
    }
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

    // Map details
    $map_term = 'data:1274';
    $name_term = chado_get_semweb_term('featuremap', 'name');
    $description_term = chado_get_semweb_term('featuremap', 'description');
    $mappos_term = chado_get_semweb_term('featurepos', 'mappos');

    // Reference sequence details.
    $ref_feature_term = chado_get_semweb_term('featurepos', 'map_feature_id');
    $ref_feature_name = chado_get_semweb_term('feature', 'name');
    $ref_feature_id = chado_get_semweb_term('feature', 'uniquename');
    $ref_feature_type = 'rdfs:type';

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    // Add the featurepos records to our $record.
    $options = [
      'return_array' => TRUE,
      'include_fk' => [
        'map_feature_id' => [
          'type_id' => 1,
        ],
        'feature_id' => [
          'type_id' => 1,
        ],
      ],
    ];
    $feature = chado_expand_var($record, 'table', 'featurepos', $options);

    // Iterate through the positions and add them to our values.
    $i = 0;
    if (isset($feature->featurepos->feature_id)) {
      foreach ($feature->featurepos->feature_id AS $featurepos) {
        // Get details about the map
        $featuremap = chado_generate_var('featuremap', ['featuremap_id' => $featurepos->featuremap_id]);
        $entity->{$field_name}['und'][$i]['value'] = [
          // Map.
          $map_term => [
            $name_term => $featuremap->name,
            $description_term => $featuremap->description,
          ],
          $ref_feature_term => [
            $ref_feature_name => $featurepos->map_feature_id->name,
            $ref_feature_id => $featurepos->map_feature_id->uniquename,
            $ref_feature_type => $featurepos->map_feature_id->type_id->name,
          ],
          $mappos_term => $featurepos->mappos,
        ];
        if (property_exists($featuremap, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value'][$map_term]['entity'] = 'TripalEntity:' . $featuremap->entity_id;
        }
        if (property_exists($featurepos->map_feature_id, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value'][$map_term]['entity'] = 'TripalEntity:' . $featurepos->map_feature_id->entity_id;
        }
        $i++;
      }
    }


    // If there are no map positions expanded above then remove the stub.
    // This is needed to ensure this field isn't displayed when there are no locations.
    if (!isset($feature->featurepos->feature_id) OR (sizeof($feature->featurepos->feature_id) == 0)) {
      $entity->{$field_name}['und'][0]['value'] = [];
    }
  }

}
