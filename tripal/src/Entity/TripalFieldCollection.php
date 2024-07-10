<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Provides a UI for YML-based TripalField creation.
 * Each instance of this entity is a single configuration for Tripal Field Collection
 * in your site.
 *
 * @ConfigEntityType(
 *   id = "tripalfield_collection",
 *   label = @Translation("Tripal Field Collection"),
 *   label_collection = @Translation("Tripal Field Collection"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalFieldCollectionListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalFieldCollectionForm",
 *       "edit" = "Drupal\tripal\Form\TripalFieldCollectionForm",
 *       "delete" = "Drupal\tripal\Form\TripalFieldCollectionDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripalfield_collection",
 *   admin_permission = "administer tripal",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "description",
 *     "fields"
 *   },
 *   links = {
 *     "collection" = "/admin/tripal/config/tripalfield-collection",
 *     "delete-form" = "/admin/tripal/config/tripalfield-collection/{tripalfield_collection}/delete"
 *   }
 * )
 */
class TripalFieldCollection extends ConfigEntityBase implements TripalFieldCollectionInterface {

  /**
   * The Config ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Config label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Config description.
   *
   * @var string
   */
  protected $description;

  /**
   *
   * @var array
   */
  protected $fields;

  /**
   * Retrieves the current description.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

}
