<?php
namespace Drupal\Tests\tripal\Kernel;

use Drupal\KernelTests\KernelTestBase;
use Drupal\Tests\tripal\Traits\TripalTestTrait;

/**
 * This is a base class for Tripal Kernel tests.
 *
 * It provides helper methods to create various Tripal-focused objects
 * during testing like Tripal content types, Tripal Content, and Tripal Terms.
 *
 * @group Tripal
 */
abstract class TripalTestKernelBase extends KernelTestBase {

  use TripalTestTrait;

  protected static $modules = ['tripal'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
  }

}
