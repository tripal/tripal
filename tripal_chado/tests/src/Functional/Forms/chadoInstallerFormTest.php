<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;

/**
 * Simple test to ensure that main page loads with module enabled.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Forms
 */
class chadoInstallerFormTest extends BrowserTestBase {

  protected $defaultTheme = 'stable';

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * A user with permission to administer site configuration.
   *
   * @var \Drupal\user\UserInterface
   */
  protected $user;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();
    $this->user = $this->drupalCreateUser([
      'administer site configuration',
      'administer tripal'
    ]);
    $this->drupalLogin($this->user);
  }

  /**
   * Tests that the Chado Installer form loads as expected.
   *
   * @group form
   * @group chado-install
   */
  public function testLoadInstallerForm() {
    $assert = $this->assertSession();

    // Check that the page opens.
    $this->drupalGet(Url::fromRoute('tripal_chado.chado_install_form'));
    $assert->statusCodeEquals(200);

    // Check that the page contains the header.
    $assert->pageTextContains('Chado Installation');

    // Check that the form can set the action and the schema name.
    $assert->fieldExists('Installation/Upgrade Action');
    $assert->fieldExists('Chado Schema Name');

    // Now check that we can submit the form.
    $values = [
      'action_to_do' => 'Install Chado v1.3',
      'schema_name' => uniqid(),
    ];
    // Submit the form.
    $this->submitForm($values, 'Submit');

    // Now there should be a message mentioning the schema to be installed.
    $assert->responseContains($values['schema_name']);
  }

}
