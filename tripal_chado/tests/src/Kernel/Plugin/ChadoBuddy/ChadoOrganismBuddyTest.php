<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the Chado Organism Buddy.
 *
 * @group ChadoBuddy
 */
#[Group('bio-organism')]
#[Group('plugin-chado-buddy')]
#[RunTestsInSeparateProcesses]
class ChadoOrganismBuddyTest extends ChadoTestBuddyBase {

  /**
   * A Database query interface for querying Chado using Tripal DBX.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  public ChadoConnection $chado_connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to a test Chado.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests the various methods of the ChadoOrganismBuddy.
   */
  public function testOrganismMethods() {
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_organism_buddy', []);

    // TEST: if there is no record then it should return an empty
    // array when we try to get it.
    $chado_buddy_records = $instance->getOrganism([
      'organism.genus' => 'notanorganismgenus',
      'organism.species' => 'notanorganismspecies',
    ]);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for an Organism record that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for an Organism record that does not exist');
  }

}
