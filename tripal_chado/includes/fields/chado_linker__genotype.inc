<?php

class chado_linker__genotype extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  public function field_info() {
    return array(
      'label' => t('Genotypes'),
      'description' => t('Associates an indviddual or organization with
          this record.'),
      'default_widget' => 'chado_linker__genotype_widget',
      'default_formatter' => 'chado_linker__genotype_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Genotypes'),
      'field types' => array('chado_linker__genotype'),
    );
  }

  /**
   * @see TripalField::formatter_info()
   */
  public function formatter_info() {
    return array(
      'label' => t('Genotypes'),
      'field types' => array('chado_linker__genotype'),
      'settings' => array(
      ),
    );
  }
  /**
   * @see TripalField::can_attach()
   */
  protected function can_attach($entity_type, $bundle, $details) {
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $genotype_table = $table_name . '_genotype';
    if (chado_table_exists($genotype_table)) {
      return TRUE;
    }
    return FALSE;
  }
  /**
   * @see TripalField::create_info()
   */
  function create_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    $genotype_table = $table_name . '_genotype';
    $schema = chado_get_schema($genotype_table);
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $table_name . '__genotype',
      'type' => 'chado_feature__md5checksum',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $genotype_table,
        'chado_column' => $pkey,
        'semantic_web' => 'SO:0001027',
        'base_table' => $table_name,
      ),
    );
  }
  /**
   * @see TripalField::create_instance_info()
   */
  function create_instance_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__genotype',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Genotypes',
      'description' => 'The genotypes associated with this record.',
      'required' => FALSE,
      'settings' => array(),
      'widget' => array(
        'type' => 'chado_linker__genotype_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_linker__genotype_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
        // Get the settings
        $settings = $display['settings'];
        $record = $entity->chado_record;

        $headers = array('Name', 'Description', 'Type');
        $rows = array();

        foreach ($items as $delta => $item) {
          $genotype = $item['value'];
          if (!$genotype) {
            continue;
          }

          // Get the field values
          $genotype_name = $genotype['name'];
          $description = $genotype['description'];
          $type = $genotype['type'];

          // Add a link i there is an entity.
          if (array_key_exists('entity_id', $item) and $item['entity_id']) {
            $entity_id = $item['entity_id'];
            $genotype_name = l($genotype_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
          }
          $rows[] = array($genotype_name, $description, $type);
        }
        $table = array(
          'header' => $headers,
          'rows' => $rows,
          'attributes' => array(
            'id' => 'tripal_linker-table-genotype-object',
            'class' => 'tripal-data-table'
          ),
          'sticky' => FALSE,
          'caption' => "",
          'colgroups' => array(),
          'empty' => 'No genotypes available',
        );
        $content = theme_table($table);

        // once we have our table array structure defined, we call Drupal's theme_table()
        // function to generate the table.
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
  }


  /**
   * @see TripalField::widget_form()
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

  /**
   * @see TripalField::load()
   */
  public function load($field, $entity, $details) {
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
      $field_table . '__' . 'genotype_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_genotype';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'genotype_id' => array(
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
    $genotype_linkers = $record->$linker_table->$fkey_rcolumn;
    if ($genotype_linkers) {
      foreach ($genotype_linkers as $i => $genotype_linker) {
        $genotype = $genotype_linker->genotype_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            '@type' => $genotype->type_id->dbxref_id->db_id->name . ':' . $genotype->type_id->dbxref_id->accession,
            'type' => $genotype->type_id->name,
            'name' => $genotype->name,
            'description' => $genotype->description,
          ),
          $field_table . '__' . $pkey => $genotype_linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $genotype_linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'genotype_id' => $genotype->genotype_id
        );

        if ($genotype && property_exists($genotype, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['entity_id'] = $genotype->entity_id;
          $entity->{$field_name}['und'][$i]['entity_type'] = 'TripalEntity';
        }
      }
    }
  }
}