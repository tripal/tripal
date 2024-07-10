<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX\Subclass;

use Drupal\tripal\TripalDBX\TripalDbxConnection;
use Drupal\Tests\tripal\Kernel\TripalDBX\Subclass\TripalDbxSchemaFake;

/**
 * Fake connection class.
 */
class TripalDbxConnectionFake extends TripalDbxConnection {

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
  public function getTripalDbxClass($class) :string {
    static $classes = [
      'Schema' => TripalDbxSchemaFake::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid Tripal DBX class '$class'.");
    }
    return $classes[$class];
  }

}
