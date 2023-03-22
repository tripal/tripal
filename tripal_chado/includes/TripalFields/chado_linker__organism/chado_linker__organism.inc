<?php

class chado_linker__organism extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and its default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Organisms';

  // The default description for this field.
  public static $description = 'Associates an organism with this record';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'NCIT',
    // The name of the term.
    'term_name' => 'Organism',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'C14250',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_linker__organism_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__organism_formatter';

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
   * @see TripalField::elements()
   */
  public function elementInfo() {

    $field_term = $this->getFieldTermID();
    // Because the scientific name is constructed from multiple table
    // columns, it requires its own cv term.
    $name_term = 'data:2909'; // EDAM:Organism name
    $common_name_term = chado_get_semweb_term('organism', 'common_name');
    $abbreviation_term = chado_get_semweb_term('organism', 'abbreviation');
    $comment_term = chado_get_semweb_term('organism', 'comment');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $name_term => [
            'searchable' => TRUE,
            'label' => 'Organism Name',
            'help' => 'The name of the organism.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $common_name_term => [
            'searchable' => TRUE,
            'label' => 'Organism Common Name',
            'help' => 'The common name of the organism.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $abbreviation_term => [
            'searchable' => TRUE,
            'label' => 'Organism Abbreviation',
            'help' => 'The abbreviation of the name of the organism.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $comment_term => [
            'searchable' => TRUE,
            'label' => 'Organism Comments',
            'help' => 'Additional description of the organism.',
            'operations' => ['contains'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
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
    $base_table = $this->instance['settings']['base_table'];

    // Because the scientific name is constructed from multiple table
    // columns, it requires its own cv term.
    $name_term = 'data:2909'; // EDAM:Organism name
    $common_name_term = chado_get_semweb_term('organism', 'common_name');
    $abbreviation_term = chado_get_semweb_term('organism', 'abbreviation');
    $comment_term = chado_get_semweb_term('organism', 'comment');
    $summary_term = 'NCIT:C25463'; // ncit:Count

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    if (!isset($schema['foreign keys'][$base_table]['columns'])) {
      return;
    }
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // If we don't have a chado record, return before creating a stub for this field!
    if (!$record) {
      return;
    }

    // Linker table name may have organism as either prefix or suffix.
    $linker_table = 'organism_' . $base_table;
    if (!chado_table_exists($linker_table)) {
      $linker_table = $base_table . '_organism';
    }

    if (!chado_table_exists($linker_table)) {
      return;
    }

    // Evaluate how many linked records there are, and if too many just show a summary.
    $tripal_field_max_delta = variable_get('tripal_field_max_delta', 200);
    if (array_key_exists('tripal_field_max_delta', $this->instance['settings'])) {
      $tripal_field_max_delta = $this->instance['settings']['tripal_field_max_delta'];
    }
    $sql1 = 'SELECT COUNT(*) FROM {'.$linker_table.'} WHERE '.$fkey_lcolumn.' = :fkey_lcolumn_id';
    $args1 = [':fkey_lcolumn_id' => $record->$fkey_lcolumn];
    $count = chado_query($sql1, $args1)->fetchField();

    if ($count <= $tripal_field_max_delta) {
      $options = [
        'return_array' => 1,
        'include_fk' => [
          'organism_id' => [
            'type_id' => [
              'dbxref_id' => [
                'db_id' => TRUE,
              ],
            ],
          ],
          $fkey_lcolumn => TRUE,
        ],
      ];
      $record = chado_expand_var($record, 'table', $linker_table, $options);
      $organism_linkers = $record->$linker_table;
      if ($organism_linkers) {
        foreach ($organism_linkers as $i => $organism_linker) {
          // Expand comment if necessary due to length.
          if (in_array('organism.comment', $organism_linker->expandable_fields)) {
            $organism_linker = chado_expand_var($organism_linker, 'field', 'organism.comment', []);
          }
          // Organism is a bit of a special case to construct the displayed
          // name, use the api function for this.
          $organism = $organism_linker->organism_id;
          $scientific_name = chado_get_organism_scientific_name($organism);
          $entity->{$field_name}['und'][$i] = [
            'value' => [
              $name_term => $scientific_name,
              $common_name_term => $organism->common_name,
              $abbreviation_name_term => $organism->abbreviation,
              $comment_term => $organism->comment,
            ],
            // Add elements that are not part of the values but used by
            // the chado storage back-end for saving values.
            'chado-' . $field_table . '__' . $pkey => $organism_linker->$pkey,
            'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_lcolumn,
            'chado-' . $field_table . '__' . 'organism_id' => $organism->organism_id,
            // For the widget to work properly we will preform values.
            'organism_name' => $scientific_name . ' [id: ' . $organism->organism_id . ']',
          ];

          if (property_exists($organism, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $organism->entity_id;
          }
        }
      }
    }
    else {
      // If too many records to display, just return a summary. The widget won't
      // show any existing records, but can be used to add additional records.
      // Web services will only show this summary.
      $content = t('There are :count related organisms', [':count' => $count]);

      // The formatter will check for the $summary_term key, and when
      // present will display this summary and not display the usual table.
      $entity->{$field_name}['und'][0] = [
        'value' => [
          $summary_term => $content,
        ],
      ];
    }
  }
}


/**
 * An Ajax callback for the widget.
 */
function chado_linker__organism_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}

/**
 * Theme function for the widget.
 *
 * @param $variables
 */
function theme_chado_linker__organism_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"chado-linker--organism-widget\">
        <div class=\"chado-linker--organism-widget-item\">" .
    drupal_render($element['name']) . "
        </div>
      </div>
    ";

  return $layout;
}
