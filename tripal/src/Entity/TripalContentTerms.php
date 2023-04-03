<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Provides a UI for YML-based term creation.
 * Each instance of this entity is a single configuration for terms in your site.
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
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalContentTermsListBuilder",
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
 *     "description",
 *     "vocabularies"
 *   },
 *   links = {
 *     "delete-form" = "/admin/tripal/config/terms/{config}/delete",
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

  /**
   * Retrieves the current description for the term mapping setup.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

}
