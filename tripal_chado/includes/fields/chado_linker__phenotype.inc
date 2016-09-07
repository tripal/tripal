<?php

class chado_linker__phenotype extends TripalField {

  /**
   * @see TripalField::fieldDefaults()
   */
  public static function fieldDefaults() {
    return array(
      'label' => t('Phenotypes'),
      'description' => t('Associates an indviddual or organization with
          this record.'),
      'default_widget' => 'chado_linker__phenotype_widget',
      'default_formatter' => 'chado_linker__phenotype_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_linker__phenotype_widget' => array(
        'label' => t('Phenotypes'),
        'field types' => array('chado_linker__phenotype'),
      ),
    );
  }

  /**
   * @see TripalField::formatterInfo()
   */
  public static function formatterInfo() {
    return array(
      'chado_linker__phenotype_formatter' => array(
        'label' => t('Phenotypes'),
        'field types' => array('chado_linker__phenotype'),
        'settings' => array(
        ),
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
    $cvterm_id  = $this->details['chado_cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $phenotype_table = $table_name . '_phenotype';
    if (chado_table_exists($phenotype_table)) {
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

    $this->field_name = $table_name . '_phenotype';
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

    $phenotype_table = $table_name . '_phenotype';
    $schema = chado_get_schema($phenotype_table);
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_linker__phenotype',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $phenotype_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'SBO:0000358',
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
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'entity_type' => $this->entity_type,
      'bundle' => $this->bundle->name,
      'label' => 'Phenotypes',
      'description' => 'The phenotypes associated with this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__phenotype_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'chado_linker__phenotype_formatter',
          'settings' => array(),
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

    $headers = array('Name', 'Value', 'Type');
    $rows = array();

    foreach ($items as $delta => $item) {
      $phenotype = $item['value'];
      if (!$phenotype) {
        continue;
      }

      // Get the field values
      $phenotype_name = $phenotype['name'];
      $value = $phenotype['value'];
      $type = $phenotype['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $phenotype_name = l($phenotype_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($phenotype_name, $value, $type);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-phenotype-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'No phenotypes available',
    );
    $content = theme_table($table);

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
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
      $field_table . '__' . 'phenotype_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_phenotype';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'phenotype_id' => array(
          'attr_id' => array(
            'dbxref_id' => array(
              'db_id' => TRUE,
            ),
          ),
        ),
        $fkey_lcolumn => TRUE,
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $phenotype_linkers = $record->$linker_table;
    if ($phenotype_linkers) {
      foreach ($phenotype_linkers as $i => $phenotype_linker) {
        $phenotype = $phenotype_linker->phenotype_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
       '@type' => $phenotype->attr_id->dbxref_id->db_id->name . ':' . $phenotype->attr_id->dbxref_id->accession,
            'type' => $phenotype->attr_id->name,
            'name' => $phenotype->name,
            'value' => $phenotype->value,
          ),
          $field_table . '__' . $pkey => $phenotype_linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $phenotype_linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'phenotype_id' => $phenotype->phenotype_id
        );

        if ($phenotype && property_exists($phenotype, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $phenotype->entity_id;
        }
      }
    }
  }
}