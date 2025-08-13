<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal assay formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_assay_formatter_default',
  label: new TranslatableMarkup('Chado assay formatter'),
  description: new TranslatableMarkup('A chado assay formatter'),
  field_types: [
    'chado_assay_type_default',
  ],
  valid_tokens: [
    '[name]',
    '[description]',
    '[arraydesign]',
    '[arrayidentifier]',
    '[arraybatchidentifier]',
    '[protocol]',
    '[operator]',
    '[database_name]',
    '[database_accession]',
  ],
)]
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
    parent::viewElements($items, $langcode);
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'entity_id' => $item->get('entity_id')->getString(),
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

      // Create a clickable link to the corresponding entity when one exists.
      $renderable_item = $lookup_manager->getRenderableItem($displayed_string, $values['entity_id']);

      $list[$delta] = $renderable_item;
    }

    // Will convert $list to a markup list if there is more than one item.
    $elements = $this->createListMarkup($list);
    return $elements;
  }

}
