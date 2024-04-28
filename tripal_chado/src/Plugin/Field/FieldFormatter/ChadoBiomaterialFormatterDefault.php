<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal biomaterial formatter.
 *
 * @FieldFormatter(
 *   id = "chado_biomaterial_formatter_default",
 *   label = @Translation("Chado biomaterial formatter"),
 *   description = @Translation("A chado biomaterial formatter"),
 *   field_types = {
 *     "chado_biomaterial_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[biosourceprovider]",
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
class ChadoBiomaterialFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('biomaterial_name')->getString(),
        'description' => $item->get('biomaterial_description')->getString(),
        'biosourceprovider' => $item->get('biomaterial_biosourceprovider')->getString(),
        'database_name' => $item->get('biomaterial_database_name')->getString(),
        'database_accession' => $item->get('biomaterial_database_accession')->getString(),
        'genus' => $item->get('biomaterial_genus')->getString(),
        'species' => $item->get('biomaterial_species')->getString(),
        'infratype' => $item->get('biomaterial_infraspecific_type')->getString(),
        'infraname' => $item->get('biomaterial_infraspecific_name')->getString(),
        'abbreviation' => $item->get('biomaterial_abbreviation')->getString(),
        'common_name' => $item->get('biomaterial_common_name')->getString(),
      ];

      // Special case handling for abbreviation of infraspecific type
      $values['infratype_abbrev'] = chado_abbreviate_infraspecific_rank($values['infratype']);

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
