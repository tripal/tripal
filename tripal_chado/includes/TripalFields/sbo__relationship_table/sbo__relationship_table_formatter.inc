<?php

class sbo__relationship_table_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Relationship Table by Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sbo__relationship'];

  public static $default_settings = [
    'subject_caption' => 'This @content_type is <em>@rel_type</em> the following:',
    'object_caption' => 'The following are <em>@rel_type</em> this @content_type:',
  ];

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    $element = [];
    $element['subject_caption'] = [
      '#type' => 'textfield',
      '#title' => 'Caption: where current entity is the subject.',
      '#description' => 'This labels the relationship tables where the current entity is the subject of the relationship.',
      '#default_value' => $settings['subject_caption'],
    ];

    $element['object_caption'] = [
      '#type' => 'textfield',
      '#title' => 'Caption: where current entity is the object.',
      '#description' => 'This labels the relationship tables where the current entity is the object of the relationship.',
      '#default_value' => $settings['object_caption'],
    ];

    $element['tokens'] = [
      '#type' => 'item',
      '#title' => 'Tokens',
      '#markup' => 'The following tokens should be used in <strong>both the above captions</strong>:</p>
                     <ul>
                       <li>@rel_type: the type name of the current relationship.</li>
                       <li>@content_type: the human-readable name of the content type for the current entity.</li>
                     </ul>',
    ];

    return $element;
  }

  /**
   * @see TripalFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    $summary = t('<strong>Subject Caption:</strong> @subject<br><strong>Object Caption:</strong> @object',
      [
        '@subject' => $settings['subject_caption'],
        '@object' => $settings['object_caption'],
      ]
    );

    return $summary;
  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings and set defaults.
    $settings = $display['settings'];
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    // Headers depending on which fields are available.
    $headers = [
      'all' => ['Name', 'Unique Name', 'Species', 'Type'],
      'nst' => ['Name', 'Species', 'Type'],
      'nut' => ['Name', 'Unique Name', 'Type'],
      'nt' => ['Name', 'Type'],
    ];

    // This is an array of tables where each table corresponds to a relationship type.
    // The header is the same for all tables and the caption includes a sentence
    // stating the relationship type.
    $tables = [];

    // For each relationship...
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
      foreach (['subject_type', 'object_type', 'relationship_type'] as $var) {
        $$var = ucwords(str_replace('_', ' ', $$var));
      }

      // Handle some special cases.
      // For mRNA objects we don't want to show the CDS, exons, 5' UTR, etc.
      // we want to show the parent gene and the protein.
      if ($object_type == 'mRNA' and
        (in_array($subject_type, [
          'CDS',
          'exon',
          'five_prime_UTR',
          'three_prime_UTR',
        ]))) {
        continue;
      }

      // Set header type based on what values are available.
      if (!isset($tables[$relationship_type])) {
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
          if ($object_uniquename) {
            $object_uniquename = l($object_uniquename, 'bio_data/' . $object_entity_id);
          }
        }
      }
      if (array_key_exists('entity', $item['value']['local:relationship_subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['local:relationship_subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $subject_name = l($subject_name, 'bio_data/' . $subject_entity_id);
          if ($subject_uniquename) {
            $subject_uniquename = l($subject_uniquename, 'bio_data/' . $subject_entity_id);
          }
        }
      }

      // Determine if the subject or object is the current entity.
      $is_subject = TRUE;
      if ($object_name == $entity->schema__name['und'][0]['value']) {
        $is_subject = FALSE;
      }


      // Add the related entity to the tables array by type of relationship.
      if ($is_subject) {
        $tables[$relationship_type]['caption'] = t($settings['subject_caption'],
          [
            '@rel_type' => $relationship_type,
            '@content_type' => $entity->rdfs__type['und'][0]['value'],
          ]);

        $row = [];
        $row['name'] = $object_name;
        if ($object_uniquename) {
          $row['uniquename'] = $object_uniquename;
        }
        if ($object_species) {
          $row['species'] = $object_species;
        }
        $row['type'] = $object_type;

        $tables[$relationship_type]['rows'][] = $row;
      }
      else {
        $tables[$relationship_type]['caption'] = t($settings['object_caption'],
          [
            '@rel_type' => $relationship_type,
            '@content_type' => $entity->rdfs__type['und'][0]['value'],
          ]);

        $row = [];
        $row['name'] = $subject_name;
        if ($subject_uniquename) {
          $row['uniquename'] = $subject_uniquename;
        }
        if ($subject_species) {
          $row['species'] = $subject_species;
        }
        $row['type'] = $subject_type;

        $tables[$relationship_type]['rows'][] = $row;
      }
    }

    $element[0] = [
      '#type' => 'markup',
      '#tree' => TRUE,
    ];

    foreach ($tables as $relationship_type => $details) {

      $table_id = 'sbo__relationship-' . $item['value']['local:relationship_subject']['rdfs:type']; //$this->getPagerElementID();
      $element[0][$relationship_type] = [
        '#type' => 'markup',
        '#theme' => 'table',
        '#header' => $headers[$details['header_type']],
        '#rows' => $details['rows'],
        '#caption' => $details['caption'],
        '#sticky' => FALSE,
        '#attributes' => [
          'class' => 'tripal-data-table',
          'id' => $table_id,
        ],
      ];
    }
  }
}


