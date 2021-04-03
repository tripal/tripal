<?php

class so__transcript_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Transcript';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__transcript'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $mRNA_fieldset = [];

    // If there are no items return an empty element.
    if (count($items) == 0) {
      return $this->returnEmpty($element);
    }

    // Get the list of fields that are t o be shown
    $fields_to_show = [];
    if (array_key_exists('transcript_fields', $this->instance['settings'])) {
      $fields_to_show = $this->fieldsToShow($this->instance['settings']['transcript_fields']);
    }

    // For backwards compatibility if the 'transcript_fields' setting
    // is not available then return the default table.
    if (count($fields_to_show) == 0) {
      return $this->returnDefaultTable($element, $items);
    }

    // If any of the transcripts are not published then just provide the
    // default view
    foreach ($items as $delta => $item) {

      // Skip empty items;
      if (!$item['value']) {
        continue;
      }

      // If any transcript is not published (i.e. it doesn't have an entity)
      // then we want to just return the default table and stop.
      if (!array_key_exists('entity', $item['value'])) {
        return $this->returnDefaultTable($element, $items);
      }
    }

    // Iterate through the list of fields so that we can load the mRNA
    // entity with all of the field requested by the site admin. If we
    // don't do this first then auto_attach fields won't be added.
    $field_ids = [];
    foreach ($fields_to_show as $field_name) {
      $field_info = field_info_field($field_name);
      $field_ids[] = $field_info['id'];
    }

    // For backwards compatibility if no field IDs were provided then
    // return the original table of four rows.
    if (count($field_ids) == 0) {
      return $this->returnDefaultTable($element, $items);
    }

    #return $this->returnFieldsets($items, $element, $field_ids, $fields_to_show);
    return $this->returnDropDown($items, $element, $field_ids, $fields_to_show);

  }

  /**
   * Returns a list of fields names that the site admin wants to show.
   */
  private function fieldsToShow($transcript_fields) {
    $to_show = [];
    foreach($transcript_fields as $field_name => $details) {
      if ($details['show'] == 1) {
        $to_show[] = $field_name;
      }
    }
    return $to_show;
  }

  /**
   *
   */
  private function returnDropDown($items, &$element, $field_ids, $fields_to_show) {

    drupal_add_js(drupal_get_path ('module', 'tripal_chado') . '/theme/js/so__transcript.js');

    // Iterate through each mRNA (transcript).
    $options = [0 => '--Select a transcript to view--'];
    $transcripts = [];
    foreach ($items as $delta => $item) {

      // Skip empty items;
      if (!$item['value']) {
        continue;
      }

      list($entity_type, $mRNA_entity_id) = explode(':', $item['value']['entity']);
      $result = tripal_load_entity('TripalEntity', [$mRNA_entity_id], FALSE, $field_ids);
      reset($result);
      $mRNA_entity = $result[$mRNA_entity_id];
      $options[$mRNA_entity_id] = $mRNA_entity->title;

      // Create the fieldset for this transcript.
      $transcripts[$mRNA_entity_id] = [
        '#type' => 'markup',
        '#prefix' => '<div id="' . 'tripal-chado-so__transcript-' . $mRNA_entity_id . '" class="tripal-chado-so__transcript-box">',
        '#suffix' => '</div>',
      ];

      // Add a link to the mRNA page.
      $feature_name = $mRNA_entity->title;
      $feature_name = l($feature_name, "bio_data/" . $mRNA_entity->id, ['attributes' => ['target' => "_blank"]]);
      $transcripts[$mRNA_entity_id]['mRNA_link'] = [
        '#type' => 'markup',
        '#markup' => "<i>Click, " . $feature_name . ", for the full transcript page.</i>",
        '#weight' => -102
      ];

      // Now add all fields to the fieldset.
      $this->addFields($mRNA_entity, $fields_to_show, $transcripts[$mRNA_entity_id]);

    }
    $transcripts['transcript_dropdown'] = [
      '#type' => 'select',
      '#title' => 'Transcripts for this gene',
      '#options' => $options,
      '#weight' => -100,
      '#description' => 'Select a transcript to view more details.',
      '#attributes' => ['class' => ['tripal-chado-so__transcript-select']]
    ];
    $element[0] = $transcripts;
  }

  /**
   *
   */
  private function returnFieldsets($items, &$element, $field_ids, $fields_to_show) {

    // Iterate through each mRNA (transcript).
    $transcripts = [];
    foreach ($items as $delta => $item) {

      // Skip empty items;
      if (!$item['value']) {
        continue;
      }

      list($entity_type, $mRNA_entity_id) = explode(':', $item['value']['entity']);

      // Now load the mRNA entity with all of the fields.
      $result = tripal_load_entity('TripalEntity', [$mRNA_entity_id], FALSE, $field_ids);
      reset($result);
      $mRNA_entity = $result[$mRNA_entity_id];

      // Create the fieldset for this transcript.
      $transcripts[$mRNA_entity_id] = [
        '#type' => 'fieldset',
        '#title' => $mRNA_entity->title,
        '#description' => '',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#attributes' => ['class' => ['collapsible', 'collapsed']],
      ];
      drupal_add_library('system', 'drupal.collapse');


      // Add a link to the mRNA page.
      $feature_name = $mRNA_entity->title;
      $feature_name = l($feature_name, "bio_data/" . $mRNA_entity->id, ['attributes' => ['target' => "_blank"]]);
      $transcripts[$mRNA_entity_id]['mRNA_link'] = [
        '#type' => 'markup',
        '#markup' => "<i>Click, " . $feature_name . ", for the full transcript page.</i>",
      ];

      // Now add all fields to the fieldset.
      $this->addFields($mRNA_entity, $fields_to_show, $transcripts[$mRNA_entity_id]);

    }
    $element[0] = $transcripts;
  }

  /**
   * Builds a fieldset for the entity.
   */
  private function addFields($mRNA_entity, $fields, &$fieldset) {

    // Get some settings about the transcript content type.
    $mRNA_bundle = tripal_load_bundle_entity(['name'=> $mRNA_entity->bundle]);
    $hide_empty = tripal_get_bundle_variable('hide_empty_field', $mRNA_bundle->id);

    // Get the Entity ID for the transcript.
    $entity_id = $mRNA_entity->id;

    // We will use a summary table to house the fields that have a
    // cardinality of one. These will appear at the top of the fieldset.
    $summary_rows = [];

    // Iterate through the list of fields that the site admin has indicated
    // that they want to show in the transcript fieldset. They are provided
    // in order that they should be shown.
    foreach ($fields as $field_name) {

        // Load the field instance info. We'll need this to determine the
      // cardinality of the field and to render it for display.
      $field_info = field_info_field($field_name);
      $field_instance = field_info_instance('TripalEntity', $field_name, $mRNA_entity->bundle);

      // For the display we want to honor the site admin's wishes and not
      // show fields that are empty if they have that setting turned on.
      $field_items = field_get_items('TripalEntity', $mRNA_entity, $field_name);
      $field_is_empty = tripal_field_is_empty($field_info, $field_items);
      if ($field_is_empty and $hide_empty) {
        continue;
      }

      // If the default display is to hide this field then skip it too.
      if ($field_instance['display']['default']['type'] == 'hidden') {
        continue;
      }

      // Get the render array for this field.
      $field_element = field_view_field('TripalEntity', $mRNA_entity, $field_name, $field_instance['display']['default']);
      $field_element['#label_display'] = 'hidden';

      // We need to know the cardinality of this field. If it has a
      // cardinatliy of 1 we'll put it in the summary table for the
      // transcript that appears at the top of the fieldset. If not we'll
      // let it render as is, in the order it's provided to us here.
      $cardinality = array_key_exists('cardinality', $field_info) ? $field_info['cardinality'] : 1;
      if ($cardinality == 1) {

        // add field to a special transcripts table, where fields have cardinality of 1
        $summary_rows[] = [
          [
            'data' => $field_instance['label'],
            'header' => TRUE,
          ],
          [
            'data' => drupal_render($field_element),
          ]
        ];
      }
      // Else add the field as is.
      else {
        $fieldset[$field_name . '_header'] = [
          '#type' => 'markup',
          '#markup' => '<h3 class="tripal-chado-gene-transcript-fieldset-item">' . $field_instance['label'] . '</h3>',
        ];
        $fieldset[$field_name] = [
          '#type' => 'markup',
          '#markup' => drupal_render($field_element),
        ];
      }
    } // End looping over fields.

    // If the summary table has values then
    if (count($summary_rows) > 0) {
      // display fields of single cardinality in a special transcripts table
      $summary_table = [
        'header' => [],
        'rows' => $summary_rows,
        'attributes' => [
          'id' => 'tripal_feature-table-transcript-fields-object',
          'class' => 'tripal-data-table',
        ],
        'sticky' => FALSE,
        'caption' => "",
        'colgroups' => [],
        'empty' => 'This feature has no single cardinality transcript fields',
      ];

      $fieldset['summary_header'] = [
        '#type' => 'markup',
        '#markup' => '<h3>Transcript ' . $mRNA_entity->title . '</h3>',
        '#weight' => -101,
      ];
      $fieldset['summary_table'] = [
        '#type' => 'markup',
        '#markup' => theme_table($summary_table),
        '#weight' => -100,
      ];
    }
  }

  /**
   * Returns the default table.
   *
   * For backwards compatibility this function returns a table
   * of four columns and one row per transcript.
   */
  private function returnDefaultTable(&$element, $items) {

    $default_headers = ['Transcript Name', 'Identifier', 'Type', 'Location'];
    $default_rows = [];

    foreach ($items as $delta => $item) {

      if (!$item['value']) {
        continue;
      }

      // Get the field values.
      $feature_name = $item['value']['schema:name'];
      $feature_uname = $item['value']['data:0842'];
      $loc = $item['value']['SO:0000735'];
      $type = $item['value']['rdfs:type'];
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $feature_name = l($feature_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
      }


      $default_rows[] = [$feature_name, $feature_uname, $type, $loc];
    }

    // Build the default table
    $default_table = [
      'header' => $default_headers,
      'rows' => $default_rows,
      'attributes' => [
        'id' => 'tripal_feature-table-transcripts-object',
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'This feature has no transcripts',
    ];

    $element[0] = [
      '#type' => 'markup',
      '#markup' => theme_table($default_table),
    ];
  }

  /**
   * Returns an empty element for the view.
   */
  private function returnEmpty(&$element){
    $element[0] = [
      '#type' => 'markup',
      '#markup' => '',
    ];
  }
}

