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
      $phrase = $item['value']['SIO:000493'];

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

      // Add bold font to the object and subject names.
      // @todo add back in bolding...
      // @todo Fix Current Bug: if type name is in the object name, wierd bolding happens.
      // $phrase = preg_replace("/$subject_type/", "<b>$subject_type</b>", $phrase);
      // $phrase = preg_replace("/$object_type/", "<b>$object_type</b>", $phrase);

      // Convert the object/subject to a link if an entity exists for it.
      if (array_key_exists('entity', $item['value']['local:relationship_object'])) {
        list($entity_type, $object_entity_id) = explode(':', $item['value']['local:relationship_object']['entity']);

        if ($object_entity_id != $entity->id) {
          $link = l($object_name, 'bio_data/' . $object_entity_id);
          $phrase = str_replace($object_name, $link, $phrase);
        }
      }
      if (array_key_exists('entity', $item['value']['local:relationship_subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['local:relationship_subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $link = l($subject_name, 'bio_data/' . $subject_entity_id);
          $phrase = str_replace($subject_name, $link, $phrase);
        }
      }

      $rows[][] = [
        'data' => $phrase,
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


