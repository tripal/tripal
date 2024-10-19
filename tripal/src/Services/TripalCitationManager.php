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
   *   A token string defining fields used to generate the citation.
   *   Tokens are text enclosed in curly braces, e.g. "{Title}"
   *   Tokens may be "doubled" inside another set of curly braces to
   *   indicate a prefix or suffix that is only added if the token has
   *   a value. For example, a journal may not have issue numbers.
   *   Thus, for "{ {Volume}}{({Issue})}{:{Pages}.}", if there is
   *   no issue number, then the parentheses will not be included.
   */
  public function generateCitation(array $publication, string $format) {
    $citation = $format;

    // Match double tokens e.g. {({issue})} or { {title}.}
    if (preg_match_all('/\{[^\{\}]*\{[^\}]+\}[^\}]*\}/', $format, $matches)) {
      foreach ($matches[0] as $token_string) {
        // separate into prefix, key, suffix
        preg_match('/\{([^\{\}]*)\{([^\}]+)\}([^\}]*)\}/', $token_string, $submatches);
        $prefix = $submatches[1];
        $key = $this->firstMatchedToken($submatches[2], $publication);
        $suffix = $submatches[3];
        $value = $publication[$key] ?? '';
        // If prefix or suffix are already part of the value string, then
        // omit them, e.g. title already ends in period and token is "{ {Title}.}"
        if (strlen($value)) {
          if (strlen($prefix) and substr($value, 0, strlen($prefix)) == $prefix) {
            $prefix = '';
          }
          if (strlen($suffix) and substr($value, -strlen($suffix)) == $suffix) {
            $suffix = '';
          }
          $value = $prefix . $value . $suffix;
        }
        $citation = str_replace($token_string, $value, $citation);
      }
    }

    // Match any remaining single tokens, e.g. {Authors}
    if (preg_match_all('/\{[^\{\}]+\}/', $format, $matches)) {
      foreach ($matches[0] as $token_string) {
        $key = substr($token_string, 1, strlen($token_string)-2);
        $key = $this->firstMatchedToken($key, $publication);
        $value = $publication[$key] ?? '';
        $citation = str_replace($token_string, $value, $citation);
      }
    }

    return $citation;
  }

  /**
   * Determine the first defined token in a '|' delimited string of tokens
   *
   * @param string $token_string
   *   One or more tokens delimited by "|"
   * @param array $values
   *   Associative array of key value pairs where keys correspond to the tokens.
   *
   * @return string
   *   The first matching token that is defined as a key in $values.
   *   Returns an empty string if none of the tokens are defined.
   */
  protected function firstMatchedToken(string $token_string, array $values): string {
    $tokens = explode('|', $token_string);
    foreach ($tokens as $token) {
      if (array_key_exists($token, $values)) {
        return $token;
      }
    }
    return '';
  }

  /**
   * Returns a token string suitable for citation generation
   * of the specified publication type
   *
   * @param string $pub_type
   *   The publication type.
   *
   */
  public function getCitationTemplate(string $pub_type) {
    $templates = [
      'default' =>
        '{{Authors}.}{ {Title}.}{ {Publication Date|Year}.}{ {Journal Name|Journal Abbreviation|Series Name|Series Abbreviation}}{ {Volume}}{({Issue})}{:{Pages}.}',
      // These five templates implement equivalent citations as done by Tripal 3
      // as found in tripal_chado/api/modules/tripal_chado.pub.api.inc
      'Journal Article' =>
        '{{Authors}.}{ {Title}.}{ {Publication Date|Year}.}{ {Journal Name|Journal Abbreviation|Series Name|Series Abbreviation}}{ {Volume}}{({Issue})}{:{Pages}.}',
      'Review' =>
        '{{Authors}.}{ {Title}.}{ {Journal Name|Journal Abbreviation|Series Name|Series Abbreviation}}{ {Publisher}.}{ {Publication Date|Year}.}{ {Volume}}{({Issue})}{:{Pages}.}',
      "Research Support, Non-U.S. Gov't" =>
        '{{Authors}.}{ {Title}.}{ {Journal Name}}{ {Publication Date|Year}.}',
      'Letter' =>
        '{{Authors}.}{ {Title}.}{ {Journal Name|Journal Abbreviation|Series Name|Series Abbreviation}}{ {Publication Date|Year}.}{ {Volume}}{({Issue})}{:{Pages}.}',
      'Conference Proceedings' =>
        '{{Authors}.}{ {Title}.}{ {Conference Name|Series Name|Series Abbreviation}}{ {Publication Date|Year}.}{ {Volume}}{({Issue})}{:{Pages}.}',
    ];
    if (array_key_exists($pub_type, $templates)) {
      return $templates[$pub_type];
    }
    else {
      return $templates['default'];
    }
  }
}
