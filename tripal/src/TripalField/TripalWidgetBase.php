<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\WidgetBase;


/**
 * Defines the Tripal field widget base class.
 */
abstract class TripalWidgetBase extends WidgetBase {


  /**
   * Santizies a property key.
   *
   * Property keys are often controlled vocabulary IDs, which is the IdSpace
   * and accession separated by a colon. The colon is not supported by the
   * storage backend and must be converted to an underscore. This
   * function performs that task
   *
   * @param string $key
   *
   * @return string
   *   A sanitizied string.
   */
  public function sanitizeKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
  }
}
