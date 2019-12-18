<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;

/**
 * Defines the Tripal Content type entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_entity_type",
 *   label = @Translation("Tripal Content Type"),
 *   label_collection = @Translation("Tripal Content Types"),
 *   label_singular = @Translation("Tripal content type"),
 *   label_plural = @Translation("Tripal content types"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Tripal content type",
 *     plural = "@count Tripal content types",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\TripalEntityTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalEntityTypeForm",
 *       "edit" = "Drupal\tripal\Form\TripalEntityTypeForm",
 *       "delete" = "Drupal\tripal\Form\TripalEntityTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\TripalEntityTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "bio_data",
 *   admin_permission = "administer tripal content types",
 *   bundle_of = "tripal_entity",
 *   entity_keys = {
 *     "id" = "name",
 *     "label" = "label",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/bio_data/{tripal_entity_type}",
 *     "add-form" = "/admin/structure/bio_data/add",
 *     "edit-form" = "/admin/structure/bio_data/manage/{tripal_entity_type}",
 *     "delete-form" = "/admin/structure/bio_data/manage/{tripal_entity_type}/delete",
 *     "collection" = "/admin/structure/bio_data"
 *   },
 *   config_export = {
 *     "id",
 *     "name",
 *     "label",
 *     "term_id",
 *     "help_text"
 *   }
 * )
 */
class TripalEntityType extends ConfigEntityBundleBase implements TripalEntityTypeInterface {

  /**
   * The Tripal Content type ID.
   *
   * @var integer
   */
  protected $id;

  /**
   * The Tripal Content machine name.
   *
   * @var string
   */
  protected $name;

  /**
   * The Tripal Content type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Tripal Term which describes this content type.
   *
   * @var integer
   */
  protected $term_id;

  /**
   * Help text to describe to the administrator what this content type is.
   *
   * @var string
   */
  protected $help_text;

  /**
   * {@inheritdoc}
   */
  public function id() {
    return $this->name;
  }

  /**
   * Gets the index of the machine name (e.g. 1).
   *
   * @return string
   *   Index of the machine name of the Tripal Entity Type.
   */
  public function getID() {
    return $this->id;
  }

  /**
   * Sets the index of the machine name.
   *
   * @param integer $id
   *   The index of the machine name of the Tripal Entity Type.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setID($id) {
    $this->id = $id;
  }

  /**
   * Gets the machine name of the Tripal Entity Type (e.g. bio_data_1).
   *
   * @return string
   *   Machine name of the Tripal Entity Type.
   */
  public function getName() {
    return $this->name;
  }

  /**
   * Sets the machine name of the Tripal Entity Type.
   *
   * @param string $name
   *   The machine name of the Tripal Entity Type.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setName($name) {
    $this->name = $name;
  }

  /**
   * Gets the Tripal Entity Type label (e.g. gene).
   *
   * @return string
   *   Label of the Tripal Entity Type.
   */
  public function getLabel() {
    return $label;
  }

  /**
   * Sets the Tripal Entity Type label (e.g. gene).
   *
   * @param string $label
   *   The Tripal Entity Type label.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setLabel($label) {
    $this->label = $label;
  }

  /**
   * Gets the Tripal Entity Type CVTerm.
   *
   * @return object
   *   The Tripal Controlled Vocabulary Term describing this Tripal Entity Type.
   */
  public function getTerm() {
    $term = \Drupal\tripal\Entity\TripalTerm::load($this->term_id);
    return $term;
  }

  /**
   * Sets the Tripal Entity Type CV Term.
   *
   * @param object $term
   *   The Tripal Controlled Vocabulary Term
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setTerm($term_id) {
    $this->term_id = $term_id;
  }

  /**
   * Gets help text for admin for this Tripal Entity Type.
   *
   * @return string
   *   Help text for the Tripal Entity Type.
   */
  public function getHelpText() {
    return $this->help_text;
  }

  /**
   * Sets the Tripal Entity Type help text.
   *
   * @param string $help_text
   *   The Tripal Entity Type help text.
   *
   * @return \Drupal\tripal\Entity\TripalEntityTypeInterface
   *   The called Tripal Entity Type entity.
   */
  public function setHelpText($help_text) {
    $this->help_text = $help_text;
  }

}
