<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal Phylotree Visualization formatter.
 */
#[FieldFormatter(
  id: 'chado_phylotree_vis_formatter_default',
  label: new TranslatableMarkup('Chado Phylogenetic Tree Visualization formatter'),
  description: new TranslatableMarkup('Formats and visually displays a chado phylogenetic tree'),
  field_types: [
    'chado_phylotree_vis_type_default',
  ],
)]
/**
 * Plugin implementation of default Tripal Phylotree Visualization formatter.
 *
 * @FieldFormatter(
 *   id = "chado_phylotree_vis_formatter_default",
 *   label = @Translation("Chado Phylogenetic Tree Visualization formatter"),
 *   description = @Translation("Formats and visually displays a chado phylogenetic tree"),
 *   field_types = {
 *     "chado_phylotree_vis_type_default"
 *   },
 * )
 */
class ChadoPhylotreeVisFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    // Attaches the css and js for visualization as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.phylotree';

    // Placeholder image prior to ajax load of phylogram.
    $ajax_image_path = base_path() . \Drupal::service('extension.list.module')->getPath('tripal') . '/images/ajax-loader.gif';

    // Collect the tree display settings to pass to the javascript.
    $tripal_chado_settings = \Drupal::config('tripal_chado.settings');

    // Get the node colors as set by the administrator.
    $color_defaults = $tripal_chado_settings->get('tripal_phylogeny_org_colors') ??
      [
        '1' => [
          'organism' => '',
          'color' => '',
        ],
      ];
    $colors = [];
    foreach ($color_defaults as $i => $details) {
      if ($details['organism']) {
        // Strip the [id:xxx] from the name
        $organism_id = preg_replace('/^.+\[id: (\d+)\].*$/', '\1', $details['organism']);
        $colors[$organism_id] = $details['color'];
      }
    }

    // All of the settings used for formatting the phylotree.
    $treeOptions = [
      'phylogram_width' => $tripal_chado_settings->get('tripal_phylogeny_default_phylogram_width') ?? 350,
      'root_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_root_node_size') ?? 3,
      'interior_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_interior_node_size') ?? 1,
      'leaf_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_leaf_node_size') ?? 6,
      'skipTicks' => 0, //@@@todo
      'phylogram_scale' => $tripal_chado_settings->get('tripal_phylogeny_default_phylogram_scale') ?? 1,
      'org_colors' => $colors,
    ];

    // Will only be one item because cardinality = 1.
    foreach ($items as $delta => $item) {
      $values = [
        'record_id' => $item->get('record_id')->getString(),
        'tree_json' => $item->get('tree_json')->getString(),
      ];

      // Placeholder for the phylogram image.
      $elements[$delta]['phylogram'] = [
        '#markup' => '<div id="chado-phylogram"></div>',
      ];

      // Add the variables used by the javascript.
      $elements['#attached']['drupalSettings']['treeJSON'] = $values['tree_json'];
      $elements['#attached']['drupalSettings']['treeOptions'] = $treeOptions;
    }

    return $elements;
  }

}
