<?php

namespace Drupal\Tests\tripal_chado\Functional\Services;

use Drupal\Tests\UnitTestCase;

use Drupal\Core\Database\Connection;
use Drupal\Tests\Core\Database\Stub\StubConnection;
use Drupal\Tests\Core\Database\Stub\StubPDO;
use Drupal\Core\DependencyInjection\ContainerBuilder;

// use Drupal\Core\Database\Driver\pgsql\Connection;
use Drupal\tripal_chado\Services\ChadoCloner;
use Drupal\tripal_chado\api\ChadoSchema;

/**
 * Tests for Chado cloner.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Services\ChadoCloner
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Services
 */
class ChadoClonerTest extends UnitTestCase {

  /**
   * PDO driver.
   *
   * @var Drupal\Tests\Core\Database\Stub\StubPDO
   */
  protected $mockPdo;

  /**
   * Database connection.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $mockDatabase;

  /**
   * Logger.
   *
   * @var \Psr\Log\LoggerInterface
   */
  protected $mockLogger;

  /**
   * Database persitent shared lock backend to test.
   *
   * @var \Drupal\tripal_chado\Lock\SharedLockBackendInterface
   */
  protected $mockLocker;
  
  /**
   *
   */
  protected $stringTranslation;

  /**
   * Tests setup method.
   */
  protected function setUp() :void { 
    parent::setUp(); 

    ChadoSchema::testMode(TRUE);
    // Method 1
    // $this->mockPdo = $this->getMockBuilder(Drupal\Tests\Core\Database\Stub\StubPDO::class)
    //   ->disableOriginalConstructor() 
    //   ->getMock()
    // ;
    // $this->mockDatabase = new Connection($this->mockPdo, []);

    // Method 2
    // $this->mockDatabase = $this->prophesize(Connection::class);
    // $this->mockDatabase
    //   ->escapeField($field_name)
    //   ->will(function ($args) {
    //   return preg_replace('/[^A-Za-z0-9_.]+/', '', $args[0]);
    // });

    // Method 3
    $options = [
        'database'  => 'mock_database',
        'username'  => 'toto',
        'password'  => '1234',
        'prefix'    => ['default' => '',],
        'host'      => '127.0.0.1',
        'port'      => '5432',
        'namespace' => 'Drupal\\Core\\Database\\Driver\\pgsql',
        'driver'    => 'pgsql',
    ];
    $options['namespace'] = 'Drupal\\Driver\\Database\\mock';
    $this->mockPdo = $this->createMock(StubPDO::class);
    $this->mockDatabase = new StubConnection($this->mockPdo, $options);
    // $this->mockDatabase = $this->createMock(StubConnection::class);
    // $this->mockDatabase
    //   ->expects($this->any()) 
    //   ->method('getConnectionOptions') 
    //   ->willReturn([
    //     'database'  => 'mock_database',
    //     'username'  => 'toto',
    //     'password'  => '1234',
    //     'prefix'    => ['default' => '',],
    //     'host'      => '127.0.0.1',
    //     'port'      => '5432',
    //     'namespace' => 'Drupal\\Core\\Database\\Driver\\pgsql',
    //     'driver'    => 'pgsql',
    //   ]);

    $this->mockLogger = $this->getMockBuilder(\Psr\Log\LoggerInterface::class)
      ->disableOriginalConstructor() 
      ->getMock()
    ;
    $this->mockLocker = $this->getMockBuilder(\Drupal\tripal_chado\Lock\SharedLockBackendInterface::class)
      ->disableOriginalConstructor() 
      ->getMock()
    ;

    // Mock the container. 
    $container = new ContainerBuilder(); 
    $container->set('string_translation', $this->getStringTranslationStub());
    $container->set('database', $this->mockDatabase);
    \Drupal::setContainer($container); 
  }

  /**
   * Provides parameters.
   */
  public function parametersProvider() {
    $drupal_schema = 'chado'; // ChadoSchema::getDrupalSchema();
    $target_schema = ChadoSchema::TEST_SCHEMA_NAME . '_cloner' . mt_rand(10000000, 99999999);
    return [
      [$drupal_schema, $target_schema, NULL, '', 'Valid parameters'],
    ];
  }

  /**
   * Test with valid parameters.
   *
   * @dataProvider parametersProvider
   *
   * @covers ::validateParameters
   */
  public function testValidateParametersException(
    $source_schema,
    $target_schema,
    $exception_class,
    $exception_regexp,
    $test_description
  ) {

    // Setup parameters.
    $parameters = [
      'input_schemas'  => [$source_schema],
      'output_schemas' => [$target_schema], 
    ];
    $cloner = new ChadoCloner(
      $parameters,
      $this->mockDatabase,
      $this->mockLogger,
      $this->mockLocker
    );

    // $this->expectException($exception_class);
    // $this->expectExceptionMessageRegExp($exception_regexp);
    $cloner->validateParameters();
    $this->assertTrue(TRUE, $test_description);
  }

}
