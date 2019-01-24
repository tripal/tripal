<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__child_properties_formatter extends ChadoFieldFormatter {

  public static $default_label = 'Child Properties';
  public static $field_types = ['local__child_properties'];
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   **/
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * @see ChadoFieldFormatter::View()
   **/
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings.
    $settings = $display['settings'];

    // Get the data for this field.
    $data = $entity->{'local__child_properties'}['und'];

    if (!$data) {
      return;
    }

    foreach ($data as $i => $value) {

      $child = $value['value'];
      // Assumption: we're on the gene entity.  We've got an array of mRNA feature ID's.
      $header = [
        'Feature Name',
        'Feature Type',
        'Property Name',
        'Property Value',
      ];

      $info = $child['info'];
      $name = $info->uniquename;
      $element[0][$i] = [
        '#type' => 'fieldset',
        '#title' => $name,
        '#attributes' => [
          'class' => [
            'collapsible',
            'collapsed',
          ],
        ],
        '#attached' => ['js' => ['misc/collapse.js', 'misc/form.js']],
      ];

      $rows = $this->getPropRows($child);

      $table = theme('table', ['rows' => $rows, 'header' => $header]);
      $element[0][$i]['prop_table'] = [
        '#markup' => $table,
        '#title' => t("Child Properties for !root", ['!root' => $name]),
      ];
    }
  }

  /**
   * Recursively goes through the child feature array for property listings.
   *
   * @param array $data
   *   Data formatted by the data__sequence_features field.
   *    Expects an info and a children key.
   *
   * @return array
   *   Rows array suitable for table.
   */
  private function getPropRows($data) {

    $rows = [];
    $info = $data['info'];
    $children = $data['children'] ?? NULL;
    $props = $info->featureprop;

    if ($props) {
      // If there is only one property, it will be an object not an array.
      if (is_array($props)) {
        foreach ($props as $prop) {
          $rows[] = [
            $info->uniquename,
            $info->type_id->name,
            $prop->type_id->name,
            $prop->value,
          ];
        }
      }
      else {
        $rows[] = [
          $info->uniquename,
          $info->type_id->name,
          $props->type_id->name,
          $props->value,
        ];
      }
    }

    if ($children && !empty($children)) {
      foreach ($children as $child) {
        $result = array_merge($this->getPropRows($child), $rows);
        $rows = $result;
      }
    }
    return $rows;
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   **/
  public function settingsSummary($view_mode) {
    return '';
  }

}
