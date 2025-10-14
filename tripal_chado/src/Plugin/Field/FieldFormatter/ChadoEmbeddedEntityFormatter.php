<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Cache\CacheableMetadata;
use Drupal\Core\Entity\EntityDisplayRepositoryInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Logger\LoggerChannelFactoryInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Plugin implementation of the 'embedded entity' formatter.
 */
#[TripalFieldFormatter(
  id: 'embedded_entity_view',
  label: new TranslatableMarkup('Rendered entity'),
  description: new TranslatableMarkup('A chado embedded entity.'),
  field_types: [
    'chado_analysis_type_default',
    'chado_assay_type_default',
    'chado_biomaterial_type_default',
    'chado_contact_by_role_type_default',
    'chado_contact_type_default',
    'chado_dbxref_type_default',
    'chado_featuremap_type_default',
    'chado_feature_type_default',
    'chado_organism_type_default',
    'chado_project_type_default',
    'chado_protocol_type_default',
    'chado_pub_type_default',
    'chado_stock_type_default',
    'chado_study_type_default',
    'chado_synonym_type_default',
  ],
)]
class ChadoEmbeddedEntityFormatter extends ChadoFormatterBase {

  /**
   * The number of times this formatter allows rendering the same entity.
   *
   * @var int
   */
  const RECURSIVE_RENDER_LIMIT = 20;

  /**
   * The logger factory.
   *
   * @var \Drupal\Core\Logger\LoggerChannelFactoryInterface
   */
  protected $loggerFactory;

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The entity display repository.
   *
   * @var \Drupal\Core\Entity\EntityDisplayRepositoryInterface
   */
  protected $entityDisplayRepository;

  /**
   * An array of counters for the recursive rendering protection.
   *
   * Each counter takes into account all the relevant information about the
   * field and the entity that is being rendered.
   *
   * @var array
   *
   * @see \Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoEmbeddedEntityFormatter::viewElements()
   */
  protected static $recursiveRenderDepth = [];

  /**
   * Constructs an ChadoEmbeddedEntityFormatter instance.
   *
   * @param string $plugin_id
   *   The plugin ID for the formatter.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Field\FieldDefinitionInterface $field_definition
   *   The definition of the field to which the formatter is associated.
   * @param array $settings
   *   The formatter settings.
   * @param string $label
   *   The formatter label display setting.
   * @param string $view_mode
   *   The view mode.
   * @param array $third_party_settings
   *   Any third party settings.
   * @param \Drupal\Core\Logger\LoggerChannelFactoryInterface $logger_factory
   *   The logger factory.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Drupal\Core\Entity\EntityDisplayRepositoryInterface $entity_display_repository
   *   The entity display repository.
   */
  public function __construct($plugin_id, $plugin_definition, FieldDefinitionInterface $field_definition, array $settings, $label, $view_mode, array $third_party_settings, LoggerChannelFactoryInterface $logger_factory, EntityTypeManagerInterface $entity_type_manager, EntityDisplayRepositoryInterface $entity_display_repository) {
    parent::__construct($plugin_id, $plugin_definition, $field_definition, $settings, $label, $view_mode, $third_party_settings);
    $this->loggerFactory = $logger_factory;
    $this->entityTypeManager = $entity_type_manager;
    $this->entityDisplayRepository = $entity_display_repository;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $plugin_id,
      $plugin_definition,
      $configuration['field_definition'],
      $configuration['settings'],
      $configuration['label'],
      $configuration['view_mode'],
      $configuration['third_party_settings'],
      $container->get('logger.factory'),
      $container->get('entity_type.manager'),
      $container->get('entity_display.repository')
    );
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'view_mode' => 'default',
      'link' => FALSE,
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements['view_mode'] = [
      '#type' => 'select',
      '#options' => $this->entityDisplayRepository->getViewModeOptions('tripal_entity'),
      '#title' => $this->t('View mode'),
      '#default_value' => $this->getSetting('view_mode'),
      '#required' => TRUE,
    ];

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = [];

    $view_modes = $this->entityDisplayRepository->getViewModeOptions('tripal_entity');
    $view_mode = $this->getSetting('view_mode');
    $summary[] = $this->t('Rendered as @mode', ['@mode' => $view_modes[$view_mode] ?? $view_mode]);

    return $summary;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    parent::viewElements($items, $langcode);
    $view_mode = $this->getSetting('view_mode');
    $elements = [];

