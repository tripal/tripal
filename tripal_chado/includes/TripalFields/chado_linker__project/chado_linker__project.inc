<?php

class chado_linker__project extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'projects';

  // The default description for this field.
  public static $description = 'Associates a project with this record';

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
    'term_name' => 'project',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'C47885',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_linker__project_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__project_formatter';

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
    $name_term = chado_get_semweb_term('project', 'name');
    $description_term = chado_get_semweb_term('project', 'description');

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
            'label' => 'project Name',
            'help' => 'The name of the project.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $description_term => [
            'searchable' => TRUE,
            'label' => 'project Description',
            'help' => 'A description of the project.',
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

    $name_term = chado_get_semweb_term('project', 'name');
    $description_term = chado_get_semweb_term('project', 'description');

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    if (!isset($schema['foreign keys'][$base_table]['columns'])) {
      return;
    }
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // If we don't have a chado record return before creating a stub for this field!
    if (!$record) {
      return;
    }

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_lcolumn,
      'chado-' . $field_table . '__' . 'project_id' => '',
    ];

    $linker_table = 'project_' . $base_table;
    if (!chado_table_exists($linker_table)) {
      $linker_table = $base_table . '_project';
    }
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'project_id' => [
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
    $project_linkers = $record->$linker_table;
    if ($project_linkers) {
      foreach ($project_linkers as $i => $project_linker) {
        // Expand description if necessary due to length.
        if (in_array('project.description', $project_linker->expandable_fields)) {
          $project_linker = chado_expand_var($project_linker, 'field', 'project.description', []);
        }
        $project = $project_linker->project_id;
        $entity->{$field_name}['und'][$i] = [
          'value' => [
            $name_term => $project->name,
            $description_term => $project->description,
          ],
          // Add elements that are not part of the values but used by
          // the chado storage back-end for saving values.
          'chado-' . $field_table . '__' . $pkey => $project_linker->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'project_id' => $project->project_id,
        ];

        if (property_exists($project, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $project->entity_id;
        }
      }
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $name_term = chado_get_semweb_term('project', 'name');
    $description_term = chado_get_semweb_term('project', 'description');

    $project_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($project_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the project linker table and then join the project table.
    $calias = 'project_' . $project_linker;
    if (!chado_table_exists($calias)) {
      $calias = $project_linker . '_project';
    }
    $this->queryJoinOnce($query, $project_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn");
    $this->queryJoinOnce($query, 'project', $calias, "$calias.project_id = $alias.project_id");

    // Search by the project name
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $field_term_id . ',' . $name_term) {
      $query->condition("$calias.name", $condition['value'], $operator);
    }
    // Search on the project description.
    if ($condition['column'] == $field_term_id . ',' . $description_term) {
      $query->condition("$calias.description", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];
    $field_term_id = $this->getFieldTermID();
    $name_term = chado_get_semweb_term('project', 'name');
    $description_term = chado_get_semweb_term('project', 'description');

    $project_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($project_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the project linker table and then join the project table.
    $calias = 'project_' . $project_linker;
    if (!chado_table_exists($calias)) {
      $calias = $project_linker . '_project';
    }
    $this->queryJoinOnce($query, $project_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn", "LEFT OUTER");
    $this->queryJoinOnce($query, 'project', $calias, "$calias.project_id = $alias.project_id", "LEFT OUTER");

    // Search by the project name
    if ($order['column'] == $field_term_id or
      $order['column'] == $field_term_id . ',' . $name_term) {
      $query->orderBy("$calias.name", $order['direction']);
    }
    // Search on the project description.
    if ($order['column'] == $field_term_id . ',' . $description_term) {
      $query->orderBy("$calias.description", $order['direction']);
    }
  }
}


/**
 * An Ajax callback for the pub widget.
 */
function chado_linker__project_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}

/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_chado_linker__project_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
    drupal_render($element['name']) . "
        </div>
      </div>
    ";

  return $layout;
}
