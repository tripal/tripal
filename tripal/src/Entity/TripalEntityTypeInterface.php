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
   */
  public function setID($id);

  /**
   * Gets the machine name synonyms of the Tripal Entity Type (e.g. bio_data_1).
   *
   * @return string
   *   Machine name synonyms of the Tripal Entity Type.
   */
  public function getSynonyms();


  /**
   * Sets the machine name synonyms of the Tripal Entity Type.
   *
   * @param string $name
   *   The machine name of the Tripal Entity Type.
   *
   */
  public function setSynonyms($name);

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
   */
  public function setLabel($label);

  /**
   * Gets the Tripal Entity Type CV Term ID Space.
   *
   * @return string
   *   The Tripal Controlled Vocabulary Term ID Space.
   */
  public function getTermIdSpace();

  /**
   * Sets the Tripal Entity Type CV Term ID Space.
   *
   * @param string $termIdSpace
   *   The new Tripal Controlled Vocabulary Term ID Space
   *
   */
  public function setTermIdSpace($termIdSpace);

  /**
   * Gets the Tripal Entity Type CV Term Accession.
   *
   * @return string
   *   The Tripal Controlled Vocabulary Term Accession.
   */
  public function getTermAccession();

  /**
   * Sets the Tripal Entity Type CV Term Accession.
   *
   * @param string $termIdSpace
   *   The new Tripal Controlled Vocabulary Term Accession
   *
   */
  public function setTermAccession($termAccession);

  /**
   * Gets the Tripal Entity Type CV Term Object based off its CV Term ID Space and Accession.
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalTerm
   *   The Tripal Controlled Vocabulary Term Object.
   */
  public function getTerm();

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
   */
  public function setHelpText($help_text);

  /**
   * Gets the category for this Tripal Entity Type.
   *
   * @return string
   *   Category for the Tripal Entity Type.
   */
  public function getCategory();

  /**
   * Sets the Tripal Entity Type category.
   *
   * @param string $category
   *   The Tripal Entity Type category.
   *
   */
  public function setCategory($category);

  /**
   * Gets the title format for this Tripal Entity Type.
   *
   * @return string
   *   Title format for the Tripal Entity Type.
   */
  public function getTitleFormat();

  /**
   * Sets the Tripal Entity Type title format.
   *
   * @param string $title_format
   *   The Tripal Entity Type title format.
   *
   */
  public function setTitleFormat($title_format);

  /**
   * Gets the URL format for this Tripal Entity Type.
   *
   * @return string
   *   URL format for the Tripal Entity Type.
   */
  public function getURLFormat();

  /**
   * Sets the Tripal Entity Type URL format.
   *
   * @param string $url_format
   *   The Tripal Entity Type URL format.
   *
   */
  public function setURLFormat($url_format);

  /**
   * Configures the entity such that empty fields will be hidden.
   */
  public function hideEmptyFields();

  /**
   * Configures the entity such that empty fields will be shown.
   */
  public function showEmptyFields();

  /**
   * Retrieves the indicator for whether to hide empty fields or not.
   *
   * @return
   *   true if empty fields should be hidden and false otherwise.
   */
  public function getEmptyFieldDisplay();

  /**
   * Configures the entity such that fields will be loaded via AJAX after page load.
   */
  public function enableAJAXLoading();

  /**
   * Configures the entity such that fields will be loaded on page load.
   */
  public function disableAJAXLoading();

  /**
   * Retrieves the indicator for whether to load fields using AJAX or not.
   *
   * @return
   *   true if AJAX should be used to load fields and false otherwise.
   */
  public function getAJAXLoadingStatus();
}
