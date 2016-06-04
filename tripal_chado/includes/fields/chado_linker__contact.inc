<?php

class chado_linker__contact extends TripalField {

  /**
   * @see TripalField::fieldInfo()
   */
  public static function fieldInfo() {
    return array(
      'label' => t('Contacts'),
      'description' => t('Associates an indviddual or organization with
          this record.'),
      'default_widget' => 'chado_linker__contact_widget',
      'default_formatter' => 'chado_linker__contact_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::can_attach()
   */
  protected function setCanAttach() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];;

    // If the linker table does not exists then we don't want to add attach.
    $contact_table = $table_name . '_contact';
    if (chado_table_exists($contact_table)) {
      $this->can_attach = TRUE;
      return;
    }
    $this->can_attach = FALSE;
  }

  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $this->field_name = $table_name . '_contact';
  }

  /**
   * @see TripalField::create_info()
   */
  function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $contact_table = $table_name . '_contact';
    $schema = chado_get_schema($contact_table);
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_linker__contact',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $contact_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'local:contact'
      ),
    );
  }
  /**
   * @see TripalField::createInstanceInfo()
   */
  function createInstanceInfo() {
    if (!$this->can_attach) {
      return;
    }
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];;

    $contact_table = $table_name . '_contact';

    return array(
      'field_name' => $this->field_name,
      'entity_type' => $this->entity_type,
      'bundle' => $this->bundle->name,
      'label' => 'Contacts',
      'description' => 'An individual, organization or entity that has had
        some responsibility for the creation, delivery or maintenance of
        the associated data.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__contact_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'chado_linker__contact_formatter',
          'settings' => array(),
        ),
      ),
    );
  }
  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_linker__contact_widget' => array(
        'label' => t('Contacts'),
        'field types' => array('chado_linker__contact'),
      ),
    );
  }

  /**
   * @see TripalField::formatterInfo()
   */
  public static function formatterInfo() {
    return array(
      'chado_linker__contact_formatter' => array(
        'label' => t('Contacts'),
        'field types' => array('chado_linker__contact'),
        'settings' => array(
        ),
      ),
    );
  }


  /**
   * @see TripalField::formatterView()
   */
  public static function formatterView(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];
    $record = $entity->chado_record;

    $headers = array('Name', 'Description', 'Type');
    $rows = array();

    foreach ($items as $delta => $item) {
      $contact = $item['value'];
      if (!$contact) {
        continue;
      }

      // Get the field values
      $contact_name = $contact['name'];
      $description = $contact['description'];
      $type = $contact['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $contact_name = l($contact_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($contact_name, $description, $type);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-contact-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'No contacts available',
    );
    $content = theme_table($table);

    if (count($items) > 0) {
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }


  /**
   * @see TripalField::widgetForm()
   */
  public static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

  /**
   * @see TripalField::load()
   */
  static function load($field, $entity, $details = array()) {

    $record = $details['record'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'contact_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_contact';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'contact_id' => array(
          'type_id' => array(
            'dbxref_id' => array(
              'db_id' => TRUE,
            ),
          ),
        ),
        $fkey_lcolumn => TRUE,
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'type' => $contact->type_id->name,
            'name' => $contact->name,
            'description' => $contact->description,
          ),
          'semantic_web' => array(
            'type' => $contact->type_id->dbxref_id->db_id->name . ':' . $contact->type_id->dbxref_id->accession,
            'name' => tripal_get_chado_semweb_term('contact', 'name'),
            'description' => tripal_get_chado_semweb_term('contact', 'description'),
          ),
          $field_table . '__' . $pkey => $contact_linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $contact_linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'contact_id' => $contact->contact_id
        );

        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
        }
      }
    }
  }
}