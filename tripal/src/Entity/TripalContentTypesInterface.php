<?php
namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides a UI for YML-based TripalEntityType creation.
 * Each instance of this entity is a single configuration for tripal content
 * types in your site.
 */
interface TripalContentTypesInterface extends ConfigEntityInterface {
  // Add get/set methods for your configuration properties here.
}
