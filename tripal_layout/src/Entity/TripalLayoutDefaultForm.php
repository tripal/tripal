<?php
namespace Drupal\tripal_layout\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Defines the Default Layout entity controlling the form layout.
 *
 * @ConfigEntityType(
 *   id = "tripal_layout_default_form",
 *   label = @Translation("Tripal Default Form Layout"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal_layout\ListBuilders\TripalLayoutDefaultFormListBuilder",
 *     "form" = {
 *       "delete" = "Drupal\tripal_layout\Form\TripalLayoutDefaultFormDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripal_layout_default_form",
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
 *     "delete-form" = "/admin/tripal/config/tripal-layout-default-form/{tripal_layout_default_form}/delete",
 *     "layouts" = "/admin/tripal/config/tripal-layout-default-form"
 *   }
 * )
 */
class TripalLayoutDefaultForm extends ConfigEntityBase {

  /**
   * A unique ID for this form layout entity.
   *
   * @var string
   */
  protected $id;

  /**
   * A label to provide to the admin identifying this collection of form layouts.
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
   * The collection of form layouts itself directly from the YAML.
   *
   * @var array
   */
  protected $layouts;

  /**
   * Retrieves the description of this form layout collection.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }
}
