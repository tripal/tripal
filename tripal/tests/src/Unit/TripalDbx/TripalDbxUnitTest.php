<?php

namespace Drupal\Tests\tripal\Unit\TripalDBX;

use Drupal\Tests\UnitTestCase;
use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\tripal\TripalDBX\TripalDbx;
use Prophecy\Argument;

/**
 * Tests for Tripal Dbx service.
 *
 * @coversDefaultClass \Drupal\tripal\TripalDBX\TripalDbx
 *
 * @group Tripal
 * @group Tripal DBX
 * @group Tripal DBX Service
 */
class TripalDbxUnitTest extends UnitTestCase {

  /**
   * Test members.
   *
   * "pro*" members are prophesize objects while their "non-pro*" equivqlent are
   * the revealed objects.
   */
  protected $proContainer;
  protected $container;
  protected $proConnection;
  protected $connection;
  protected $proConfig;
  protected $config;
  protected $proConfigFactory;
  protected $configFactory;
  protected $proModuleHandler;
  protected $moduleHandler;
  protected $proTripalDbxDb;
  protected $tripalDbxDb;
  protected $tripaldbx;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Mock Drupal connection (\Drupal::database).
    $this->proConnection = $this->prophesize(\Drupal\Core\Database\Connection::class);
    $this->connection = $this->proConnection->reveal();

    // Mock the Config object, but methods will be mocked in the test class.
    $this->proConfig = $this->prophesize(\Drupal\Core\Config\ImmutableConfig::class);
    $this->config = $this->proConfig->reveal();

    // Mock the ConfigFactory service.
    $this->proConfigFactory = $this->prophesize(\Drupal\Core\Config\ConfigFactory::class);
    $this->proConfigFactory->get('tripaldbx.settings')->willReturn($this->config);
    $this->configFactory = $this->proConfigFactory->reveal();

    // Mock the module handler.
    $this->proModuleHandler = $this->prophesize(\Drupal\Core\Extension\ModuleHandlerInterface::class);
    $this->moduleHandler = $this->proModuleHandler->reveal();

    // Mock the TripalDbx static functions when needed.
    $is_invalid_schema_name = function($args) {
      $tripaldbx = new TripalDbx();
      if (1 == count($args)) {
        return $tripaldbx->isInvalidSchemaName($args[0]);
      }
      elseif (2 == count($args)) {
        return $tripaldbx->isInvalidSchemaName($args[0], $args[1]);
      }
      else {
        return $tripaldbx->isInvalidSchemaName($args[0], $args[1], $args[2]);
      }
    };
    $this->proTripalDbxDb = $this->prophesize(\Drupal\tripal\TripalDBX\TripalDbx::class);
    // $this->proTripalDbxDb->isInvalidSchemaName('invalid')->willReturn('Invalid schema name.');
    // $this->proTripalDbxDb->isInvalidSchemaName('valid')->willReturn('');
    $this->proTripalDbxDb->isInvalidSchemaName(Argument::cetera())->will($is_invalid_schema_name);
    $this->tripalDbxDb = $this->proTripalDbxDb->reveal();

    // Container initialization.
    $this->container = new ContainerBuilder();
    $this->container->set('database', $this->connection);
    $this->container->set('config.factory', $this->configFactory);
    $this->container->set('module_handler', $this->moduleHandler);
    $this->container->set('tripal.dbx', $this->tripalDbxDb);
    \Drupal::setContainer($this->container);

