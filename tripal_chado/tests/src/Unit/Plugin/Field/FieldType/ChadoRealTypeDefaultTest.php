<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\FieldType;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\Plugin\Field\FieldType\ChadoRealTypeDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoRealTypeDefault.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldType\ChadoRealTypeDefault
 *
 * @group tripal-chado-field
 */
#[CoversClass(ChadoRealTypeDefault::class)]
#[Group('tripal-chado-field')]
class ChadoRealTypeDefaultTest extends UnitTestCase {

  /**
   * TripalTypes() returns null when base_table is empty.
   */
  public function testTripalTypesReturnsNullWithoutBaseTable(): void {
    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getTargetEntityTypeId')->willReturn('tripal_entity');
    $field_def->method('getSetting')
      ->with('storage_plugin_settings')
      ->willReturn(['base_table' => '', 'base_column' => '']);

    $result = ChadoRealTypeDefault::tripalTypes($field_def);

    $this->assertNull($result);
  }

  /**
   * Discover() returns an empty array when base_table is empty.
   */
  public function testDiscoverReturnsEmptyArrayWithoutBaseTable(): void {
    $bundle = $this->createMock(TripalEntityType::class);
    $bundle->method('getThirdPartySetting')
      ->with('tripal', 'chado_base_table')
      ->willReturn('');

    $result = ChadoRealTypeDefault::discover($bundle, 'chado_Real_type_default', [], []);

    $this->assertSame([], $result);
  }

  /**
   * GenerateSampleValue() returns the expected array structure.
   *
   * Record_id is always 0 (no Chado record exists yet for a sample), and
   * value is a floating point value between -5000 and 5000.
   */
  public function testGenerateSampleValue(): void {
    $field_def = $this->createMock(FieldDefinitionInterface::class);

    $result = ChadoRealTypeDefault::generateSampleValue($field_def);
    $this->assertIsArray($result);
    $this->assertArrayHasKey(0, $result, 'Real sample value supports cardinality -1, so should be nested under a delta');
    $this->assertArrayHasKey('record_id', $result[0]);
    $this->assertArrayHasKey('value', $result[0]);
    $this->assertSame(0, $result[0]['record_id']);
    $this->assertGreaterThanOrEqual(-5000, $result[0]['value'], 'Sample floating point value should be >= -5000.0');
    $this->assertLessThanOrEqual(5000, $result[0]['value'], 'Sample floating point value should be <= 5000.0');
  }

  /**
   * IsCompatible() is true when the base table has a floating point column.
   */
  public function testIsCompatibleReturnsTrueWhenFloatingpointColumnExists(): void {
    $this->setUpChadoContainer([
      'fields' => [
        'name' => ['pgsql_type' => 'character varying'],
        'areal' => ['pgsql_type' => 'real'],
        'adouble' => ['pgsql_type' => 'double precision'],
        'anumeric' => ['pgsql_type' => 'numeric'],
      ],
    ]);

    $entity_type = $this->createMock(TripalEntityType::class);
    $entity_type->method('getThirdPartySetting')
      ->with('tripal', 'chado_base_table')
      ->willReturn('analysis');

    $field = (new \ReflectionClass(ChadoRealTypeDefault::class))
      ->newInstanceWithoutConstructor();

    $this->assertTrue($field->isCompatible($entity_type));
  }

  /**
   * IsCompatible() is false when the base table has no floating point column.
   */
  public function testIsCompatibleReturnsFalseWhenNoFloatingpointColumn(): void {
    $this->setUpChadoContainer([
      'fields' => [
        'name'        => ['pgsql_type' => 'character varying'],
        'description' => ['type' => 'text'],
      ],
    ]);

    $entity_type = $this->createMock(TripalEntityType::class);
    $entity_type->method('getThirdPartySetting')
      ->with('tripal', 'chado_base_table')
      ->willReturn('cv');

    $field = (new \ReflectionClass(ChadoRealTypeDefault::class))
      ->newInstanceWithoutConstructor();

    $this->assertFalse($field->isCompatible($entity_type));
  }

  /**
   * Sets up a Drupal container with a stub Chado database returning $table_def.
   *
   * This is because real Chado is kinda complex to mock.
   *
   * IsCompatible() → getTableColumns() → getChadoTableDef() resolves
   * tripal_chado.database from the container when no schema is provided.
   * Anonymous class stubs avoid the complex Drupal database class hierarchy.
   */
  private function setUpChadoContainer(array $table_def): void {
    $schema_stub = new class($table_def) {

      public function __construct(private array $def) {}

      /**
       * Returns the test table definition.
       */
      public function getTableDef(string $table, array $parameters): array {
        return $this->def;
      }

    };

    $chado_stub = new class($schema_stub) {

      public function __construct(private object $schema) {}

      /**
       * Returns the test schema.
       */
      public function schema(): object {
        return $this->schema;
      }

    };

    $container = new ContainerBuilder();
    $container->set('tripal_chado.database', $chado_stub);
    \Drupal::setContainer($container);
  }

}
