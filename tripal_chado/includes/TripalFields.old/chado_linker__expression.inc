<?php

class chado_linker__expression extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Expression';

  // The default description for this field.
  public static $default_description = 'Associates an expression with
          this record.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
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

    $content = '';
    $rows = array();
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      // Iterate through all of the children of the $item['value']. Add each
      // one as an independent row in the table.

      foreach ($item['value'] as $key => $value) {

        // If this key is the name, then we want to link to the entity if one
        // exists.
        if ($key == 'name') {
          if (array_key_exists('entity', $item['value']) and $item['value']['$entity_id']) {
            list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
            $value = l($value, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
          }
        }
        // If this key is the publication then we want to get the citation
        // and link to the pub if an entity exits.
        if ($key == 'publication') {
          $pub = $value['Citation'];
          if (array_key_exists('publication', $item) and array_key_exists('entity', $item['publication'][0])) {
            $entity_id = $item['publication'][0]['entity_id'];
            $title =  $item['value']['publication']['Title'];
            $link = l($title, 'bio_data/' . $entity_id);
            $pub = preg_replace("/$title/", $link, $pub);
          }
          $value = $pub;
        }
        // Add the item as a new row.
        $rows[] = array(
          array(
            'data' => ucfirst(str_replace('_', ' ', $key)),
            'header' => TRUE,
            'width' => '20%',
          ),
          $value
        );
      }
    }
    $table = array(
      'header' => array(),
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-expression-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'There is no curated expression data.',
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
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $record = $details['record'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    $linker_table = $base_table . '_expression';
    $options = array(
      'return_array' => 1,
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $exp_linkers = $record->$linker_table;
    if ($exp_linkers) {
      foreach ($exp_linkers as $i => $exp_linker) {

        // Because the unqiuename is a text field we must expand it.
        $expression = $exp_linker->expression_id;
        $expression = chado_expand_var($expression, 'field', 'expression.uniquename', $options);

        // Set the values that will be seen by the user on the page and in
        // web services, or anwhere this field is viewed.
        $entity->{$field_name}['und'][$i]['value'] = array(
          'name' => $expression->uniquename,
          'description' => $expression->description,
          //'md5checksum' => $expression->md5checksum,
        );

        // Add the pub information if a real pub is associated with the record.
        $pub = $exp_linker->pub_id;
        if ($pub->uniquename != 'null') {
          $pub_details = tripal_get_minimal_pub_info($pub);

          $entity->{$field_name}['und'][$i]['value']['publication'] = $pub_details;
          $entity->{$field_name}['und'][$i]['value']['publication']['type'] = $pub->type_id->name;
          if (property_exists($pub, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['publication'][0]['value']['entity'] = 'TripalEntity:' . $pub->entity_id;
          }
        }

        // Add the linker_expressionprop
        $linkerprop_table =  $linker_table . 'prop';
        if (db_table_exists('chado.' . $linkerprop_table)) {
          $exp_linker = chado_expand_var($exp_linker, 'table', $linkerprop_table, $options);
          $exp_linkerprops = $exp_linker->feature_expressionprop;
          if ($exp_linkerprops) {
            foreach ($exp_linkerprops AS $linkerprop) {
              $entity->{$field_name}['und'][$i]['value'][$linkerprop->type_id->name] = $linkerprop->value;
            }
          }
        }
        // Add the fields for the widget form.  The name requres the following
        // format if the fields should be used as values for insertint/updating
        // into the chado table:  [table_name]__[field_name]
        $entity->{$field_name}['und'][$i][$linker_table . '__expression_id'] = $expression->expression_id;
        $entity->{$field_name}['und'][$i][$linker_table . '__uniquename'] = $expression->uniquename;
        //$entity->{$field_name}['und'][$i][$linker_table . '__md5checksum'] = $expression->md5checksum;
        $entity->{$field_name}['und'][$i][$linker_table . '__description'] = $expression->description;

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