<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal phylotree formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_phylotree_formatter_default',
  label: new TranslatableMarkup('Chado phylotree formatter'),
  description: new TranslatableMarkup('A chado phylotree formatter'),
  field_types: [
    'chado_phylotree_type_default',
  ],
  valid_tokens: [
    '[name]',
    '[comment]',
    '[type]',
    '[analysis]',
    '[accession]',
    '[db]',
  ],
)]
class ChadoPhylotreeFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('phylotree_name')->getString(),
        'comment' => $item->get('phylotree_comment')->getString(),
        'type' => $item->get('phylotree_type')->getString(),
        'analysis' => $item->get('phylotree_analysis_name')->getString(),
        'accession' => $item->get('phylotree_database_accession')->getString(),
        'db' => $item->get('phylotree_database_name')->getString(),
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
