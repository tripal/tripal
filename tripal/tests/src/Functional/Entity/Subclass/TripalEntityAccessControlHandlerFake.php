<?php

namespace Drupal\Tests\tripal\Functional\Entity\Subclass;

use Drupal\tripal\Access\TripalEntityAccessControlHandler;

/**
 * Mock class.
 */
class TripalEntityAccessControlHandlerFake extends TripalEntityAccessControlHandler {

  /**
   * Public method to allow us to test protected checkAccess().
   */
  public function returnProtectedCheckAccess($entity, $operation, $account) {
    return $this->checkAccess($entity, $operation, $account);
  }

  /**
   * Public method to allow us to test protected checkCreateAccess().
   */
  public function returnProtectedCheckCreateAccess($account, $entity_bundle) {
    return $this->checkCreateAccess($account, [], $entity_bundle);
  }

}
