<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Entity\Attribute\ConfigEntityType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Form\TripalFieldCollectionForm;
use Drupal\tripal\Form\TripalFieldCollectionDeleteForm;
use Drupal\tripal\ListBuilders\TripalFieldCollectionListBuilder;

/**
 * Provides a UI for YML-based TripalField creation.
 *
 * Each instance of this entity is a single configuration for Tripal Field
 * Collection in your site.
 */
#[ConfigEntityType(
  id: 'tripalfield_collection',
  label: new TranslatableMarkup('Tripal Field Collection'),
  label_collection: new TranslatableMarkup('Tripal Field Collection'),
  handlers: [
    'list_builder' => TripalFieldCollectionListBuilder::class,
    'form' => [
      'add' => TripalFieldCollectionForm::class,
      'edit' => TripalFieldCollectionForm::class,
      'delete' => TripalFieldCollectionDeleteForm::class,
    ],
  ],
  config_prefix: 'tripalfield_collection',
  admin_permission: 'administer tripal',
  entity_keys: [
    'id' => 'id',
    'label' => 'label',
  ],
  links: [
    'delete-form' => '/admin/tripal/config/tripalfield-collection/{tripalfield_collection}/delete',
    'collection' => '/admin/tripal/config/tripalfield-collection',
  ],
  config_export: [
    'id',
    'label',
    'description',
    'fields',
  ],
)]
class TripalFieldCollection extends ConfigEntityBase implements TripalFieldCollectionInterface {

  /**
   * The Config ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Config label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Config description.
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
   * Retrieves the current description.
   *
   * @return string
   */
  public function description() {
    return $this->description;
  }

}
