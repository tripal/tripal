<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
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

    // TEST: If there is no organism record, then it should return an empty
    // array when we try to get it.
    $chado_buddy_records = $instance->getOrganism([
      'organism.genus' => 'notanorganismgenus',
      'organism.species' => 'notanorganismspecies',
    ]);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for an Organism record that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for an Organism record that does not exist');

    // TEST: Try to insert an organism record without an infraspecific_name.
    $simple_organism_values = [
      'organism.genus' => 'Tripalus',
      'organism.species' => 'databasica',
      'organism.common_name' => 'Tripal',
    ];
    $test_records = [];
    $test_records['set'] = $instance->insertOrganism($simple_organism_values);
    $test_records['get'] = $instance->getOrganism($simple_organism_values);
    $values = $this->multiAssert(
      'insertOrganism',
      $test_records,
      'organism',
      'organism.organism_id',
      'Organism "Tripal: Tripalus databasica"',
      28
    );
    $organism_id = $values['get']['organism.organism_id'];
    $this->assertTrue(is_numeric($organism_id), 'We did not retrieve an integer organism_id for the new organism "Tripal: Tripalus databasica"');

    // TEST: Updating a non-existent Organism should return FALSE.
    $chado_buddy_records = $instance->updateOrganism(
      [
        'organism.abbreviation' => 'notorg',
        'organism.common_name' => 'notanorganism',
      ],
      [
        'organism.genus' => 'notanorganismgenus',
        'organism.species' => 'notanorganismspecies',
      ]
    );
    $this->assertFalse($chado_buddy_records, "We received a value other than FALSE for an update to an Organism that does not exist");

    // TEST: We should be able to update an existing Organism record.
    $test_records = [];
    $test_records['set'] = $instance->updateOrganism(
      ['organism.abbreviation' => 'Trp'],
      $simple_organism_values,
    );
    $test_records['get'] = $instance->getOrganism(
      ['organism.common_name' => 'Tripal'],
    );
    $values = $this->multiAssert(
      'updateOrganism',
      $test_records,
      'organism',
      'organism.organism_id',
      'Organism "Tripal" updated with abbreviation "Trp"',
      28
    );
    $this->assertEquals('Trp', $values['get']['organism.abbreviation'], 'The Organism abbreviation was not updated for Organism "Tripal"');

    // TEST: Upsert should insert an Organism record that doesn't exist.
    $test_records = [];
    $upsert_organism_values = [
      'organism.genus' => 'Genus02',
      'organism.species' => 'species02',
      'organism.common_name' => 'Organism02',
    ];
    $test_records['set'] = $instance->upsertOrganism($upsert_organism_values);
    $test_records['get'] = $instance->getOrganism($upsert_organism_values);
    $values = $this->multiAssert(
      'upsertOrganism',
      $test_records,
      'organism',
      'organism.organism_id',
      'Organism "Organism02" inserted via upsert',
      28
    );
    $organism_id = $values['get']['organism.organism_id'];
    $this->assertTrue(is_numeric($organism_id), 'We did not retrieve an integer organism_id for the new organism "Organism02: Genus02 species02"');

    // TEST: Upsert should update an Organism record that does exist.
    $test_records = [];
    $upsert_organism_values['organism.common_name'] = 'UPDATED02';
    $test_records['set'] = $instance->upsertOrganism($upsert_organism_values);
    $test_records['get'] = $instance->getOrganism($upsert_organism_values);
    $values = $this->multiAssert(
      'upsertOrganism',
      $test_records,
      'organism',
      'organism.organism_id',
      'Organism "Organism02" updated via upsert',
      28
    );
    $organism_id_2 = $values['get']['organism.organism_id'];
    $this->assertTrue(is_numeric($organism_id_2), 'We did not retrieve an integer organism_id for the updated organism "UPDATED02: Genus02 species02"');
    // Make sure the retrieved organism_id is the same as when it was inserted.
    $this->assertEquals($organism_id, $organism_id_2, 'The organism_id changed after upserting the same organism twice, when it should have stayed the same.');
    foreach ($upsert_organism_values as $column => $value) {
      $this->assertEquals($value, $values['get'][$column], "The value for column $column after upserting with the intent to update did not match the expected value.");
    }

    // TEST: Try grabbing the scientific name of one of our test organisms.
    $simple_organism_name = $instance->getOrganismScientificName($simple_organism_values);
    $this->assertEquals('Tripalus databasica', $simple_organism_name, 'We did not retrieve the correct organism scientific name for an organism we inserted: Tripalus databasica');

    // TEST: Try grabbing the organism record again using its scientific name.
    $organism_buddy_from_scientific_name = $instance->getOrganismFromScientificName($simple_organism_name);
    $this->assertIsArray($organism_buddy_from_scientific_name, 'We did not retrieve an array for an Organism record using its scientific name.');
    $this->assertEquals(1, count($organism_buddy_from_scientific_name), 'We did not retrieve a single array for an Organism record using its scientific name.');
  }

  /**
   * Tests the helper methods for infraspecific ranks.
   *
   * Specifically:
   *   - abbreviateInfraspecificRank()
   *   - unabbreviateInfraspecificRank()
   */
  public function testInfraspecificRankHelpers() {
    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_organism_buddy', []);

    // Data to test abbreviation of infraspecific rank, where key = full rank
    // and value = abbreviation.
    $expected_abbreviated = [
      'no_rank' => '',
      'subspecies' => 'subsp.',
      'varietas' => 'var.',
      'variety' => 'var.',
      'subvarietas' => 'subvar.',
      'subvariety' => 'subvar.',
      'convariety' => 'convar.',
      'cultivar' => 'cv.',
      'cultivar group' => 'Group',
      'forma' => 'f.',
      'subforma' => 'subf.',
      'anything_else' => 'anything_else',
    ];

    foreach ($expected_abbreviated as $full => $abbrev) {
      $abbrev_result = $instance->abbreviateInfraspecificRank($full);
      $this->assertEquals(
        $abbrev_result,
        $abbrev,
        'Did not properly abbreviate "' . $full . '", abbreviateInfraspecificRank() returned "' . $abbrev_result . '" instead.'
      );
    }

    // An array of infraspecific ranks to test, where key = abbreviation and
    // value = full (unabbreviated) rank.
    $expected_unabbreviated = [
      '' => '',
      'subsp' => 'subspecies',
      'subsp.' => 'subspecies',
      'ssp' => 'subspecies',
      'ssp.' => 'subspecies',
      'var' => 'varietas',
      'var.' => 'varietas',
      'subvar' => 'subvarietas',
      'subvar.' => 'subvarietas',
      'convar' => 'convariety',
      'convar.' => 'convariety',
      'cv' => 'cultivar',
      'cv.' => 'cultivar',
      'Group' => 'cultivar group',
      'group' => 'cultivar group',
      'f' => 'forma',
      'f.' => 'forma',
      'subf' => 'subforma',
      'subf.' => 'subforma',
      'anything_else' => 'anything_else',
      'anything_else.' => 'anything_else.',
    ];

    foreach ($expected_unabbreviated as $abbrev => $full) {
      $full_result = $instance->unabbreviateInfraspecificRank($abbrev);
      $this->assertEquals(
        $full_result,
        $full,
        'Did not properly unabbreviate "' . $abbrev . '", unabbreviateInfraspecificRank() returned "' . $full_result . '" instead.'
      );
    }
  }

}
