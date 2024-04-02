<?php

namespace Drupal\Tests\tripal_biodb\Kernel\Task;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal_biodb\Task\BioTaskBase;
use Drupal\Tests\tripal\Kernel\TripalDBX\Subclass\TripalDbxConnectionFake;

/**
 * Tests for tasks.
 *
 * @coversDefaultClass \Drupal\tripal_biodb\Task\BioTaskBase
 *
 * @group Tripal
 * @group Tripal BioDb
 * @group Tripal BioDb Task
 */
class BioTaskBaseTest extends TripalTestKernelBase {

  /**
   * Test members.
   *
   * "pro*" members are prophesize objects while their "non-pro*" equivqlent are
   * the revealed objects.
   */
  protected $proConfigFactory;
  protected $configFactory;
  protected $proConfig;
  protected $config;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();
    // Register Tripal DBX service.
    $this->enableModules(['tripal']);
    $this->enableModules(['tripal_biodb']);
  }

  /**
   * Setup a mock version of the abstract Bi9oTaskBase for testing.
   */
  public function getMock() {

    // Create a mock for the abstract class
    // but specify not to run the contructor + mention this is an abstract class.
    $tmock = $this->getMockBuilder(\Drupal\tripal_biodb\Task\BioTaskBase::class)
      ->setMethods(['getTripalDbxClass'])
      ->getMockForAbstractClass();
    // Ensure when getTripalDbxClass() is asked for the connection class, it returns our fake class.
    $tmock
      ->expects($this->any())
      ->method('getTripalDbxClass')
      ->with('Connection')
      ->willReturn('\Drupal\Tests\tripal\Kernel\TripalDBX\Subclass\TripalDbxConnectionFake');

    return $tmock;
  }

  /**
   * Tests constructor: check constructor calls.
   *
   * @cover ::__construct
   * @cover ::initId
   * @cover ::getId
   * @cover ::getLogger
   */
  public function testBioTaskBaseConstructor() {

    $tmock = $this->getMock();

    // Because we did not disable the constructor in the above mock,
    // it should have been run when we created the mock.
    // Since the variables set by the constructor are protected properties,
    // we cannot test them directly. As such, we will use PHP closures to
    // access these properties for testing.
    //  -- Create a variable to store a copy of this test object for use within the closure.
    $that = $this;
    //  -- Create a closure (i.e. a function tied to a variable) that does not need any parameters.
    //     Within this function we will want all of the assertions we will use to test the private methods.
    //     Also, $this within the function will actually be the plugin object that you bind later (mind blown).
    $assertConstructorClosure = function ()  use ($that){
      $that->assertIsObject($this->connection,
        "The connection object was not set properly by our constructor.");
      $that->assertIsObject($this->logger,
        "The logger object was not set properly by our constructor.");
      $that->assertIsObject($this->locker,
        "The locker object was not set properly by our constructor.");
      $that->assertIsObject($this->state,
        "The state object was not set properly by our constructor.");
      $that->assertIsString($this->id,
        "The id was not set properly by our constructor.");

      $that->assertEquals($this->id, $this->getId(),
        "Retrieving the ID did not return the id set in the object.");
      $that->assertEquals($this->logger, $this->getLogger(),
        "Retrieving the logger did not return the logger set in the object.");
    };
    //  -- Now, bind our assertion closure to the $plugin object. This is what makes the plugin available
    //     inside the function.
    $doAssertConstructorClosure = $assertConstructorClosure->bindTo($tmock, get_class($tmock));
    //  -- Finally, call our bound closure function to run the assertions on our plugin.
    $doAssertConstructorClosure();
  }

  /**
   * Tests setting + preparing input/output schema.
   *
   * @cover ::setParameters
   * @cover ::prepareSchemas
   */
  public function testBioTaskBaseParameters() {

    $tmock = $this->getMock();

    // Check that input/output schema can be set and prepared.
    $parameters = [
      'input_schemas' => ['insch'],
      'output_schemas' => ['outsch'],
    ];
    $tmock->setParameters($parameters);
    $that = $this;
    $assertParametersClosure = function ()  use ($that, $parameters){
      $that->assertIsArray($this->parameters, "Parameters should be an array.");
      $that->assertArrayHasKey('input_schemas', $this->parameters,
        "Input schema key not set in parameters array.");
      $that->assertArrayHasKey('output_schemas', $this->parameters,
        "Output schema key not set in parameters array.");
      $that->assertEquals($parameters, $this->parameters,
        "The parameters we passed in should match those set withing the object.");

      // Check that the prepareSchema function was run for both input and output
      // schema and that TripalDbxConnection were successfully initialized.
      $that->assertCount(1, $this->inputSchemas,
        "We expect there should be one input schema based on the parameters passed in.");
      $that->assertContainsOnlyInstancesOf(
        \Drupal\tripal\TripalDBX\TripalDbxConnection::class,
        $this->inputSchemas,
        "All input schema should be prepared as TripalDBXConnection objects but are not."
      );
      $that->assertCount(1, $this->outputSchemas,
        "We expect there should be one output schema based on the parameters passed in.");
      $that->assertContainsOnlyInstancesOf(
        \Drupal\tripal\TripalDBX\TripalDbxConnection::class,
        $this->outputSchemas,
        "All output schema should be prepared as TripalDBXConnection objects but are not."
      );
    };
    $doAssertParametersClosure = $assertParametersClosure->bindTo($tmock, get_class($tmock));
    $doAssertParametersClosure();
  }
}
