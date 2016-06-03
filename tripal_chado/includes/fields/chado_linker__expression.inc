<?php

class chado_linker__expression extends TripalField {

  /**
   * @see TripalField::fieldInfo()
   */
  public static function fieldInfo() {
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
    $expression_table = $table_name . '_expression';
    if (chado_table_exists($expression_table)) {
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

    $this->field_name = $table_name . '_expression';
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

    $schema = chado_get_schema('featureloc');
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_linker__expression',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name . '_expression',
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'local:expression',
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
      'label' => 'Expression',
      'description' => 'Information about the expression of this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__expression_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_linker__expression_formatter',
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
      'chado_linker__expression_widget' => array(
        'label' => t('Expressions'),
        'field types' => array('chado_linker__expression'),
      ),
    );
  }

  /**
   * @see TripalField::formatterInfo()
   */
  public static function formatterInfo() {
    return array(
      'chado_linker__expression_formatter' => array(
        'label' => t('Expression'),
        'field types' => array('chado_linker__expression'),
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

    $content = '';
    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      // Iterate through all of the children of the $item['value']. Add each
      // one as an independent row in the table.
      $rows = array();
      foreach ($item['value'] as $key => $value) {

        // If this key is the name, then we want to link to the entity if one
        // exists.
        if ($key == 'name') {
          if (array_key_exists('entity_id', $item) and $item['$entity_id']) {
            $entity_id = $item['entity_id'];
            $value = l($value, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
          }
        }
        // If this key is the publication then we want to get the citation
        // and link to the pub if an entity exits.
        if ($key == 'publication') {
          $pub = $value['Citation'];
          if (array_key_exists('publication', $item) and array_key_exists('entity_id', $item['publication'][0])) {
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
        'empty' => '',
      );
      $content .= theme_table($table);
    }


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
          $entity->{$field_name}['und'][$i]['value']['publication']['@type'] = $pub->type_id->dbxref_id->db_id->name . ':' . $pub->type_id->dbxref_id->accession;
          $entity->{$field_name}['und'][$i]['value']['publication']['type'] = $pub->type_id->name;
          if (property_exists($pub, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['publication'][0]['entity_id'] = $pub->entity_id;
            $entity->{$field_name}['und'][$i]['publication'][0]['entity_type'] = 'TripalEntity';
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

}