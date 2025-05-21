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

  private string $tempfile;

  /**
   * @var string $mock_error
   *   The most recent error message from the mocked tripal logger
   */
  protected string $mock_error = '';

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a mocked logger so we can access error messages from the Tripal logger
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function($message, $context, $options) {
          $this->mock_error = 'ERROR: ' . str_replace(array_keys($context), $context, $message);
          return NULL;
        });
    $container->set('tripal.logger', $mock_logger);

    // Create a path to a temporary local file
    $fs_service = \Drupal::service('file_system');
    $this->tempfile = $fs_service->tempnam("temporary://", 'file_retriever_test_');
    $this->tempfile = $fs_service->realpath($this->tempfile);
  }

  /**
   * {@inheritdoc}
   */
  protected function tearDown(): void {
    parent::tearDown();

    // Remove the local temporary file if a test failed
    if ($this->tempfile and file_exists($this->tempfile)) {
      unlink($this->tempfile);
    }
  }

  /**
   * Tests the Tripal File Retrieval service.
   */
  public function testTripalFileRetriever() {
    // Get the service to be tested
    $retrieval_service = \Drupal::service('tripal.fileretriever');

    // Tests retrieveFileContents()

    // Test retrieval of non-existant local file
    $url = DRUPAL_ROOT . 'modules/contrib/bogus/NOLICENSE.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFileContents($url);
    $this->assertStringContainsString('Local file', $this->mock_error,
      'Did not generate an error for an invalid local file');
    $this->assertNull($content,
      'Did not receive NULL for invalid local file');

    // Test retrieval of valid local file
    $url = DRUPAL_ROOT . '/modules/contrib/tripal/LICENSE.txt';
    $content = $retrieval_service->retrieveFileContents($url);
    $this->assertNotNull($content,
      'Received NULL for valid local file');
    $this->assertGreaterThan(100, strlen($content),
      'Received truncated content for valid local file');

    // Test retrieval of non-existent URL (invalid host)
    $url = 'https://vmasiufekxlkajfd.org/fail.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFileContents($url);
    $this->assertStringContainsString('ERROR: Invalid hostname', $this->mock_error,
      'Did not generate an error for an invalid host name');
    $this->assertNull($content,
      'Did not receive NULL for nonexistent URL');

    // Test retrieval of non-existent URL (valid host, invalid file)
    $url = 'https://github.com/vmasiufekxlkajfd.txt';
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFileContents($url);
    $this->assertStringContainsString('ERROR: Invalid file', $this->mock_error,
      'Did not generate an error for a valid host with invalid file name');
    $this->assertNull($content,
      'Did not receive NULL for nonexistent URL');

    // Test retrieval of existing URL
    $url = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';
    $content = $retrieval_service->retrieveFileContents($url);
    // Because this test accesses an external site, and it could be unavailable,
    // do not throw an error for this test
    if (is_null($content)) {
      $this->markTestSkipped('Received NULL for valid URL. Remote host might be down, so skipping this test');
    }
    $this->assertGreaterThan(100, strlen($content),
      'Received truncated content for valid URL');

    // Tests downloadFile()

    // Test copy to local file from non-existant local file
    $url = DRUPAL_ROOT . 'modules/contrib/bogus/NOLICENSE.txt';
    $this->mock_error = '';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    $this->assertFalse($status,
      'Local file was incorrectly created, status not FALSE');
    $this->assertStringContainsString('Local file', $this->mock_error,
      'Did not generate an error for an invalid local file');

    // Test copy to local file from another local file
    $url = DRUPAL_ROOT . '/modules/contrib/tripal/LICENSE.txt';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    $this->assertTrue($status,
      'Local file not created, status not TRUE');
    $this->assertTrue(file_exists($this->tempfile),
      'Local file not created, file not created');
    $this->assertGreaterThan(100, filesize($this->tempfile),
      'Local file is too small');
    unlink($this->tempfile);

    // Test copy to local file of non-existent URL (invalid host)
    $url = 'https://vmasiufekxlkajfd.org/fail.txt';
    $this->mock_error = '';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    $this->assertFalse($status,
      'Local file was incorrectly created, status not FALSE');
    $this->assertStringContainsString('ERROR: Invalid hostname', $this->mock_error,
      'Did not generate an error for an invalid host name');

    // Test copy to local file of non-existent URL (valid host, invalid file)
    $url = 'https://github.com/vmasiufekxlkajfd.txt';
    $this->mock_error = '';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    $this->assertFalse($status,
      'Local file was incorrectly created, status not FALSE');
    $this->assertStringContainsString('ERROR: Invalid file', $this->mock_error,
      'Did not generate an error for a valid host with invalid file name');

    // Test copy to local file for valid URL
    $url = 'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    // Because this test accesses an external site, and it could be unavailable,
    // do not throw an error for this test
    if ($status === FALSE) {
      $this->markTestSkipped('Received FALSE for valid URL. Remote host might be down, so skipping this test');
    }
    $this->assertTrue(file_exists($this->tempfile),
      'Local file not created, file not created');
    $this->assertGreaterThan(100, filesize($this->tempfile),
      'Local file is too small');
    unlink($this->tempfile);
  }

}
