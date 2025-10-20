<?php

namespace Drupal\tripal\Access;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Cache\CacheableMetadata;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Access controller for the Tripal Content entity.
 *
 * @see \Drupal\tripal\Entity\TripalEntity.
 */
class TripalEntityAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  public function access(EntityInterface $entity, $operation, ?AccountInterface $account = NULL, $return_as_object = FALSE) {
    $account = $this->prepareUser($account);

    if ($account->hasPermission('administer tripal content')) {
      $result = AccessResult::allowed()->cachePerPermissions();
      return $return_as_object ? $result : $result->isAllowed();
    }
    if (!$account->hasPermission('access content')) {
      $result = AccessResult::forbidden("The 'access content' permission is required.")->cachePerPermissions();
      return $return_as_object ? $result : $result->isAllowed();
    }
    $result = parent::access($entity, $operation, $account, TRUE)->cachePerPermissions();

    return $return_as_object ? $result : $result->isAllowed();
  }

  /**
   * {@inheritdoc}
   */
  public function createAccess($entity_bundle = NULL, ?AccountInterface $account = NULL, array $context = [], $return_as_object = FALSE) {
    $account = $this->prepareUser($account);

    if ($account->hasPermission('administer tripal content')) {
      $result = AccessResult::allowed()->cachePerPermissions();
      return $return_as_object ? $result : $result->isAllowed();
    }
    if (!$account->hasPermission('access content')) {
      $result = AccessResult::forbidden("The 'access content' permission is required.")->cachePerPermissions();
      return $return_as_object ? $result : $result->isAllowed();
    }

    $result = parent::createAccess($entity_bundle, $account, $context, TRUE)->cachePerPermissions();
    return $return_as_object ? $result : $result->isAllowed();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {

    // Ensure that the Tripal Content Admin permission bypasses the following permissions.
    if ($account->hasPermission('administer tripal content')) {
      return AccessResult::allowed();
    }

    switch ($operation) {
      case 'view':
        return AccessResult::allowedIfHasPermission($account, 'view tripal content entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit tripal content entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete tripal content entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * Performs view access checks.
   *
   * @param \Drupal\tripal\Entity\TripalEntityInterface $tripal_entity
   *   The node for which to check access.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The user for which to check access.
   * @param \Drupal\Core\Cache\CacheableMetadata $cacheability
   *   Allows cacheability information bubble up from this method.
   *
   * @return \Drupal\Core\Access\AccessResultInterface|null
   *   The calculated access result or null when no opinion.
   */
  protected function checkViewAccess(TripalEntityInterface $tripal_entity, AccountInterface $account, CacheableMetadata $cacheability): ?AccessResultInterface {
    // If the node status changes, so does the outcome of the check below, so
    // we need to add the node as a cacheable dependency.
    $cacheability->addCacheableDependency($tripal_entity);

    if ($tripal_entity->isPublished()) {
      return NULL;
    }
    $cacheability->addCacheContexts(['user.permissions']);

    if (!$account->hasPermission('view own unpublished content')) {
      return NULL;
    }

    $cacheability->addCacheContexts(['user.roles:authenticated']);
    // The "view own unpublished content" permission must not be granted
    // to anonymous users for security reasons.
    if (!$account->isAuthenticated()) {
      return NULL;
    }

    // When access is granted due to the 'view own unpublished content'
    // permission and for no other reason, node grants are bypassed. However,
    // to ensure the full set of cacheable metadata is available to variation
    // cache, additionally add the node_grants cache context so that if the
    // status or the owner of the node changes, cache redirects will continue to
    // reflect the latest state without needing to be invalidated.
    $cacheability->addCacheContexts(['user']);
    if ($this->moduleHandler->hasImplementations('node_grants')) {
      $cacheability->addCacheContexts(['user.node_grants:view']);
    }
    if ($account->id() != $tripal_entity->getOwnerId()) {
      return NULL;
    }

    return AccessResult::allowed()->addCacheableDependency($cacheability);
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIf($account->hasPermission('create ' . $entity_bundle . ' content'))->cachePerPermissions();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkFieldAccess($operation, FieldDefinitionInterface $field_definition, AccountInterface $account, ?FieldItemListInterface $items = NULL) {
    $fieldName = $field_definition->getName();
    if ($operation == 'edit' && $fieldName === 'status') {
      return AccessResult::allowedIfHasPermissions($account, ['administer node published status', 'administer nodes'], 'OR');
    }
    // Only users with the administer nodes permission can edit administrative
    // fields.
    $administrative_fields = ['uid', 'created', 'promote', 'sticky'];
    if ($operation == 'edit' && in_array($fieldName, $administrative_fields, TRUE)) {
      return AccessResult::allowedIfHasPermission($account, 'administer nodes');
    }
    return parent::checkFieldAccess($operation, $field_definition, $account, $items);
  }

}
