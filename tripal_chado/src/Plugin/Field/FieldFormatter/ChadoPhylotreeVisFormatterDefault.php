<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
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
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    // Attaches the css and js for visualization as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.phylotree';

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
    foreach ($color_defaults as $details) {
      if ($details['organism']) {
        // Strip the [id:xxx] from the name.
        $organism_id = preg_replace('/^.+\[id: (\d+)\].*$/', '\1', $details['organism']);
        $colors[$organism_id] = $details['color'];
      }
    }

    // All of the settings used for formatting the phylotree.
    $treeOptions = [
      'phylogram_width' => $this->getSetting('phylogram_width'),
      'root_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_root_node_size') ?? 3,
      'interior_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_interior_node_size') ?? 1,
      'leaf_node_size' => $tripal_chado_settings->get('tripal_phylogeny_default_leaf_node_size') ?? 6,
      // @todo get the setting for skipTicks.
      'skipTicks' => 0,
      'phylogram_scale' => $tripal_chado_settings->get('tripal_phylogeny_default_phylogram_scale') ?? 1,
      'org_colors' => $colors,
    ];

dpm($treeOptions, "CP1 treeOptions");//@@@
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

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['phylogram_width'] = 600;
    $settings['phylogram_scale'] = 'linear';
    $settings['phylogram_root_node_size'] = 3;
    $settings['phylogram_interior_node_size'] = 4;
    $settings['phylogram_leaf_node_size'] = 6;
    $settings['phylogram_colors'] = [];
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['phylogram_width'] = [
      '#type' => 'number',
      '#title' => $this->t('Tree Width'),
      '#description' => $this->t('Please specify the width in pixels for the phylogram.'),
      '#default_value' => $this->getSetting('phylogram_width'),
      '#min' => 64,
      '#max' => 4096,
      '#required' => FALSE,
    ];
    $form['phylogram_scale'] = [
      '#type' => 'select',
      '#title' => $this->t('Phylogram Scale'),
      '#description' => $this->t('Please specify the scale to use.'),
      '#options' => ['linear' => 'Linear', 'logarithmic' => 'Logarithmic'],
      '#default_value' => $this->getSetting('phylogram_scale'),
      '#required' => TRUE,
    ];
    $form['phylogram_root_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Root Node Size'),
      '#description' => $this->t('Please specify a size for the root node. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_root_node_size'),
      '#min' => 0,
      '#max' => 127,
      '#required' => FALSE,
    ];
    $form['phylogram_interior_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Interior Node Size'),
      '#description' => $this->t('Please specify a size for interior nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_interior_node_size'),
      '#min' => 0,
      '#max' => 127,
      '#required' => FALSE,
    ];
    $form['phylogram_leaf_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Leaf Node Size'),
      '#description' => $this->t('Please specify a size for the terminal nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_leaf_node_size'),
      '#min' => 0,
      '#max' => 127,
      '#required' => FALSE,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Width: @phylogram_width',
                          ['@phylogram_width' => $this->getSetting('phylogram_width') ?? 600]);
    $summary[] = $this->t('Scale: @phylogram_scale',
                          ['@phylogram_scale' => $this->getSetting('phylogram_scale') ?? 'Linear']);
    $summary[] = $this->t('Root node: @phylogram_root_node_size',
                          ['@phylogram_root_node_size' => $this->getSetting('phylogram_root_node_size') ?? 3]);
    $summary[] = $this->t('Int. node: @phylogram_interior_node_size',
                          ['@phylogram_interior_node_size' => $this->getSetting('phylogram_interior_node_size') ?? 4]);
    $summary[] = $this->t('Leaf node: @phylogram_leaf_node_size',
                          ['@phylogram_leaf_node_size' => $this->getSetting('phylogram_leaf_node_size') ?? 6]);
    return $summary;
  }

}
