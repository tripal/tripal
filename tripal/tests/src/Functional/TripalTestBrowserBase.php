<?php
namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\Tests\tripal\Traits\TripalTestTrait;

/**
 * This is a base class for Tripal tests that need a full Drupal install.
 *
 * It provides helper methods to create various Tripal-focused objects
 * during testing like Tripal content types, Tripal Content, and Tripal Terms.
 *
 * @group Tripal
 */
abstract class TripalTestBrowserBase extends BrowserTestBase {

  use TripalTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

  /**
   * {@inheritdoc}
   */
  protected $defaultTheme = 'stark';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();
    // Not ready to add this to all tests just yet.
    // \Drupal::state()->set('is_a_test_environment', TRUE);
  }

}
