<?php

class chado_linker__contact extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Contacts';

  // The default description for this field.
  public static $description = 'Associates an indviddual or organization with
          this record';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'contact',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'contact',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_linker__contact_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__contact_formatter';

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
    $type_term = chado_get_semweb_term('contact', 'type_id');
    $name_term = chado_get_semweb_term('contact', 'name');
    $description_term = chado_get_semweb_term('contact', 'description');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $type_term => [
            'searchable' => TRUE,
            'label' => 'Contact Type',
            'help' => 'The type of contact',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $name_term => [
            'searchable' => TRUE,
            'label' => 'Contact Name',
            'help' => 'The name of the contact.',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $description_term => [
            'searchable' => TRUE,
            'label' => 'Contact Description',
            'help' => 'A descriptoin of the contact.',
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

    $type_term = chado_get_semweb_term('contact', 'type_id');
    $name_term = chado_get_semweb_term('contact', 'name');
    $description_term = chado_get_semweb_term('contact', 'description');

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
      'chado-' . $field_table . '__' . 'contact_id' => '',
      // Ignore the synonym_sgml column for now.
    ];

    $linker_table = $base_table . '_contact';
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'contact_id' => [
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
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i] = [
          'value' => [
            $type_term => $contact->type_id ? $contact->type_id->name : '',
            $name_term => $contact->name,
            $description_term => $contact->description,
          ],
          // Add elements that are not part of the values but used by
          // the chado storage back-end for saving values.
          'chado-' . $field_table . '__' . $pkey => $contact_linker->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'contact_id' => $contact->contact_id,
        ];

        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
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
    $type_term = chado_get_semweb_term('contact', 'type_id');
    $name_term = chado_get_semweb_term('contact', 'name');
    $description_term = chado_get_semweb_term('contact', 'description');

    $contact_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($contact_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the contact linker table and then join the contact table.
    $calias = $contact_linker . '_contact';
    $this->queryJoinOnce($query, $contact_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn");
    $this->queryJoinOnce($query, 'contact', $calias, "$calias.contact_id = $alias.contact_id");

    // Search by the contact name
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $field_term_id . ',' . $name_term) {
      $query->condition("$calias.name", $condition['value'], $operator);
    }
    // Search on the contact description.
    if ($condition['column'] == $field_term_id . ',' . $description_term) {
      $query->condition("$calias.description", $condition['value'], $operator);
    }
    // Search on the contact type.
    if ($condition['column'] == $field_term_id . ',' . $type_term) {
      $talias = $contact_linker . '_contact_type';
      $this->queryJoinOnce($query, 'cvterm', $talias, "$calias.type_id = $talias.cvterm_id");
      $query->condition("$talias.name", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];
    $field_term_id = $this->getFieldTermID();
    $type_term = chado_get_semweb_term('contact', 'type_id');
    $name_term = chado_get_semweb_term('contact', 'name');
    $description_term = chado_get_semweb_term('contact', 'description');

    $contact_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($contact_linker);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Join the contact linker table and then join the contact table.
    $calias = $contact_linker . '_contact';
    $this->queryJoinOnce($query, $contact_linker, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn", "LEFT OUTER");
    $this->queryJoinOnce($query, 'contact', $calias, "$calias.contact_id = $alias.contact_id", "LEFT OUTER");

    // Search by the contact name
    if ($order['column'] == $field_term_id or
      $order['column'] == $field_term_id . ',' . $name_term) {
      $query->orderBy("$calias.name", $order['direction']);
    }
    // Search on the contact description.
    if ($order['column'] == $field_term_id . ',' . $description_term) {
      $query->orderBy("$calias.description", $order['direction']);
    }
    // Search on the contact type.
    if ($order['column'] == $field_term_id . ',' . $type_term) {
      $talias = $contact_linker . '_contact_type';
      $this->queryJoinOnce($query, 'cvterm', $talias, "$calias.type_id = $talias.cvterm_id", "LEFT OUTER");
      $query->orderBy("$talias.name", $order['direction']);
    }
  }
}


/**
 * An Ajax callback for the pub widget.
 */
function chado_linker__contact_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}

/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_chado_linker__contact_widget($variables) {
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