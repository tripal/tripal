<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal stock formatter.
 *
 * @FieldFormatter(
 *   id = "chado_stock_formatter_default",
 *   label = @Translation("Chado stock formatter"),
 *   description = @Translation("A chado stock formatter"),
 *   field_types = {
 *     "chado_stock_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[uniquename]",
 *     "[description]",
 *     "[type]",
 *     "[is_obsolete]",
 *     "[database_name]",
 *     "[database_accession]",
 *     "[genus]",
 *     "[species]",
 *     "[infratype]",
 *     "[infratype_abbrev]",
 *     "[infraname]",
 *     "[abbreviation]",
 *     "[common_name]",
 *   },
 * )
 */
class ChadoStockFormatterDefault extends ChadoFormatterBase {

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

    foreach ($items as $delta => $item) {
      $values = [
        'name' => $item->get('stock_name')->getString(),
        'uniquename' => $item->get('stock_uniquename')->getString(),
        'description' => $item->get('stock_description')->getString(),
        'is_obsolete' => $item->get('stock_is_obsolete')->getString(),
        'type' => $item->get('stock_type')->getString(),
        'database_name' => $item->get('stock_database_name')->getString(),
        'database_accession' => $item->get('stock_database_accession')->getString(),
        'genus' => $item->get('stock_genus')->getString(),
        'species' => $item->get('stock_species')->getString(),
        'infratype' => $item->get('stock_infraspecific_type')->getString(),
        'infraname' => $item->get('stock_infraspecific_name')->getString(),
        'abbreviation' => $item->get('stock_abbreviation')->getString(),
        'common_name' => $item->get('stock_common_name')->getString(),
      ];

      // Special case handling for abbreviation of infraspecific type
      $values['infratype_abbrev'] = chado_abbreviate_infraspecific_rank($values['infratype']);

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }
      $list[$delta] = [
        '#markup' => $displayed_string,
      ];
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
