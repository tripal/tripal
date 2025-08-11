<?php

namespace Drupal\tripal_file\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

#[TripalFieldFormatter(
  id: 'tripal_license_formatter_default',
  label: new TranslatableMarkup('Tripal license formatter'),
  description: new TranslatableMarkup('A tripal license formatter'),
  field_types: [
    'tripal_license_type_default',
  ],
  valid_tokens: [
    '[name]',
    '[description]',
    '[uri]',
  ],
)]
class TripalLicenseFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('license_name')->getString(),
        'description' => $item->get('license_description')->getString(),
        'type' => $item->get('license_type')->getString(),
      ];

      // Change the non-user-friendly 'null' license, which is specified by chado.
      if ($values['name'] == 'null') {
        $values['name'] = 'Unknown';
      }

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // Create a clickable link to the corresponding entity when one exists.
      $renderable_item = $lookup_manager->getRenderableItem($displayed_string, $values['entity_id']);

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
