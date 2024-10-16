<?php
namespace Drupal\tripal_layout\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\tripal_layout\Entity\TripalLayoutConfigEntityTrait;

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

  use TripalLayoutConfigEntityTrait;

  /**
   * A unique ID for this form layout entity.
   *
   * @var string
   */
  protected string $id;

  /**
   * A label to provide to the admin identifying this collection of form layouts.
   *
   * @var string
   */
  protected string $label;

  /**
   * A description to provide to the admin describing this collection.
   *
   * @var string
   */
  protected string $description;


  /**
   * The collection of form layouts itself directly from the YAML.
   *
   * @var array
   */
  protected array $layouts;

  /**
   * Retrieves the description of this form layout collection.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

  /**
   * Retrieve all the layouts defined by this entity.
   *
   * @return array
   *   An list of all the layouts defined by this entity.
   */
  public function getLayouts() {
    return $this->layouts;
  }
}
