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
   * @{inheritdoc}
   *
   * OVERRIDES TripalStorageBase to ensure all field properties are set
   * to save to Drupal!
   */
  public function addTypes(string $field_name, array $types) {

    // Esnure all properties of a field using Drupal storage is
    // set to store it's values in the Drupal database.
    foreach ($types as $type) {
      $storage_settings = $type->getStorageSettings();
      $storage_settings['drupal_store'] = TRUE;
      $type->setStorageSettings($storage_settings);
    }

    // Now let TripalStorageBase deal with these improved property types.
    parent::addTypes($field_name, $types);
  }

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
    return TRUE;
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

    // All types are stored for this storage backend.
    // Thus we can return all of them!
    return $this->getTypes();
  }

  /**
   * {@inheritDoc}
   */
  public function getNonStoredTypes() {

    // All types are stored for this storage backend.
    // Thus this function returns nothing.
    return [];
  }

}
