<?php

class data__sequence_coordinates_table_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Sequence Coordinate Table';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence_coordinates');

  public static $default_settings = array(
    'caption' => 'This @content_type is aligned to the following:',
  );

  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

    $display = $this->instance['display'][$view_mode];
    $settings = $display['settings'];

    // Ensure the default are set if the value is not configured.
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) { $settings[$key] = $value; }
    }

    $element = array();
    $element['caption'] = array(
      '#type' => 'textfield',
      '#title' => 'Caption',
      '#description' => 'Provides a caption for the table describing the purpose to the user.',
      '#default_value' => $settings['caption'],
    );

    $element['tokens'] = array(
      '#type' => 'item',
      '#title' => 'Tokens',
      '#markup' => 'The following tokens should be used in <strong>both the above captions</strong>:</p>
                     <ul>
                       <li>@content_type: the human-readable name of the content type for the current entity.</li>
                     </ul>',
    );

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
      if (!isset($settings[$key])) { $settings[$key] = $value; }
    }

    $summary = t('<strong>Caption:</strong> ' . $settings['caption']);

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
      if (!isset($settings[$key])) { $settings[$key] = $value; }
    }

    // Replace tokens in the caption.
    $settings['caption'] = t($settings['caption'],
      array('@content_type' => $entity->rdfs__type['und'][0]['value']));

    // Determine the terms for each column.
    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');
    $phase_term = chado_get_semweb_term('featureloc', 'phase');

    // For each location, add it to the table.
    $header = array('Name','Location','Strand');
    $rows = array();
    foreach ($items as $item) {
      if (!empty($item['value'])) {
        $rows[] = array(
          $item['value'][$reference_term],
          $item['value'][$fmin_term] . '..' . $fmax = $item['value'][$fmax_term],
          $item['value'][$strand_term]
        );
      }
    }

    // Add the markup for the table.
    $element[0] = array(
      '#type' => 'markup',
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#caption' => $settings['caption'],
    );
  }
}
