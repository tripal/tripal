<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\Plugin\TripalStorage\TripalStorageInterface;

/**
 * Provides the default Tripal Storage.
 * This storage uses the Drupal SQL storage exclusively.
 *
 * @TripalStorage(
 *   id = "chadostorage",
 *   label = @Translation("Chado Storage"),
 *   description = @Translation("This storage maps your data to the GMOD Chado schema."),
 * )
 */
class ChadoStorage extends PluginBase implements TripalStorageInterface {

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
	 *
	 * NOTE: This is where we actually save the record to chado. It is done here
	 * to ensure that the id is available for saving in the Drupal schema in
	 * saveEntity(). This maps to the Tripal3 tripal_chado_field_storage_write().
	 */
	public function preSaveEntity(&$entity) {

		// Get the Tripal Content Type.
		$bundle = \Drupal\tripal\Entity\TripalEntityType::load($entity->getType());
		// Then get the Tripal Term and Tripal Vocab.
		$term = $bundle->getTerm();
		$vocab = $term->getVocab();

		// Use the Tripal Vocab/Term to get the chado dbxref/cvterm.
		$term_accession = $term->getAccession();
		$vocab_name = $vocab->getName();
		// @debug dpm($vocab_name . ':' . $term_accession, 'term');
		/* @todo upgrade
		$dbxref = chado_get_dbxref([
	    'accession' => $term_accession,
	    'db_id' => ['name' => $vocab_name],
	  ]);
	  $cvterm = chado_get_cvterm(['dbxref_id' => $dbxref->dbxref_id]);
		*/

		// Get the value for each field in the current entity.
		foreach ($entity->getFields() as $name => $field) {
			// @debug dpm($field->getValue(), $name);
		}

		return FALSE; // Entities not altered.
	}

	/**
	 * @{inheritdoc}
	 */
	public function saveEntity($id, &$entity) {
		return FALSE; // Entities not altered.
	}

	/**
	 * @{inheritdoc}
	 */
	public function postSaveEntity(&$entity, $update) {
		return FALSE; // Entities not altered.
	}
}
