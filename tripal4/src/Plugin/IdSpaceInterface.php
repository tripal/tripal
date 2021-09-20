<?php

namespace Drupal\tripal4\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\tripal4\Term

/**
 * Defines an interface for tripal id space plugins.
 */
interface IdSpaceInterface extends PluginInspectionInterface {

  /**
   * Tests if the given term exists in this id space.
   *
   * @param \Drupal\tripal4\Vocabulary\Term term
   *   The given term.
   *
   * @return bool
   *   True if the given term exists or false otherwise.
   */
  public function hasTerm(Term $term);

  /**
   * Gets the parent of the given term. The given term must be a valid term for
   * this id space. If the given term is a root of this id space then NULL
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
   * term for this id space or NULL. If the given term is NULL then the root
   * children of this id space is returned.
   *
   * @param mixed parent
   *   The given term or NULL.
   *
   * @return array
   *   An array of \Drupal\tripal4\Vocabulary\Term children objects.
   */
  public function getChildren($parent = NULL);

}
