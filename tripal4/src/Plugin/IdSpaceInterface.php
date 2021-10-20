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
   * Returns the term in this id space with the given accession. If no such term exists then NULL is returned.
   *
   * @param string $accession
   *   The accession.
   *
   * @return \Drupal\tripal4\Term|NULL
   *   The term or NULL.
   */
  public function getTerm($accession);

  /**
   * Returns the terms in this id space whose names match the given name.
   * Matches can only be exact or a substring depending on the given flag. The
   * default is to only return exact matches.
   *
   * @param string $name
   *   The name.
   *
   * @param bool $exact
   *   True to only include exact matches else include all substring matches.
   *
   * @return array
   *   Array of matching \Drupal\tripal4\Term instances.
   */
  public function getTerms($name,$exact = True);

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
   * Saves the given term to this id space with the given parent term. If the
   * given term does not exist in this id space then it is added as a new term,
   * else it is updated. If this is an update to an existing term then the
   * parent argument is ignored. If no parent term is given and this is a new
   * term then it is added as a root term of this id space.
   *
   * @param \Drupal\tripal4\Term $term
   *   The term.
   *
   * @param \Drupal\tripal4\Term|NULL $parent
   *   The parent term or NULL.
   *
   * @return bool
   *   True on success or false otherwise.
   */
  public function saveTerm($term,$parent = NULL);

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

  /**
   * Returns the URL prefix of this id space.
   *
   * @return string
   *   The URL prefix.
   */
  public function getURLPrefix();

  /**
   * Sets the URL prefix of this id space to the given URL prefix.
   *
   * @param string $prefix
   *   The URL prefix.
   */
  public function setURLPrefix($prefix);

}
