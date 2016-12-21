<?php

class sbo__phenotype_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Phenotype';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__phenotype');

  // The list of default settings for this formatter.
  public static $default_settings = array();
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

    $headers = array('Name', 'Value', 'Type');
    $rows = array();

    foreach ($items as $delta => $item) {
      $phenotype = $item['value'];
      if (!$phenotype) {
        continue;
      }

      // Get the field values
      $phenotype_name = $phenotype['name'];
      $value = $phenotype['value'];
      $type = $phenotype['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $phenotype_name = l($phenotype_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($phenotype_name, $value, $type);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-phenotype-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'No phenotypes available',
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
}
