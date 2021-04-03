<?php

class chado_linker__contact_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Contacts';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__contact'];


  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];

    $type_term = chado_get_semweb_term('contact', 'type_id');
    $name_term = chado_get_semweb_term('contact', 'name');
    $description_term = chado_get_semweb_term('contact', 'description');

    $headers = ['Name', 'Description', 'Type'];
    $rows = [];

    foreach ($items as $delta => $item) {
      $contact = $item['value'];
      if (!$contact) {
        continue;
      }

      // Get the field values
      $contact_name = $contact[$name_term];
      $description = $contact[$description_term];
      $type = $contact[$type_term];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $contact_name = l($contact_name, "bio_data/" . $entity_id, ['attributes' => ['target' => "_blank"]]);
      }
      $rows[] = [$contact_name, $description, $type];
    }

    $content = '';
    if (count($items) > 0) {
      $table = [
        'header' => $headers,
        'rows' => $rows,
        'attributes' => [
          'id' => 'tripal_linker-table-contact-object',
          'class' => 'tripal-data-table',
        ],
        'sticky' => FALSE,
        'caption' => "",
        'colgroups' => [],
        'empty' => 'There are no contacts available.',
      ];
      $content = theme_table($table);
    }

    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
