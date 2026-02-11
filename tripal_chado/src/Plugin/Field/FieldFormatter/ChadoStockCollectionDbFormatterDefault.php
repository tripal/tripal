<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Url;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Implementation of 'chado_stockcollection_db_formatter_default' formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_stockcollection_db_formatter_default',
  label: new TranslatableMarkup('Chado Stock Collection DB Formatter'),
  description: new TranslatableMarkup('Displays the specific database that a stock collection links to.'),
  field_types: [
    'chado_stockcollection_db_type_default',
  ],
)]
class ChadoStockCollectionDbFormatterDefault extends ChadoFormatterBase {

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
        'entity_id' => $item->get('entity_id')->getString(),
        'name' => $item->get('db_name')->getString(),
        'url' => $item->get('db_url')->getString(),
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // Create a clickable link to the corresponding entity when one exists.
      if ($values['entity_id'] > 0) {
        $renderable_item = $lookup_manager->getRenderableItem($displayed_string, $values['entity_id']);
      }
      // Otherwise, check for a URL saved in chado and link to that.
      elseif ($values['url']) {
        $renderable_item = [
          '#type' => 'link',
          '#title' => $displayed_string,
          '#url' => Url::fromUri($values['url']),
        ];
      }
      // If neither exists, use the displayed string without a link.
      else {
        $renderable_item = ['#markup' => $displayed_string];
      }

      $list[$delta] = $renderable_item;
    }

    // If only one element has been found, don't make into a list.
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
    // @todo add pager here.
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
