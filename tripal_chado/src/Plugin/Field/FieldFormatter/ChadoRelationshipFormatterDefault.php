<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Utility\UrlHelper;

/**
 * Plugin implementation of default Tripal Relationship formatter.
 *
 * @FieldFormatter(
 *   id = "chado_relationship_formatter_default",
 *   label = @Translation("Chado Relationship Formatter"),
 *   description = @Translation("A chado relationship formatter"),
 *   field_types = {
 *     "chado_relationship_type_default"
 *   },
 *   valid_tokens = {
 *     "[accession]",
 *     "[version]",
 *     "[description]",
 *     "[db_name]",
 *     "[db_description]",
 *     "[db_urlprefix]",
 *     "[db_url]",
 *   },
 * )
 */
class ChadoRelationshipFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[subject_name] [type_name] [object_name]';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');
$token_string = '[subject_name]([subject_id]) [type_name]([type_id]) [object_name]([object_id])'; //@@@ FOR DEBUGGING
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'record_id' => $item->get('record_id')->getString(),
        'subject_id' => $item->get('subject_id')->getString(),
        'subject_name' => $item->get('subject_name')->getString(),
        'subject_entity_id' => $item->get('subject_entity_id')->getString(),
        'object_id' => $item->get('object_id')->getString(),
        'object_name' => $item->get('object_name')->getString(),
        'object_entity_id' => $item->get('object_entity_id')->getString(),
        'type_id' => $item->get('type_id')->getString(),
        'type_name' => $item->get('type_name')->getString(),
      ];
      $direction = 1;
      if ($values['subject_id'] == $values['record_id']) {
        $direction = -1;
      }

      // As we did in Tripal 3, the term is processed up a bit to make for nicer display
      $this->formatTypeName($values);

      // Create a clickable link to the corresponding related entity when one exists.
      // We need to pre-render so that we can replace into the token string.
      if ($direction == 1) {
        $item = $lookup_manager->getRenderableItem($values['subject_name'], $values['subject_entity_id']);
        $values['subject_name'] = \Drupal::service('renderer')->render($item);
      }
      else {
        $item = $lookup_manager->getRenderableItem($values['object_name'], $values['object_entity_id']);
        $values['object_name'] = \Drupal::service('renderer')->render($item);
      }

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

  /**
   * Format the controlled vocabulary term for nicer display.
   * Tripal 3: tripal_chado/includes/TripalFields/sbo__relationship/sbo__relationship_formatter.inc
   *
   * @param array &$values
   *   The associative array of field values.
   *   The value with the key 'term_name' is updated.
   */
  protected function formatTypeName(&$values): void {
    $values['type_name'] = preg_replace('/_/', ' ', $values['type_name']);
  }

}
