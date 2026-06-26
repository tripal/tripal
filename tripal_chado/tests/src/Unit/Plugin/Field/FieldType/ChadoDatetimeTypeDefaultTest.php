<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\FieldType;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\Plugin\Field\FieldType\ChadoDatetimeTypeDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoDatetimeTypeDefault.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldType\ChadoDatetimeTypeDefault
 *
 * @group tripal-chado-field
 */
#[CoversClass(ChadoDatetimeTypeDefault::class)]
#[Group('tripal-chado-field')]
class ChadoDatetimeTypeDefaultTest extends UnitTestCase {

  /**
   * TripalTypes() returns null when base_table is empty.
   */
  public function testTripalTypesReturnsNullWithoutBaseTable(): void {
    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getTargetEntityTypeId')->willReturn('tripal_entity');
    $field_def->method('getSetting')
      ->with('storage_plugin_settings')
      ->willReturn(['base_table' => '', 'base_column' => '']);

    $result = ChadoDatetimeTypeDefault::tripalTypes($field_def);

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

    $result = ChadoDatetimeTypeDefault::discover($bundle, 'chado_datetime_type_default', [], []);

    $this->assertSame([], $result);
  }

  /**
   * GenerateSampleValue() returns the expected array structure.
   *
   * Record_id is always 0 (no Chado record exists yet for a sample), and
   * value is a Y-m-d H:i:s timestamp string in the range [epoch, now].
   */
  public function testGenerateSampleValue(): void {
    $field_def = $this->createMock(FieldDefinitionInterface::class);

    $result = ChadoDatetimeTypeDefault::generateSampleValue($field_def);
    $after = time();

    $this->assertIsArray($result);
    $this->assertArrayHasKey('record_id', $result);
    $this->assertArrayHasKey('value', $result);

    $this->assertSame(0, $result['record_id']);

    $dt = \DateTime::createFromFormat('Y-m-d H:i:s', $result['value']);
    $this->assertNotFalse($dt, 'value should parse as Y-m-d H:i:s');

    $ts = $dt->getTimestamp();
    $this->assertGreaterThanOrEqual(0, $ts, 'timestamp should be >= epoch');
    $this->assertLessThanOrEqual($after, $ts, 'timestamp should not be in the future');
  }

  /**
   * IsCompatible() returns true when the base table has a timestamp column.
   */
  public function testIsCompatibleReturnsTrueWhenTimestampColumnExists(): void {
    $this->setUpChadoContainer([
      'fields' => [
        'name'        => ['pgsql_type' => 'character varying'],
        'timeexecuted' => ['pgsql_type' => 'timestamp without time zone'],
      ],
    ]);

    $entity_type = $this->createMock(TripalEntityType::class);
    $entity_type->method('getThirdPartySetting')
      ->with('tripal', 'chado_base_table')
      ->willReturn('analysis');

    $field = (new \ReflectionClass(ChadoDatetimeTypeDefault::class))
      ->newInstanceWithoutConstructor();

    $this->assertTrue($field->isCompatible($entity_type));
  }

  /**
   * IsCompatible() returns false when the base table has no timestamp column.
   */
  public function testIsCompatibleReturnsFalseWhenNoTimestampColumn(): void {
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

    $field = (new \ReflectionClass(ChadoDatetimeTypeDefault::class))
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
