<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal Phylotree Visualization formatter.
 */
#[FieldFormatter(
  id: 'chado_phylotreevis_formatter_default',
  label: new TranslatableMarkup('Chado Phylogenetic Tree Visualization formatter'),
  description: new TranslatableMarkup('Formats and visually displays a chado phylogenetic tree'),
  field_types: [
    'chado_phylotreevis_type_default',
  ],
)]
/**
 * Plugin implementation of default Tripal Phylotree Visualization formatter.
 *
 * @FieldFormatter(
 *   id = "chado_phylotreevis_formatter_default",
 *   label = @Translation("Chado Phylogenetic Tree Visualization formatter"),
 *   description = @Translation("Formats and visually displays a chado phylogenetic tree"),
 *   field_types = {
 *     "chado_phylotreevis_type_default"
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

    // Get the node colors as set by the administrator.
    $color_settings = $this->getSetting('phylogram_colors');
    $colors = [];
    foreach ($color_settings as $details) {
      if ($details['organism'] && $details['color']) {
        // Extract the organism_id from the name.
        $organism_id = preg_replace('/^.+\((\d+)\)$/', '\1', $details['organism']);
        $colors[$organism_id] = $details['color'];
      }
    }

    // Contains all of the settings used for formatting the phylotree.
    $treeOptions = [
      'phylogram_width' => $this->getSetting('phylogram_width'),
      'phylogram_scale' => $this->getSetting('phylogram_scale'),
      'skipTicks' => $this->getSetting('phylogram_skip_ticks'),
      'root_node_size' => $this->getSetting('phylogram_root_node_size'),
      'interior_node_size' => $this->getSetting('phylogram_interior_node_size'),
      'leaf_node_size' => $this->getSetting('phylogram_leaf_node_size'),
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

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['phylogram_width'] = 600;
    $settings['phylogram_scale'] = 1;
    $settings['phylogram_skip_ticks'] = 0;
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

    // Attaches the css for the settings form as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $form['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoPhylotreeVisFormatterSettings';

    // Form elements for each of the settings.
    $form['phylogram_width'] = [
      '#type' => 'number',
      '#title' => $this->t('Tree Width'),
      '#description' => $this->t('Please specify the width in pixels for the phylogram.'),
      '#default_value' => $this->getSetting('phylogram_width'),
      '#min' => 200,
      '#max' => 4000,
      '#required' => FALSE,
    ];
    $form['phylogram_scale'] = [
      '#type' => 'select',
      '#title' => $this->t('Phylogram Scale'),
      '#description' => $this->t('Please specify the scale to use.'),
      '#options' => ['1' => 'Linear', '2' => 'Logarithmic'],
      '#default_value' => $this->getSetting('phylogram_scale'),
    ];
    $form['phylogram_skip_ticks'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Turn Off Tick Marks'),
      '#description' => $this->t('Check to prevent display of tick marks.'),
      '#default_value' => $this->getSetting('phylogram_skip_ticks'),
    ];
    $form['phylogram_root_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Root Node Size'),
      '#description' => $this->t('Please specify a size for the root node. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_root_node_size'),
      '#min' => 0,
      '#max' => 30,
      '#required' => FALSE,
    ];
    $form['phylogram_interior_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Interior Node Size'),
      '#description' => $this->t('Please specify a size for interior nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_interior_node_size'),
      '#min' => 0,
      '#max' => 30,
      '#required' => FALSE,
    ];
    $form['phylogram_leaf_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Leaf Node Size'),
      '#description' => $this->t('Please specify a size for the terminal nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_leaf_node_size'),
      '#min' => 0,
      '#max' => 30,
      '#required' => FALSE,
    ];

    $colors = $this->getSetting('phylogram_colors') ?? [];
    $colors = $this->removeEmptyColors($colors);
    $form['phylogram_colors_info']['desc'] = [
      '#type' => 'item',
      '#title' => t('Node Colors by Organism'),
      '#markup' => t('If the trees are associated with features (e.g. proteins)
        then the nodes can be color-coded by their organism. This helps the user
        visualize which nodes belong to each organism. Please enter the
        name of the organism and its corresponding color in Hex format
        (e.g. #FF0000 or #F00) or a valid color name (e.g. Crimson or
        DarkGreen). Organisms that are not given a color will be gray.'),
    ];
    $form['phylogram_colors'] = [
      '#element_validate' => [[$this, 'settingsFormValidateColors']],
    ];
    // Iterate through the number of organism colors and add a field for each one.
    for ($i = 0; $i < count($colors) + 1; $i++) {
      // Wrapper is used so both fields can be styled onto one the same line.
      $form['phylogram_colors'][$i]['organism'] = [
        '#prefix' => '<div class="chado-phylotreevis-settings-field-wrapper form-item">',
        '#type' => 'textfield',
        '#description' => t('Organism'),
        '#default_value' => $colors[$i]['organism'] ?? '',
        '#autocomplete_route_name' => 'tripal_chado.organism_autocomplete',
        '#autocomplete_route_parameters' => ['match_limit' => 10],
        '#size' => 20,
      ];
      $form['phylogram_colors'][$i]['color'] = [
        '#type' => 'textfield',
        '#description' => t('Color'),
        '#default_value' => $colors[$i]['color'] ?? '',
        '#size' => 10,
        '#suffix' => '</div>',
      ];
    }

    return $form;
  }

  /**
   * Removes empty or incomplete color array elements.
   *
   * @param array $colors
   *   Array of associative arrays with 'organism' and 'color' keys.
   *
   * @return array
   *   The updated array with empty elements removed.
   */
  protected function removeEmptyColors(array $colors): array {
    $updated_colors = [];
    foreach ($colors as $config) {
      if ($config['organism'] && $config['color']) {
        $updated_colors[] = $config;
      }
    }
    return $updated_colors;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $scales = [1 => 'Linear', 2 => 'Log'];
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Width: @phylogram_width',
                          ['@phylogram_width' => $this->getSetting('phylogram_width') ?? '']);
    $summary[] = $this->t('Scale: @phylogram_scale',
                          ['@phylogram_scale' => $scales[$this->getSetting('phylogram_scale') ?? 1]]);
    if ($this->getSetting('phylogram_skip_ticks') ?? 0) {
      $summary[] = $this->t('Tick marks off');
    }
    $summary[] = $this->t('Root node: @phylogram_root_node_size',
                          ['@phylogram_root_node_size' => $this->getSetting('phylogram_root_node_size') ?? '']);
    $summary[] = $this->t('Int. node: @phylogram_interior_node_size',
                          ['@phylogram_interior_node_size' => $this->getSetting('phylogram_interior_node_size') ?? '']);
    $summary[] = $this->t('Leaf node: @phylogram_leaf_node_size',
                          ['@phylogram_leaf_node_size' => $this->getSetting('phylogram_leaf_node_size') ?? '']);
    $n_colors = count($this->removeEmptyColors($this->getSetting('phylogram_colors') ?? []));
    if ($n_colors) {
      $summary[] = $this->t('Colors: @n_colors',
                          ['@n_colors' => $n_colors]);
    }
    return $summary;
  }

  /**
   * Form element validation handler for organism colors.
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public function settingsFormValidateColors(array $form, FormStateInterface $form_state) {
    // ID for this field is "chado_phylotreevis_formatter_default".
    $plugin_definition = $this->getPluginDefinition();
    $id = $plugin_definition['id'];

    // This is used for setting validation errors on specific fields.
    $field_parents = implode('][', $form['#parents']);

    // This form state can contain settings for all of the fields for the
    // current content type, but we only want to validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field_settings) {
      if (($field_settings['type'] == $id) and (array_key_exists('settings_edit_form', $field_settings))) {
        $phylogram_colors = $field_settings['settings_edit_form']['settings']['phylogram_colors'] ?? [];
        foreach ($phylogram_colors as $delta => $config) {
          // Ignore blank entries.
          if ($config['organism'] || $config['color']) {
            if (!preg_match('/\(\d+\)/', $config['organism'])) {
              $form_state->setErrorByName($field_parents . "][$delta][organism",
                  $this->t('Organism must include numeric record ID inside parentheses, please let the autocomplete add this value'));
            }
            // Only hex color codes are validated, others are assumed to be
            // color names, e.g. Blue.
            if (preg_match('/^#/', $config['color'])) {
              if (!(preg_match('/^#[0-9A-Fa-f]{3}$/', $config['color'])
                  || preg_match('/^#[0-9A-Fa-f]{6}$/', $config['color']))) {
                $form_state->setErrorByName($field_parents . "][$delta][color",
                    $this->t('Hex color codes must be of the format #000000 or #000'));
              }
            }
          }
        }
      }
    }
  }

}
