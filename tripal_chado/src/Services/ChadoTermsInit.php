<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\TripalDBX\TripalDbxConnection;
use Drupal\tripal\TripalVocabTerms\TripalTerm;


class ChadoTermsInit{

  /**
   * Instantiates a new ChadoTermsInit object.
   */
  public function __construct() {

  }

  /**
   * Gets a controlled vocabulary object.
   *
   * @param string $name
   *   The name of the vocabulary
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalVocabularyBase
   */
  private function getVocabulary($name) {
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocabulary = $vmanager->loadCollection($name, 'chado_vocabulary');
    if (!$vocabulary) {
      $vocabulary = $vmanager->createCollection($name, 'chado_vocabulary');
      if (!$vocabulary) {
        throw new \Exception("Unable to create vocabulary with the name $name.");
      }
    }
    return $vocabulary;
  }

  /**
   * Gets a controlled vocabulary IDspace object.
   *
   * @param string $name
   *   The name of the IdSpace
   *
   * @return \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  private function getIdSpace($name) {
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($name, 'chado_id_space');
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($name, 'chado_id_space');
      if (!$idSpace) {
        throw new \Exception("Unable to create ID Space with the name $name.");
      }
    }
    return $idSpace;
  }

  /**
   * Installs the module's default terms into Chado.
   *
   * @param string $id
   *   The id of the terms configuration you want to install.
   */
  public function installTerms(string $id = 'chado_content_terms') {
    $config_factory = \Drupal::service('config.factory');

    $config_key = 'tripal.tripal_content_terms.' . $id;
    $config = $config_factory->get($config_key);
    if (!$config) {
      throw new \Exception("Unable to find configuration with the key $id.");
    }
    $vocabs = $config->get('vocabularies');
    foreach ($vocabs as $vocab_info) {

      // Step 1: Create the Vocabulary.
      $vocab = $this->getVocabulary($vocab_info['name']);
      if (array_key_exists('label', $vocab_info)) {
        $vocab->setLabel($vocab_info['label']);
      }

      // Step 2: Create the IdSpaces.
      foreach ($vocab_info['idSpaces'] as $idSpace_info) {
        $idspace = $this->getIdSpace($idSpace_info['name']);
        if (array_key_exists('description', $idSpace_info)) {
          $idspace->setDescription($idSpace_info['description']);
        }
        if (array_key_exists('urlPrefix', $idSpace_info)) {
          $idspace->setUrlPrefix($idSpace_info['urlPrefix']);
        }
        // A vocabulary can borrow a term from another IdSpace
        // If so, that IdSpace should have been created prior to
        // this vocabulary using it.
        if (!array_key_exists('isBorrowed', $idSpace_info) or
            $idSpace_info['urlPrefix']['isBorrowed'] !== True) {
          $idspace->setDefaultVocabulary($vocab_info['name']);
          $vocab->addIdSpace($idSpace_info['name']);
        }
      }

      // The vocab URL should be set after all IdSpaces have been added.
      if (array_key_exists('url', $vocab_info)) {
        $vocab->setURL($vocab_info['url']);
      }

      // Step 3: Add the terms.
      if (array_key_exists('terms', $vocab_info)) {
        foreach ($vocab_info['terms'] as $term_info) {
          list($idSpace_name, $accession) = explode(':', $term_info['id']);
          $idspace = $this->getIdSpace($idSpace_name);
          if ($idspace) {
            $term = new TripalTerm([
              'name' => $term_info['name'],
              'accession' => $accession,
              'idSpace' => $idSpace_name,
              'vocabulary' => $vocab_info['name'],
              'definition' => array_key_exists('description', $term_info) ? $term_info['description'] : '' ,
            ]);
            $idspace->saveTerm($term);
          }
        }
      }
    }
  }
}
