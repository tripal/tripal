<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\tripal4\Vocabulary\Term

/**
 * Defines an interface for tripal vocabulary plugins.
 */
interface TripalVocabInterface extends PluginInspectionInterface {

  /**
   * Tests if the given term exists in this vocabulary.
   *
   * @param \Drupal\tripal4\Vocabulary\Term term
   *   The given term.
   *
   * @return bool
   *   True if the given term exists or false otherwise.
   */
  public function hasTerm(Term $term);

  /**
   * Returns a list of valid terms based off matches from the given partial
   * term name. A given max number of terms are returned.
   * 
   * @param string partial
   *   The partial term name.
   *
   * @return array
   *   An array of valid \Drupal\tripal4\Vocabulary\Term objects.
   */
  public function suggestTerms(string $partial);

}
