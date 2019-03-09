<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__child_annotations_formatter extends ChadoFieldFormatter {

  public static $default_label = 'Child Annotations';
  public static $field_types = ['local__child_annotations'];
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
    $data = $entity->{'local__child_annotations'}['und'];

    if (!$data) {

      return;
    }

    foreach ($data as $i => $value) {

      $child = $value['value'];
      // Assumption: we're on the gene entity.  We've got an array of mRNA feature ID's.

      if (!$child) {
        continue;
      }
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

      $rows = $this->getAnnotationRows($child);

      $table = theme('table', ['rows' => $rows, 'header' => $header]);
      $element[0][$i]['prop_table'] = [
        '#markup' => $table,
        '#title' => t("Child Annotations for !root", ['!root' => $name]),
      ];
    }
  }

  /**
   * Recursively goes through the child feature array for annotation listings.
   *   Such listings are in feature_cvterm.
   *
   * @param array $data
   *   Data formatted by the data__sequence_features field.
   *    Expects an info and a children key.
   *
   * @return array
   *   Rows array suitable for table.
   */
  private function getAnnotationRows($data) {

    $rows = [];

    $info = $data['info'];

    $children = $data['children'] ?? NULL;

    $annotations = $info->feature_cvterm;

    if ($annotations) {

      if (is_array($annotations)) {
        foreach ($annotations as $ann) {

          $annotation_name = $ann->cvterm_id->name;
          if ($ann->is_not) {
            $annotation_name = "is not " . $annotation_name;
          }

          $rows[] = [
            $info->uniquename,
            $info->type_id->name,
            $annotation_name,
          ];
        }
      }
      else {
        $annotation_name = $annotations->cvterm_id->name;
        if ($annotations->is_not) {
          $annotation_name = "is not " . $annotation_name;
        }

        $rows[] = [
          $info->uniquename,
          $info->type_id->name,
          $annotation_name,
        ];

      }

    }

    if ($children && !empty($children)) {

      foreach ($children as $child) {
        $rows = array_merge($this->getAnnotationRows($child), $rows);
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
