<?php

namespace Drupal\tripal\Plugin\TripalStorage;

/**
 * Defines the interface for Tripal Storage plugins.
 *
 * Tripal Storage Plugins provide alternate data backends for fields attached
 * to Tripal Content Types.
 */
interface TripalStorageInterface {

	/**
	 * Load all fields for this storage interface in the provided entities.
	 *
	 * DO NOT remove any content from the entities.
	 * Instead add content to each field using this storage.
	 *
	 * @param int[] $ids
	 *   The entity ids of the TripalEntity entities to be loaded.
	 * @param array $entities
	 *   The entity objects with the default SqlContentEntityStorage loaded.
	 */
	public function loadMultipleEntities(array $ids, array &$entities);

	/**
	 * Run after all entities are loaded to provide an opprotunity to override values.
	 *
	 * @param array $entities
	 *   The entity objects with the all fields loaded.
	 */
	public function postEntityLoad(array &$entities);

	/**
	 * Provides an opprotunity to alter values before the entities are saved.
	 *
	 * Run before SQLContentEntityStorage::doPreSave()
	 *   inherited from ContentEntityBase.
	 *
	 * @param EntityInterface $entity
	 *   The entity object to be saved.
	 * @return bool
	 *   TRUE if the entity was altered; FALSE otherwise.
	 */
	public function preSaveEntity(&$entity);

	/**
	 * Provides an opprotunity to save data before the entity is saved
	 * but after the default entity presave.
	 *
	 * Run after SQLContentEntityStorage::doPreSave()
	 *   inherited from ContentEntityBase.
	 * Run before SQLContentEntityStorage::doSave()
	 *   inherited from ContentEntityBase.
	 *
	 * @param int $id
	 *   The default SQL Storage ID of the entity to be saved.
	 * @param EntityInterface $entity
	 *   The entity object to be saved.
	 * @return bool
	 *   TRUE if the entity was altered; FALSE otherwise.
	 */
	public function saveEntity($id, &$entity);

	/**
	 * Provides an opprotunity to clean up values after the save.
	 *
	 * Run after SQLContentEntityStorage::doPostSave()
	 *   inherited from ContentEntityBase.
	 *
	 * @param EntityInterface $entity
	 *   The completely saved entity.
	 * @param bool $update
	 *   TRUE if the entity was updated, FALSE if it was new.
	 * @return bool
	 *   TRUE if the entity was altered; FALSE otherwise.
	 */
	public function postSaveEntity(&$entity, $update);

}
