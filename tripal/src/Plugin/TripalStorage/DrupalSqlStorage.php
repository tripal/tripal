<?php

namespace Drupal\tripal\Plugin\TripalStorage;

use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Validator\ConstraintViolation;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Services\TripalLogger;

/**
 * Chado implementation of the TripalStorageInterface.
 *
 * @TripalStorage(
 *   id = "drupal_sql_storage",
 *   label = @Translation("Drupal SQL Storage"),
 *   description = @Translation("This storage backend is used for fields which would like to use the default Drupal SQL-based field storage with a Tripal-based Field."),
 * )
 */
class DrupalSqlStorage extends TripalStorageBase implements TripalStorageInterface {

  /**
   * {@inheritDoc}
   */
  public function updateValues(&$values): bool {
    // No need to do anything here.  This is handled by the
    // default SQL storage provided by Drupal
    return TRUE;
  }

  /**
   * {@inheritDoc}
   */
  public function deleteValues($values): bool {
    // No need to do anything here.  This is handled by the
    // default SQL storage provided by Drupal
    return TRUE;
  }

  /**
   * {@inheritDoc}
   */
  public function findValues($values) {
    /** @todo: impelment this function properly **/
    return $values;
  }

  /**
   * {@inheritDoc}
   */
  public function insertValues(&$values): bool {
    // No need to do anything here.  This is handled by the
    // default SQL storage provided by Drupal
    return TRUE;
  }


  /**
   * {@inheritDoc}
   */
  public function loadValues(&$values): bool {
    // No need to do anything here.  This is handled by the
    // default SQL storage provided by Drupal
    return $values;
  }

  /**
   * {@inheritDoc}
   */
  public function validateValues($values) {
    // No need to do anything here.  This is handled by the
    // default SQL storage provided by Drupal
    $violations = [];
    return $violations;
  }

  /**
   * {@inheritDoc}
   */
  public function getStoredTypes() {
    return $this->property_types;
  }
}
