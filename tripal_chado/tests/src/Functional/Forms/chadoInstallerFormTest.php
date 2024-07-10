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

  protected $defaultTheme = 'stark';

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_biodb', 'tripal_chado'];

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
   * @group chado-install-form
   */
  public function testLoadInstallerForm() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

    // Check that the page opens.
    $page_content = $this->drupalGet(Url::fromRoute('tripal_chado.chado_install_form'));
    $status_code = $session->getStatusCode();
    // @debug print $page_content;
    $this->assertEquals(200, $status_code, "We should be able to access the chado install page.");

    // @todo we will want to test more then just this at some point.
  }

}
