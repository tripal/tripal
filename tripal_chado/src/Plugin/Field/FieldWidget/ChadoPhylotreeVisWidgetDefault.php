<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Phylotree Visualization widget.
 */
#[TripalFieldWidget(
  id: 'chado_phylotreevis_widget_default',
  label: new TranslatableMarkup('Chado Phylogenetic Visualization default widget'),
  description: new TranslatableMarkup('Provides visualization settings for a Chado Phylotree Visualization.'),
  field_types: [
    'chado_phylotreevis_type_default',
  ],
)]
class ChadoPhylotreeVisWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $elements = [];

    // Attaches the css for the settings form as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoPhylotreeVisWidgetSettings';

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();
    $field_name = $field_definition->get('field_name');
    $formatterSettingsTerm = $field_settings['formatterSettingsTerm'];
    $term_parts = explode(':', $formatterSettingsTerm, 2);
    $cvterm_instance = \Drupal::service('tripal_chado.chado_buddy')->createInstance('chado_cvterm_buddy', []);
    $cvterm_records = $cvterm_instance->getCvterm(['db.name' => $term_parts[0], 'dbxref.accession' => $term_parts[1]]);
    $settings_cvterm_id = $cvterm_records[0]->getValue('cvterm.cvterm_id');

    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $tree_data = $item_vals['tree_data'] ?? '';
    $prop_id = $item_vals['formatter_settings_id'] ?? 0;
    $linker_id = $item_vals['formatter_settings_prop_fkey'] ?? 0;
    $json_settings = $item_vals['formatter_settings_value'] ?? '';
    $formatter_settings = json_decode($json_settings, TRUE);
    $settings_rank = 0;

    // Get the bundle level formatter settings (used for display only).
    $entity_type = $field_definition->get('entity_type');
    $bundle = $field_definition->get('bundle');
    $display = \Drupal::service('entity_display.repository')->getViewDisplay($entity_type, $bundle);
    $formatter = $display->getComponent($field_name);
    $bundle_settings = $formatter['settings'];
