<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__sequence_features_formatter extends ChadoFieldFormatter {

  /**
   * The default label for this field.
   */
  public static $default_label = 'Transcript Information';

  /**
   * The list of field types for which this formatter is appropriate.
   */
  public static $field_types = ['data__sequence_features'];

  /**
   * The list of default settings for this formatter.
   */
  public static $default_settings = [
    'setting1' => 'default_value',
  ];


  /**
   * Featureloc start rel to parent.
   */
  private $parent_start;

  /**
   * Featureloc stop rel to parent.
   */
  private $parent_stop;

  /**
   * Featureloc strand rel to parent.
   */
  private $parent_strand;

  /**
   * Holds converted featureloc information for the feature viewer drawing.
   */
  private $feature_coords;

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

    $parent = $entity->chado_record->feature_id;

    drupal_add_js("https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.0/dist/feature-viewer.bundle.js", [
      'type' => 'external',
      'scope' => 'header',
      'group' => 15,
      'every_page' => TRUE,
      'weight' => 500,
    ]);

    $child_draw = [];
    $sequence = $entity->data__sequence['und'][0]['value'];

    if (!$sequence) {
      // TODO: We cant draw without a sequence.
      // Now what?
    }

    $coordinates = $entity->data__sequence_coordinates['und'][0]['value'];

    $this->parent_start = $coordinates['local:fmin'];
    $this->parent_stop = $coordinates['local:fmax'];
    $this->parent_strand = $coordinates['data:0853'];

    $child_draw['residues'] = $sequence;

    foreach ($entity->{'data__sequence_features'}['und'] as $i => $data) {
      $child = $data['value'];

      $info = $child['info'];
      $name = $info->uniquename;

      $element[$i] = [
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
      $element[$i]['drawing'] = [
        '#type' => 'item',
        '#title' => t('Drawing'),
        '#prefix' => '<div id="tripal_manage_expression_featureloc_viewer_' . $i . '">',
        '#suffix' => '</div>',
      ];

      $rows = $this->buildChildTable($child);

      $this->build_featureviewer_data($i, $child);

      if (empty($rows)) {
        continue;
      }

      $header = [
        'Name',
        'Type',
        'Location',
      ];
      $output = theme('table', ['header' => $header, 'rows' => $rows]);
      $element[$i][$i . 'table'] = ['#markup' => $output];

      unset($rows);

    }

    // Un-collapse the first fieldset.
    $element[0]['#attributes']['class'] = ['collapsible'];

    $child_draw['children'] = $this->feature_coords;
    // Pass in the needed JS info.
    drupal_add_js([
      'children_draw_info' => $child_draw,
    ], 'setting');

    drupal_add_js(drupal_get_path('module', 'tripal_manage_analyses') . "/theme/js/tripal_manage_analyses_featureloc.js");

  }

  /**
   *
   */
  private function build_featureviewer_data($i, $child) {

    $info = $child['info'];

    $grand_children = $child['children'] ?? NULL;

    // Set base info
    // All child features will be drawn on this one in 'data'
    // Convert and store the coordinates for hte feature viewer.
    $this->convertFeatureCoords($i, $info->feature_id, $info);

    // Repeat for grandchildren;.
    if ($grand_children) {
      foreach ($grand_children as $grand_child) {
        $this->build_featureviewer_data($i, $grand_child);
      }
    }
  }

  /**
   * Builds featureloc string for display to user.
   *
   * @param $featureloc
   *   The featureloc object returned from chado_expand_var on featureloc.
   *
   * @return string
   */
  private function buildFeatureString($featureloc) {

    $info = $featureloc->feature_id;

    $min = $info->fmin;
    $max = $info->fmax;
    $strand = $info->strand;
    $parent = $info->srcfeature_id->name;

    if (!$min or !$max or !$strand) {
      return 'No location available.';
    }

    $out = "${parent}:  ${min}-${max} (${strand})";

    return $out;
  }

  /**
   * Converts featureloc coordinates to be based on the entity.
   *
   * @param $i
   * @param $feature_id
   * @param $info
   */
  private function convertFeatureCoords($i, $feature_id, $info) {

    $featureloc = $info->featureloc->feature_id;
    // TODO: what if theres no featureloc relative to a parent?
    $parent_start = $this->parent_start;
    $parent_stop = $this->parent_stop;
    $strand = $this->parent_strand;

    $min = $featureloc->fmin;
    $max = $featureloc->fmax;
    $strand = $featureloc->strand;

    if ($strand == '+') {

      // It doesnt matter what strand it is, we always do this.
      // TODO: check that assertion.
      $start = $min - $parent_start + 1;
      $stop = $max - $parent_start + 1;
    }

    else {
      $start = $min - $parent_start + 1;
      $stop = $max - $parent_start + 1;
    }

    $type = $info->type_id->name;

    $color = $this->get_feature_color($type);

    if (!isset($this->feature_coords[$i][$type])) {

      $this->feature_coords[$i][$type] = [
        'name' => $type,
        'color' => $color,
        'type' => 'rect',
      ];
    }

    $this->feature_coords[$i][$type]['data'][] = [
      'x' => $start,
      'y' => $stop,
      'description' => $info->uniquename,
    ];

  }

  /**
   * A color lookup to pass different colors to different feature subtypes.
   *
   * @param string $name
   *   The feature type name.
   *
   * @return string
   *   a hex color code.
   */
  private function get_feature_color(string $name) {

    switch ($name) {
      case 'mRNA':
        return '#12E09D';

      case 'polypeptide':
        return '#808080';

      case 'CDS':
        return '#FF0000';

      case 'exon':
        return '#F4D4AD';

      case NULL:
        return '#000000';
    }

  }

  /**
   *
   */
  private function buildChildTable($child) {

    $rows = [];

    $children = $child['children'];

    foreach ($children as $gchild) {

      $info = $gchild['info'];

      $location = isset($info->featureloc) ? $this->buildFeatureString($info->featureloc) : 'Not available';

      $rows[] = [
        'Name' => $info->uniquename,
        'Type' => $info->type_id->name,
        'Locations' => $location,
      ];

      if (isset($gchild['children'])) {
        $ggchild = $this->buildChildTable($gchild);

        $rows = array_merge($rows, $ggchild);
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
