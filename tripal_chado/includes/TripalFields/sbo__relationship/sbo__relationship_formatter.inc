<?php

class sbo__relationship_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Relationship Statements';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sbo__relationship'];

  public static $default_settings = [
    'title' => 'Relationship',
    'empty' => 'There are no relationships',
  ];

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];
    $element = [];
    $element['title'] = [
      '#type' => 'textfield',
      '#title' => 'Table Header',
      '#default_value' => array_key_exists('title', $settings) ? $settings['title'] : 'Relationship',
    ];
    $element['empty'] = [
      '#type' => 'textfield',
      '#title' => 'Empty text',
      '#default_value' => array_key_exists('empty', $settings) ? $settings['empty'] : 'There are no relationships',
    ];

    return $element;
  }

  /**
   * @see TripalFieldFormatter::settingsSummary()
   */
  public function settingsSummary($view_mode) {
    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    $summary = t('Title: @title<br>Empty: @empty',
      [
        '@title' => $settings['title'],
        '@empty' => $settings['empty'],
      ]
    );

    return $summary;
  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Load the Field class into scope for use below.
    tripal_load_include_field_class('sbo__relationship');

    // Get the settings
    $settings = $display['settings'];
    $rows = [];
    $headers = [$settings['title']];

    foreach ($items as $delta => $item) {
      if (empty($item['value'])) {
        continue;
      }
      $subject_name = $item['value']['local:relationship_subject']['schema:name'];
      $subject_type = $item['value']['local:relationship_subject']['rdfs:type'];
      $object_name = $item['value']['local:relationship_object']['schema:name'];
      $object_type = $item['value']['local:relationship_object']['rdfs:type'];
      $rel_type = $item['value']['local:relationship_type'];

      // Get some better human-readable formats for the type and verb.
      $verb = sbo__relationship::get_rel_verb($rel_type);
      $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));

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

      // Convert the object/subject to a link if an entity exists for it.
      if (array_key_exists('entity', $item['value']['local:relationship_object'])) {
        list($entity_type, $object_entity_id) = explode(':', $item['value']['local:relationship_object']['entity']);

        if ($object_entity_id != $entity->id) {
          $object_name = l($object_name, 'bio_data/' . $object_entity_id);
        }
      }
      if (array_key_exists('entity', $item['value']['local:relationship_subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['local:relationship_subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $subject_name = l($subject_name, 'bio_data/' . $subject_entity_id);
        }
      }

      // Add bold font to the subject and object names.
      $subject_name = '<b>' . $subject_name . '</b>';
      $object_name = '<b>' . $object_name . '</b>';

      // The $clause text here will match what is already in $item['value']['SIO:000493']
      // but we rebuild it here to incorporated links and formatting
      $clause = 'The ' . $subject_type . ', ' .
        $subject_name . ', ' . $verb . ' ' . $rel_type_clean . ' ' .
        $object_type . ', ' . $object_name . '.';

      $rows[][] = [
        'data' => $clause,
        //'class' => array('tripal-entity-unattached field-items')
      ];
    }

    // Build the pager
    $items_per_page = array_key_exists('items_per_page', $this->instance['settings']) ? $this->instance['settings']['items_per_page'] : 10;
    $total_records = count($rows);
    $total_pages = (int) ($total_records / $items_per_page) + 1;
    $pelement = 0;
    $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);
    $pager = theme('pager', [
      'tags' => [],
      'element' => $pelement,
      'parameters' => [],
      'quantity' => $total_pages,
    ]);
    $pager = $this->ajaxifyPager($pager, $entity);
    $page_items = array_chunk($rows, $items_per_page);

    $caption = '';
    if ($total_records == 1) {
      $caption = 'There is ' . count($rows) . ' relationship.';
    }
    else {
      $caption = 'There are ' . count($rows) . ' relationships.';
    }
    $content = theme_table([
      'header' => $headers,
      'rows' => count($rows) > 0 ? $page_items[$current_page] : [],
      'attributes' => [
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => $caption,
      'colgroups' => [],
      'empty' => $settings['empty'],
    ]);

    $element[0] = [
      '#type' => 'markup',
      '#markup' => $content . $pager,
    ];
  }
}

