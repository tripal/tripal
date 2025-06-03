<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal analysis formatter.
 *
 * @FieldFormatter(
 *   id = "chado_analysis_formatter_default",
 *   label = @Translation("Chado analysis formatter"),
 *   description = @Translation("A chado analysis formatter"),
 *   field_types = {
 *     "chado_analysis_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[program]",
 *     "[programversion]",
 *     "[algorithm]",
 *     "[sourcename]",
 *     "[sourceversion]",
 *     "[sourceuri]",
 *   },
 * )
 */
class ChadoAnalysisFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('analysis_name')->getString(),
        'description' => $item->get('analysis_description')->getString(),
        'program' => $item->get('analysis_program')->getString(),
        'programversion' => $item->get('analysis_programversion')->getString(),
        'algorithm' => $item->get('analysis_algorithm')->getString(),
        'sourcename' => $item->get('analysis_sourcename')->getString(),
        'sourceversion' => $item->get('analysis_sourceversion')->getString(),
        'sourceuri' => $item->get('analysis_sourceuri')->getString(),
        // timeexecuted not implemented
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

    // If multiple items, convert to a list.
    $elements = $this->createListMarkup($list);
    return $elements;
  }

}
