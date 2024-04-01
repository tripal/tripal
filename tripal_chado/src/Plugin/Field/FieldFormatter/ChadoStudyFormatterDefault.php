<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal study formatter.
 *
 * @FieldFormatter(
 *   id = "chado_study_formatter_default",
 *   label = @Translation("Chado study formatter"),
 *   description = @Translation("A chado study formatter"),
 *   field_types = {
 *     "chado_study_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[contact_name]",
 *     "[pub_title]",
 *     "[database_name]",
 *     "[database_accession]",
 *   },
 * )
 */
class ChadoStudyFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('study_name')->getString(),
        'description' => $item->get('study_description')->getString(),
        'contact_name' => $item->get('study_contact_name')->getString(),
        'pub_title' => $item->get('study_pub_title')->getString(),
        'database_name' => $item->get('study_database_name')->getString(),
        'database_accession' => $item->get('study_database_accession')->getString(),
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // When possible, create a clickable link to the corresponding entity.
      $item_settings = $item->getDataDefinition()->getSettings();
      $id = $item_settings['storage_plugin_settings']['linker_fkey_column'] ?? 'study_id';
      $entity_id = $lookup_manager->getEntityId(
        $item->get($id)->getString(),
        $item_settings['termIdSpace'],
        $item_settings['termAccession']
      );
      $renderable_item = $lookup_manager->getRenderableItem(
        $displayed_string,
        $entity_id
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
