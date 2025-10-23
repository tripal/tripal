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
    // -- Content Entity.
    $values = [];
    $values['title'] = 'Mini Fredicity ' . uniqid();
    $values['type'] = $content_type;
    $entity = TripalEntity::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test entity.");
    $entity->save();
    $entity_id = $entity->id();

    // Get the access check object.
    $entity_type_interface = \Drupal::entityTypeManager()->getDefinition('tripal_entity');
    $access_check_obj = new TripalEntityAccessControlHandlerFake($entity_type_interface);

    $entity_bundle = $entity->getType();

    $user_unprivileged = $this->drupalCreateUser([]);
    $user_view = $this->drupalCreateUser(["view all $entity_bundle content"]);
    $user_edit = $this->drupalCreateUser(["edit any $entity_bundle content"]);
    $user_delete = $this->drupalCreateUser(["delete any $entity_bundle content"]);
    $user_add = $this->drupalCreateUser(["create $entity_bundle content"]);

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

    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_unprivileged, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "An unprivileged user should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_view, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with view permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_edit, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with edit permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_delete, $entity_bundle);
    $this->assertInstanceOf(AccessResultForbidden::class, $result, "A user with delete permission should NOT be allowed to CREATE the entity.");
    $result = $access_check_obj->returnProtectedCheckCreateAccess($user_add, $entity_bundle);
    $this->assertInstanceOf(AccessResultAllowed::class, $result, "A user with add permission should be allowed to CREATE the entity.");
  }

}
