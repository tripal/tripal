<?php

namespace Drupal\Tests\tripal_biodb\Functional\Database\Subclass;

use Drupal\tripal_biodb\Database\BioConnection;
use Drupal\Tests\tripal_biodb\Functional\Database\Subclass\BioSchemaFake;

/**
 * Fake connection class.
 */
class BioConnectionFake extends BioConnection {

  /**
   * {@inheritdoc}
   */
  public function findVersion(
    ?string $schema_name = NULL,
    bool $exact_version = FALSE
  ) :string {
    return '';
  }

  /**
   * {@inheritdoc}
   */
  public function getAvailableInstances() :array {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public function getBioClass($class) :string {
    static $classes = [
      'Schema' => BioSchemaFake::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid BioDb class '$class'.");
    }
    return $classes[$class];
  }

}
