<?php

namespace Drupal\tripal\Services;

use Exception;


class TripalCitationManager {

  /**
   * Generate citation
   *
   * @param array $publication
   *   An associative array defining the publication properties
   * @param string $format
   *   String defining fields and order to generate the citation.
   *   Tokens may be nested to indicate prefix or suffix that are only
   *   added if the token has a value. For example, a journal may not have issue numbers.
   *   Example format: '{{Authors}.}{ {Title}.}{ {Year}.}{ {Journal}}{ {Volume}}{({Issue})}{: {Pages}.}'
   */
  public static function generateCitation(array $publication, string $format) {
    $citation = $format;

    // First match double tokens e.g. {({issue})} or {{title}.}
    if (preg_match_all('/\{[^\{\}]*\{[^\}]+\}[^\}]*\}/', $format, $matches)) {
      foreach ($matches[0] as $token) {
        // separate into prefix, key, suffix
        preg_match('/\{([^\{\}]*)\{([^\}]+)\}([^\}]*)\}/', $token, $submatches);
        $prefix = $submatches[1];
        $key = $submatches[2];
        $suffix = $submatches[3];
        $value = $publication[$key] ?? '';
        // If prefix or suffix are already part of the string, then omit them,
        // e.g. title already ends in period and token is {{title}.}
        if (strlen($value)) {
          if (strlen($prefix) and substr($value, 0, strlen($prefix)) == $prefix) {
            $prefix = '';
          }
          if (strlen($suffix) and substr($value, -strlen($suffix)) == $suffix) {
            $suffix = '';
          }
          $value = $prefix . $value . $suffix;
        }
        $citation = str_replace($token, $value, $citation);
      }
    }
    // Now match single tokens, e.g. {Authors}
    if (preg_match_all('/\{[^\{\}]+\}/', $format, $matches)) {
      foreach ($matches[0] as $token) {
        $key = substr($token, 1, strlen($token)-2);
        $value = $publication[$key] ?? '';
        $citation = str_replace($token, $value, $citation);
      }
    }

    return $citation;
  }
}
