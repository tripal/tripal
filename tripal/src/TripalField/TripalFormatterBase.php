<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Defines the Tripal field formatter base class.
 */
abstract class TripalFormatterBase extends FormatterBase {

  /**
   * Maximum number of records that will be published, 0 means no limit.
   *
   * @var int
   */
  public int $max_delta = 100;

  /**
   * Santizies a property key.
   *
   * Property keys are often controlled vocabulary IDs, which is the IdSpace
   * and accession separated by a colon. The colon is not supported by the
   * storage backend and must be converted to an underscore. This
   * function performs that task.
   *
   * @param string $key
   *   A property key e.g. "operation:2945"
   *
   * @return string
   *   A santizied string.
   */
  public function sanitizeKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
  }

  /**
   * {@inheritdoc}
   *
   * Initializes value of max_delta.
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $max_delta = \Drupal::config('tripal.settings')->get('tripal_entity_type.publish_global_max_delta');
    // max_delta defaults to 100 if not defined.
    if (is_null($max_delta) or (trim($max_delta) === '')) {
      $max_delta = 100;
    }
    $this->max_delta = $max_delta;
  }

  /**
   * Converts a list of multiple renderable items into a list.
   *
   * A single item is passed through unchanged.
   *
   * @param array $list
   *   A list of renderable items.
   *
   * @return array
   *   A render array. Will be empty if there were no items.
   */
  public function createListMarkup(array $list): array {
    $elements = [];

    // If only one element has been found, don't make into a list.
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
    elseif (count($list) > 1) {
      $this->addMaxDeltaWarning($elements, $list);
      $elements[] = [
        '#theme' => 'item_list',
        '#list_type' => 'ul',
        '#items' => $list,
        '#wrapper_attributes' => ['class' => 'container'],
      ];
    }
    return $elements;
  }

  /**
   * Provides a visible warning message when max_delta has been exceeded.
   *
   * @param array &$elements
   *   The render array elements.
   * @param array &$list
   *   The viewElements to be formatted. The list size may exceed max_delta
   *   by one, this is the indicator to display the message, and this last
   *   element will be removed.
   * @param array|null $markup
   *   Custom render array markup to override the default message.
   *
   * @return void
   *   &$elements is modified when appropriate
   */
  public function addMaxDeltaWarning(array &$elements, array &$list, ?array $markup = NULL): void {
    if (count($list) > $this->max_delta) {
      $list = array_slice($list, 0, $this->max_delta, TRUE);
      if (!$markup) {
        $markup = [
          '#markup' => '<em>'
          . $this->t('Notice: Only the first @max_delta items are displayed here',
                        ['@max_delta' => $this->max_delta])
          . '</em>',
        ];
      }
      $elements[] = $markup;
    }
  }

}
