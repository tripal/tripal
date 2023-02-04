<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Entity\TripalEntityType;


class TripalContentTypes {


  /**
   * Instantiates a new TripalContentTypes object.
   */
  public function __construct() {

  }

  /**
   * Installs the module's Tripal content types.
   *
   */
  public function loadConfig($logger) {
    $config_factory = \Drupal::service('config.factory');
    $config_list = $config_factory->listAll('tripal.tripal_content_types');
    foreach ($config_list as $config_item) {
      $config = $config_factory->get($config_item);
      $label = $config->get('label');
      $logger->notice("Creating Tripal content types from: " . $label);
      $content_types = $config->get('content_types');
      foreach ($content_types as $content_type) {
        $content_type = $this->createContentType($content_type, $logger);
      }
    }
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
    }
    return $idSpace;
  }


  /**
   * Gets a controlled voabulary object.
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
    }
    return $vocabulary;
  }

  /**
   * Gets a term by its idSpace and accession
   *
   * @param string $idSpace
   *   The Id space name for the term.
   * @param string $accession
   *   The accession for the term.
   * @return TripalTerm|NULL
   *   A tripal term object.
   */
  private function getTerm($idSpace, $accession, $vocabulary = NULL) {
    $id = $this->getIdSpace($idSpace);
    if ($vocabulary) {
      $id->setDefaultVocabulary($vocabulary);
    }
    return $id->getTerm($accession);
  }

  /**
   * Validates a Tripal content type definition array.
   *
   * This function can be used to check a definition prior to adding
   * the content type.
   *
   * @param array $details
   *   A definition array for the content type.
   * @return bool
   *   True if the array passes validation checks. False otherwise.
   */
  public function validate($details, $logger) {

    if (!array_key_exists('term', $details) or !$details['term']) {
      $logger->error(t('Creation of content type, "@type", failed. No term provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('name', $details) or !$details['name']) {
      $logger->error(t('Creation of content type, "@type", failed. No name provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('label', $details) or !$details['label']) {
      $logger->error(t('Creation of content type, "@type", failed. No label provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('category', $details) or !$details['category']) {
      $logger->error(t('Creation of content type, "@type", failed. No category provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    return TRUE;
  }


  /**
   * Creates the content type.
   *
   * @param array $details
   *   Describes the content type you would like to create.
   *   Should contain the following:
   *    - label: the human-readable label to be used for the content type.
   *    - category: a human-readable category to group like content types
   *      together.
   *    - term: a tripal term object which should be associated with the
   *      content type.
   *    - id: the machine name of the content type.
   *    - synonms: (optional) a list of synonyms for this content type.
   * @param \Psr\Log\LoggerInterface $logger
   *   A logger object to which messages will be logged.
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   */
  public function createContentType($details, $logger) {

    $entityType = NULL;
    $bundle = '';

    // Make sure the field definition is valid.
    if (!$this->validate($details, $logger)) {
      return FALSE;
    }


    // Get the term object and make sure it's valid.
    list($termIdSpace, $termAccession) = explode(':', $details['term']);
    $term = $this->getTerm($termIdSpace, $termAccession);
    $details['term'] = $term;
    if (!$term->isValid()) {
      $logger->error(t('Creation of content type, "@type", failed. The provided term, "@term", was not valid.',
          ['@type' => $details['label'], '@term' => $term->getTermId()]));
      return NULL;
    }

    // Check if the entity type already exists.
    $entityTypes = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['label' => $details['label']]);
    if (!empty($entityTypes)) {
      $logger->notice(t('Skipping content type, "@type", as it already exists.',
          ['@type' => $details['label']]));
      $bundle = array_pop(array_keys($entityTypes));
      $entityType = $entityTypes[$bundle];
    }
    else {
      $entityType = TripalEntityType::create($details);
      if (is_object($entityType)) {
        $entityType->save();
        $logger->notice(t('Content type, "@type", created.',
            ['@type' => $details['label']]));
        $bundle = $entityType->getID();
      }
      else {
        $logger->error(t('Creation of content type, "@type", failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }

    // Create the default view mode for this new content type.
    $storage = \Drupal::entityTypeManager()->getStorage('entity_view_display');
    $view_display = $storage->load('tripal_entity.' . $bundle . '.default');
    if (!$view_display) {
      $view_details = [
        'langcode' => 'en',
        'status' => True,
        'dependencies' => [
          'module' => ['tripal']
        ],
        'targetEntityType' => 'tripal_entity',
        'bundle' => $bundle,
        'mode' => 'default',
        'content' => [],
        'hidden' => [],
      ];
      $view_display = $storage->create($view_details, 'entity_view_display');
      if (!$view_display->save()) {
        $logger->error(t('Creation of content type, "@type", default view mode failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }

    // Create the default form mode for this new content type.
    $storage = \Drupal::entityTypeManager()->getStorage('entity_form_display');
    $form_display = $storage->load('tripal_entity.' . $bundle . '.default');
    if (!$form_display) {
      $form_details = [
        'langcode' => 'en',
        'status' => True,
        'dependencies' => [
          'module' => ['tripal']
        ],
        'targetEntityType' => 'tripal_entity',
        'bundle' => $bundle,
        'mode' => 'default',
        'content' => [],
        'hidden' => [],
      ];
      $form_display = $storage->create($form_details, 'entity_view_display');
      if (!$form_display->save()) {
        $logger->error(t('Creation of content type, "@type", default form mode failed. The provided details were: ',
            ['@type' => $details['label']]) . print_r($details));
      }
    }
    return $entityType;
  }

}
