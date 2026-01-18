<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal pub formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_pub_formatter_default',
  label: new TranslatableMarkup('Chado pub formatter'),
  description: new TranslatableMarkup('A chado pub formatter'),
  field_types: [
    'chado_pub_type_default',
  ],
  valid_tokens: [
    '[title]',
    '[volumetitle]',
    '[volume]',
    '[series_name]',
    '[issue]',
    '[pyear]',
    '[pages]',
    '[miniref]',
    '[uniquename]',
    '[type]',
    '[is_obsolete]',
    '[publisher]',
    '[pubplace]',
  ],
)]
class ChadoPubFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    // uniquename is generally the citation and has a not null constraint
    $settings['token_string'] = '[uniquename]';
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
        'title' => $item->get('pub_title')->getString(),
        'volumetitle' => $item->get('pub_volumetitle')->getString(),
        'volume' => $item->get('pub_volume')->getString(),
        'series_name' => $item->get('pub_series_name')->getString(),
        'issue' => $item->get('pub_issue')->getString(),
        'pyear' => $item->get('pub_pyear')->getString(),
        'pages' => $item->get('pub_pages')->getString(),
        'miniref' => $item->get('pub_miniref')->getString(),
        'uniquename' => $item->get('pub_uniquename')->getString(),
        'type' => $item->get('pub_type')->getString(),
        'is_obsolete' => $item->get('pub_is_obsolete')->getString(),
        'publisher' => $item->get('pub_publisher')->getString(),
        'pubplace' => $item->get('pub_pubplace')->getString(),
      ];

      // Change the non-user-friendly 'null' publication.
      if ($values['uniquename'] == 'null') {
        $values['uniquename'] = 'Unknown';
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

    // Will convert $list to a markup list if there is more than one item.
    $elements = $this->createListMarkup($list);
    return $elements;
  }

}
