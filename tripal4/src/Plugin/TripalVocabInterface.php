<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for Tripal Vocabulary plugins.
 */
interface TripalVocabInterface extends PluginInspectionInterface {

  /**
   * Tests if the given term name and accession exists in this vocabulary.
   *
   * @param string name
   *   The term's name.
   *
   * @param string accession
   *   The term's accession.
   *
   * @return bool
   *   True if the given term exists or false otherwise.
   */
  public function hasTerm(string $name, string $accession);

  /**
   * Returns a list of valid terms based off matches from the given partial
   * term name. A given max number of terms are returned.
   * 
   * @param string partial
   *   The partial term name.
   *
   * @return array
   *   An array of valid term items. Each item of the array is an array with
   *   two strings. The first string is the term's name and the second is its
   *   accession.
   */
  public function suggestTerms(string $partial);

}
