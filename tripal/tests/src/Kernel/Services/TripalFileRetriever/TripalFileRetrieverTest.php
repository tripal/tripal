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
          $this->mock_error .= str_replace(array_keys($context), $context, $message);
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
   * Provide scenarios for testing retrieveFileContents().
   *
   * @return array
   *   A list of scenarios to be tested. Each scenario has the following:
   *   - a URL to be passed to retrieveFileContents().
   *   - an array of options taken by retrieveFileContents().
   *   - an array of expectations where the following keys are expected:
   *     - error_message: part of the expected error message to be logged.
   *       Use FALSE if there should not be an error message.
   *     - has_content: indicates if we expect retrieveFileContents() to return
   *       content where FALSE means we expect NULL and TRUE means > 100 size.
   *     - skip: indicates if we should be more lenient when checking content.
   *       When TRUE we will skip the test when we would otherwise fail.
   */
  public static function provideFiles2Retrieve(): array {
    $scenarios = [];

    // Test retrieval of non-existant local file.
    $scenarios[] = [
      DRUPAL_ROOT . 'modules/contrib/bogus/NOLICENSE.txt',
      [],
      [
        'error_message' => 'Local file',
        'has_content' => FALSE,
        'skip' => FALSE,
      ]
    ];

    // Test retrieval of valid local file.
    $scenarios[] = [
      DRUPAL_ROOT . '/modules/contrib/tripal/LICENSE.txt',
      [],
      [
        'error_message' => FALSE,
        'has_content' => TRUE,
        'skip' => FALSE,
      ]
    ];

    // Test retrieval of non-existent URL (invalid host).
    $scenarios[] = [
      'https://vmasiufekxlkajfd.org/fail.txt',
      [],
      [
        'error_message' => 'Invalid hostname',
        'has_content' => FALSE,
        'skip' => FALSE,
      ]
    ];

    // Test retrieval of non-existent URL (valid host, invalid file)
    $scenarios[] = [
      'https://github.com/vmasiufekxlkajfd.txt',
      [],
      [
        'error_message' => 'Invalid file',
        'has_content' => FALSE,
        'skip' => FALSE,
      ]
    ];

    // Test retrieval of existing URL.
    $scenarios[] = [
      'https://raw.githubusercontent.com/tripal/tripal/4.x/LICENSE.txt',
      [],
      [
        'error_message' => FALSE,
        'has_content' => TRUE,
        'skip' => TRUE,
      ]
    ];

    return $scenarios;
  }

  /**
   * Tests the Tripal File Retrieval service.
   *
   * @dataProvider provideFiles2Retrieve
   */
  public function testTripalFileRetriever(string $url, array $options, array $expectations) {
    // Get the service to be tested
    $retrieval_service = \Drupal::service('tripal.fileretriever');

    // Tests retrieveFileContents()
    $this->mock_error = '';
    $content = $retrieval_service->retrieveFileContents($url);
    // -- Check the error message.
    if ($expectations['error_message'] !== FALSE) {
      $this->assertStringContainsString(
        $expectations['error_message'],
        $this->mock_error,
        'Did not log an error for this scenario when we expected one.'
      );
    }
    else {
      $this->assertEquals('', $this->mock_error,
        "We logged an error when we did not expect one.");
    }
    // -- Skip instead of fail for certain scenarios.
    if (is_null($content) AND $expectations['skip']) {
      $this->markTestSkipped("Received NULL for valid URL. Remote host might be down, so skipping this test.");
    }
    // -- Check the contents.
    if ($expectations['has_content'] == TRUE) {
      $this->assertNotNull($content,
        "Recieved NULL when we expected file retrieval to be successful.");
      $this->assertGreaterThan(100, strlen($content),
        'Received truncated content when retrieving a valid file.'
      );
    }
    else {
      $this->assertNull($content,
        'Did not receive NULL when we expect file retrieval to have failed.');
    }

      /*
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
    $this->assertStringContainsString('Invalid hostname', $this->mock_error,
      'Did not generate an error for an invalid host name');

    // Test copy to local file of non-existent URL (valid host, invalid file)
    $url = 'https://github.com/vmasiufekxlkajfd.txt';
    $this->mock_error = '';
    $status = $retrieval_service->downloadFile($url, $this->tempfile);
    $this->assertFalse($status,
      'Local file was incorrectly created, status not FALSE');
    $this->assertStringContainsString('Invalid file', $this->mock_error,
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
    */
  }

}
