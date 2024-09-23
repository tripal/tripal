<?php
namespace Drupal\tripal_layout\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Defines the Default Layout entity controlling the page display/layout.
 *
 * @ConfigEntityType(
 *   id = "tripal_layout_default_view",
 *   label = @Translation("Tripal Default Display Layout"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal_layout\ListBuilders\TripalDefaultViewLayoutListBuilder",
 *     "form" = {
 *       "delete" = "Drupal\tripal_layout\Form\TripalDefaultViewLayoutDeleteForm",
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
class TripalDefaultViewLayout extends ConfigEntityBase implements TripalLayoutDefaultViewInterface {

  /**
   * A unique ID for this display layout entity.
   *
   * @var string
   */
  protected $id;

  /**
   * A label to provide to the admin identifying this collection of display layouts.
   *
   * @var string
   */
  protected $label;

  /**
   * A description to provide to the admin describing this collection.
   *
   * @var string
   */
  protected $description;


  /**
   * The collection of display layouts itself directly from the YAML.
   *
   * @var array
   */
  protected $layouts;

  /**
   * Retrieves the description of this display layout collection.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }
}
