<?php

class sbo__relationship_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Relationship';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__relationship');

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

    $rows = array();
    $headers = array('Subject' ,'Type', 'Object');
    $headers = array('Relationship');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $subject_name = $item['value']['subject']['name'];
      $subject_type = $item['value']['subject']['type'];
      $object_name = $item['value']['object']['name'];
      $object_type = $item['value']['object']['type'];
      $phrase = $item['value']['phrase'];

      // Handle some special cases.
      // For mRNA objects we don't want to show the CDS, exons, 5' UTR, etc.
      // we want to show the parent gene and the protein.
      if ($object_type == 'mRNA' and ($subject_type != 'polypeptide')) {
        continue;
      }
      if ($subject_type == 'mRNA' and ($object_type != 'gene')) {
        continue;
      }

      $phrase = preg_replace("/$subject_type/", "<b>$subject_type</b>", $phrase);
      $phrase = preg_replace("/$object_type/", "<b>$object_type</b>", $phrase);

      if (array_key_exists('entity', $item['value']['object'])) {
        list($entity_type, $object_entity_id) = explode(':', $item['value']['object']['entity']);
        if ($object_entity_id != $entity->id) {
          $link = l($object_name, 'bio_data/' . $object_entity_id);
          $phrase = preg_replace("/$object_name/", $link, $phrase);
        }
      }
      if (array_key_exists('entity', $item['value']['subject'])) {
        list($entity_type, $subject_entity_id) = explode(':', $item['value']['subject']['entity']);
        if ($subject_entity_id != $entity->id) {
          $link = l($subject_name, 'bio_data/' . $subject_entity_id);
          $phrase = preg_replace("/$subject_name/", $link, $phrase);
        }
      }

      $rows[] = array($phrase);
    }


    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'chado-linker--relationship-table',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => 'There are no relationships',
    );

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
      );
    }
  }
}
