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
    // Contains the list of fields on the transcript child that the
    // user wants to have displayed for the transcript.  The
    // `so__transcript_expanded_formatter will display these.
    'transcript_fields' => []
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

  /**
   * @see TripalField::globalSettingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();

    $settings = $this->instance['settings'];


    $element['transcript_fields'] = [
      '#type' => 'value',
      '#default_value' => $settings['transcript_fields'],
    ];

    // Get the content type for the mRNA term (SO:0000234) and its fields.
    // If it isn't published then don't show the form below.
    $bundle = tripal_load_bundle_entity(['accession' => 'SO:0000234']);
    if (!$bundle) {
      return $element;
    }

    $fields = field_info_instances('TripalEntity', $bundle->name);

    foreach ($fields as $field_name => $details) {

      $defaults = [];
      if (array_key_exists($field_name, $settings['transcript_fields'])) {
        $defaults = $settings['transcript_fields'][$field_name];
      }

      $weight = array_key_exists('weight', $defaults) ? $defaults['weight'] : $details['display']['default']['weight'];
      $checked =  array_key_exists('show', $defaults) ? $defaults['show'] : 0;
      $element['themeable']['field-'. $weight] = [
        '#type' => 'value',
        '#value' => $field_name,
        '#weight' => $weight,
      ];
      $element['themeable']['field-'. $weight . '-check'] = [
        '#type' => 'checkbox',
        '#weight' => $weight,
        '#default_value' => $checked,
      ];
      $element['themeable']['field-'. $weight . '-label'] = [
        '#type' => 'markup',
        '#markup' => t($details['label']),
        '#weight' => $weight,
      ];
      $element['themeable']['field-'. $weight . '-description'] = [
        '#type' => 'markup',
        '#markup' => t($details['description']),
        '#weight' => $weight,
      ];
      $element['themeable']['field-'. $weight . '-weight'] = [
        '#type' => 'textfield',
        '#default_value' => $weight,
        '#weight' => $weight,
        '#size' => 3,
        '#attributes' => [
          'class' => ['tripal-field-settings-table-weights'],
        ],
      ];
    }
    return $element;
  }

  /**
   * @see TripalField::instanceSettingsFormValidate()
   */
  public function instanceSettingsFormValidate($form, &$form_state) {
    $settings = $form_state['values']['instance']['settings']['themeable'];

    // Iterate through the themable elements to get the weights and
    // checks for each field. Store that info in a temp array.
    $temp = [];
    foreach ($settings as $element => $value) {
      $matches = [];
      if (preg_match('/^field-(\d+)-check$/', $element, $matches)) {
        $orig_weight = $matches[1];
        $new_weight = $settings['field-' . $orig_weight . '-weight'];
        $field_name = $settings['field-' . $orig_weight];
        $temp[$new_weight]['name'] = $field_name;
        $temp[$new_weight]['show'] = $value;
      }
    }

    // Sort and resstart the weights from zero.
    ksort($temp);
    $temp = array_values($temp);

    // Now build the transcirpt_fields setting value.
    $transcript_fields = [];
    foreach ($temp as $weight => $field) {
      $field_name = $field['name'];
      $transcript_fields[$field_name] = [
        'weight' => $weight,
        'show' => $field['show'],
      ];
    }

    // Now store the values for the setting properly and remove the unwanted.
    $form_state['values']['instance']['settings']['transcript_fields'] = $transcript_fields;
    unset($form_state['values']['instance']['settings']['themeable']);
  }

  /**
   * @see TripalField::settingsTheme()
   */
  public function settingsTheme($element) {

    $headers = ['', 'Field', 'Description', ''];
    $rows = [];
    foreach (element_children($element) as $child) {
      $matches = [];
      if (preg_match('/^field-(\d+)-(.+)$/', $child, $matches)) {
        $weight = $matches[1];
        switch ($matches[2]) {
          case 'check':
            $rows[$weight]['data'][0] = drupal_render($element[$child]);
            break;
          case 'label':
            $rows[$weight]['data'][1] = drupal_render($element[$child]);
            break;
          case 'description':
            $rows[$weight]['data'][2] = drupal_render($element[$child]);
            break;
          case 'weight':
            $rows[$weight]['data'][3] = drupal_render($element[$child]);
            break;
        }
        ksort($rows[$weight]['data']);
        $rows[$weight]['class'] = ['draggable'];
      }
    }
    ksort($rows);

    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => ["id" => 'tripal-field-settings-table'],
      'sticky' => TRUE,
      'caption' => t('Content Panes Available in the TOC'),
      'colgroups' => [],
      'empty' => t('There are no content panes for this page. The mRNA content type must exist to customize.'),
    ];
    drupal_add_tabledrag('tripal-field-settings-table', 'order', 'sibling', 'tripal-field-settings-table-weights');

    $fset = [];
    $fset['fields'] = [
      '#type' => 'fieldset',
      '#title' => 'Transcript (mRNA) Field Selection',
      '#description' => t('Here, you can customize what information is provided to the end-user about transcripts.  Select the fields that you want displayed on the page.  If no fields are selected then a default display is provided. All mRNA for a gene must be published or the default display will be provided.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => ['class' => ['collapsible', 'collapsed']],
    ];
    drupal_add_library('system', 'drupal.collapse');

    $fset['fields']['table'] = [
      '#type' => 'markup',
      '#markup' => theme_table($table),
    ];

    return drupal_render($fset['fields']);
  }

}
