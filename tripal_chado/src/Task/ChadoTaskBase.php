<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_biodb\Task\BioTaskBase;

/**
 * Chado task base class.
 */
abstract class ChadoTaskBase extends BioTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'chado';

  /**
   * {@inheritdoc}
   */
  public function getTripalDbxClass($class) {
    static $classes = [
      'Connection' => ChadoConnection::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid Tripal Dbx class '$class'.");
    }
    return $classes[$class];
  }

}
