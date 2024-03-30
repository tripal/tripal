<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal assay formatter.
 *
 * @FieldFormatter(
 *   id = "chado_assay_formatter_default",
 *   label = @Translation("Chado assay formatter"),
 *   description = @Translation("A chado assay formatter"),
 *   field_types = {
 *     "chado_assay_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[arraydesign]",
 *     "[arrayidentifier]",
 *     "[arraybatchidentifier]",
 *     "[protocol]",
 *     "[operator]",
 *     "[database_name]",
 *     "[database_accession]",
 *   },
 * )
 */
class ChadoAssayFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('assay_name')->getString(),
        'description' => $item->get('assay_description')->getString(),
        'arraydesign' => $item->get('assay_arraydesign')->getString(),
        'arrayidentifier' => $item->get('assay_arrayidentifier')->getString(),
        'arraybatchidentifier' => $item->get('assay_arraybatchidentifier')->getString(),
        'protocol' => $item->get('assay_protocol')->getString(),
        'operator' => $item->get('assay_operator')->getString(),
        'database_name' => $item->get('assay_database_name')->getString(),
        'database_accession' => $item->get('assay_database_accession')->getString(),
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // When possible, create a clickable link to the corresponding entity.
      $item_settings = $item->getDataDefinition()->getSettings();
      $id = $item_settings['storage_plugin_settings']['linker_fkey_column'] ?? 'assay_id';
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
