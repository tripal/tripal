<?php

namespace Drupal\Tests\tripal\Functional\Entity\Subclass;

use \Drupal\tripal\Access\TripalEntityAccessControlHandler;

/**
 * Mock class.
 */
class TripalEntityAccessControlHandlerFake extends TripalEntityAccessControlHandler {

	public function returnProtectedCheckAccess($entity, $operation, $account) {
		return $this->checkAccess($entity, $operation, $account);
	}

	public function returnProtectedCheckCreateAccess($account) {
		return $this->checkCreateAccess($account, []);
	}
}
