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
 
    $importers = tripal_get_importers();
    foreach($importers as $class_name) {
        $importer_object = new $class_name;
        $links[$class_name] = [
            'title' => $importer_object::$name,
            'description' => $importer_object::$description,
            'route_name' => 'tripal.data_loaders_tripalimporter',
            'route_parameters' => ['class' => $class_name]
        ] + $base_plugin_definition;
        unset($importer_object);
    }
 
    return $links;
  }
}