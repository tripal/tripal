<?php

class chado_linker__phenotype extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  public function field_info() {
    return array(
      'label' => t('Genotypes'),
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
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Genotypes'),
      'field types' => array('chado_linker__phenotype'),
    );
  }

  /**
   * @see TripalField::formatter_info()
   */
  public function formatter_info() {
    return array(
      'label' => t('Genotypes'),
      'field types' => array('chado_linker__phenotype'),
      'settings' => array(
      ),
    );
  }
  /**
   * @see TripalField::attach_info()
   */
  public function attach_info($entity_type, $bundle, $settings) {
    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $phenotype_table = $table_name . '_phenotype';
    if (!chado_table_exists($phenotype_table)) {
      return $field_info;
    }

    $schema = chado_get_schema($phenotype_table);
    $pkey = $schema['primary key'][0];

    // Initialize the field array.
    $field_info = array(
      'field_name' => $table_name . '__phenotype',
      'field_type' => 'chado_linker__phenotype',
      'widget_type' => 'chado_linker__phenotype_widget',
      'widget_settings' => array('display_label' => 1),
      'description' => '',
      'label' => 'Genotypes',
      'is_required' => 0,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'storage' => 'field_chado_storage',
      'field_settings' => array(
        'chado_table' => $phenotype_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => array(
          'name' => 'phenotype',
          'accession' => 'phenotype',
          'ns' => 'local',
          'nsurl' => '',
        ),
      ),
    );
    return $field_info;
  }

  /**
   * @see TripalField::formatter_view()
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
        // Get the settings
        $settings = $display['settings'];
        $record = $entity->chado_record;

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
          if (array_key_exists('entity_id', $item) and $item['entity_id']) {
            $entity_id = $item['entity_id'];
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
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
  }


  /**
   * @see TripalField::widget_form()
   */
  public function widget_form(&$widget, $form, $form_state, $field, $instance,
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
          $entity->{$field_name}['und'][$i]['entity_id'] = $phenotype->entity_id;
          $entity->{$field_name}['und'][$i]['entity_type'] = 'TripalEntity';
        }
      }
    }
  }
}