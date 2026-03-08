<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;
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

    // Build the form.
    $elements = $element;//@todo

    // Attaches the css for the settings form as defined in
    // tripal_chado/tripal_chado.libraries.yml.
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoPhylotreeVisWidgetSettings';

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

    // Removes any color items without an organism.
    $colors = $formatter_settings['colors'] ?? [];
    $colors = $this->removeEmptyColors($colors);

    // The "Add another item" button passes through an incremented value
    // through the following form_state variable.
    $color_rows = $form_state->getValue('color_rows');
    if (is_null($color_rows)) {
      $color_rows = count($colors) + 1;
    }

    // This element passes through the current number of color rows for the
    // "Add another item" button.
    $elements['color_rows'] = [
      '#type' => 'value',
      '#value' => $color_rows,
    ];

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
#@todo      '#element_validate' => [[$this, 'settingsFormValidateOrganism']],
      '#prefix' => '<div id="edit-colors">',
      '#suffix' => '</div>',
    ];

    // Iterate through the number of organism colors and add a field for
    // each one.
    for ($i = 0; $i < $color_rows; $i++) {
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

    $elements['actions']['add_row'] = [
      '#type' => 'submit',
      '#value' => $this->t('Add another item'),
      '#ajax' => [
        'callback' => [static::class, 'addRowCallback'],
        'event' => 'click',
        'wrapper' => 'colors',
#        'method' => 'replace', // Replace the entire table wrapper content
        'effect' => 'fade',
      ],
      '#attributes' => [
        'field_name' => $field_name,
        'delta' => $delta,
      ],
      '#submit' => [[static::class, 'incrementRows']],
      '#limit_validation_errors' => [[$field_name, '0', 'color_rows']],
    ];
    return $elements;
  }

  /**
   * Increment number of color settings fields.
   *
   * This submit function passes back an incremented number of
   * rows for color settings by passing it through the form_state
   * as the variable 'color_rows'.
   *
   * @param array &$form
   *   The form array.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   */
  public static function incrementRows(array &$form, FormStateInterface $form_state) {
    $triggering_element = $form_state->getTriggeringElement();
    $machine_name = $triggering_element['#attributes']['field_name'];
    $color_rows = ($form_state->getValue([$machine_name, '0', 'color_rows']) ?? 0);
    $form_state->setValue('color_rows', $color_rows + 1);
    $form_state->setRebuild(TRUE);
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
//@todo
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
   * Builds a select list array for selecting an integer value.
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
    $delta = 0;
    foreach ($keys as $key) {
      if (array_key_exists($key, $values[$delta])) {
        // Key 'colors' is an array, all others are scalars.
        if (is_array($values[$delta][$key])) {
          foreach ($values[$delta][$key] as $index => $color) {
            // Skip if no organism specified (i.e. blank row).
            if ($color['organism']) {
              $settings[$key][$index] = $color;
            }
          }
        }
        else {
          // Don't include if just a null string. This allows the
          // global content type default to be used instead.
          if (strlen($values[$delta][$key])) {
            // For colors, don't include if pure white. We use white to
            // indicate to use the global content type default.
            if (strtolower($values[$delta][$key]) != '#ffffff') {
              $settings[$key] = $values[$delta][$key];
            }
          }
        }
        unset($values[$delta][$key]);
      }
    }
    $values[$delta]['formatter_settings_value'] = json_encode($settings);
    return $values;
  }

  /**
   * Ajax callback to add a row for organism colors.
   *
   * @param array $form
   *   The form array.
   * @param Drupal\Core\Form\FormStateInterface &$form_state
   *   The form state object.
   */
  public static function addRowCallback($form, &$form_state) {

    // Extract the field's machine name and delta from the triggering element.
    $triggering_element = $form_state->getTriggeringElement();
    $machine_name = $triggering_element['#attributes']['field_name'];
    $delta = $triggering_element['#attributes']['delta'];

    $response = new AjaxResponse();
    $color_table = $form[$machine_name]['widget'][$delta]['colors'];
    $response->addCommand(new ReplaceCommand('#edit-colors', $color_table));
    return $response;
  }

}
