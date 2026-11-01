<?php

namespace Drupal\tripal_file\ProxyClass\Services;

use Drupal\Core\DependencyInjection\DependencySerializationTrait;

/**
 * Provides a proxy class for TripalFileUninstallValidator.
 *
 * Full class is \Drupal\tripal_file\Services\TripalFileUninstallValidator.
 *
 * The uninstall validator is only ever called when uninstalling a module,
 * so for efficiency we don't want to load the service unless it is needed.
 *
 * @see \Drupal\Component\ProxyBuilder
 */
class TripalFileUninstallValidator {

  use DependencySerializationTrait;

  /**
   * The id of the original proxied service.
   *
   * @var string
   */
  protected $drupalProxyOriginalServiceId;

  /**
   * The real proxied service, after it was lazy loaded.
   *
   * @var \Drupal\tripal_file\Services\TripalFileUninstallValidator
   */
  protected $service;

  /**
   * The service container.
   *
   * @var \Symfony\Component\DependencyInjection\ContainerInterface
   */
  protected $container;

  /**
   * Constructs a ProxyClass Drupal proxy object.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The container.
   * @param string $drupal_proxy_original_service_id
   *   The service ID of the original service.
   */
  public function __construct(ContainerInterface $container, $drupal_proxy_original_service_id) {
    $this->container = $container;
    $this->drupalProxyOriginalServiceId = $drupal_proxy_original_service_id;
  }

  /**
   * Lazy loads the real service from the container.
   *
   * @return object
   *   Returns the constructed real service.
   */
  protected function lazyLoadItself() {
    if (!isset($this->service)) {
      $this->service = $this->container->get($this->drupalProxyOriginalServiceId);
    }

    return $this->service;
  }

  /**
   * {@inheritdoc}
   */
  public function validate() {
    return $this->lazyLoadItself()->validate();
  }

}
