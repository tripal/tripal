<?php

namespace Drupal\tripal\Plugin\Derivative;

use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Derivative class that provides the menu links for the TripalImporters.
 */
class TripalImporterLink extends DeriverBase implements ContainerDeriverInterface {

   /**
   * @var EntityTypeManagerInterface $entityTypeManager.
   */
  protected $entityTypeManager;

  /**
   * Creates a TripalImporterLink instance.
   *
   * @param $base_plugin_id
   * @param EntityTypeManagerInterface $entity_type_manager
   */
  public function __construct($base_plugin_id, EntityTypeManagerInterface $entity_type_manager) {
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static(
      $base_plugin_id,
      $container->get('entity_type.manager')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    $links = [];

    $importer_manager = \Drupal::service('tripal.importer');
    $importer_defs = $importer_manager->getDefinitions();

    foreach ($importer_defs as $plugin_id => $def) {
      $links[$plugin_id] = [
        'title' => $def['label'],
        'description' => $def['description'],
        'route_name' => 'tripal.data_loaders_tripalimporter',
        'route_parameters' => ['plugin_id' => $plugin_id]
      ] + $base_plugin_definition;
    }
    return $links;
  }
}