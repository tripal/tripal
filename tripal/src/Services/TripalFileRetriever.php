<?php
namespace Drupal\tripal\Services;

use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use GuzzleHttp\Exception\ConnectException;
use GuzzleHttp\Exception\ClientException;

/**
 * The TripalFileLoader class handles copying files from both
 * remote URLs or local file paths, with resiliance against
 * errors.
 */
class TripalFileRetriever {

  /**
   * The Drupal HTTP service (i.e. guzzle)
   *
   * @var object \GuzzleHttp\ClientInterface $httpClient
   */
  protected $httpClient = NULL;

  /**
   * A logger object.
   *
   * @var TripalLogger $logger
   */
  protected $logger;

  /**
   * Constructor
   */
  public function __construct(\GuzzleHttp\ClientInterface $httpClient, TripalLogger $logger) {
    $this->httpClient = $httpClient;
    $this->logger = $logger;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static (
      $container->get('http_client'),
      $container->get('tripal.logger')
    );
  }

  /**
   * Download the contents of a remote or local file from a specified URL,
   * and returns it in a string variable.
   *
   * @param string $url
   *   The address of the file to download
   * @param int $retries
   *   The number of times to retry the remote download if an error occurs
   * @param array $options
   *   Any options to pass to the HTTP request
   * @return string
   *   The data obtained from the specified url, or NULL if it could not be downloaded
   */
  public function retrieveFileContents(string $url, int $retries = 3, array $options = []): string|null {
    $contents = NULL;

    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {
      while (is_null($contents) && ($retries > 0)) {
        try {
          $response = $this->httpClient->get($url, $options);
          $contents = (string) $response->getBody();
        }
        catch (\Exception $e) {
          $this->handleURLExceptions($retries, $e, $url);
        }
$fail = \Drupal::state()->get('FAIL', '0');if ($fail) { $contents = NULL; $retries = 0; $this->logger->error('Unable to get response from @url: mocked_error', ['@url' => $url]); } //@@@ simulate error
        $retries--;
        if (is_null($contents) && ($retries > 0)) {
          sleep(1);
        }
      }
    }
    // If there was no host in the url, then it is considered a local file
    else {
      if (!file_exists($url)) {
        $this->logger->error('Local file @url does not exist',
            ['@url' => $url]);
      }
      else {
        try {
          $contents = file_get_contents($url);
        }
        catch (\Exception $e) {
          $this->logger->error('Error reading from local file @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
        }
      }
      // file_get_contents() should return FALSE for error, convert to NULL here
      if ($contents === FALSE) {
        $contents = NULL;
      }
    }
    return $contents;
  }

  /**
   * Download the contents of a remote or local file from a specified URL,
   * and saves it to a file in the local filesystem.
   *
   * @param string $url
   *   The address of the file to download
   * @param string $localfile
   *   The path to a local file where data is saed
   * @param int $retries
   *   The number of times to retry the remote download if an error occurs
   * @param array $options
   *   Any options to pass to the HTTP request
   * @return bool
   *   Returns TRUE if successful, FALSE if error.
   */
  public function downloadFile(string $url, string $localfile, int $retries = 3, array $options = []): bool {
    $status = FALSE;

    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {

      $options['sink'] = $localfile;
      $options['stream'] = FALSE;

      while (!$status && ($retries > 0)) {
        try {
          /** @var GuzzleHttp\Psr7\Response **/
          $response = $this->httpClient->get($url, $options);
          $status = TRUE;
        }
        catch (\Exception $e) {
          $this->handleURLExceptions($retries, $e, $url);
        }
$fail = \Drupal::state()->get('FAIL', '0');if ($fail) { $status = FALSE; $retries = 0; $this->logger->error('Unable to get response from @url: mocked_error', ['@url' => $url]); } //@@@ simulate error 10% of the time
        $retries--;
        if (!$status && ($retries > 0)) {
          sleep(1);
        }
      }
    }
    // If there was no host in the url, then it is considered a local file
    else {
      if (!file_exists($url)) {
        $this->logger->error('Local file @url does not exist',
            ['@url' => $url]);
      }
      else {
        try {
          copy($url, $localfile);
          $status = TRUE;
        }
        catch (\Exception $e) {
          $this->logger->error('Error copying @url to @local: @exception',
              ['@url' => $url, '@local' => $localfile, '@exception' => $e->getMessage()]);
        }
      }
    }
    return $status;
  }

  /**
   * Logs various types of exceptions that may occur when downloading
   *
   * @param int &$retries
   *   The number of times to retry the remote download if an error occurs
   * @param \Exception $e
   *   An exception to be handled
   * @param string $url
   *   The URL that caused the exception
   * @return void
   *   Any problems are sent to the logger
   */
  private function handleURLExceptions (int &$retries, \Exception $e, string $url): void {
    if ($e instanceof ConnectException) {
      $this->logger->error('Invalid hostname in URL @url: @exception',
          ['@url' => $url, '@exception' => $e->getMessage()]);
      $retries = 0;
    }
    elseif ($e instanceof ClientException) {
      $this->logger->error('Invalid file in URL @url: @exception',
          ['@url' => $url, '@exception' => $e->getMessage()]);
      $retries = 0;
    }
    elseif ($e instanceof RequestException) {
      if ($retries > 1) {
        $this->logger->error('Unable to get response from @url: @exception. Will retry',
            ['@url' => $url, '@exception' => $e->getMessage()]);
      }
      else {
        $this->logger->error('Unable to get response from @url: @exception',
            ['@url' => $url, '@exception' => $e->getMessage()]);
      }
    }
    else {
      $this->logger->error('Unhandled exception downloading URL @url: @exception',
          ['@url' => $url, '@exception' => $e->getMessage()]);
      $retries = 0;
    }
  }
}
