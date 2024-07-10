<?php

namespace Drupal\tripal\Services;

use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class TripalEntityTypeCollection implements ContainerInjectionInterface  {

  /**
   * The IdSpace service
   *
   * @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idSpaceManager
   */
  protected $idSpaceManager;

  /**
   * The vocabulary service
   *
   * @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager $vocabularyManager
   */
  protected $vocabularyManager;

  /**
   * A logger object.
   *
   * @var TripalLogger $logger
   */
  protected $logger;


  /**
   * Instantiates a new TripalEntityTypeCollection object.
   */
  public function __construct(TripalIdSpaceManager $idSpaceManager,
      TripalVocabularyManager $vocabularyManager, TripalLogger $logger) {

    $this->idSpaceManager = $idSpaceManager;
    $this->vocabularyManager = $vocabularyManager;
    $this->logger = $logger;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static (
      $container->get('tripal.collection_plugin_manager.idspace'),
      $container->get('tripal.collection_plugin_manager.vocabulary'),
      $container->get('tripal.logger')
    );
  }

  /**
   * Retrieve a list of Tripal Entity Collections.
   */
  public function getTypeCollections() {

    $config_factory = \Drupal::service('config.factory');
    $config_list = $config_factory->listAll('tripal.tripalentitytype_collection');

    $collections = [];
    foreach ($config_list as $config_item) {
      $config = $config_factory->get($config_item);
      $label = $config->get('label');
      $id = $config->get('id');
      $collections[$id] = [
        'id' => $id,
        'label' => $config->get('label'),
        'description' => $config->get('description'),
      ];
    }

    return $collections;
  }

  /**
   * Installs content types using all appropriate YAML files.
   *
   * @param array $collection_ids
   *   An array of the collection 'id' you would like to install.
   */
  public function install(array $collection_ids) {
    $yaml_prefix = 'tripal.tripalentitytype_collection.';

    /** @var \Drupal\Core\Config\ConfigFactory $config_factory **/
    $config_factory = \Drupal::service('config.factory');

    // Iterate through the configurations and create the content types.
    foreach ($collection_ids as $config_id) {

      /** @var \Drupal\Core\Config\ImmutableConfig $config **/
      $config_item = $yaml_prefix . $config_id;
      $config = $config_factory->get($config_item);

      if (is_object($config)) {
        $label = $config->get('label');

        $this->logger->notice("Creating Tripal Content Types from: " . $label);

        // Iterate through each of the content types in the config.
        $content_types = $config->get('content_types');
        if ($content_types) {
          foreach ($content_types as $content_type) {

            // Replace the term ID with a term object
            list($termIdSpace, $termAccession) = explode(':', $content_type['term']);
            $idspace = $this->idSpaceManager->loadCollection($termIdSpace);
            $term =  $idspace->getTerm($termAccession);
            $content_type['term'] = $term;

            // Add the content type.
            $content_type = $this->createContentType($content_type);

            // Now add any third party settings.
            $settings = [];
            if (property_exists($content_type, 'settings')) {
              $settings = $content_type->get('settings');
            }
            if (!empty($settings)){
              foreach ($settings as $key => $value) {
                $content_type->setThirdPartySetting('tripal', $key, $value);
              }
              $content_type->save();
            }
          }
        }
      }
      else {
        throw new \Exception("Unable to retrieve the configuration with an id of $config_id using the assumption that it's in the file $config_item.");
      }
    }
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
  public function validate($details) {

    if (!array_key_exists('id', $details) or !$details['id']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No id provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('label', $details) or !$details['label']) {
      $this->logger->error(t('Creation of content name, "@id", failed. No label provided.',
          ['@id' => $details['id']]));
      return FALSE;
    }

    if (!array_key_exists('term', $details) or !$details['term']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No term provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!is_a($details['term'],TripalTerm::class)) {
      $this->logger->error(t('Creation of content type, "@type", failed. The provided term was not an instance of the TripalTerm class.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!$details['term']->isValid()) {
      $this->logger->error(t('Creation of content type, "@type", failed. The provided TripalTerm object was not valid due to missing details.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('category', $details) or !$details['category']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No category was provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (!array_key_exists('help_text', $details) or !$details['help_text']) {
      $this->logger->error(t('Creation of content type, "@type", failed. No help text was provided.',
          ['@type' => $details['label']]));
      return FALSE;
    }

    if (array_key_exists('synonyms', $details) and !is_array($details['synonyms'])) {
      $this->logger->error(t('Creation of content type, "@type", failed. The synonyms should be an array.',
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
   *    - name: the machine-name of the content type.
   *    - help_text: a brief description for how this content type is used.
   *    - url_format: a tokenized string for specifying the format of the URL.
   *    - title_format: a tokenized string for the title.
   *    - term: a TripalTerm object that the content type is associated with.
   *    - id: the machine name of the content type.
   *    - synonms: (optional) a list of synonyms for this content type.
   *
   *
   * @return \Drupal\tripal\Entity\TripalEntityType
   */
  public function createContentType($details) {

    $entityType = NULL;
    $bundle = '';

    // Make sure the field definition is valid.
    if (!$this->validate($details)) {
      return $entityType;
    }

    // Check if the entity type already exists.
    $entityTypes = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['label' => $details['label']]);
    if (!empty($entityTypes)) {
      $this->logger->notice(t('Skipping content type, "@type", as it already exists.',
          ['@type' => $details['label']]));
      $entity_ids = array_keys($entityTypes);
      $bundle = array_pop($entity_ids);
      $entityType = $entityTypes[$bundle];
    }
    else {
      $entityType = TripalEntityType::create($details);
      if (is_object($entityType)) {
        $entityType->save();
        $this->logger->notice(t('Content type, "@type", created.',
            ['@type' => $details['label']]));
        $bundle = $entityType->id();
      }
      else {
        $this->logger->error(t('Creation of content type, "@type", failed. The provided details were: ',
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
        $this->logger->error(t('Creation of content type, "@type", default view mode failed. The provided details were: ',
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
