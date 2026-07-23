<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Cache\Cache;
#use Drupal\Core\Config\ConfigFactory;
use Drupal\Core\Controller\ControllerBase;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Controller to refresh (i.e. republish) a single Tripal Entity.
 *
 * After refresh, perform a redirect back to the calling entity page.
 */
class TripalEntityRefreshController extends ControllerBase {

#  /**
#   * Drupal configuration service. Used to get chado schema.
#   *
#   * @var Drupal\Core\Config\ConfigFactory
#   */
#  protected ConfigFactory $config_factory;
#
  /**
   * The publish manager service.
   *
   * @var Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager
   */
  protected TripalBackendPublishManager $publish_manager;

  /**
   * Provides injected service.
   */
  public function __construct(
#    ConfigFactory $config_factory,
    TripalBackendPublishManager $publish_manager,
  ) {
#    $this->config_factory = $config_factory;
    $this->publish_manager = $publish_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
#      $container->get('config.factory'),
      $container->get('tripal.backend_publish'),
    );
  }

  /**
   * Refreshes, i.e. republishes, a single Tripal entity.
   *
   * @param Symfony\Component\HttpFoundation\Request $request
   *   The page request object.
   * @param Drupal\tripal\Entity\TripalEntity $tripal_entity
   *   The entity that called this controller.
   */
  public function refresh(Request $request, TripalEntity $tripal_entity) {

    // Gather information needed for republishing.
    $field_storage_info = $tripal_entity->tripalstorage_fields;
    if (count($field_storage_info) != 1) {
      $this->messenger->addError($this->t('Refresh not supported for an entity with multiple datastores "@datastores"',
        ['@datastores' => implode(',', array_keys($field_storage_info))]));
    }
    else {
      $datastore = array_key_first($field_storage_info);
#      $schema_name = NULL;
#      if ($datastore == 'chado_storage') {
#        $settings = $this->config_factory->get('tripal_chado.settings');
#        if ($settings) {
#          $schema_name = $settings->get('default_schema');
#        }
#      }
      $bundle_id = $tripal_entity->getBundle()->getID();
      $pkey = $tripal_entity->getBackendRecordId($datastore);

      // This should not happen, but just in case, make sure we have
      // all the values that we need.
      if (!$bundle_id || !$pkey) {
        $this->messenger()->addError($this->t('This content does not support refresh'));
      }
      else {
        // Republish this single entity.
        $publish_instance = $this->publish_manager->createInstance($datastore);
        $publish_options = [
          'record_ids' => [$pkey],
          'batch_size' => 100,
          'republish' => TRUE,
          'migration-file' => NULL,
          'lenient-migration' => NULL,
          'bundle' => $bundle_id,
          'datastore' => $datastore,
          'job' => NULL,
        ];
        # if ($schema_name) {
        #   $publish_options['schema_name'] = $schema_name';
        # }
        $publish_instance->publish($publish_options);

        // Clear cache for this entity.
        Cache::invalidateTags($tripal_entity->getCacheTags());

        // Status message so that we know something happened.
        $this->messenger()->addStatus($this->t('Refresh completed'));
      }
    }

    // Redirect back to the calling page. Drupal will automatically
    // substitute the destination here that we set by using
    // TripalDynamicDestinationMenuLink in tripal.links.task.yml.
    return $this->redirect('<front>');
  }

}
