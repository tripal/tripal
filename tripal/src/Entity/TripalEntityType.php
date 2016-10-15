<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;

/**
 * Defines the Tripal Content type entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_entity_type",
 *   label = @Translation("Tripal Content type"),
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
 *   config_prefix = "tripal_entity_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "tripal_entity",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_entity_type/{tripal_entity_type}",
 *     "add-form" = "/admin/structure/tripal_entity_type/add",
 *     "edit-form" = "/admin/structure/tripal_entity_type/{tripal_entity_type}/edit",
 *     "delete-form" = "/admin/structure/tripal_entity_type/{tripal_entity_type}/delete",
 *     "collection" = "/admin/structure/tripal_entity_type"
 *   }
 * )
 */
class TripalEntityType extends ConfigEntityBundleBase implements TripalEntityTypeInterface {

  /**
   * The Tripal Content type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Tripal Content type label.
   *
   * @var string
   */
  protected $label;

}
