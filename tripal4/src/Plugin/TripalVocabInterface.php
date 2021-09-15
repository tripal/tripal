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
   * Returns a list of valid terms based off matches from the given partial term
   * name. A given max number of terms are returned.
   *
   * @param string partial
   *   The partial term name.
   *
   * @param int max
   *   The given max number returned.
   *
   * @return array
   *   An array of valid \Drupal\tripal4\Vocabulary\Term objects.
   */
  public function suggestTerms(string $partial, int $max = 10);

  /**
   * Gets the parent of the given term. The given term must be a valid term for
   * this vocabulary. If the given term is a root of this vocabulary then NULL
   * is returned.
   *
   * @param \Drupal\tripal4\Vocabulary\Term child
   *   The given term.
   *
   * @return mixed
   *   The parent term or NULL.
   */
  public function getParent($child);

  /**
   * Gets the children terms of the given term. The given term must be a valid
   * term for this vocabulary or NULL. If the given term is NULL then the root
   * children of this vocabulary is returned.
   *
   * @param mixed parent
   *   The given term or NULL.
   *
   * @return array
   *   An array of \Drupal\tripal4\Vocabulary\Term children objects.
   */
  public function getChildren($parent = NULL);

}
