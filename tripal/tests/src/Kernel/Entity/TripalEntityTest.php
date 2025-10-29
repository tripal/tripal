<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\user\Entity\User;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\Tests\user\Traits\UserCreationTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the TripalEntity Class.
 *
 * @group TripalEntity
 * @group TripalTokenParser
 */
#[Group('tripal-content')]
#[Group('tripal-token-parser')]
#[RunTestsInSeparateProcesses]
class TripalEntityTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'path', 'path_alias', 'tripal'];

  /**
   * The name of the bundle to use when testing.
   *
   * @var string
   */
  protected string $bundle_name = 'fake_organism_bundle_028519';

  use UserCreationTrait;

  /**
   * The token parser service.
   *
   * @var \Drupal\tripal\Services\TripalTokenParser
   */
  protected ?object $token_parser = NULL;

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    // Add any schema needed for the functionality I am testing.
    $this->prepareEnvironment(['TripalEntity', 'TripalTerm']);

    // Get the token parser service.
    $this->token_parser = \Drupal::service('tripal.token_parser');

    // Create a term to use for the entity.
    $term_values = [
      'id_space_name' => 'FAKE',
      'term' => [
        'accession' => 'ORGANISM',
        'name' => 'Organism',
      ],
    ];
    /** @var \Drupal\tripal\TripalVocabTerms\TripalTerm $term */
    $term = $this->createTripalTerm($term_values, 'tripal_default_id_space', 'tripal_default_vocabulary');
    $this->assertIsObject($term,
      'We were unable to create a tripal term during test setup');

    // Create a Tripal Entity Type to be used in the following tests.
    // Note: We can't mock one since they both use SqlContentEntityStorage
    // and we need the entity storage in place to test TripalEntity.
    $entityType = TripalEntityType::create([
      'id' => $this->bundle_name,
      'label' => 'FAKE Organism For Testing',
      'term' => $term,
      'help_text' => 'You are on your own here',
      'category' => '',
      'title_format' => '[TripalEntityType__term_label] Entity #[TripalEntity__entity_id]',
      'url_format' => '/[TripalEntityType__term_namespace]/[TripalEntityType__term_accession]/[TripalEntity__entity_id]',
      'hide_empty_field' => '',
      'ajax_field' => '',
    ]);
    $this->assertIsObject($entityType,
      'We were unable to create our Tripal Entity type during test setup');
    $entityType->save();
  }

  /**
   * Tests creating a new Tripal Entity with all defaults used.
   */
  public function testTripalEntityCreateAllDefaults() {

    $user = $this->setUpCurrentUser();

    // Create an extremely basic tripal entity without any fields
    // to confirm the most basic state.
    // Title will use the format "Entity [entity_id]" by default rather then
    // what we set below. That said, just set what we expect below so that
    // we check for the right value.
    $details = [
      'title' => 'Entity 1',
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
    $this->assertEquals(1, $entity_id,
      "We were unable to save the tripal entity.");

    // Core metadata.
    // -- title.
    $ret_title = $entity->getTitle();
    $this->assertEquals('Organism Entity #1', $ret_title,
      "The title should match the format set.");
    // -- alias.
    $ret_alias = $entity->getAlias();
    $this->assertEquals('/FAKE/ORGANISM/1', $ret_alias['alias'],
      "The alias returned should match the format set.");
    // -- bundle.
    $ret_type = $entity->getType();
    $this->assertEquals($this->bundle_name, $ret_type,
      "The type should be set to what we passed in on creation.");
    // -- bundle cache.
    $entity->setBundleCache($ret_type, NULL);
    $ret_bundle = $entity->getBundle();
    $this->assertEquals($this->bundle_name, $ret_bundle->id(),
      "The bundle returned after clearing the cache should match the type returned."
    );
    // -- id.
    $ret_entity_id = $entity->getID();
    $this->assertEquals($entity_id, $ret_entity_id,
      "The retrieved entity_id should be the same one returned fom save()");
    // -- label (i.e. title).
    $ret_label = $entity->label();
    $this->assertEquals($ret_title, $ret_label,
      "The label should match the title.");

    // Published Status.
    $ret_status = $entity->isPublished();
    $this->assertTrue($ret_status,
      "The published status should be set to published by default.");
    $entity->setPublished(FALSE);
    $ret_status = $entity->isPublished();
    $this->assertFalse(
      $ret_status,
      "The entity should no longer be published but that is not what isPublished() says."
    );

    // Timestamps.
    // -- Created.
    $ret_created = $entity->getCreatedTime();
    $this->assertNotNull($ret_created,
      "The created time should be set when an entity is created.");
    $submitted_timestamp = '1741312332';
    $entity->setCreatedTime($submitted_timestamp);
    $ret_set_created = $entity->getCreatedTime();
    $this->assertEquals(
      $submitted_timestamp,
      $ret_set_created,
      "The created time should match what we set it to using setCreatedTime()."
    );
    // -- Changed.
    $ret_changed = $entity->getChangedTime();
    $this->assertEquals($ret_created, $ret_changed,
      "The changed time should match the created time since we have not updated the entity."
    );
    $submitted_timestamp = '1741312332';
    $entity->setChangedTime($submitted_timestamp);
    $ret_set_changed = $entity->getChangedTime();
    $this->assertEquals(
      $submitted_timestamp,
      $ret_set_changed,
      "The changed time should match what we set it to using setChangedTime()."
    );

    // Owner.
    $user1 = $this->createUser();
    $user2 = $this->createUser();
    // -- By ID.
    $ret_owner_id = $entity->getOwnerId();
    $this->assertEquals($user->id(), $ret_owner_id,
      "The owner should be set to the current user by default.");
    // -- By Object.
    $ret_owner = $entity->getOwner();
    $this->assertIsObject($ret_owner,
      "We were unable to retrieve the owner object for this entity.");
    $this->assertInstanceOf(User::class, $ret_owner,
      "The owner returned should be a Drupal User object.");
    $this->assertEquals($user->id(), $ret_owner->id(),
      "The owner returned should be the current user.");
    // -- Setting by ID.
    $entity->setOwnerId($user1->id());
    $ret_owner_set = $entity->getOwnerId();
    $this->assertEquals(
      $user1->id(),
      $ret_owner_set,
      "The owner should now match the user we set by ID."
    );
    // -- Setting by Object.
    $entity->setOwner($user2);
    $ret_owner_set = $entity->getOwner();
    $this->assertIsObject(
      $ret_owner_set,
      "We were unable to retrieve the owner object for this entity after setting it by object."
    );
    $this->assertInstanceOf(
      User::class,
      $ret_owner_set,
      "The owner returned after setting should be a Drupal User object."
    );
    $this->assertEquals(
      $user2->id(),
      $ret_owner_set->id(),
      "The owner returned after setting should be the user we set it to."
    );

    // allNull Helper method.
    $ret_all_null = TripalEntity::allNull([NULL, NULL, NULL, NULL]);
    $this->assertTrue($ret_all_null, "We passed in an array of NULL therefore, the helper method should have confirmed that they were all null.");
    $ret_all_null = TripalEntity::allNull([NULL, NULL, NULL, NULL, 5, NULL]);
    $this->assertFalse($ret_all_null, "We passed in an array with one integer near the end therefore, the helper method should have confirmed that they were NOT all null.");
  }

}