    // Hack to clear TripalDbx cache on each run.
    $clear = function() {
      TripalDbx::$drupalSchema
      = TripalDbx::$reservedSchemaPatterns
      = NULL;
    };
    $clear->call(new TripalDbx());
  }

  /**
   * Tests getDrupalSchemaName() method cache on a non-public schema.
   *
   * We tests 2 consecutive calls to ::getDrupalSchemaName but only the first
   * one will cache the result from Connection::getConnectionOptions so
   * the returned values of the second one should remaine the same and be
   * faster.
   *
   * @cover ::getDrupalSchemaName
   */
  public function testGetDrupalSchemaNameOtherAndCache() {

    $this->proConnection->getConnectionOptions()->willReturn([
      'prefix' => [
        // We use a dot to specify Drupal is in a non-public schema.
        // This first use case is simpler to test for cache.
        'default' => 'other.',
      ],
    ]);

    $tripaldbx = new TripalDbx();
    // First call.
    $start_time = hrtime(true);
    $drupal_schema = $tripaldbx->getDrupalSchemaName();
    // Non-public, and in our case 'other'.
    $this->assertEquals('other', $drupal_schema, 'Got a non-public schema name.');
    $end_time = hrtime(true);

    $next_start_time = hrtime(true);
    $drupal_schema = $tripaldbx->getDrupalSchemaName();
    $this->assertEquals('other', $drupal_schema, 'Got the same schema name from cache.');
    $next_end_time = hrtime(true);

    // Performances.
    $first_round = $end_time - $start_time;
    $second_round = $next_end_time - $next_start_time;
    $this->assertLessThan($first_round, $second_round, 'Cache is faster.');
  }

  /**
   * Tests getDrupalSchemaName() method on a default schema name.
   *
   * @cover ::getDrupalSchemaName
   */
  public function testGetDrupalSchemaNameDefault() {

    // Mock a statement.
    $prophecy = $this->prophesize(\Drupal\Core\Database\StatementInterface::class);
    $prophecy->fetch()->willReturn((object)['schema' => 'pub']);
    $statement = $prophecy->reveal();

    $this->proConnection->getConnectionOptions()->willReturn([
      'database' => 'drupal_db',
    ]);
    $this->proConnection->query(Argument::cetera())->willReturn($statement);

    $tripaldbx = new TripalDbx();

    // Get Drupal schema.
    $drupal_schema = $tripaldbx->getDrupalSchemaName();
    $this->assertEquals('pub', $drupal_schema, 'Got expected schema name.');
  }

  /**
   * Tests getDrupalSchemaName() method with a failure to get Drupal schema.
   *
   * @cover ::getDrupalSchemaName
   */
  public function testGetDrupalSchemaNameFailure() {

    // Mock a statement.
    $prophecy = $this->prophesize(\Drupal\Core\Database\StatementInterface::class);
    $prophecy->fetch()->willReturn(NULL);
    $statement = $prophecy->reveal();

    $this->proConnection->getConnectionOptions()->willReturn([
      'database' => 'drupal_db',
    ]);
    $this->proConnection->query(Argument::cetera())->willReturn($statement);

    $tripaldbx = new TripalDbx();

    // Try to get Drupal schema.
    $drupal_schema = $tripaldbx->getDrupalSchemaName();
    $this->assertEmpty($drupal_schema, "We should not get a Drupal schema here.");
  }

  /**
   * Schema name provider.
   */
  public function schemaNamesProvider() {
    return [
      [
        'nameok',
        '',
        NULL,
        NULL,
        'Valid name.',
      ],
      [
        'name_ok',
        '',
        NULL,
        NULL,
        'Valid name with underscore.',
      ],
      [
        'voilà_ok',
        '',
        NULL,
        NULL,
        'Valid name with diacritical marks.',
      ],
      [
        'Name_Not_Ok',
        '',
        NULL,
        NULL,
        'Valid name with ignored capital letters.',
      ],
      [
        'toolongnamenotoktoolongnamenotoktoolongnamenotoktoolongnamenotok',
        'too long',
        NULL,
        NULL,
        'Invalid name too long.',
      ],
      [
        'dollar$_not_ok',
        'must not',
        NULL,
        NULL,
        'Forbidden name with dollar sign.',
      ],
      [
        '8namenotok',
        'must not',
        NULL,
        NULL,
        'Invalid name starting by a number.',
      ],
      [
        'pg_name_not_ok',
        'must not',
        NULL,
        NULL,
        'Invalid name starting with pg_.',
      ],
      [
        'test*_not_ok',
        'must not',
        NULL,
        NULL,
        'Invalid name containing special character star.',
      ],
      [
        'test@not_ok',
        'must not',
        NULL,
        NULL,
        'Invalid name containing arobase.',
      ],
      [
        'test-not-ok',
        'must not',
        NULL,
        NULL,
        'Invalid name containing dash.',
      ],
      [
        'test.not_ok',
        'must not',
        NULL,
        NULL,
        'Invalid name containing dot.',
      ],
      [
        "'test_not_ok'",
        'must not',
        NULL,
        NULL,
        'Invalid name containing quotes.',
      ],
      [
        '_test_ok',
        '',
        NULL,
        NULL,
        'Valid name starting with prefix "_test" which has not been reserved.',
      ],
      [
        '_test_not_ok',
        'reserved',
        ['_test*' => 'reserved prefix',],
        NULL,
        'Invalid name starting with prefix "_test" which has been reserved.',
      ],
      [
        '_other_not_ok',
        'reserved',
        ['_other.*' => 'reserved prefix',],
        NULL,
        'Invalid name starting with prefix "_other" which has been reserved.',
      ],
      [
        '_chado_test_not_ok',
        'reserved',
        ['_test*' => 'reserved prefix', '_chado*' => 'second reserved prefix',],
        NULL,
        'Invalid name starting with prefix "_chado" which has been also reserved.',
      ],
      [
        'to2foo_ok',
        '',
        ['.*(to)+\d+foo' => 'regex pattern reservation',],
        NULL,
        'Valid name not reserved by a pattern.',
      ],
      [
        'to0foo_not_ok',
        'reserv',
        ['.*(to)+\d+foo_not_ok' => 'regex pattern reservation',],
        NULL,
        'Invalid name reserved by a pattern.',
      ],
      [
        'barToto123foo_not_ok',
        'reserv',
        ['.*(to)+\d+foo_not_ok' => 'regex pattern reservation',],
        NULL,
        'Invalid name reserved by a pattern.',
      ],
      [
        'to2foo_ok',
        'reserv',
        ['.*(to)+\d+foo' => 'regex pattern reservation',],
        ['to\dfoo_.*' => 'added regex pattern reservation',],
        'Invalid name reserved by a pattern added by ::reserveSchemaPattern.',
      ],
      [
        'to0foo_not_ok',
        '',
        ['.*(to)+\d+foo_not_ok' => 'regex pattern reservation',],
        ['.*(to)+\d+foo_not_ok' => FALSE,],
        'Valid unreserved name.',
      ],
      [
        'to2foo_not_ok',
        'special xyz word',
        ['.*(to)+\d+foo_not_ok' => 'for something',],
        ['.*(to)+\d+foo_not_ok' => 'special xyz word',],
        'Invalid name reserved by a pattern with new reservation value.',
      ],
    ];
  }

  /**
   * Tests isInvalidSchemaName() method.
   *
   * @dataProvider schemaNamesProvider
   *
   * @cover ::isInvalidSchemaName
   */
  public function testIsInvalidSchemaName(
    $schema_name,
    $expected,
    $reserved,
    $alter,
    $message
  ) {
    $reserved = $reserved ?? [];
    $alter = $alter ?? [];
    $this->proConfig->get('reserved_schema_patterns')->willReturn($reserved);
    $tripaldbx = new TripalDbx();

    if (!empty($alter)) {
      foreach ($alter as $regex => $desc) {
        if (FALSE === $desc) {
          $tripaldbx->freeSchemaPattern($regex);
        }
        else {
          $tripaldbx->reserveSchemaPattern($regex, $desc);
        }
      }
    }

    $result = $tripaldbx->isInvalidSchemaName($schema_name);
    if (empty($expected)) {
      $this->assertEmpty($result, $message);
    }
    else {
      $this->assertStringContainsStringIgnoringCase($expected, $result, $message);
    }
  }

  /**
   * Tests schema pattern reservation system.
   *
   * @cover ::reserveSchemaPattern
   * @cover ::getReservedSchemaPattern
   * @cover ::freeSchemaPattern
   */
  public function testReservedSchemaPattern() {
    $tripaldbx = new TripalDbx();
    // No default reservation loaded in test environment.
    $result = $tripaldbx->isInvalidSchemaName('public');
    $this->assertEmpty($result, 'Public schema not is reserved yet.');

    // Reserve public schema.
    $tripaldbx->reserveSchemaPattern('public', 'public schema is reserved');
    $tripaldbx->reserveSchemaPattern('myschema*', 'private reservation');

    $patterns = $tripaldbx->getReservedSchemaPattern();
    $this->assertEquals(
      [
        'public' => 'public schema is reserved',
        'myschema*' => 'private reservation',
      ],
      $patterns,
      'All reserved well.'
    );

    $result = $tripaldbx->isInvalidSchemaName('public');
    $this->assertStringContainsStringIgnoringCase('public schema is reserved', $result, 'Public schema is reserved.');
    $result = $tripaldbx->isInvalidSchemaName('myschema_abc');
    $this->assertStringContainsStringIgnoringCase('private reservation', $result, 'Private schema is reserved.');

    // Should not change a thing.
    $result = $tripaldbx->freeSchemaPattern('myschema_abc');
    $this->assertEmpty($result, 'Nothing freed from reservation.');
    $patterns = $tripaldbx->getReservedSchemaPattern();
    $this->assertEquals(
      [
        'public' => 'public schema is reserved',
        'myschema*' => 'private reservation',
      ],
      $patterns,
      'All reserved well.'
    );

    // Release public reservation.
    $result = $tripaldbx->freeSchemaPattern('public');
    $this->assertEquals(
      ['public' => 'public schema is reserved',],
      $result,
      'Public schema not reserved.'
    );
    $patterns = $tripaldbx->getReservedSchemaPattern();
    $this->assertEquals(
      ['myschema*' => 'private reservation',],
      $patterns,
      'All reserved well.'
    );
    $result = $tripaldbx->isInvalidSchemaName('public');
    $this->assertEmpty($result, 'Public schema not is reserved anymore.');

    // Add more reservations.
    $tripaldbx->reserveSchemaPattern('public', 'public schema is reserved again');
    $tripaldbx->reserveSchemaPattern('myschema*', 'changed private reservation');
    $tripaldbx->reserveSchemaPattern('.*schema_\w\w\w', 'pattern 2');
    $tripaldbx->reserveSchemaPattern('myschema_\w\w\wxyz', 'pattern 3');
    $patterns = $tripaldbx->getReservedSchemaPattern();
    $this->assertEquals(
      [
        'public' => 'public schema is reserved again',
        'myschema*' => 'changed private reservation',
        '.*schema_\w\w\w' => 'pattern 2',
        'myschema_\w\w\wxyz' => 'pattern 3',
      ],
      $patterns,
      'All reserved well again.'
    );

    // Free private reservation.
    $result = $tripaldbx->freeSchemaPattern('myschema_abc', TRUE);
    $this->assertEquals(
      [
        'myschema*' => 'changed private reservation',
        '.*schema_\w\w\w' => 'pattern 2',
      ],
      $result,
      'Removed 2 reservations.'
    );
    $patterns = $tripaldbx->getReservedSchemaPattern();
    $this->assertEquals(
      [
        'public' => 'public schema is reserved again',
        'myschema_\w\w\wxyz' => 'pattern 3',
      ],
      $patterns,
      'Kept 2 reservations.'
    );
  }

  /**
   * Tests schemaExists() method with invalid names.
   *
   * @cover ::schemaExists
   */
  public function testSchemaExistsInvalid() {
    $tripaldbx = new TripalDbx();
    $exists = $tripaldbx->schemaExists('0invalid');
    $this->assertFalse($exists, 'Invalid schema name');
  }

  /**
   * Tests schemaExists() method.
   *
   * @cover ::schemaExists
   */
  public function testSchemaExistsValid() {

    // Mock a statement.
    $prophecy = $this->prophesize(\Drupal\Core\Database\StatementInterface::class);
    $prophecy->fetchField()->willReturn(TRUE, FALSE);
    $statement = $prophecy->reveal();

    $prophecy = $this->prophesize(\Drupal\Core\Database\Connection::class);
    $prophecy->query(Argument::cetera())->willReturn($statement);
    $connection = $prophecy->reveal();

    $this->container->set('database', $connection);

    $tripaldbx = new TripalDbx();
    $exists = $tripaldbx->schemaExists('valid');
    $this->assertTrue($exists, 'Schema exists.');

    $exists = $tripaldbx->schemaExists('unexisting');
    $this->assertFalse($exists, 'Schema does not exist.');
  }

  /**
   * Tests schemaExists() method.
   *
   * @cover ::parseTableDdl
   */
  public function testParseTableDdl() {
    // Use regular service.
    $this->container->set('tripal.dbx', new \Drupal\tripal\TripalDBX\TripalDbx());
    // Get fixture data.
    $ddl = file_get_contents( __DIR__ . '/../../../fixtures/feature_ddl.sql');
    // Import $feature_basic.
    include  __DIR__ . '/../../../fixtures/feature_parsed_basic.php';
    // Import $feature_drupal.
    include  __DIR__ . '/../../../fixtures/feature_parsed_drupal.php';

    $tripaldbx = new TripalDbx();
    $parsed_ddl = $tripaldbx->parseTableDdl($ddl);
    $this->assertEquals($feature_basic, $parsed_ddl, 'DDL parsed basic.');

    $parsed_ddl = $tripaldbx->parseTableDdlToDrupal($ddl);
    $this->assertEquals($feature_drupal, $parsed_ddl, 'DDL parsed Drupal.');
  }

}
