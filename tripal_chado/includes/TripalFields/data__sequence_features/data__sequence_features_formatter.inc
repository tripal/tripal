<?php

/**
 * @class
 * Purpose: Displays all child features recursively via a featureviewer drawing.
 *
 * Display:
 * Configuration:
 */
class data__sequence_features_formatter extends ChadoFieldFormatter {

  public static $default_label = 'Transcript Information';
  public static $field_types = ['data__sequence_features'];
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * Featureloc start relative to parent.
   *
   * @var int
   */
  private $parent_start;

  /**
   * Featureloc stop rel to parent.
   *
   * @var int
   */
  private $parent_stop;

  /**
   * Featureloc strand rel to parent.
   *
   * @var string
   */
  private $parent_strand;

  /**
   * Holds converted featureloc information for the feature viewer drawing.
   *
   * @var array
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

    drupal_add_js("https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.4/dist/feature-viewer.bundle.js", [
      'type' => 'external',
      'scope' => 'header',
      'group' => 15,
      'every_page' => TRUE,
      'weight' => 500,
    ]);


    $child_draw = [];
    $sequence = $entity->data__sequence['und'][0]['value'];

    if (!$sequence) {
      // We can't draw without a sequence unfortunately.
      return;
    }

    $coordinates = $entity->data__sequence_coordinates['und'][0]['value'];
    // Featureloc stores location relative to some parent feature, ie a contig.
    // We want the drawing to be relative to the gene feature.
    $this->parent_start = $coordinates['local:fmin'];
    $this->parent_stop = $coordinates['local:fmax'];
    $this->parent_strand = $coordinates['data:0853'];
    $child_draw['residues'] = $sequence;

    foreach ($entity->{'data__sequence_features'}['und'] as $i => $data) {
      // The direct child of the entity feature.
      // If the entity is a gene, this is an mRNA/transcript.
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
        // JS to render collapsible fieldsets properly.
        //'#attached' => ['js' => ['misc/collapse.js', 'misc/form.js']],
      ];
      // The featureviewer drawing div.
      $element[$i]['drawing'] = [
        '#type' => 'item',
        '#prefix' => '<div id="tripal_sequence_features_featureloc_viewer_' . $i . '">',
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
    // TODO: should we allow a property to specify which transcript is primary?
    // Is there any example GFF?
    $element[0]['#attributes']['class'] = ['collapsible'];
    $child_draw['children'] = $this->feature_coords;
    drupal_add_js([
      'children_draw_info' => $child_draw,
    ], 'setting');

    drupal_add_js(drupal_get_path('module', 'tripal_chado') . "/theme/js/tripal_chado_data_sequence_features_featureloc.js");

  }

  /**
   * Builds the featureviewer drawing for a given child feature.
   *
   * @param int $i
   *   Cardinal tracker.
   * @param array $child
   *   Child array as created by data__sequence_features field.
   */
  private function build_featureviewer_data($i, $child) {

    $info = $child['info'];

    $grand_children = $child['children'] ?? NULL;

    // All child features will be drawn on this one in 'data'
    // Convert and store the coordinates for hte feature viewer.
    $this->convertFeatureCoords($i, $info);

    // Repeat recursively for grandchildren and their children etc.
    if ($grand_children) {
      foreach ($grand_children as $grand_child) {
        $this->build_featureviewer_data($i, $grand_child);
      }
    }
  }

  /**
   * Builds featureloc string for display to user in table.
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
   * @param int $i
   *   Cardinality tracker.
   * @param object $info
   *   The chado_expand_var object generated by the field.
   */
  private function convertFeatureCoords($i, $info) {

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
      // TODO: check this assertion.
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
        return '#AC92EB';

      case 'polypeptide':
        return '#4FC1E8';

      case 'CDS':
        return '#A0D568';

      case 'exon':
        return '#FFCE54';

      case NULL:
        return '#ED5564';
    }

  }

  /**
   * Builds a table of recursive children featurelocs.
   *
   * @param array $child
   *   Child array from the base field.  Should have an info and a children key.
   *
   * @return array
   *   Array of rows suitable for theme_table().
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
