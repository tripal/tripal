<?php

namespace Drupal\Tests\tripal\Kernel\TripalStorage;

use Drupal\KernelTests\KernelTestBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;
use Drupal\tripal\TripalStorage\TripalStorageBase;

/**
 * Tests for Tripal Storage Base class.
 *
 * @group Tripal
 * @group TripalStorage
 */
class TripalStorageTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * Tests the add/get field definition functionality.
   */
  public function testTripalStorageBaseFieldDefn() {

    // To create a tripal storage object we will need the parameters required
    // for the constructor.
    $configuration = [];
    $plugin_id = 'fakePluginName';
    $plugin_definition = [];
    $logger = \Drupal::service('tripal.logger');
    // Tripal Storage Base is an abstract class.
    // Therefore, in order to test it we need to mock the abstract methods.
    $tripalStorage = $this->getMockForAbstractClass(
      'Drupal\tripal\TripalStorage\TripalStorageBase',
      [$configuration, $plugin_id, $plugin_definition, $logger]
    );
    $this->assertIsObject($tripalStorage, "Unable to create tripal storage mock object.");

    // This will be our set of fields to test.
    // We're checking there are no special assumptions about field names here.
    $fields = [
      'name_all_underscores' => NULL,
      'NameSnakeCase' => NULL,
      'Name with Spaces' => NULL,
      'name.with-slightly.special-chars' => NULL,
      'name!with+symbols' => NULL,
    ];

    // We also need a FieldConfig object for each field
    foreach ($fields as $field_name => $placeholder) {
      $fields[$field_name] = $this->createMock(\Drupal\field\Entity\FieldConfig::class);
      $fields[$field_name]->method('getLabel')
        ->willReturn($field_name);

      // Now add it to the storage
      $success = $tripalStorage->addFieldDefinition($field_name, $fields[$field_name]);
      $this->assertTrue($success, "add Field Definition did not return true for $field_name");
    }

    // Now that we've added all fields we want to show that we can
    // retrieve each field definition back out from storage as needed.
    foreach ($fields as $field_name => $expected_defn) {
      $retrieved_defn = $tripalStorage->getFieldDefinition($field_name);
      $this->assertIsObject($retrieved_defn, "Unable to retrieve an object when given $field_name.");
      $this->assertEquals($expected_defn, $retrieved_defn,
        "The retrieved definition did not match the original one we mocked for $field_name.");
    }

    // Check that if we alter a field definition
    // and reset it that we get the most recent one.
    $altered_mock = $fields['NameSnakeCase'];
    $altered_mock->method('getLabel')
        ->willReturn('NEW LABEL');
    $success = $tripalStorage->addFieldDefinition('NameSnakeCase', $altered_mock);
    $this->assertTrue($success, "add Field Definition did not return true for NameSnakeCase (second time)");
    $retrieved_defn = $tripalStorage->getFieldDefinition('NameSnakeCase');
    $this->assertIsObject($retrieved_defn, "Unable to retrieve an object when given NameSnakeCase (second time).");
    $this->assertEquals($altered_mock, $retrieved_defn,
      "The retrieved definition did not match the one we altered for NameSnakeCase (second time).");
  }
}
