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
   * The field's cardinality.
   *
   * @var int
   */
  public int $cardinality;

  /**
   * Santizies a property key.
   *
   * Property keys are often controlled vocabulary IDs, which is the IdSpace
   * and accession separated by a colon. The colon is not supported by the
   * storage backend and must be converted to an underscore. This
   * function performs that task.
   *
   * @param string $key
   *   A property key e.g. "operation:2945".
   *
   * @return string
   *   A santizied string.
   */
  public function sanitizeKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
  }

  /**
   * Retrieve the max delta value specific to this field.
   *
   * @param int $cardinality
   *   The field's cardinality.
   *
   * @return int
   *   The max delta value specific to this field.
   */
  protected function getFieldMaxDelta(int $cardinality): int {
    // Retrieve the global setting.
    $max_delta = \Drupal::config('tripal.settings')->get('tripal_entity_type.publish_global_max_delta');

    // max_delta defaults to 100 if not defined.
    if (is_null($max_delta) or (trim($max_delta) === '')) {
      $max_delta = 100;
    }

    // Finite field cardinality will override the global max_delta setting.
    if ($cardinality > 1) {
      $max_delta = $cardinality;
    }

    return $max_delta;
  }

  /**
   * {@inheritdoc}
   *
   * Initializes value of max_delta specific to this field.
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $this->cardinality = $this->fieldDefinition->getFieldStorageDefinition()->getCardinality();
    $this->max_delta = $this->getFieldMaxDelta($this->cardinality);
  }

  /**
   * HELPER: Makes multiple renderable items into a list.
   *
   * Note: If the number of items exceeds the max delta for this field,
   * then a warning will be added right before the list.
   *
   * @param array $list
   *   A list of renderable items.
   *
   * @return array
   *   A render array where (1) if no items then an empty element is
   *   returned, (2) if there is one item it is passed through unchanged,
   *   (3) if `$items <= $max_delta` then a list of items is returned,
   *   (4) if `$items > $max_delta` then two elements are returned where
   *   the first is a warning string and the second is a list of items.
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
    // For unlimited cardinality fields, an extra record is published as a
    // flag to indicate that there are more records that were not published.
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
    // For finite cardinality that is greater than one, an extra record
    // may have been published, however Drupal will not pass it through,
    // so we will not be able to know if it exists or not. Here we
    // display a different message when the cardinality limit is reached.
    elseif ($this->cardinality > 1 && count($list) == $this->max_delta) {
      if (!$markup) {
        $markup = [
          '#markup' => '<em>'
          . $this->t('Notice: There may be more items than the @max_delta that are displayed here',
                        ['@max_delta' => $this->max_delta])
          . '</em>',
        ];
      }
      $elements[] = $markup;
    }
  }

}
