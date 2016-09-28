<?php

class chado_gene__transcripts extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Transcripts';

  // The default description for this field.
  public static $default_description = 'Transcripts of genes.';

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
   * @see TripalField::formatter_settings_summary()
   */
  public function formatterSettingsSummary($view_mode) {

  }

  /**
   * @see TripalField::formatter_settings_form()
   */
  public function formatterSettingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $headers = array('Transcript Name', 'Identifier', 'Type', 'Location');
    $rows = array();
    foreach ($items as $delta => $item) {

      if (!$item['value']) {
        continue;
      }
      $transcript = $item['value'];

      // Get the field values
      $feature_name = $transcript['name'];
      $feature_uname = $transcript['identifier'];
      $loc = $transcript['location'];
      $type = $transcript['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
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
      'empty' => 'This feature has no transcripts',
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

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        'type' => '',
        'name' => '',
        'identifier' => '',
        'location' => '',
      ),
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

    $headers = array('Name' ,'Identifier', 'Location');
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
        'type' => $type->name,
        'name' => $feature_name,
        'identifier' => $transcript->record->subject_id->uniquename,
        'location' => $loc,

      );
      // Add in the semantic web information that describes each key in the
      // value array.
      $entity->{$field_name}['und'][$i]['semantic_web'] = array(
        'type' => $type->dbxref_id->db_id->name . ":" . $type->dbxref_id->accession,
        'name' => tripal_get_chado_semweb_term('cvterm', 'name'),
        'identifier' => tripal_get_chado_semweb_term('feature', 'uniquename'),
        'location' => '',
      );
      if (property_exists($transcript->record->subject_id, 'entity_id')) {
        $entity_id = $transcript->record->subject_id->entity_id;
        $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $entity_id;
      }
      $i++;
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

  }
}