#array:10 [▼
#  "phylogram_layout" => "linear"
#  "phylogram_font_size" => "12"
#  "phylogram_skip_ticks" => 1
#  "phylogram_root_node_size" => 3
#  "phylogram_root_node_color" => "#404040"
#  "phylogram_interior_node_size" => 4
#  "phylogram_interior_node_color" => "#808080"
#  "phylogram_leaf_node_size" => 6
#  "phylogram_leaf_node_color" => "#40A040"
#  "phylogram_colors" => []

    // Build the form.
    $elements['record_id'] = [
      '#type' => 'value',
      '#value' => $record_id,
    ];
    $elements['tree_data'] = [
      '#type' => 'value',
      '#value' => $tree_data,
    ];
    $elements['formatter_settings_id'] = [
      '#type' => 'value',
      '#value' => $prop_id,
    ];
    $elements['formatter_settings_prop_fkey'] = [
      '#type' => 'value',
      '#value' => $linker_id,
    ];
    // Pass the field machine name through the form for massageFormValues().
    $elements['field_name'] = [
      '#type' => 'value',
      '#value' => $field_name,
    ];
    $elements['formatter_settings_value'] = [
      '#type' => 'value',
      '#value' => $json_settings,
    ];
    $elements['formatter_settings_type_id'] = [
      '#type' => 'value',
      '#value' => $settings_cvterm_id,
    ];
    $elements['formatter_settings_rank'] = [
      '#type' => 'value',
      '#value' => $settings_rank,
    ];

    $elements['phylogram_layout'] = [
      '#type' => 'select',
      '#title' => $this->t('Phylogram Layout'),
      '#description' => $this->t('Please specify how the phylogram should be presented, Linear or Radial.'),
      '#options' => [
        'linear' => $this->t('Linear'),
        'radial' => $this->t('Radial'),
      ],
      '#default_value' => $formatter_settings['phylogram_layout'] ?? 'linear',
    ];

    $default = $bundle_settings['phylogram_skip_ticks'] ? $this->t('Hide') : $this->t('Display');
    $elements['skip_ticks'] = [
      '#type' => 'select',
      '#title' => $this->t('Display of Tick Marks'),
      '#description' => $this->t('Controls display of a scale bar with tick marks.'),
      '#options' => [
        '' => $this->t('Use content type default (@default)', ['@default' => $default]),
        '0' => $this->t('Display scale bar'),
        '1' => $this->t('Hide scale bar'),
      ],
      '#default_value' => $formatter_settings['skip_ticks'] ?? '',
    ];

    $elements['font_size'] = [
      '#type' => 'select',
      '#options' => $this->getSelectOptions(4, 12, $bundle_settings['phylogram_font_size'] ?? 3),
      '#title' => $this->t('Font Size'),
      '#description' => $this->t('Specify the font size to use to display the phylogram, valid values are from 4 to 12.'),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['root_node_size'] ?? '',
    ];

    $elements['root_node_size'] = [
      '#type' => 'select',
      '#options' => $this->getSelectOptions(0, 12, $bundle_settings['phylogram_root_node_size'] ?? 3),
      '#title' => $this->t('Root Node Size'),
      '#description' => $this->t('Specify the diameter of the root node, between 0 and 12.'),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['root_node_size'] ?? '',
    ];

    $elements['root_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Root Node Color'),
      '#description' => $this->t('Specify the color of the root node. If white is specified, the content type default (@color) will be used.',
         ['@color' => $bundle_settings['phylogram_root_node_color']]),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['root_node_color'] ?? '#ffffff',
    ];

    $elements['interior_node_size'] = [
      '#type' => 'select',
      '#options' => $this->getSelectOptions(0, 12, $bundle_settings['phylogram_interior_node_size'] ?? 4),
      '#title' => $this->t('Internal Node Size'),
      '#description' => $this->t('Specify the diameter of internal nodes, between 0 and 12.'),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['interior_node_size'] ?? '',
    ];
    $elements['interior_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Internal Node Color'),
      '#description' => $this->t('Specify the color of the internal nodes. If white is specified, the content type default (@color) will be used.',
         ['@color' => $bundle_settings['phylogram_interior_node_color']]),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['interior_node_color'] ?? '#ffffff',
    ];

    $elements['leaf_node_size'] = [
      '#type' => 'select',
      '#options' => $this->getSelectOptions(0, 12, $bundle_settings['phylogram_leaf_node_size'] ?? 6),
      '#title' => $this->t('Leaf Node Size'),
      '#description' => $this->t('Specify the diameter of leaf nodes, between 0 and 12.'),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['leaf_node_size'] ?? '',
    ];

    $elements['leaf_node_color'] = [
      '#type' => 'color',
      '#title' => $this->t('Leaf Node Color'),
      '#description' => $this->t('Specify the color of the leaf nodes. If white is specified, the content type default (@color) will be used.',
         ['@color' => $bundle_settings['phylogram_leaf_node_color']]),
      '#required' => FALSE,
      '#default_value' => $formatter_settings['leaf_node_color'] ?? '#ffffff',
    ];

    $colors = $formatter_settings['colors'] ?? [];
    $colors = $this->removeEmptyColors($colors);
    $elements['colors_info']['desc'] = [
      '#type' => 'item',
      '#title' => $this->t('Node Colors by Organism'),
      '#markup' => $this->t('If the trees are associated with features (e.g. proteins)
        then the nodes can be color-coded by their organism. This helps the user
        visualize which nodes belong to each organism. Please enter the
        name of the organism and specify its color. Organisms that are not given a
        color will the leaf node color as set above.'),
    ];
    $elements['colors'] = [
      '#element_validate' => [[$this, 'settingsFormValidateOrganism']],
    ];
    // Iterate through the number of organism colors and add a field for
    // each one.
    for ($i = 0; $i < count($colors) + 1; $i++) {
      // Wrapper is used so both fields can be styled onto the same line.
      $elements['colors'][$i]['organism'] = [
        '#prefix' => '<div class="chado-phylotreevis-widget-settings-field-wrapper form-item">',
        '#type' => 'textfield',
        '#description' => $this->t('Organism'),
        '#default_value' => $colors[$i]['organism'] ?? '',
        '#autocomplete_route_name' => 'tripal_chado.organism_autocomplete',
        '#autocomplete_route_parameters' => ['match_limit' => 10],
        '#size' => 20,
      ];
      $elements['colors'][$i]['color'] = [
        '#type' => 'color',
        '#description' => $this->t('Color'),
        '#default_value' => $colors[$i]['color'] ?? '#808080',
        '#suffix' => '</div>',
      ];
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'formatter_settings' => '',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    $json_settings = $this->getSetting('formatter_settings_value') ?? '';
    $formatter_settings = json_decode($json_settings, TRUE);
    $summary[] = $this->t("Json(temp): @json", ['@json' => $json_settings]);

    return $summary;
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
   * Builds a list for selecting an integer.
   *
   * @param int $min
   *   The minimum allowed value.
   * @param int $max
   *   The maximum allowed value.
   * @param int $default
   *   The global default value.
   */
  protected function getSelectOptions(int $min, int $max, int $default) {
    $options = [];
    $options[''] = $this->t('Use Content Type Default (@default)', ['@default' => $default]);
    for ($i = $min; $i <= $max; $i++) {
      $options[$i] = $i;
    }
    return $options;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Convert the individual formatter settings from the widget form
    // into a single json string. Delta is hardcoded as zero because
    // this field is always cardinality 1.
    $keys = [
      'phylogram_layout',
      'font_size',
      'skip_ticks',
      'root_node_size',
      'root_node_color',
      'interior_node_size',
      'interior_node_color',
      'leaf_node_size',
      'leaf_node_color',
      'colors',
    ];
    $settings = [];
    foreach ($keys as $key) {
      if (array_key_exists($key, $values[0])) {
        // Don't include if just a null string. This allows the
        // global content type default to be used instead.
        if (strlen($values[0][$key])) {
          // For colors, don't include if pure white, this indicates
          // to use the global content type default.
          if (strtolower($values[0][$key]) != '#ffffff') {
            $settings[$key] = $values[0][$key];
          }
        }
        unset($values[0][$key]);
      }
    }
    // In JSON, an empty array is represented by a pair of square brackets, [].
    $values[0]['formatter_settings_value'] = json_encode($settings);
    return $values;
  }

}
