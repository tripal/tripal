<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Controlled Vocabulary Term entities.
 *
 * @ingroup tripal
 */
interface TripalTermInterface extends ContentEntityInterface, EntityChangedInterface {

  /**
   * Retrieves the internal unique identifier for this term.
   *
   * @return int
   *   The internal unique identifier.
   */
  public function getID();

  /**
   * Retrieves the human-readable label for this term.
   *
   * @return string
   *   The human-readable label for the term.
   */
  public function getLabel();

  /**
   * Retrieves the unique identifier for the linked Tripal IDSpace.
   *
   * @return int
   *   The unique identifier for the linked TripalVocabSpace.
   */
  public function getIDSpaceID();

  /**
   * Link an existing Tripal IDSpace to this term.
   *
   * @param int $idspace_id
   *   The internal unique identifier for the TripalVocabSpace to link.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setIDSpaceID($idspace_id);

  /**
   * The linked Tripal IDSpace.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The linked Tripal IDSpace.
   */
  public function getIDSpace();

  /**
   * Retrieves the unique identifier for the linked Tripal Vocabulary.
   *
   * @return int
   *   The unique identifier for the linked Tripal Vocabulary.
   */
  public function getVocabID();

  /**
   * Link an existing Tripal Vocabulary to this term.
   *
   * @param int $vocab_id
   *   The internal unique identifier for the TripalVocab to link.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setVocabID($vocab_id);

  /**
   * The linked Tripal Vocabulary.
   *
   * @return \Drupal\tripal\Entity\TripalVocabInterface
   *   The linked Tripal Vocabulary.
   */
  public function getVocab();

  /**
   * Retrieves the unique ontology-assigned accession for this term.
   *
   * NOTE: The accession does not include the IDSpace. For example, the gene
   * term from the sequence ontology has an accession of "0000704" and an
   * IDSpace of "SO". This is usually expressed as SO:0000704.
   *
   * @return string
   *   The accession for this term not including the IDSpace.
   */
  public function getAccession();

  /**
   * Sets the unique ontology-assigned accession for this term.
   *
   * NOTE: The accession does not include the IDSpace. For example, the gene
   * term from the sequence ontology has an accession of "0000704" and an
   * IDSpace of "SO". This is usually expressed as SO:0000704.
   *
   * @param string $accession
   *   The accession for this term not including the IDSpace.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setAccession($accession);

  /**
   * Gets the Controlled Vocabulary Term name.
   *
   * @return string
   *   Name of the Controlled Vocabulary Term.
   */
  public function getName();

  /**
   * Sets the Controlled Vocabulary Term name.
   *
   * @param string $name
   *   The Controlled Vocabulary Term name.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setName($name);

  /**
   * Retrieves the definition of the Tripal Term.
   *
   * @return string
   *   The definition of the current Tripal Term.
   */
  public function getDefinition();

  /**
   * Sets the Controlled Vocabulary Term definition.
   *
   * @param string $definition
   *   The Controlled Vocabulary Term definition.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setDefinition($definition);

  /**
   * Gets the Controlled Vocabulary Term creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Controlled Vocabulary Term.
   */
  public function getCreatedTime();

  /**
   * Sets the Controlled Vocabulary Term creation timestamp.
   *
   * @param int $timestamp
   *   The Controlled Vocabulary Term creation timestamp.
   *
   * @return \Drupal\tripal\Entity\TripalTermInterface
   *   The current term is returned to allow chaining of commands.
   */
  public function setCreatedTime($timestamp);

  /**
   * Retrieves the full set of details for the Tripal Term.
   *
   * @return array
   *   An array of properties where the key is the property name and the value
   *   is the string value. This does include a nested array for the vocabulary.
   */
  public function getDetails();
}
