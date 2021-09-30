<?php

namespace Drupal\tripal4\Plugin;

use Drupal\tripal4\Plugin\CollectionPluginInterface;

/**
 * Defines an interface for tripal id space plugins.
 */
interface IdSpaceInterface extends CollectionPluginInterface {

  /**
   * Gets the parent of the given term. The given term must be a valid term for
   * this id space. If the given term is a root of this id space then NULL
   * is returned.
   *
   * @param \Drupal\tripal4\Term $child
   *   The given term.
   *
   * @return \Drupal\tripal4\Term|NULL
   *   The parent term or NULL.
   */
  public function getParent($child);

  /**
   * Gets the children terms of the given term. The given term must be a valid
   * term for this id space or NULL. If the given term is NULL then the root
   * children of this id space is returned.
   *
   * @param \Drupal\tripal4\Vocabulary\Term|NULL $parent
   *   The given term or NULL.
   *
   * @return array
   *   An array of \Drupal\tripal4\Term children objects.
   */
  public function getChildren($parent = NULL);

  /**
   * Sets the default vocabulary of this id space to the given vocabulary name.
   *
   * @param string name
   *   The vocabulary name.
   */
  public function setDefaultVocabulary($name);

  /**
   * Returns this id space's default vocabulary name or NULL if no default has
   * been set.
   *
   * @return string
   *   The vocabulary name.
   */
  public function getDefaultVocabulary();

  /**
   * Adds a new term to this id space with the given name, accesion, and
   * optional parent term. If no parent term is given then this new term is
   * added as a root term of this id space. The given accession must be unique
   * among all terms of this id space.
   *
   * @param string $name
   *   The name.
   *
   * @param string $accession
   *   The accession.
   *
   * @param \Drupal\tripal4\Term|NULL
   *   The parent term or NULL.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function addTerm($name,$accession,$parent = NULL);

  /**
   * Removes the term with the given accession from this id space. All children
   * terms are also removed.
   * !!!WARNING!!!
   * If the removed term in this id space is referenced by entities this could
   * break data integrity. This method must be used with extreme caution!
   *
   * @param string $accession
   *   The accession.
   *
   * @return bool
   *   True if the term was removed or false otherwise.
   */
  public function removeTerm($accession);

}
