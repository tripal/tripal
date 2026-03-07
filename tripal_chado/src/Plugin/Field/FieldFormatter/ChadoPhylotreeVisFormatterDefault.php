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
#[TripalFieldFormatter(
  id: 'chado_phylotreevis_formatter_default',
  label: new TranslatableMarkup('Chado Phylogenetic Tree Visualization formatter'),
  description: new TranslatableMarkup('Formats and visually displays a chado phylogenetic tree'),
  field_types: [
    'chado_phylotreevis_type_default',
  ],
)]
class ChadoPhylotreeVisFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    // Attaches the css and js for visualization as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.phylotree';

    // Note that there will only be one item because cardinality = 1.
    foreach ($items as $delta => $item) {
      // Adds the toolbar to the render array.
      $this->toolBarElements($elements, $delta);

      // Adds the placeholder for the phylogram image.
      $elements[$delta]['phylogram'] = [
        '#markup' => '<div class="tree-container" id="phylogram-container"></div>',
      ];

      // Add the variables used by the javascript.
      $elements['#attached']['drupalSettings']['treeData'] = $item->get('tree_data')->getString();
      $elements['#attached']['drupalSettings']['treeOptions'] = $this->getFormatterOptions($item->get('formatter_settings_value')->getString());
    }

    return $elements;
  }

  /**
   * Provides tree formatting options.
   *
   * The content type can override the global defaults, and then an
   * individual entity can further override the content type defaults.
   * These per-entity settings are set in the widget and stored in a
   * chado property.
   *
   * @param string $formatter_settings_json
   *   Formatting settings stored in a JSON string. These are set in
   *   the widget and apply to just this entity.
   *
   * @return array
   *   Options array ready to pass to the javascript.
   */
  protected function getFormatterOptions(string $formatter_settings_json): array {

    // We first get the formatting options that may be provided by the
    // content type. The resulting array is passed to the javascript.
    $color_settings = $this->getSetting('phylogram_colors');
    $colors = [];
    if ($color_settings) {
      foreach ($color_settings as $details) {
        if ($details['organism'] && $details['color']) {
          // Extract the organism_id from the name.
          $organism_id = preg_replace('/^.+\((\d+)\)$/', '\1', $details['organism']);
          $colors[$organism_id] = $details['color'];
        }
      }
    }
    $treeOptions = [
      'phylogram_layout' => $this->getSetting('phylogram_layout'),
      'font_size' => $this->getSetting('phylogram_font_size'),
      'skip_ticks' => $this->getSetting('phylogram_skip_ticks'),
      'root_node_size' => $this->getSetting('phylogram_root_node_size'),
      'root_node_color' => $this->getSetting('phylogram_root_node_color'),
      'interior_node_size' => $this->getSetting('phylogram_interior_node_size'),
      'interior_node_color' => $this->getSetting('phylogram_interior_node_color'),
      'leaf_node_size' => $this->getSetting('phylogram_leaf_node_size'),
      'leaf_node_color' => $this->getSetting('phylogram_leaf_node_color'),
      'org_colors' => $colors,
    ];

    // Formatting options can be overridden for an individual entity.
    // These are set in the widget.
    $formatter_settings = json_decode($formatter_settings_json, TRUE);
    if ($formatter_settings) {
      foreach ($formatter_settings as $key => $value) {
        $treeOptions[$key] = $value;
      }
    }

    return $treeOptions;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    // Valid options are 'linear' or 'radial'.
    $settings['phylogram_layout'] = 'linear';
    $settings['phylogram_font_size'] = 12;
    $settings['phylogram_skip_ticks'] = 0;
    $settings['phylogram_root_node_size'] = 3;
    $settings['phylogram_root_node_color'] = '#404040';
    $settings['phylogram_interior_node_size'] = 4;
    $settings['phylogram_interior_node_color'] = '#808080';
    $settings['phylogram_leaf_node_size'] = 6;
    $settings['phylogram_leaf_node_color'] = '#40A040';
    $settings['phylogram_colors'] = [];
    return $settings;
  }

  /**
   * Adds form elements for the toolbar above the phylotree.
   *
   * @param array &$elements
   *   Render array to have elements added.
   * @param int $delta
   *   Delta, but cardinality is 1, so will always be zero.
   *
   * @return void
   *   No return value, changes are made to the &$elements array.
   */
  protected function toolBarElements(array &$elements, int $delta): void {
    $elements[$delta]['row'] = [
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',
    ];
    $elements[$delta]['row']['toolbar'] = [
      '#prefix' => '<div class="col-md-8 btn-toolbar" role="toolbar" id="phylogram-toolbar">',
      '#suffix' => '</div>',
    ];

    // Group 1.
    $elements[$delta]['row']['toolbar']['vertical_plus'] = [
      '#prefix' => '<div class="btn-group">',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'data-direction' => 'vertical',
        'data-amount' => '2',
        'title' => 'Expand vertical spacing',
      ],
      '#value' => '<i class="fa fa-fw fa-arrows-v"></i>',
    ];
    $elements[$delta]['row']['toolbar']['vertical_minus'] = [
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'data-direction' => 'vertical',
        'data-amount' => '-2',
        'title' => 'Compress vertical spacing',
      ],
      '#value' => '<i class="fa fa-fw fa-compress fa-rotate-135"></i>',
    ];
    $elements[$delta]['row']['toolbar']['horizontal_plus'] = [
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'data-direction' => 'horizontal',
        'data-amount' => '2',
        'title' => 'Expand horizontal spacing',
      ],
      '#value' => '<i class="fa fa-fw fa-arrows-h"></i>',
    ];
    $elements[$delta]['row']['toolbar']['horizontal_minus'] = [
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'data-direction' => 'horizontal',
        'data-amount' => '-2',
        'title' => 'Compress horizontal spacing',
      ],
      '#value' => '<i class="fa fa-fw fa-compress fa-rotate-45"></i>',
    ];
    $elements[$delta]['row']['toolbar']['sort_ascending'] = [
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'id' => 'sort_ascending',
        'title' => 'Sort deepest clades to the bottom',
      ],
      '#value' => '<i class="fa fa-fw fa-sort-amount-asc"></i>',
    ];
    $elements[$delta]['row']['toolbar']['sort_descending'] = [
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'id' => 'sort_descending',
        'title' => 'Sort deepest clades to the top',
      ],
      '#value' => '<i class="fa fa-fw fa-sort-amount-desc"></i>',
    ];
    $elements[$delta]['row']['toolbar']['sort_original'] = [
      '#suffix' => '</div>',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm',
        'id' => 'sort_original',
        'title' => 'Restore original order',
      ],
      '#value' => '<i class="fa fa-fw fa-sort"></i>',
    ];
    // @todo make this work.
    // .  $elements[$delta]['row']['toolbar']['save_image'] = [
    // .    '#suffix' => '</div>',
    // .    '#type' => 'html_tag',
    // .    '#tag' => 'button',
    // .    '#attributes' => [
    // .      'class' => 'btn btn-light btn-sm',
    // .      'id' => 'save_image',
    // .      'title' => 'Save image',
    // .    ],
    // .    '#value' => '<i class="fa fa-fw fa-picture-o"></i>',
    // .  ];
    //
    // Group 2.
    $elements[$delta]['row']['toolbar']['linear'] = [
      '#prefix' => '<div class="btn-group" role="group">',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm active phylotree-layout-mode',
        'data-mode' => 'linear',
      ],
      '#value' => 'Linear',
    ];
    $elements[$delta]['row']['toolbar']['radial'] = [
      '#suffix' => '</div>',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm phylotree-layout-mode',
        'data-mode' => 'radial',
      ],
      '#value' => 'Radial',
    ];

    // Group 3.
    $elements[$delta]['row']['toolbar']['align-left'] = [
      '#prefix' => '<div class="btn-group" role="group">',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm active phylotree-align-toggler',
        'data-align' => 'left',
      ],
      '#value' => '<i class="fa fa-fw fa-align-left"></i>',
    ];
    $elements[$delta]['row']['toolbar']['align-right'] = [
      '#suffix' => '</div>',
      '#type' => 'html_tag',
      '#tag' => 'button',
      '#attributes' => [
        'class' => 'btn btn-light btn-sm phylotree-align-toggler',
        'data-align' => 'right',
      ],
      '#value' => '<i class="fa fa-fw fa-align-right"></i>',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    // The "token_string" setting is not applicable to this field.
    unset($form['token_string']);

    // Attaches the css for the settings form as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $form['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoPhylotreeVisFormatterSettings';

    $form['phylogram_layout'] = [
      '#type' => 'select',
      '#title' => $this->t('Phylogram Layout'),
      '#description' => $this->t('Specify how the phylogram should be presented, Linear or Radial.'),
      '#options' => ['linear' => $this->t('Linear'), 'radial' => $this->t('Radial')],
      '#default_value' => $this->getSetting('phylogram_layout'),
    ];
    $form['phylogram_skip_ticks'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Turn Off Tick Marks'),
      '#description' => $this->t('Check to prevent display of a scale bar with tick marks.'),
      '#default_value' => $this->getSetting('phylogram_skip_ticks'),
    ];
    $form['phylogram_font_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Font Size'),
      '#description' => $this->t('Specify the font size to use to display the phylogram, valid values are from 4 to 12.'),
      '#default_value' => $this->getSetting('phylogram_font_size'),
      '#min' => 4,
      '#max' => 12,
      '#required' => FALSE,
    ];
    $form['phylogram_root_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Root Node Size'),
      '#description' => $this->t('Specify a size for the root node. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_root_node_size'),
      '#min' => 0,
      '#max' => 12,
      '#required' => FALSE,
    ];
    $form['phylogram_root_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Root Node Color'),
      '#description' => $this->t('Specify a color for the root node.'),
      '#default_value' => $this->getSetting('phylogram_root_node_color'),
      '#required' => FALSE,
    ];
    $form['phylogram_interior_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Interior Node Size'),
      '#description' => $this->t('Specify a size for interior nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_interior_node_size'),
      '#min' => 0,
      '#max' => 12,
      '#required' => FALSE,
    ];
    $form['phylogram_interior_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Interior Node Color'),
      '#description' => $this->t('Specify a color for interior nodes.'),
      '#default_value' => $this->getSetting('phylogram_interior_node_color'),
      '#required' => FALSE,
    ];
    $form['phylogram_leaf_node_size'] = [
      '#type' => 'number',
      '#title' => $this->t('Leaf Node Size'),
      '#description' => $this->t('Specify a size for the terminal nodes. If set to zero, the node will not appear.'),
      '#default_value' => $this->getSetting('phylogram_leaf_node_size'),
      '#min' => 0,
      '#max' => 12,
      '#required' => FALSE,
    ];
    $form['phylogram_leaf_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Leaf Node Color'),
      '#description' => $this->t('Specify a color for the terminal nodes.'),
      '#default_value' => $this->getSetting('phylogram_leaf_node_color'),
      '#required' => FALSE,
    ];

    $colors = $this->getSetting('phylogram_colors') ?? [];
    $colors = $this->removeEmptyColors($colors);
    $form['phylogram_colors_info']['desc'] = [
      '#type' => 'item',
      '#title' => $this->t('Node Colors by Organism'),
      '#markup' => $this->t('If the trees are associated with features (e.g. proteins)
        then the nodes can be color-coded by their organism. This helps the user
        visualize which nodes belong to each organism. Please enter the
        name of the organism and specify its color. Organisms that are not given a
        color will the leaf node color as set above.'),
    ];
    $form['phylogram_colors'] = [
      '#element_validate' => [[$this, 'settingsFormValidateOrganism']],
    ];
    // Iterate through the number of organism colors and add a field for
    // each one.
    for ($i = 0; $i < count($colors) + 1; $i++) {
      // Wrapper is used so both fields can be styled onto the same line.
      $form['phylogram_colors'][$i]['organism'] = [
        '#prefix' => '<div class="chado-phylotreevis-formatter-settings-field-wrapper form-item">',
        '#type' => 'textfield',
        '#description' => $this->t('Organism'),
        '#default_value' => $colors[$i]['organism'] ?? '',
        '#autocomplete_route_name' => 'tripal_chado.organism_autocomplete',
        '#autocomplete_route_parameters' => ['match_limit' => 10],
        '#size' => 20,
      ];
      $form['phylogram_colors'][$i]['color'] = [
        '#type' => 'color',
        '#description' => $this->t('Color'),
        '#default_value' => $colors[$i]['color'] ?? '#808080',
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
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Layout: @phylogram_layout',
                          ['@phylogram_layout' => ($this->getSetting('phylogram_layout') ?? 'linear')]);
    if ($this->getSetting('phylogram_skip_ticks') ?? 0) {
      $summary[] = $this->t('Tick marks off');
    }
    $summary[] = $this->t('Font size: @font_size',
                          ['@font_size' => $this->getSetting('phylogram_font_size') ?? '']);
    $summary[] = $this->t('Root: @root_size:@root_color',
                          [
                            '@root_size' => $this->getSetting('phylogram_root_node_size') ?? '',
                            '@root_color' => $this->getSetting('phylogram_root_node_color') ?? '',
                          ]);
    $summary[] = $this->t('Int.: @interior_size:@interior_color',
                          [
                            '@interior_size' => $this->getSetting('phylogram_interior_node_size') ?? '',
                            '@interior_color' => $this->getSetting('phylogram_interior_node_color') ?? '',
                          ]);
    $summary[] = $this->t('Leaf: @leaf_size:@leaf_color',
                          [
                            '@leaf_size' => $this->getSetting('phylogram_leaf_node_size') ?? '',
                            '@leaf_color' => $this->getSetting('phylogram_leaf_node_color') ?? '',
                          ]);
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
  public function settingsFormValidateOrganism(array $form, FormStateInterface $form_state) {
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
          if ($config['organism']) {
            if (!preg_match('/\(\d+\)/', $config['organism'])) {
              $form_state->setErrorByName($field_parents . "][$delta][organism",
                  $this->t('Organism must include numeric record ID inside parentheses, please let the autocomplete add this value'));
            }
          }
        }
      }
    }
  }

}