    foreach ($this->getEntitiesToView($items, $langcode) as $delta => $entity) {
      // Due to render caching and delayed calls, the viewElements() method
      // will be called later in the rendering process through a '#pre_render'
      // callback, so we need to generate a counter that takes into account
      // all the relevant information about this field and the
      // entity that is being rendered.
      $recursive_render_id = $items->getFieldDefinition()->getTargetEntityTypeId()
        . $items->getFieldDefinition()->getTargetBundle()
        . $items->getName()
        // We include the referencing entity, so we can render default images
        // without hitting recursive protections.
        . $items->getEntity()->id()
        . $entity->getEntityTypeId()
        . $entity->id();

      if (isset(static::$recursiveRenderDepth[$recursive_render_id])) {
        static::$recursiveRenderDepth[$recursive_render_id]++;
      }
      else {
        static::$recursiveRenderDepth[$recursive_render_id] = 1;
      }

      // Protect ourselves from recursive rendering.
      if (static::$recursiveRenderDepth[$recursive_render_id] > static::RECURSIVE_RENDER_LIMIT) {
        $this->loggerFactory->get('entity')->error('Recursive rendering detected when rendering entity %entity_type: %entity_id, using the %field_name field on the %parent_entity_type:%parent_bundle %parent_entity_id entity. Aborting rendering.', [
          '%entity_type' => $entity->getEntityTypeId(),
          '%entity_id' => $entity->id(),
          '%field_name' => $items->getName(),
          '%parent_entity_type' => $items->getFieldDefinition()->getTargetEntityTypeId(),
          '%parent_bundle' => $items->getFieldDefinition()->getTargetBundle(),
          '%parent_entity_id' => $items->getEntity()->id(),
        ]);
        return $elements;
      }

      $view_builder = $this->entityTypeManager->getViewBuilder($entity->getEntityTypeId());
      $elements[$delta] = $view_builder->view($entity, $view_mode, $entity->language()->getId());

      // Add a resource attribute to set the mapping property's value to the
      // entity's URL. Since we don't know what the markup of the entity will
      // be, we shouldn't rely on it for structured data.
      if (!empty($items[$delta]->_attributes) && !$entity->isNew() && $entity->hasLinkTemplate('canonical')) {
        $items[$delta]->_attributes += ['resource' => $entity->toUrl()->toString()];
      }
    }

    return $this->createListMarkup($elements);
  }

  /**
   * Returns the entities for display.
   *
   * The method takes care of:
   * - checking entity access,
   * - placing the entities in the language expected for display.
   * It is thus strongly recommended that formatters use it in their
   * implementation of viewElements($items) rather than dealing with $items
   * directly.
   *
   * For each entity, the EntityItem by which the entity is referenced
   * is available in $entity->_referringItem. This is useful for field types
   * that store additional values next to the reference itself.
   *
   * @param \Drupal\Core\Field\FieldItemListInterface $items
   *   The item list.
   * @param string $langcode
   *   The language code of the entities to display.
   *
   * @return \Drupal\Core\Entity\EntityInterface[]
   *   The array of entities to display, keyed by delta.
   *
   * @see ::prepareView()
   */
  protected function getEntitiesToView(FieldItemListInterface $items, $langcode) {
    $entities = [];

    foreach ($items as $delta => $item) {
      // Ignore items where no entity could be loaded in prepareView().
      if (!empty($item->_loaded)) {
        $entity = $item->entity;

        // Set the entity in the correct language for display.
        if ($entity instanceof TranslatableInterface) {
          $entity = \Drupal::service('entity.repository')->getTranslationFromContext($entity, $langcode);
        }

        $access = $this->checkAccess($entity);
        // Add the access result's cacheability, ::view() needs it.
        $item->_accessCacheability = CacheableMetadata::createFromObject($access);
        if ($access->isAllowed()) {
          // Add the referring item, in case the formatter needs it.
          if (isset($entity->_referringItem) && ($entity->_referringItem !== $item)) {
            // If the entity is already being referenced by another field item,
            // clone the entity so that _referringItem is set to the correct
            // item in each instance.
            $entity = clone $entity;
            $item->entity = $entity;
          }
          $entity->_referringItem = $item;
          $entities[$delta] = $entity;
        }
      }
    }

    return $entities;
  }

  /**
   * {@inheritdoc}
   *
   * Loads the entities referenced in that field across all the entities being
   * viewed.
   */
  public function prepareView(array $entities_items) {
    // Collect entity IDs to load. For performance, we want to use a single
    // "multiple entity load" to load all the entities for the multiple
    // "entity reference item lists" being displayed. We thus cannot use
    // \Drupal\Core\Field\EntityReferenceFieldItemList::referencedEntities().
    $ids = [];
    foreach ($entities_items as $items) {
      foreach ($items as $item) {
        // To avoid trying to reload non-existent entities in
        // getEntitiesToView(), explicitly mark the items where $item->entity
        // contains a valid entity ready for display. All items are initialized
        // at FALSE.
        $item->_loaded = FALSE;
        $ids[] = $item->entity_id;
      }
    }

    if ($ids) {
      $target_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadMultiple($ids);
    }

    // For each item, pre-populate the loaded entity in $item->entity, and set
    // the 'loaded' flag.
    foreach ($entities_items as $items) {
      foreach ($items as $item) {
        if (isset($target_entities[$item->entity_id])) {
          $item->entity = $target_entities[$item->entity_id];
          $item->_loaded = TRUE;
        }
      }
    }
  }

  /**
   * Checks access to the given entity.
   *
   * By default, entity 'view' access is checked. However, a subclass can choose
   * to exclude certain items from entity access checking by immediately
   * granting access.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity to check.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   A cacheable access result.
   */
  protected function checkAccess(EntityInterface $entity) {
    return $entity->access('view', NULL, TRUE);
  }

}
