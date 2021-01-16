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

    // Iterate through the list of fields so that we can load the mRNA
    // entity with all of the field requested by the site admin. If we
    // don't do this first then auto_attach fields won't be added.
    $fields = $this->instance['settings']['transcript_fields'];
    $field_ids = [];
    foreach ($fields as $field_name => $field_value) {
      $field_info = field_info_field($field_name);
      $field_ids[] = $field_info['id'];
    }

    // For backwards compatibility if no field IDs were provided then
    // return the original table of four rows.
    if (count($field_ids) == 0) {
      return $this->returnDefaulTable($element, $items);
    }

    // Iterate through each mRNA (transcript).
    $transcripts = [];
    foreach ($items as $delta => $item) {

      // Skip empty items;
      if (!$item['value']) {
        continue;
      }

      // If any transcript is not published (i.e. it doesn't have an entity)
      // then we want to just return the default table and stop.
      if (!array_key_exists('entity', $item['value'])) {
        return $this->returnDefaulTable($element, $items);
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
        '#markup' => "<i>Click, " . $feature_name . ", For the full transcript page.</i>",
      ];

      // Now add all fields to the fieldset.
      $this->addFieldsetFields($mRNA_entity, $fields, $transcripts[$mRNA_entity_id]);

    }
    $element[0] = $transcripts;
  }

  /**
   * Builds a fieldset for the entity.
   */
  private function addFieldsetFields($mRNA_entity, $fields, &$fieldset) {

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
    foreach ($fields as $field_name => $field_value) {

      // Skip fields that the site admin does not want to show.
      if (array_key_exists('show', $field_value) and ($field_value['show'] != 1)) {
        continue;
      }

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
        '#markup' => '<h3>Transcript Summary</h3>',
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
  private function returnDefaulTable(&$element, $items) {

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
      $feature_name = l($feature_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);

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

