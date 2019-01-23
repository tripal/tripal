<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__child_properties_formatter extends ChadoFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Child Properties';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = ['local__child_properties'];

  /**
   * The list of default settings for this formatter.
   */
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

    // For now, values come from the data__sequence_features field.
    // $data = $entity->{'data__sequence_features'}['und'];.
    // insert into chado.featureprop (feature_id, type_id, value, rank) VALUES (5505, 100, 'some madeup prop value on a protein', 0);.
    $data = $entity->{'local__child_properties'}['und'];

    if (!$data) {
      return;
    }

    $i = 0;

    foreach ($data as $i => $value) {

      $child = $value['value'];

      // TODO: this is an extra undefined!  bad.
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
        // see: https://www.drupal.org/forum/support/module-development-and-code-questions/2012-02-07/drupal-render-fieldset-element
        '#attached' => ['js' => ['misc/collapse.js', 'misc/form.js']],
      ];

      $rows = $this->getPropRows($child);

      $table = theme('table', ['rows' => $rows, 'header' => $header]);
      $element[0][$i]['prop_table'] = [
        '#markup' => $table,
        '#title' => t("Child Properties for !root", ['!root' => $name]),
      ];

      $rows = $this->getAnnotationRows($child);
      $header = ['name', 'type', 'annotation'];

      $table = theme('table', ['rows' => $rows, 'header' => $header]);
      $element[0][$i]['annotation_table'] = [
        '#markup' => $table,
        '#title' => t("Annotations for !root", ['!root' => $name]),
      ];

      $i++;
    }
  }

  /**
   * This functionality should move to a separate field.
   *
   * @param $data
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
   * Recursively goes through the child feature array and builds an array of.
   *
   * @param $data
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
