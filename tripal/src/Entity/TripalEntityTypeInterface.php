<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Tripal Content type entities.
 */
interface TripalEntityTypeInterface extends ConfigEntityInterface {

  /**
   * Gets the index of the machine name (e.g. 1).
   *
   * @return string
   *   Index of the machine name of the Tripal Entity Type.
   */
  public function getID();

  /**
   * Sets the index of the machine name.
   *
   * @param integer $id
   *   The index of the machine name of the Tripal Entity Type.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setID($id);

  /**
   * Gets the machine name of the Tripal Entity Type (e.g. bio_data_1).
   *
   * @return string
   *   Machine name of the Tripal Entity Type.
   */
  public function getName();

  /**
   * Sets the machine name of the Tripal Entity Type.
   *
   * @param string $name
   *   The machine name of the Tripal Entity Type.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setName($name);

  /**
   * Gets the Tripal Entity Type label (e.g. gene).
   *
   * @return string
   *   Label of the Tripal Entity Type.
   */
  public function getLabel();

  /**
   * Sets the Tripal Entity Type label (e.g. gene).
   *
   * @param string $label
   *   The Tripal Entity Type label.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setLabel($label);

  /**
   * Gets the Tripal Entity Type CVTerm.
   *
   * @return object
   *   The Tripal Controlled Vocabulary Term describing this Tripal Entity Type.
   */
  public function getTerm();

  /**
   * Sets the Tripal Entity Type CV Term.
   *
   * @param object $term
   *   The Tripal Controlled Vocabulary Term
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setTerm($term);

  /**
   * Gets help text for admin for this Tripal Entity Type.
   *
   * @return string
   *   Help text for the Tripal Entity Type.
   */
  public function getHelpText();

  /**
   * Sets the Tripal Entity Type help text.
   *
   * @param string $help_text
   *   The Tripal Entity Type help text.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setHelpText($help_text);
}
