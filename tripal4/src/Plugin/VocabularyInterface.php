<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Plugin\IdSpaceInterface;
use Drupal\tripal4\Term

/**
 * Defines an interface for tripal vocabulary plugins.
 */
interface VocabularyInterface extends IdSpaceInterface {

  /**
   * Returns a list of valid terms based off matches from the given partial term
   * name. A given max number of terms are returned.
   *
   * @param string $partial
   *   The partial term name.
   *
   * @param int $max
   *   The given max number returned.
   *
   * @return array
   *   An array of valid \Drupal\tripal4\Vocabulary\Term objects.
   */
  public function suggestTerms(string $partial, int $max = 10);

  /**
   * Returns list of id space plugin's machine names that is contained in this vocabulary.
   * 
   * @return array
   *   An array of id space plugin machine name strings.
   */
  public function getIdSpaceNames();

}
