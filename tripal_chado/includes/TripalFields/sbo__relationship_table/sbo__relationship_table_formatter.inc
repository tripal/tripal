<?php

class sbo__relationship_table_formatter extends ChadoFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Relationship Table by Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__relationship');

  public static $default_settings = array(
    'title' => 'Relationship',
    'empty' => 'There are no relationships',
  );

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];
    $element = array();
    $element['title'] = array(
      '#type' => 'textfield',
      '#title' => 'Table Header',
      '#default_value' => array_key_exists('title', $settings) ? $settings['title'] : 'Relationship',
    );
    $element['empty'] = array(
      '#type' => 'textfield',
      '#title' => 'Empty text',
      '#default_value' => array_key_exists('empty', $settings) ? $settings['empty'] : 'There are no relationships',
    );

    return $element;
  }

  /**
   * @see TripalFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    $summary = t('Title: @title<br>Empty: @empty',
        array(
          '@title' => $settings['title'],
          '@empty' => $settings['empty'])
        );

    return $summary;
  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    $headers = array(
      'all' => array('Name', 'Unique Name', 'Species', 'Type'),
      'nst' => array('Name', 'Species', 'Type'),
      'nut' => array('Name', 'Unique Name', 'Type'),
      'nt' => array('Name', 'Type'),
    );
    // This is an array of tables where each table corresponds to a relationship type.
    // The header is the same for all tables and the caption includes a sentence
    // stating the relationship type.
    $tables = array();

    foreach ($items as $delta => $item) {
      if (empty($item['value'])) {
        continue;
      }

      $subject_name = $item['value']['local:relationship_subject']['schema:name'];
      $subject_uniquename = (isset($item['value']['local:relationship_subject']['data:0842'])) ? $item['value']['local:relationship_subject']['data:0842'] : NULL;
      $subject_type = $item['value']['local:relationship_subject']['rdfs:type'];
      $subject_species = (isset($item['value']['local:relationship_subject']['OBI:0100026'])) ? $item['value']['local:relationship_subject']['OBI:0100026'] : NULL;

      $object_name = $item['value']['local:relationship_object']['schema:name'];
      $object_uniquename = (isset($item['value']['local:relationship_object']['data:0842'])) ? $item['value']['local:relationship_object']['data:0842'] : NULL;
      $object_type = $item['value']['local:relationship_object']['rdfs:type'];
      $object_species = (isset($item['value']['local:relationship_object']['OBI:0100026'])) ? $item['value']['local:relationship_object']['OBI:0100026'] : NULL;

      $relationship_type = $item['value']['local:relationship_type'];

      // Make types more readable.
      foreach(array('subject_type', 'object_type', 'relationship_type') as $var) {
        $$var = ucwords(str_replace('_',' ', $$var));
      }

      // Handle some special cases.
      // For mRNA objects we don't want to show the CDS, exons, 5' UTR, etc.
      // we want to show the parent gene and the protein.
      if ($object_type == 'mRNA' and
          (in_array($subject_type, array('CDS', 'exon', 'five_prime_UTR', 'three_prime_UTR')))) {
        continue;
      }

      // Set header type based on what values are available.
      if ($delta == 0) {
        $tables[$relationship_type]['header_type'] = 'all';
        if ($subject_uniquename === NULL AND $subject_species === NULL) {
          $tables[$relationship_type]['header_type'] = 'nt';
        }
        elseif ($subject_uniquename === NULL) {
          $tables[$relationship_type]['header_type'] = 'nst';
        }
        elseif ($subject_species === NULL) {
          $tables[$relationship_type]['header_type'] = 'nut';
        }
      }

      // Convert the object/subject to a link if an entity exists for it.
      if (array_key_exists('entity', $item['value']['local:relationship_object'])) {
        list($entity_type, $object_entity_id) = explode(':', $item['value']['local:relationship_object']['entity']);
        if ($object_entity_id != $entity->id) {
          $object_name = l($object_name, 'bio_data/' . $object_entity_id);
          if ($object_uniquename) { $object_uniquename = l($object_uniquename, 'bio_data/' . $object_entity_id); }
        }
      }
      if (array_key_exists('entity', $item['value']['local:relationship_subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['local:relationship_subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $subject_name = l($subject_name, 'bio_data/' . $subject_entity_id);
          if ($subject_uniquename) { $subject_uniquename = l($subject_uniquename, 'bio_data/' . $subject_entity_id); }
        }
      }

      // Determine if the subject or object is the current entity.
      $is_subject = TRUE;
      if ($object_name == $entity->schema__name['und'][0]['value']) {
        $is_subject = FALSE;
      }


      // Add the related entity to the tables array by type of relationship.
      if ($is_subject) {
        $tables[$relationship_type]['caption'] = t('This @type is <em>@rel_type</em> of the following <em>@content_type(s)</em>:',
          array('@type' => $subject_type, '@rel_type' => $relationship_type, '@content_type' => $entity->rdfs__type['und'][0]['value']));

        $row = array();
        $row['name'] = $object_name;
        if ($object_uniquename) { $row['uniquename'] = $object_uniquename; }
        if ($object_species) { $row['species'] = $object_species; }
        $row['type'] = $object_type;

        $tables[$relationship_type]['rows'][] = $row;
      }
      else {
        $tables[$relationship_type]['caption'] = t('The following <em>@content_type(s)</em> are <em>@rel_type</em> of this @type:',
          array('@type' => $object_type, '@rel_type' => $relationship_type, '@content_type' => $entity->rdfs__type['und'][0]['value']));

        $row = array();
        $row['name'] = $subject_name;
        if ($subject_uniquename) { $row['uniquename'] = $subject_uniquename; }
        if ($subject_species) { $row['species'] = $subject_species; }
        $row['type'] = $subject_type;

        $tables[$relationship_type]['rows'][] = $row;
      }
    }

    $element[0] = array(
      '#type' => 'markup',
      '#tree' => TRUE,
    );

    foreach ($tables as $relationship_type => $details) {
      $element[0][$relationship_type] = array(
        '#type' => 'markup',
        '#theme' => 'table',
        '#header' => $headers[ $details['header_type'] ],
        '#rows' => $details['rows'],
        '#caption' => $details['caption'],
      );
    }
  }
}


