<?php
namespace Drupal\tripal_layout\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\example\ExampleInterface;

/**
 * Defines the Example entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_layout_default_view",
 *   label = @Translation("Default layouts for Tripal"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal_layout\ListBuilders\TripalLayoutDefaultViewListBuilder",
 *     "form" = {
 *       "delete" = "Drupal\tripal_layout\Form\TripalLayoutDefaultViewDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripal_layout_default_view",
 *   admin_permission = "administer tripal",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "description",
 *     "layouts"
 *   },
 *   links = {
 *     "delete-form" = "/admin/tripal/config/tripal-layout-default-view/{tripal_layout_default_view}/delete",
 *     "layouts" = "/admin/tripal/config/tripal-layout-default-view"
 *   }
 * )
 */
class TripalLayoutDefaultView extends ConfigEntityBase implements TripalLayoutDefaultViewInterface {
  /**
   * The Example ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Example label.
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
  protected $layouts;

  /**
   * Retrieves the current description for the content type configuration.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }
}
