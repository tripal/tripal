<?php

class chado_linker__assay extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and its default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Assays';

  // The default uniquename for this field.
  public static $uniquename = 'Associates an assay with this record';

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
    'term_name' => 'Assay',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'C60819',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_linker__assay_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__assay_formatter';

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
    $name_term = chado_get_semweb_term('assay', 'name');
    $description_term = chado_get_semweb_term('assay', 'description');

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
            'label' => 'Assay Name',
            'help' => 'The name of the assay.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $description_term => [
            'searchable' => TRUE,
            'label' => 'Assay Description',
            'help' => 'A description of the assay.',
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

    $name_term = chado_get_semweb_term('assay', 'name');
    $description_term = chado_get_semweb_term('assay', 'description');
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

    // Linker table name may have assay as either prefix or suffix
    $linker_table = $base_table . '_assay';
    if (!chado_table_exists($linker_table)) {
      $linker_table = 'assay_' . $base_table;
    }
    if (!chado_table_exists($linker_table)) { 
      return;
    }

    // Evaluate how many linked records there are, and if too many just show a summary.
    $load_limit = variable_get('chado_linker_load_limit', 500);
    if (array_key_exists('chado_linker_load_limit', $this->instance['settings'])) {
      $load_limit = $this->instance['settings']['chado_linker_load_limit'];
    }
    $sql1 = 'SELECT COUNT(*) FROM {'.$linker_table.'} WHERE '.$fkey_lcolumn.' = :fkey_lcolumn_id';
    $args1 = [':fkey_lcolumn_id' => $record->$fkey_lcolumn];
    $count = chado_query($sql1, $args1)->fetchField();

    if ($count <= $load_limit) {
      // Since there could be many records, perform SQL query directly to reduce overhead.
      $sql2 = 'SELECT LT.'.$pkey.' AS pkey_value, A.assay_id, A.name, A.description'
            . ' FROM {'.$linker_table.'} LT'
            . ' LEFT JOIN {assay} A ON LT.assay_id = A.assay_id'
            . ' WHERE LT.'.$fkey_lcolumn.' = :assay_id'
            . ' ORDER BY LOWER(A.name)';
      $args2 = [':assay_id' => $record->$fkey_lcolumn];
      $linkers = chado_query($sql2, $args2);

      $i = 0;
      while ( $linker = $linkers->fetchObject() ) {
        $entity->{$field_name}['und'][$i] = [
          'value' => [
            $name_term => $linker->name,
            $description_term => $linker->description,
          ],
          // Add elements that are not part of the values but used by
          // the chado storage back-end for saving values.
          'chado-' . $field_table . '__' . $pkey => $linker->pkey_value,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'assay_id' => $linker->assay_id,
          // For the widget to work properly we will preform values.
          'assay_name' => $linker->name . ' [id: ' . $linker->assay_id . ']',
        ];
        $entity_id = chado_get_record_entity_by_table('assay', $linker->assay_id);
        if ($entity_id) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $entity_id;
        }
        $i++;
      }
    }
    else {
      // If too many records to display, just return a summary. The widget won't
      // show any existing records, but can be used to add additional records.
      // Web services will only show this summary.
      $content = t('There are :count related assays', [':count' => $count]);

      // The formatter will check for the $summary_term key, and when
      // present will display this summary and not display the usual table.
      $entity->{$field_name}['und'][0] = [
        'value' => [
          $summary_term => $content,
        ],
      ];
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $name_term = chado_get_semweb_term('assay', 'name');
    $description_term = chado_get_semweb_term('assay', 'description');

    $assay_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($assay_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the assay linker table and then join the assay table.
    $calias = 'assay_' . $assay_linker;
    if (!chado_table_exists($calias)) {
      $calias = $assay_linker . '_assay';
    }
    $this->queryJoinOnce($query, $assay_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn");
    $this->queryJoinOnce($query, 'assay', $calias, "$calias.assay_id = $alias.assay_id");

    // Search by the assay name
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $field_term_id . ',' . $name_term) {
      $query->condition("$calias.name", $condition['value'], $operator);
    }
    // Search on the assay description.
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
    $name_term = chado_get_semweb_term('assay', 'name');
    $description_term = chado_get_semweb_term('assay', 'description');

    $assay_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($assay_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the assay linker table and then join the assay table.
    $calias = 'assay_' . $assay_linker;
    if (!chado_table_exists($calias)) {
      $calias = $assay_linker . '_assay';
    }
    $this->queryJoinOnce($query, $assay_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn", "LEFT OUTER");
    $this->queryJoinOnce($query, 'assay', $calias, "$calias.assay_id = $alias.assay_id", "LEFT OUTER");

    // Search by the assay name
    if ($order['column'] == $field_term_id or
      $order['column'] == $field_term_id . ',' . $name_term) {
      $query->orderBy("$calias.name", $order['direction']);
    }
    // Search on the assay description.
    if ($order['column'] == $field_term_id . ',' . $description_term) {
      $query->orderBy("$calias.description", $order['direction']);
    }
  }
}


/**
 * An Ajax callback for the widget.
 */
function chado_linker__assay_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}

/**
 * Theme function for the widget.
 *
 * @param $variables
 */
function theme_chado_linker__assay_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"chado-linker--assay-widget\">
        <div class=\"chado-linker--assay-widget-item\">" .
    drupal_render($element['name']) . "
        </div>
      </div>
    ";

  return $layout;
}
