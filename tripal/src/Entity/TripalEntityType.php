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
 *     "name",
 *     "label",
 *   }
 * )
 */
class TripalEntityType extends ConfigEntityBundleBase implements TripalEntityTypeInterface {

  /**
   * The Tripal Content type ID.
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
   * {@inheritdoc}
   */
  public function id() {
    return $this->name;
  }

}
