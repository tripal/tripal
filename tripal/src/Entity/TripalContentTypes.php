<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Provides a UI for YML-based TripalEntityType creation.
 * Each instance of this entity is a single configuration for tripal content
 * types in your site.
 *
 * @ConfigEntityType(
 *   id = "tripal_content_types",
 *   label = @Translation("Tripal Content Types"),
 *   label_collection = @Translation("Tripal Content Types"),
 *   label_singular = @Translation("Tripal Content Type"),
 *   label_plural = @Translation("Tripal Content Types"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Tripal content type",
 *     plural = "@count Tripal content types",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalContentTypesListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalContentTypesForm",
 *       "edit" = "Drupal\tripal\Form\TripalContentTypesForm",
 *       "delete" = "Drupal\tripal\Form\TripalContentTypesDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripal_content_types",
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
 *     "delete-form" = "/admin/tripal/config/content_types/{config}/delete",
 *   }
 * )
 */
class TripalContentTypes extends ConfigEntityBase implements TripalContentTypesInterface {

  /**
   * The TripalContentTypesConfig ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The TripalContentTypesConfig label.
   *
   * @var string
   */
  protected $label;

  /**
   * The TripalContentTypesConfig description.
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
