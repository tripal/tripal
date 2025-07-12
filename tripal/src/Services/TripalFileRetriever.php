<?php
namespace Drupal\tripal\Services;

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
   * The time of the end of the previous request, if any.
   *
   * @var float
   */
  protected $last_request_time = NULL;

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
   * Download the contents of a remote or local file from a specified URL
   *
   * The contents of the file is returned in a string variable.
   *
   * @param string $url
   *   The address of the file to download
   * @param array $options
   *   Valid keys:
   *     retries - int: how many times to retry a download, default = 3.
   *     rate_limit - float: number of seconds between successive download requests.
   *     retry_delay - float: number of seconds between retries, default = 1.
   *     client_options - array: any options to pass to the http client.
   *
   * @return string
   *   The data obtained from the specified url, or NULL if it could not be downloaded
   */
  public function retrieveFileContents(string $url, array $options = []): string|null {
    $contents = NULL;
    $retries = $options['retries'] ?? 3;

    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {
      while (is_null($contents) && ($retries > 0)) {
        $this->doRateLimit($options['rate_limit'] ?? 0.0);
        try {
          $response = $this->httpClient->get($url, $options['client_options'] ?? []);
          $contents = (string) $response->getBody();
        }
        catch (\Exception $e) {
          $this->handleURLExceptions($retries, $e, $url);
        }
        $this->last_request_time = microtime(TRUE);
        $retries--;
        if (is_null($contents) && ($retries > 0)) {
          $this->doSleep($options['retry_delay'] ?? 1.0);
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
   * Download the contents of a remote or local file from a specified URL.
   *
   * The downloaded file is saved to a file in the local filesystem.
   *
   * @param string $url
   *   The address of the file to download
   * @param string $localfile
   *   The path to a local file where data is saed
   * @param array $options
   *   Valid keys:
   *     retries - int: how many times to retry a download, default = 3.
   *     rate_limit - float: number of seconds between successive download requests.
   *     retry_delay - float: number of seconds between retries, default = 1.
   *     client_options - array: any options to pass to the http client.
   *
   * @return bool
   *   Returns TRUE if successful, FALSE if error.
   */
  public function downloadFile(string $url, string $localfile, array $options = []): bool {
    $status = FALSE;
    $retries = $options['retries'] ?? 3;

    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {

      $options['client_options']['sink'] = $localfile;
      $options['client_options']['stream'] = FALSE;

      while (!$status && ($retries > 0)) {
        $this->doRateLimit($options['rate_limit'] ?? 0.0);
        try {
          /** @var GuzzleHttp\Psr7\Response **/
          $response = $this->httpClient->get($url, $options['client_options']);
          $status = TRUE;
        }
        catch (\Exception $e) {
          $this->handleURLExceptions($retries, $e, $url);
        }
        $retries--;
        if (!$status && ($retries > 0)) {
          $this->doSleep($options['retry_delay'] ?? 1.0);
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

  /**
   * Implements a rate limit between successive download requests.
   *
   * @param float $rate_limit
   *   A number of seconds to wait between successive download requests.
   *   If zero, do not wait.
   */
  protected function doRateLimit(float $rate_limit) {
    if (isset($rate_limit) && $rate_limit > 0.0) {
      if ($this->last_request_time) {
        $delay = $rate_limit - (microtime(TRUE) - $this->last_request_time);
        $this->doSleep($delay);
      }
    }
  }

  /**
   * Sleep for the number of seconds specified by a float.
   *
   * Since usleep() may not support > 1 second, this uses time_nanosleep().
   *
   * @param float $sleep_time
   *   A positive real number specifying some amount of time to sleep.
   */
  protected function doSleep(float $sleep_time) {
    // Negative values are interpreted as no sleep time.
    if ($sleep_time > 0) {
      $seconds = intval($sleep_time);
      $nanoseconds = intval(($sleep_time - $seconds) * 1_000_000_000);
      time_nanosleep($seconds, $nanoseconds);
    }
  }

}
