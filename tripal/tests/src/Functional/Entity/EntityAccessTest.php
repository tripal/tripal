<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\Entity\Subclass\TripalEntityAccessControlHandlerFake;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Access\AccessResultForbidden;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests Access Checks associated with Tripal Entities.
 *
 * @group Tripal
 * @group Tripal Content
 * @group Tripal Permissions
 */
#[Group('Tripal')]
#[Group('Tripal Content')]
#[Group('Tripal Permissions')]
class EntityAccessTest extends BrowserTestBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['user', 'path', 'tripal'];

  /**
   * Test TripalEntityAccessControlHandler.
   */
  public function testTripalEntityAccessControlHandler() {
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
    $content_type_obj = TripalEntityType::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test content type.");
    $content_type_obj->save();
    $content_type = $content_type_obj->id();
    // -- Content Entity 1.
    $values = [];
    $values['title'] = 'Mini Fredicity' . uniqid();
    $values['type'] = $content_type;
    $entity_one = TripalEntity::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test entity one.");
    $entity_one->save();

    // -- Content Entity 2.
    $values = [];
    $values['title'] = 'Fredicity' . uniqid();
    $values['type'] = $content_type;
    $entity_two = TripalEntity::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test entity two.");
    $entity_two->save();

    // Get the access check object.
    $entity_type_interface = \Drupal::entityTypeManager()->getDefinition('tripal_entity');
    $access_check_obj = new TripalEntityAccessControlHandlerFake($entity_type_interface);

    $entity_bundle = $entity_one->getType();

    $user_unprivileged = $this->drupalCreateUser([]);
    $user_view_all = $this->drupalCreateUser(["view all $entity_bundle content"]);
    $user_view_own = $this->drupalCreateUser(["view own $entity_bundle content"]);
    $user_edit_any = $this->drupalCreateUser(["edit any $entity_bundle content"]);
    $user_edit_own = $this->drupalCreateUser(["edit own $entity_bundle content"]);
    $user_delete_any = $this->drupalCreateUser(["delete any $entity_bundle content"]);
    $user_delete_own = $this->drupalCreateUser(["delete own $entity_bundle content"]);
    $user_add = $this->drupalCreateUser(["create $entity_bundle content"]);
    $user_own_one = $this->drupalCreateUser([
      "create $entity_bundle content",
      "view own $entity_bundle content",
      "edit own $entity_bundle content",
      "delete own $entity_bundle content",
    ]);
    $user_own_two = $this->drupalCreateUser([
      "create $entity_bundle content",
      "view own $entity_bundle content",
      "edit own $entity_bundle content",
      "delete own $entity_bundle content",
    ]);

    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_view_all);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with view all permission should be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_view_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view own permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_edit_any);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit any permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_edit_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit own permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_delete_any);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete any permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_delete_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete own permission should NOT be allowed to VIEW the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'view', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to VIEW the entity.");

    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_view_all);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view all permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_view_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view own permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_edit_any);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with edit any permission should be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_edit_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit own permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_delete_any);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete any permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_delete_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete own permission should NOT be allowed to UPDATE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'update', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to UPDATE the entity.");

    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_unprivileged);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "An unprivileged user should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_view_all);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view all permission should be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_view_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with view own permission should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_edit_any);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit any permission should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_edit_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit own permission should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_delete_any);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with delete any permission should be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_delete_own);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete own permission should NOT be allowed to DELETE the entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_one, 'delete', $user_add);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with add permission should NOT be allowed to DELETE the entity.");

    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_unprivileged, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "An unprivileged user should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_view_all, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with view all permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_view_own, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with view own permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_edit_any, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with edit any permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_edit_own, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with edit own permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_delete_any, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with delete any permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_delete_own, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with delete own permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_add, $entity_bundle);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with add permission should be allowed to CREATE the entity.");

    $entity_two->setOwner($user_own_one);
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'view', $user_own_one);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with view own permission should be allowed to VIEW their own entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'view', $user_own_two);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit own permission should NOT be allowed to VIEW another user's entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'update', $user_own_one);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with edit own permission should be allowed to UPDATE their own entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'update', $user_own_two);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with edit own permission should NOT be allowed to UPDATE another user's entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'delete', $user_own_one);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with delete own permission should be allowed to DELETE their own entity.");
    $result = $access_check_obj->returnProtectedCheckAccess($entity_two, 'delete', $user_own_two);
    $this->assertInstanceOf(AccessResultNeutral::class, $result, "A user with delete own permission should NOT be allowed to DELETE another user's entity.");
  }

}
