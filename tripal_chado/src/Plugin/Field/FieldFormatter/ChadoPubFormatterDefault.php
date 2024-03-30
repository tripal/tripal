<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal pub formatter.
 *
 * @FieldFormatter(
 *   id = "chado_pub_formatter_default",
 *   label = @Translation("Chado pub formatter"),
 *   description = @Translation("A chado pub formatter"),
 *   field_types = {
 *     "chado_pub_type_default"
 *   },
 *   valid_tokens = {
 *     "[title]",
 *     "[volumetitle]",
 *     "[volume]",
 *     "[series_name]",
 *     "[issue]",
 *     "[pyear]",
 *     "[pages]",
 *     "[miniref]",
 *     "[uniquename]",
 *     "[type]",
 *     "[is_obsolete]",
 *     "[publisher]",
 *     "[pubplace]",
 *   },
 * )
 */
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
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
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

      // When possible, create a clickable link to the corresponding entity.
      $item_settings = $item->getDataDefinition()->getSettings();
      $id = $item_settings['storage_plugin_settings']['linker_fkey_column'] ?? 'pub_id';
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
