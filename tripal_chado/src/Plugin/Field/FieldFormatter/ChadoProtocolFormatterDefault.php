<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal protocol formatter.
 *
 * @FieldFormatter(
 *   id = "chado_protocol_formatter_default",
 *   label = @Translation("Chado protocol formatter"),
 *   description = @Translation("A chado protocol formatter"),
 *   field_types = {
 *     "chado_protocol_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[hardware]",
 *     "[software]",
 *     "[type]",
 *     "[pub_title]",
 *     "[database_name]",
 *     "[database_accession]",
 *   },
 * )
 */
class ChadoProtocolFormatterDefault extends ChadoFormatterBase {

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
    parent::viewElements($items, $langcode);
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'entity_id' => $item->get('entity_id')->getString(),
        'name' => $item->get('protocol_name')->getString(),
        'description' => $item->get('protocol_protocoldescription')->getString(),
        'hardware' => $item->get('protocol_hardwaredescription')->getString(),
        'software' => $item->get('protocol_softwaredescription')->getString(),
        'type' => $item->get('protocol_type')->getString(),
        'pub_title' => $item->get('protocol_pub_title')->getString(),
        'database_name' => $item->get('protocol_database_name')->getString(),
        'database_accession' => $item->get('protocol_database_accession')->getString(),
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // Create a clickable link to the corresponding entity when one exists.
      $renderable_item = $lookup_manager->getRenderableItem($displayed_string, $values['entity_id']);

      $list[$delta] = $renderable_item;
    }

    // Will convert $list to a markup list if there is more than one item.
    $elements = $this->createListMarkup($list);
    return $elements;
  }

}
