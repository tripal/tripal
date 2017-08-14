<?php

class local__contact extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Contact';

  // The default description for this field.
  public static $description = 'An indviddual or organization that serves as a contact for this record.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'contact',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'contact',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );


  // The default widget for this field.
  public static $default_widget = 'local__contact_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__contact_formatter';

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
    $type_term = tripal_get_chado_semweb_term('contact', 'type_id');
    $name_term = tripal_get_chado_semweb_term('contact', 'name');
    $description_term = tripal_get_chado_semweb_term('contact', 'description');

    return array(
      $field_term => array(
        'operations' => array('eq', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'string',
        'elements' => array(
          $type_term => array(
            'searchable' => TRUE,
            'label' => 'Contact Type',
            'help' => 'The type of contact',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $name_term => array(
            'searchable' => TRUE,
            'label' => 'Contact Name',
            'help' => 'The name of the contact.',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $description_term => array(
            'searchable' => TRUE,
            'label' => 'Contact Description',
            'help' => 'A descriptoin of the contact.',
            'operations' => array('contains'),
            'sortable' => TRUE,
          ),
          'entity' => array(
            'searchable' => FALSE,
          ),
        ),
      )
    );
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

    $type_term = tripal_get_chado_semweb_term('contact', 'type_id');
    $name_term = tripal_get_chado_semweb_term('contact', 'name');
    $description_term = tripal_get_chado_semweb_term('contact', 'description');


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    // Handle the biomaterial table.
    if ($field_table == 'biomaterial') {
      if ($record) {
        $contact = $record->biosourceprovider_id;
        $entity->{$field_name}['und'][0] = array(
          'value' => array(
            $type_term => $contact->type_id ? $contact->type_id->name : '',
            $name_term => $contact->name,
            $description_term => $contact->description,
          ),
          $entity->{$field_name}['und'][0]['chado-biomaterial__biosourceprovider_id'] = $contact->contact_id,
        );
        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
        }
      }
    };
    // Here place other non-linker tables that have a FK to the contact table.
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $type_term = tripal_get_chado_semweb_term('contact', 'type_id');
    $name_term = tripal_get_chado_semweb_term('contact', 'name');
    $description_term = tripal_get_chado_semweb_term('contact', 'description');

    if ($field_table == 'biomaterial') {
      if ($record) {
        $contact = $record->biosourceprovider_id;

        // Join the contact table
        $calias = $alias . '_provider_id';
        $this->queryJoinOnce($query, 'contact', $calias, "base.biosourceprovider_id = $calias.contact_id");

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
          $talias = $alias . 'provider_contact_type';
          $this->queryJoinOnce($query, 'cvterm', $talias, "$calias.type_id = $talias.cvterm_id");
          $query->condition("$talias.name", $condition['value'], $operator);
        }
      }
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];
    $field_term_id = $this->getFieldTermID();
    $type_term = tripal_get_chado_semweb_term('contact', 'type_id');
    $name_term = tripal_get_chado_semweb_term('contact', 'name');
    $description_term = tripal_get_chado_semweb_term('contact', 'description');

    if ($field_table == 'biomaterial') {
      if ($record) {
        $contact = $record->biosourceprovider_id;

        // Join the contact linker table and then join the contact table.
        $calias = $alias . '_provider_id';
        $this->queryJoinOnce($query, 'contact', $calias, "base.biosourceprovider_id = $calias.contact_id");

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
          $talias = $alias . 'provider_contact_type';
          $this->queryJoinOnce($query, 'cvterm', $talias, "$calias.type_id = $talias.cvterm_id", "LEFT OUTER");
          $query->orderBy("$talias.name", $order['direction']);
        }
      }
    }
  }
}