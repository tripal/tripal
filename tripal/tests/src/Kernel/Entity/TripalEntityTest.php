<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\Core\Entity\FieldableEntityInterface;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;
use \Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the TripalEntity Class.
 *
 * @group TripalEntity
 */
class TripalEntityTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal'];

  protected string $bundle_name = 'fake_organism_bundle_028519';

  use UserCreationTrait;

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_entity');
    $this->installEntitySchema('tripal_entity_type');

    // Create a Tripal Entity Type to be used in the following tests.
    // Note: We can't mock one since they both use SqlContentEntityStorage
    // and we need the entity storage in place to test TripalEntity.
    $entityType = TripalEntityType::create([
      'id' => $this->bundle_name,
      'label' => 'FAKE Organism For Testing',
      'termIdSpace' => 'FAKE',
      'termAccession' => 'ORGANISM',
      'help_text' => '',
      'category' => '',
      'title_format' => '',
      'url_format' => '',
      'hide_empty_field' => '',
      'ajax_field' => '',
    ]);
    $this->assertIsObject($entityType,
      "We were unable to create our Tripal Entity type during test setup.");
    $entityType->save();
  }

  /**
   * Tests creating a new Tripal Entity with all defaults used.
   */
  public function testTripalEntityCreateAllDefaults() {

    $user = $this->setUpCurrentUser();

    // Create an extremely basic tripal entity without any fields
    // to confirm the most basic state.
    $details = [
      'title' => 'Test Tripal Entity',
      'type' => $this->bundle_name,
    ];
    $entity = TripalEntity::create($details);
    $this->assertIsObject($entity,
      "We were unable to create an entity object.");

    // Validate our values.
    $violations = $entity->validate();
    $this->assertEmpty($violations,
      "We should not have had any violations");

    // Finally Save it. This should call preSave() and Save().
    $entity_id = $entity->save();
    $this->assertEquals(1, $entity_id, "We were unable to save the tripal entity.");

    $ret_title = $entity->getTitle();
    $this->assertEquals($details['title'], $ret_title,
      "The title should be set on creation to what we passed in.");

    $ret_type = $entity->getType();
    $this->assertEquals($this->bundle_name, $ret_type,
      "The type should be set to what we passed in on creation.");

    $ret_entity_id = $entity->getID();
    $this->assertEquals($entity_id, $ret_entity_id,
      "The retrieved entity_id should be the same one returned fom save()");

    $ret_label = $entity->label();
    $this->assertEquals($details['title'], $ret_label,
      "The label should match the title.");

    $ret_status = $entity->isPublished();
    $this->assertEquals(TRUE, $ret_status,
      "The published status should be set to published by default.");

    $ret_created = $entity->getCreatedTime();
    $this->assertNotNull($ret_created,
      "The created time should be set");

    $ret_owner_id = $entity->getOwnerId();
    $this->assertEquals($user->id(), $ret_owner_id,
      "The owner should be set to the current user by default.");

    $ret_owner = $entity->getOwner();
    $this->assertIsObject($ret_owner,
      "We were unable to retrieve the owner object for this entity.");
    $this->assertInstanceOf(\Drupal\user\Entity\User::class, $ret_owner,
      "The owner returned should be a Drupal User object.");
    $this->assertEquals($user->id(), $ret_owner->id(),
      "The onwer returned should be the current user.");
  }
}
