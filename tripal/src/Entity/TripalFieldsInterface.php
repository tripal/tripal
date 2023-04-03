<?php
namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides a UI for YML-based TripalField creation.
 * Each instance of this entity is a single configuration for tripal fields
 * in your site.
 */
interface TripalFieldsInterface extends ConfigEntityInterface {
  // Add get/set methods for your configuration properties here.
}
