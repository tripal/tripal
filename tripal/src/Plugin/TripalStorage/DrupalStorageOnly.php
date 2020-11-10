<?php

namespace Drupal\tripal\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\Plugin\TripalStorage\TripalStorageInterface;

/**
 * Provides the default Tripal Storage.
 * This storage uses the Drupal SQL storage exclusively.
 *
 * @TripalStorage(
 *   id = "drupalonly",
 *   label = @Translation("Drupal SQL Storage"),
 *   description = @Translation("This storage uses the Drupal SQL storage exclusively and is good for application-specific fields."),
 * )
 */
class DrupalStorageOnly extends PluginBase implements TripalStorageInterface {

	/**
	 * @{inheritdoc}
	 */
	public function loadMultipleEntities(array $ids, array &$entities) {
		// No Return Value.
	}

	/**
	 * @{inheritdoc}
	 */
	public function postEntityLoad(array &$entities) {
		// No Return Value.
	}

	/**
	 * @{inheritdoc}
	 */
	public function preSaveEntity(EntityInterface &$entity) {
		return FALSE; // Entities not altered.
	}

	/**
	 * @{inheritdoc}
	 */
	public function saveEntity($id, EntityInterface $entity) {
		return FALSE; // Entities not altered.
	}

	/**
	 * @{inheritdoc}
	 */
	public function postSaveEntity(EntityInterface $entity, $update) {
		return FALSE; // Entities not altered.
	}
}
