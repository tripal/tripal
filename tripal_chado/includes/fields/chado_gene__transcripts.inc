<?php

class chado_gene__transcripts extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  public function field_info() {
    return array(
      'label' => t('Transcripts'),
      'description' => t('Transcripts of genes.'),
      'default_widget' => 'chado_gene__transcripts_widget',
      'default_formatter' => 'chado_gene__transcripts_formatter',
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
  protected function can_attach($entity_type, $bundle, $details) {
    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    // If the linker table does not exists or this is not a gene then we don't want to add attach.
    $rel_table = $table_name . '_relationship';
    if (chado_table_exists($rel_table) and $bundle->label == 'gene') {
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

    $rel_table = $table_name . '_relationship';
    $schema = chado_get_schema($rel_table);
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => 'gene_transcripts',
      'type' => 'chado_gene__transcripts',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => 'md5checksum',
        'base_table' => $table_name,
        'semantic_web' => 'SO:0000673',
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
      'field_name' => 'gene_transcripts',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Transcripts',
      'description' => 'These transcripts are associated with this gene.',
      'required' => FALSE,
      'settings' => array(),
      'widget' => array(
        'type' => 'chado_gene__transcripts_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_gene__transcripts_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::widget_info()
   */
  public function widget_info() {
    return array(
      'label' => t('Transcripts Settings'),
      'field types' => array('chado_gene__transcripts')
    );
  }

  /**
   * @see TripalField::formatter_info()
   */
  public function formatter_info() {
    return array(
      'label' => t('Transcripts'),
      'field types' => array('chado_gene__transcripts'),
      'settings' => array(
      ),
    );
  }

  /**
   * @see TripalField::formatter_settings_summary()
   */
  public function formatter_settings_summary($field, $instance,
      $view_mode) {

  }

  /**
   * @see TripalField::formatter_settings_form()
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }

  /**
   * @see TripalField::formatter_view()
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    $record = $entity->chado_record;

    $headers = array('Feature Name', 'Unique Name', 'Type', 'Location');
    $rows = array();
    foreach ($items as $delta => $item) {

      if (!$item['value']) {
        continue;
      }
      $transcript = $item['value'];

      // Get the field values
      $feature_name = $transcript['name'];
      $feature_uname = $transcript['unique name'];
      $loc = $transcript['location'];
      $type = $transcript['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity_id', $item) and $item['entity_id']) {
        $entity_id = $item['entity_id'];
        $feature_name = l($feature_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($feature_name, $feature_uname, $type, $loc);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-transcripts-object',
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

  /**
   * @see TripalField::load()
   */
  public function load($field, $entity, $details) {
    $record = $details['record'];

    $field_name = $field['field_name'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    // TODO: If the tripal_get_feature_relationships() slows this down then
    // we may need to write a custom function to get the data.
    $rels = tripal_get_feature_relationships($record);

    // TODO: what if other transcripts names from SO are used. In that
    // case we should support those too (using cvtermpath table to find them).
    // mRNA should not be hard-coded below.

    // Set the value to be a array of "table" rows.
    $transcripts = array();
    if (key_exists('part of', $rels['object']) &&
        key_exists('mRNA', $rels['object']['part of'])) {
     $transcripts =  $rels['object']['part of']['mRNA'];
    }

    $headers = array('Feature Name' ,'Unique Name', 'Location');
    $rows = array();
    $i = 0;
    foreach ($transcripts as $transcript) {
      // link the feature to it's node
      $feature_name = $transcript->record->subject_id->name;

      $locations = $transcript->child_featurelocs;
      $loc = "";
      foreach ($locations AS $location) {
        $loc .= $location->srcfeature_name . ":" . $location->fmin . ".." . $location->fmax;
      }
      $type = $transcript->record->subject_id->type_id;
      $entity->{$field_name}['und'][$i]['value'] = array(
        '@type' => $type->dbxref_id->db_id->name . ":" . $type->dbxref_id->accession,
        'type' => $type->name,
        'name' => $feature_name,
        'unique name' => $transcript->record->subject_id->uniquename,
        'location' => $loc,

      );
      if (property_exists($transcript->record->subject_id, 'entity_id')) {
        $entity_id = $transcript->record->subject_id->entity_id;
        $entity->{$field_name}['und'][$i]['entity_id'] = $entity_id;
        $entity->{$field_name}['und'][$i]['entity_type'] = 'TripalEntity';
      }
      $i++;
    }
  }
  /**
   * @see TripalField::settings_form()
   */
  public function settings_form($field, $instance, $view_mode) {
  }
  /**
   * @see TripalField::widget_form()
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }
}