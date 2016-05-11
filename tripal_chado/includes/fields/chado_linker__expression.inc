<?php

class chado_linker__expression extends TripalField {

  public function field_info() {
    return array(
      'label' => t('Expression'),
      'description' => t('Associates an expression with
          this record.'),
      'default_widget' => 'chado_linker__expression_widget',
      'default_formatter' => 'chado_linker__expression_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
 function widget_info() {
    return array(
      'label' => t('Expressions'),
      'field types' => array('chado_linker__expression'),
    );
  }

  public function formatter_info() {
    return array(
      'label' => t('Expression'),
      'field types' => array('chado_linker__expression'),
      'settings' => array(
      ),
    );
  }
  public function attach_info($entity_type, $bundle, $settings) {
    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $expression_table = $table_name . '_expression';
    if (!chado_table_exists($expression_table)) {
      return $field_info;
    }

    $schema = chado_get_schema($expression_table);
    $pkey = $schema['primary key'][0];

    // Initialize the field array.
    $field_info = array(
      'field_name' => $table_name . '__expression',
      'field_type' => 'chado_linker__expression',
      'widget_type' => 'chado_linker__expression_widget',
      'widget_settings' => array('display_label' => 1),
      'description' => '',
      'label' => 'Expression',
      'is_required' => 0,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'storage' => 'field_chado_storage',
      'field_settings' => array(
        'chado_table' => $expression_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => array(
          'name' => '',
          'accession' => '',
          'ns' => '',
          'nsurl' => '',
        ),
      ),
    );
    return $field_info;
  }

  public function formatter_settings_summary($field, $instance, $view_mode) {

  }

  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }


  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
        // Get the settings
        $settings = $display['settings'];
        $record = $entity->chado_record;
        
        $headers = array('Uniquename', 'Description', 'Publication');
        $rows = array();
        foreach ($items as $delta => $item) {
        
          $expression = $item['value'];
        
          // Get the field values
          $expression_uname = $expression['uniquename'];
          $description = $expression['description'];
          $pub = $expression['publication'];
        
          // Add a link i there is an entity.
          if (array_key_exists('entity_id', $expression) and $expression['$entity_id']) {
            $entity_id = $expression['entity_id'];
            $expression_uname = l($expression_uname, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
          }
          $rows[] = array($expression_uname, $description, $pub->title);
        }
        $table = array(
          'header' => $headers,
          'rows' => $rows,
          'attributes' => array(
            'id' => 'tripal_linker-table-expression-object',
            'class' => 'tripal-data-table'
          ),
          'sticky' => FALSE,
          'caption' => "",
          'colgroups' => array(),
          'empty' => '',
        );
        $content = theme_table($table);
        
        // once we have our table array structure defined, we call Drupal's theme_table()
        // function to generate the table.
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
  }


  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

  public function load($field, $entity, $details) {
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

    $linker_table = $base_table . '_expression';
    $options = array(
      'return_array' => 1,
    );
    $details = chado_expand_var($details, 'table', $linker_table, $options);
    $record = $details['record'];
    $exp = $record->$linker_table;
    $exp = is_array($exp) ? $exp : array($exp);
    if (count($exp) > 0) {
      $i = 0;
      foreach ($exp as $index => $linker) {
        $expression = $linker->expression_id;
        $entity->{$field_name}['und'][$i]['value'] = array(
          'expression_id' => $expression->expression_id,
          'uniquename' => $expression->uniquename,
          'description' => $expression->description,
          'publication' => $linker->pub_id
        );
        $i++;
      }
    }
  }

  public function settings_form($field, $instance, $has_data) {

  }


}