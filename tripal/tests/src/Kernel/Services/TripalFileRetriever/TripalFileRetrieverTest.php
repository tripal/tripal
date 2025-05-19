<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalFileRetriever;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;


/**
 * Tests retrieval of remote or local files.
 *
 * @group Tripal
 * @group Tripal FileRetriever
 */
class TripalFileRetrieverTest extends TripalTestKernelBase {


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal'];

protected string $mock_error = '';

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Grab the container.
    $container = \Drupal::getContainer();

    // Some of our tests will check logged messages that would normally go to
    // php error_log. PHPUnit will throw an exception if anything is added to
    // error_log so we want to mock TripalLogger to ensure all errors saved and
    // not printed to the screen.
    // We only need to mock the error method. Other methods will not be mocked.
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function($message, $context, $options) {
          $this->mock_error = 'ERROR: ' . str_replace(array_keys($context), $context, $message);
          return NULL;
        });
    $container->set('tripal.logger', $mock_logger);
  }

  /**
   * Tests the Tripal File Retrieval service.
   */
  public function testTripalFileRetriever() {
    // Get the service to be tested
    $retrieval_service = \Drupal::service('tripal.fileretriever');

    // Test retrieval of non-existent URL (invalid host)
    $url = 'https://vmasiufekxlkajfd.org/fail.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFile($url);
    $this->assertStringContainsString('ERROR: Invalid hostname', $this->mock_error,
      'Did not generate an error for an invalid host name');
    $this->assertNull($content,
      'Did not receive NULL for nonexistent URL');

    // Test retrieval of non-existent URL (valid host, invalid file)
    $url = 'https://github.com/vmasiufekxlkajfd.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFile($url);
    $this->assertStringContainsString('ERROR: Invalid file', $this->mock_error,
      'Did not generate an error for a valid host with invalid file name');
    $this->assertNull($content,
      'Did not receive NULL for nonexistent URL');

    // Test retrieval of existing URL
    $url = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';
    $content = $retrieval_service->retrieveFile($url);
    $this->assertNotNull($content,
      'Received NULL for valid URL');
    $this->assertGreaterThan(100, strlen($content),
      'Received truncated content for valid URL');

    // Test retrieval of non-existant local file
    $url = DRUPAL_ROOT . 'modules/contrib/bogus/NOLICENSE.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFile($url);
    $this->assertStringContainsString('Local file', $this->mock_error,
      'Did not generate an error for an invalid local file');
    $this->assertNull($content,
      'Did not receive NULL for invalid local file');

    // Test retrieval of valid local file
    $url = DRUPAL_ROOT . '/modules/contrib/tripal/LICENSE.txt';
    $content = $retrieval_service->retrieveFile($url);
    $this->assertNotNull($content,
      'Received NULL for valid local file');
    $this->assertGreaterThan(100, strlen($content),
      'Received truncated content for valid local file');
  }

}
