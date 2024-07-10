<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\tripal\TripalContentTermsInterface;

/**
 * Defines the TripalContentTerms entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_content_terms",
 *   label = @Translation("Tripal Content Terms"),
 *   label_collection = @Translation("Tripal Content Terms"),
 *   label_singular = @Translation("Tripal Content Term"),
 *   label_plural = @Translation("Tripal Content Terms"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Tripal content term",
 *     plural = "@count Tripal content terms",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\Controller\TripalContentTermsListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalContentTermsForm",
 *       "edit" = "Drupal\tripal\Form\TripalContentTermsForm",
 *       "delete" = "Drupal\tripal\Form\TripalContentTermsDeleteForm",
 *     }
 *   },
 *   config_prefix = "tripal_content_terms",
 *   admin_permission = "administer tripal",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "vocabularies"
 *   },
 *   links = {
 *     "add-form" = "/admin/tripal/config/terms/add",
 *     "edit-form" = "/admin/tripal/config/terms/{tripal_content_terms}",
 *     "delete-form" = "/admin/tripal/config/terms/{tripal_content_terms}/delete",
 *   }
 * )
 */
class TripalContentTerms extends ConfigEntityBase implements TripalContentTermsInterface {

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
  protected $vocabularies;

  // Your specific configuration property get/set methods go here,
  // implementing the interface.
}
