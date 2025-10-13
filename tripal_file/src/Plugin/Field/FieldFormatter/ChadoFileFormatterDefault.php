<?php

namespace Drupal\tripal_file\Plugin\Field\FieldFormatter;

use Drupal\Core\Link;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * The default formatter for chado file content type.
 */
#[TripalFieldFormatter(
  id: 'chado_file_formatter_default',
  label: new TranslatableMarkup('Chado File Formatter'),
  description: new TranslatableMarkup('A chado file formatter'),
  field_types: [
    'chado_file_type_default',
  ],
)]
class ChadoFileFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $rows = [];
    $header = [
      $this->t('File'),
      $this->t('Type'),
    ];
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'entity_id' => $item->get('entity_id')->getString(),
        'name' => $item->get('file_name')->getString(),
        'description' => $item->get('file_description')->getString(),
        'type' => $item->get('file_type')->getString(),
      ];

      // Create a clickable link to the corresponding entity when one exists.
      $renderable_item = $lookup_manager->getRenderableItem($values['name'], $values['entity_id']);
      $link = Link::fromTextAndUrl($renderable_item['#title'], $renderable_item['#url']);

      $row = [$link, $values['type']];
      $rows[$delta] = $row;
    }

    $elements[0] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#wrapper_attributes' => ['class' => 'container'],
    ];

    return $elements;
  }

}
