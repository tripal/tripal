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
    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {
      while ($retries > 0) {
        try {
          $response = $this->httpClient->get($url, $options);
          $response_body = (string) $response->getBody();
          return $response_body;
        }
        catch (ConnectException $e) {
          $this->logger->error('Invalid hostname in URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return NULL;
        }
        catch (ClientException $e) {
          $this->logger->error('Invalid file in URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return NULL;
        }
        catch (RequestException $e) {
          if ($retries > 1) {
            $this->logger->error('Unable to get response from @url: @exception. Will retry',
                ['@url' => $url, '@exception' => $e->getMessage()]);
          }
          else {
            $this->logger->error('Unable to get response from @url: @exception',
                ['@url' => $url, '@exception' => $e->getMessage()]);
            return NULL;
          }
        }
        catch (Exception $e) {
          $this->logger->error('Unhandled exception downloading URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return NULL;
        }
        $retries--;
        sleep(1);
      }
      return NULL;
    }
    // If there was no host in the url, then it is considered a local file
    else {
      $contents = FALSE;
      if (!file_exists($url)) {
        $this->logger->error('Local file @url does not exist',
            ['@url' => $url]);
      }
      else {
        $contents = file_get_contents($url);
      }
      if ($contents === FALSE) {
        return NULL;
      }
      else {
        return $contents;
      }
    }
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
    // Distinguish between local and remote files
    $parsed_url = parse_url($url);
    if ($parsed_url['host'] ?? NULL) {

      $options['sink'] = $localfile;
      $options['stream'] = FALSE;

      while ($retries > 0) {
        try {
          /** @var GuzzleHttp\Psr7\Response **/
          $response = $this->httpClient->get($url, $options);
          return TRUE;
        }
        catch (ConnectException $e) {
          $this->logger->error('Invalid hostname in URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return FALSE;
        }
        catch (ClientException $e) {
          $this->logger->error('Invalid file in URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return FALSE;
        }
        catch (RequestException $e) {
          if ($retries > 1) {
            $this->logger->error('Unable to get response from @url: @exception. Will retry',
                ['@url' => $url, '@exception' => $e->getMessage()]);
          }
          else {
            $this->logger->error('Unable to get response from @url: @exception',
                ['@url' => $url, '@exception' => $e->getMessage()]);
            return FALSE;
          }
        }
        catch (Exception $e) {
          $this->logger->error('Unhandled exception downloading URL @url: @exception',
              ['@url' => $url, '@exception' => $e->getMessage()]);
          return FALSE;
        }
        $retries--;
        sleep(1);
      }
      return NULL;
    }
    // If there was no host in the url, then it is considered a local file
    else {
      if (!file_exists($url)) {
        $this->logger->error('Local file @url does not exist',
            ['@url' => $url]);
        return FALSE;
      }
      else {
        copy($url, $localfile);
        return TRUE;
      }
    }
  }
}
