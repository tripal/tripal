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
 * @group TripalTokenParser
 */
class TripalEntityTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'path', 'path_alias', 'tripal'];

  protected string $bundle_name = 'fake_organism_bundle_028519';

  use UserCreationTrait;

  /**
   * @var \Drupal\tripal\Services\TripalTokenParser $token_parser
   */
  protected ?object $token_parser = NULL;

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    // Add any schema needed for the functionality I am testing.
    $this->prepareEnvironment(['TripalEntity']);

    // Get the token parser service
    $this->token_parser = \Drupal::service('tripal.token_parser');

    // Create a Tripal Entity Type to be used in the following tests.
    // Note: We can't mock one since they both use SqlContentEntityStorage
    // and we need the entity storage in place to test TripalEntity.
    $entityType = TripalEntityType::create([
      'id' => $this->bundle_name,
      'label' => 'FAKE Organism For Testing',
      'termIdSpace' => 'FAKE',
      'termAccession' => 'ORGANISM',
      'help_text' => 'You are on your own here',
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

  /**
   * Tests token parsing as it relates to an entity
   */
  public function testTripalEntityTokenParser() {

    // Values to use for the tests
    $tokens = ['TripalEntity__entity_id', 'TripalEntityType__entity_id'];
    $token_string = '[' . implode('] [', $tokens) . ']';
    $title_format = '[TripalEntity__entity_id] <i>[organism_genus] [organism_species]</i>'
                  . '[ [organism_infraspecific_type]][ <i>[organism_infraspecific_name]</i>]';
    $alias_format = 'tripal_entity_test/[TripalEntity__entity_id][ ([organism_common_name])]';

    // Create an organism entity
    /** @var \Drupal\tripal\Entity\TripalEntity $entity **/
    $values = [];
    $values['title'] = 'Test ' . uniqid();
    $values['type'] = $this->bundle_name;
    $values['organism_genus'] = [['value' => 'Oryza']];
    $values['organism_species'] = [['value' => 'sativa']];
    $values['organism_infraspecific_type'] = [['value' => 'subspecies']];
    $values['organism_infraspecific_name'] = [['value' => 'Japonica']];
    $values['organism_abbreviation'] = [['value' => 'O. sativa']];
    $values['organism_common_name'] = [['value' => 'Japonica rice']];
    $values['organism_comment'] = [['value' => 'rice is nice']];
    $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
    $this->assertIsObject($entity, 'Unable to create an organism entity');
    $entity->save();

    // We don't have any fields on our test content type, but adding fields is overkill
    // for these tests, we can simply add some values here.
    // This function then populates the token values stored within the TripalEntity class
    $test_values = [
      'organism_genus' => 'Oryza',
      'organism_species' => 'sativa',
      'organism_infraspecific_type' => 'subspecies',
      'organism_infraspecific_name' => 'Japonica',
      'organism_common_name' => 'Japonica rice',
      'extra_value' => 42,
    ];
    $entity->setTokenValues($test_values);

    $organism_bundle = \Drupal\tripal\Entity\TripalEntityType::load($this->bundle_name);
    $entity_reflection = new \ReflectionClass($entity);
    $getBundleEntityTokenValuesMethod = $entity_reflection->getMethod('getBundleEntityTokenValues');
    $token_values = $getBundleEntityTokenValuesMethod->invokeArgs($entity, [$token_string, $organism_bundle]);

    $this->assertIsArray($token_values, 'getBundleEntityTokenValues() did not return an array');
    foreach ($tokens as $token) {
      $this->assertArrayHasKey($token, $token_values, "token_values does not have a \"$token\" element");
    }

    // Test setting the entity title with the default title format
    $expected_title = 'Entity 1';
    $entity->setTitle();
    $entity_title = $entity->getTitle();
    $this->assertEquals($expected_title, $entity_title, 'setTitle did not create the expected title');

    // Test setting the entity title with a custom title format
    $organism_bundle->setTitleFormat($title_format);
    $organism_bundle->save();
    $current_format = $organism_bundle->getTitleFormat();
    $this->assertEquals($title_format, $current_format, 'setTitleFormat did not save the expected format');
    $expected_title = '1 <i>Oryza sativa</i> subspecies <i>Japonica</i>';
    $entity->setTitle();
    $entity_title = $entity->getTitle();
    $this->assertEquals($expected_title, $entity_title, 'setTitle did not create the expected title');

    // Test setting the entity URL alias with a custom alias format
    // (We don't test the default format because a term is not set up)
    $organism_bundle->setURLFormat($alias_format);
    $organism_bundle->save();
    $current_format = $organism_bundle->getURLFormat();
    $this->assertEquals($alias_format, $current_format, 'setAliasFormat did not save the expected format');
    // Parentheses will be url escaped, spaces become dashes
    $expected_alias = '/tripal_entity_test/1-%28Japonica-rice%29';
    $entity->setAlias();
    $entity_alias = $entity->getAlias()['alias'];
    $this->assertEquals($expected_alias, $entity_alias, 'setAlias did not create the expected alias');
  }

}
