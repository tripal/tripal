<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Provides a UI for YML-based TripalEntityType creation.
 * Each instance of this entity is a single configuration for tripal content
 * types in your site.
 *
 * @ConfigEntityType(
 *   id = "tripalentitytype_collection",
 *   label = @Translation("Tripal Content Type Collection"),
 *   label_collection = @Translation("Tripal Content Type Collections"),
 *   label_singular = @Translation("Tripal Content Type Collection"),
 *   label_plural = @Translation("Tripal Content Type Collections"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Tripal Content Type collection",
 *     plural = "@count Tripal Content Type collections",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalEntityTypeCollectionListBuilder",
 *     "form" = {
 *       "delete" = "Drupal\tripal\Form\TripalEntityTypeCollectionDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripalentitytype_collection",
 *   admin_permission = "administer tripal",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "description",
 *     "content_types"
 *   },
 *   links = {
 *     "delete-form" = "/admin/tripal/config/tripalentitytype-collection/{tripalentitytype_collection}/delete",
 *     "collection" = "/admin/tripal/config/tripalentitytype-collection"
 *   }
 * )
 */
class TripalEntityTypeCollection extends ConfigEntityBase implements TripalEntityTypeCollectionInterface {

  /**
   * The TripalEntityTypeCollectionConfig ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The TripalEntityTypeCollectionConfig label.
   *
   * @var string
   */
  protected $label;

  /**
   * The TripalEntityTypeCollectionConfig description.
   *
   * @var string
   */
  protected $description;

  /**
   *
   * @var array
   */
  protected $content_types;

  /**
   * Retrieves the current description for the content type configuration.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }
}
