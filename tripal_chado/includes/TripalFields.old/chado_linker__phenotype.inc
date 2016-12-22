<?php

class chado_linker__phenotype extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Phenotypes';

  // The default description for this field.
  public static $default_description = 'Associates phenotypes with this record.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {
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
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $record = $details['record'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

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

  /**
   * We don't want a widget so override this function.
   */
  public static function widgetInfo() {
    return array();
  }

}