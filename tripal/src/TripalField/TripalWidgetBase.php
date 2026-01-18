<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Defines the Tripal field widget base class.
 */
abstract class TripalWidgetBase extends WidgetBase {

  /**
   * Sanitizes a property key.
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
   *   A sanitizied string.
   */
  public function sanitizeKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
  }

  /**
   * Override Drupal's function to change the behavior of finite cardinality.
   *
   * In the case of a fixed finite cardinality that is not small, e.g. 100,
   * we don't want to display 100 fields for every delta if they are not
   * populated.
   * See Drupal issue https://www.drupal.org/project/drupal/issues/1156338
   * We will tell Drupal that it is unlimited cardinality up until the point
   * where we actually hit the cardinality limit. Before that point, we can use
   * the "Add another item" button in the same way as unlimited cardinality
   * fields.
   *
   * @inheritdoc
   */
  protected function formMultipleElements(FieldItemListInterface $items, array &$form, FormStateInterface $form_state) {
    $cardinality = $this->fieldDefinition
      ->getFieldStorageDefinition()
      ->getCardinality();

    // Only override the Drupal behavior when we have a finite cardinality
    // that is greater than one.
    if ($cardinality > 1) {

      // If not every item in the finite cardinality is occupied,
      // temporarily set to unlimited before calling the parent class.
      $temp_cardinality = $cardinality;
      if (count($items) < $cardinality) {
        $temp_cardinality = -1;
      }
      $this->fieldDefinition
        ->getFieldStorageDefinition()
        ->setCardinality($temp_cardinality);
    }

    // Have the parent class generate elements with the adjusted cardinality.
    $elements = parent::formMultipleElements($items, $form, $form_state);

    // In the case where we have exactly as many widgets as the cardinality
    // specifies, we will remove the "Add another item" button so that the
    // user can't add another row and exceed the specified cardinality.
    // This will only happen after the ajax triggers from a previous row
    // addition.
    if ($cardinality > 1) {
      $max_delta = $elements['#max_delta'];
      if (($max_delta + 1) >= $cardinality) {
        if (array_key_exists('add_more', $elements)) {
          unset($elements['add_more']);
        }
      }
    }

    return $elements;
  }

}
