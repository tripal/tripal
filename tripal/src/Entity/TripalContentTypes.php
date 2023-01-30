<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Defines the TripalContentTypes entity.
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
 *     "list_builder" = "Drupal\tripal\Controller\TripalContentTypesListBuilder",
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
 *     "content_types"
 *   },
 *   links = {
 *     "add-form" = "/admin/tripal/config/types/add",
 *     "edit-form" = "/admin/tripal/config/types/{config}",
 *     "delete-form" = "/admin/tripal/config/types/{config}/delete",
 *   }
 * )
 */
class TripalContentTypes extends ConfigEntityBase implements ConfigEntityInterface {

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

  // Your specific configuration property get/set methods go here,
  // implementing the interface.
}
