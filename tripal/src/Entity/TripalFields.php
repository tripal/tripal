<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Defines the TripalContentTerms entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_fields",
 *   label = @Translation("Tripal Fields"),
 *   label_collection = @Translation("Tripal Fields"),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalFieldsListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalFieldsForm",
 *       "edit" = "Drupal\tripal\Form\TripalFieldsForm",
 *       "delete" = "Drupal\tripal\Form\TripalFieldsDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripal_fields",
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
 *     "delete-form" = "/admin/tripal/config/fields/{config}/delete",
 *   }
 * )
 */
class TripalFields extends ConfigEntityBase implements TripalFieldsInterface {

  /**
   * The TripalContentTermsConfig ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The TripalContentTermsConfig label.
   *
   * @var string
   */
  protected $label;

  /**
   * The TripalContentTermsConfig description.
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
   * Retrieves the current description for the term mapping setup.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

}
