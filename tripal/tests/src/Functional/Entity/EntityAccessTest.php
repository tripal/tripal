<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Access\AccessResultForbidden;

/**
 * Tests Access Checks associated with Tripal Entities.
 *
 * @group Tripal
 * @group Tripal Content
 * @group Tripal Permissions
 */
class EntityAccessTest extends BrowserTestBase {
    protected $defaultTheme = 'stark';

    protected static $modules = ['tripal'];

  /**
   * Test TripalAccessOwnContentCheck
   */
  public function testTripalAccessOwnContentCheck() {

    $account_other = $this->drupalCreateUser([]);
    $owner = $account_owner = $this->drupalCreateUser([]);


    $access_check_obj = new \Drupal\tripal\Access\TripalAccessOwnContentCheck();

    $result = $access_check_obj->access($owner, $account_other);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user other then the owner should not be allowed access.");

    $result = $access_check_obj->access($owner, $account_owner);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "The owner should be allowed access.");
  }

  /**
   * Test TripalEntityAccessControlHandler
   */
  public function testTripalEntityAccessControlHandler() {

    // One user per permission to check.
    $user_unprivileged = $this->drupalCreateUser([]);
    $user_view = $this->drupalCreateUser(['view tripal content entities']);
    $user_edit = $this->drupalCreateUser(['edit tripal content entities']);
    $user_delete = $this->drupalCreateUser(['delete tripal content entities']);
    $user_add = $this->drupalCreateUser(['add tripal content entities']);

    // Create a Content Type + Entity for this test.
    // -- Content Type.
    $values = [];
    $values['label'] = 'Freddyopolis-' . uniqid();
    $values['id'] = 'freddy';
    $values['url_format'] = 'freddy/TripalEntity__entity_id';
    $values['title_format'] = '[freddy_name]';
    $values['termIdSpace'] = 'FRED';
    $values['termAccession'] = '1g2h3j4k5';
    $values['help_text'] = 'This is just random text to meet the requirement of this field.';
    $values['category'] = 'Testing';
    $content_type_obj = \Drupal\tripal\Entity\TripalEntityType::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test content type.");
    $content_type_obj->save();
    $content_type = $content_type_obj->id();
    // -- Content Entity.
    $values = [];
    $values['title'] = 'Mini Fredicity ' . uniqid();
    $values['type'] = $content_type;
    $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test entity.");
    $entity->save();
    $entity_id = $entity->id();

    // Get the access check object.
    $entity_type_interface = \Drupal::entityTypeManager()->getDefinition('tripal_entity');
    $access_check_obj = new \Drupal\Tests\tripal\Functional\Entity\Subclass\TripalEntityAccessControlHandlerFake($entity_type_interface);

    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'view', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'view', $user_view);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with view permission should be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'view', $user_edit);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'view', $user_delete);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'view', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to VIEW the entity.");

    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'update', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'update', $user_view);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'update', $user_edit);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with edit permission should be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'update', $user_delete);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'update', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to UPDATE the entity.");

    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'delete', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'delete', $user_view);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view permission should be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'delete', $user_edit);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit permission should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'delete', $user_delete);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with delete permission should be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity, 'delete', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to DELETE the entity.");

    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_view);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_edit);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_delete);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_add);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with add permission should be allowed to CREATE the entity.");
  }
}
