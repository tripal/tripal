<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal array_design formatter.
 *
 * @FieldFormatter(
 *   id = "chado_array_design_formatter_default",
 *   label = @Translation("Chado array_design formatter"),
 *   description = @Translation("A chado array_design formatter"),
 *   field_types = {
 *     "chado_array_design_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[version]",
 *     "[manufacturer]",
 *     "[platform]",
 *     "[substrate]",
 *     "[protocol]",
 *     "[db]",
 *     "[accession]",
 *     "[array_dimensions]",
 *     "[element_dimensions]",
 *     "[n_elements]",
 *     "[n_array_columns]",
 *     "[n_array_rows]",
 *     "[n_grid_columns]",
 *     "[n_grid_rows]",
 *     "[n_sub_columns]",
 *     "[n_sub_rows]",
 *   },
 * )
 */
class ChadoArrayDesignFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[name]';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'name' => $item->get('array_design_name')->getString(),
        'description' => $item->get('array_design_description')->getString(),
        'version' => $item->get('array_design_version')->getString(),
        'manufacturer' => $item->get('array_design_manufacturer')->getString(),
        'platform' => $item->get('array_design_platformtype')->getString(),
        'substrate' => $item->get('array_design_substratetype')->getString(),
        'protocol' => $item->get('array_design_protocol')->getString(),
        'db' => $item->get('array_design_database_name')->getString(),
        'accession' => $item->get('array_design_database_accession')->getString(),
        'array_dimensions' => $item->get('array_design_array_dimensions')->getString(),
        'element_dimensions' => $item->get('array_design_element_dimensions')->getString(),
        'n_elements' => $item->get('array_design_num_of_elements')->getString(),
        'n_array_columns' => $item->get('array_design_num_array_columns')->getString(),
        'n_array_rows' => $item->get('array_design_num_array_rows')->getString(),
        'n_grid_columns' => $item->get('array_design_num_grid_columns')->getString(),
        'n_grid_rows' => $item->get('array_design_num_grid_rows')->getString(),
        'n_sub_columns' => $item->get('array_design_num_sub_columns')->getString(),
        'n_sub_rows' => $item->get('array_design_num_sub_rows')->getString(),
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // When possible, create a clickable link to the corresponding entity.
      $item_settings = $item->getDataDefinition()->getSettings();
      $id = $item_settings['storage_plugin_settings']['linker_fkey_column'] ?? 'arraydesign_id';
      $renderable_item = $lookup_manager->getRenderableItem(
        $displayed_string,
        $item->get($id)->getString(),
        $item_settings
      );

      $list[$delta] = $renderable_item;
    }

    // If only one element has been found, don't make into a list.
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
// @todo: add a pager
    elseif (count($list) > 1) {
      $elements[0] = [
        '#theme' => 'item_list',
        '#list_type' => 'ul',
        '#items' => $list,
        '#wrapper_attributes' => ['class' => 'container'],
      ];
    }

    return $elements;
  }

}
