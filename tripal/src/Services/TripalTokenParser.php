<?php
namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;


/**
 * The TripalTokenParser class replaces tokens in strings with a
 * corresponding value. Tokens are marked by square brackets, and
 * the square brackets may be 1 or 2 levels deep. This allows
 * having a prefix or suffix that is only included when the token
 * has a defined value.
 *
 * A simple token string example:
 *   "The title is [Title]."
 * An example of a token that may or may not be defined "Issue":
 *   "[Journal]. [Volume][([Issue])]"
 * If defined the replaced string could be "Science. 123(45)"
 * but if Issue is not defined then the parentheses are not
 * included "Science. 123"
 *
 * In some cases the token may have several possible values,
 * and this can be designated by a pipe "|" delimited list.
 * For example:
 * "[ [Journal Name|Series Name|Journal Abbreviation].]"
 * The first of these that has a defined value will be used
 * for the substitution, or if none are defined, the token
 * is removed.
 *
 * Any prefix or suffix in a 2-level token is checked against
 * the token value, and if already present, is not added a
 * second time. For example, for a token "[[Title].]"
 * and a title value of "Cool Science.", then the replaced
 * value will not include both periods, only one is retained.
 */
class TripalTokenParser {

  /**
   * Replaces tokens within multiple tokenized strings with corresponding values.
   *
   * @param array $tokenized_strings
   *   An array of strings with tokens contained within one or two levels
   *   of square brackets. Multiple candidate tokens are separated by pipe
   *   character. For example [ [Journal Name|Journal Abbreviation].]
   *   In this latter case, parsing left to right, the first token that
   *   has a defined value will be used for replacement.
   * @param array $token_values
   *   Values to use for token replacement. Key is token name without
   *   square brackets, value is replacement value for the token.
   * @param bool $strict
   *   If FALSE (default), tokens with no defined value are removed.
   *   If TRUE, these tokens are left in the returned string
   *
   * @return array
   *   An array with all of the strings from the input $tokenized_strings array
   *   but with tokens replaced with appropriate values, or removed if no token
   *   value is availiable.
   */
  public function replaceTokensArray(array $tokenized_strings, array $token_values, bool $strict = FALSE) {

    $replaced_strings = [];
    foreach ($tokenized_strings as $index => $tokenized_string) {
      $replaced_strings[$index] = $this->replaceTokens($tokenized_string, $token_values);
    }
    return $replaced_strings;
  }

  /**
   * Replaces tokens within a single tokenized string with corresponding values.
   *
   * @param string $tokenized_string
   *   A string with tokens contained within one or two levels
   *   of square brackets. Multiple candidate tokens are separated by pipe
   *   character. For example [ [Journal Name|Journal Abbreviation].]
   *   In this latter case, parsing left to right, the first token that
   *   has a defined value will be used for replacement.
   * @param array $token_values
   *   Values to use for token replacement. Key is token name without
   *   square brackets, value is replacement value for the token.
   * @param bool $strict
   *   If FALSE (default), tokens with no defined value are removed.
   *   If TRUE, these tokens are left in the returned string
   *
   * @return string
   *   The passed $tokenized_string with tokens replaced with
   *   corresponding values, or the token is removed if no token
   *   replacement value is defined.
   */
  public function replaceTokens(string $tokenized_string, array $token_values, bool $strict = FALSE) {

    // Match double tokens e.g. [([issue])] or [ [title|name].]
    if (preg_match_all('/\[[^\[\]]*\[[^\[\]]+\][^\]\]]*\]/', $tokenized_string, $matches)) {
      foreach ($matches[0] as $token_string) {
        // separate into prefix, key, suffix
        preg_match('/\[([^\[\]]*)\[([^\]]+)\]([^\]]*)\]/', $token_string, $submatches);
        $prefix = $submatches[1];
        $key = $this->firstMatchedToken($submatches[2], $token_values);
        $suffix = $submatches[3];
        // $key could be an empty string here, in which case there is no value
        $value = $token_values[$key] ?? '';
        // If prefix or suffix are already part of the ends of the value string,
        // then omit them, e.g. title already ends in a period and token is "{ {Title}.}"
        $full_value = '';
        if (strlen($value)) {
          if (strlen($prefix) and substr($value, 0, strlen($prefix)) == $prefix) {
            $prefix = '';
          }
          if (strlen($suffix) and substr($value, -strlen($suffix)) == $suffix) {
            $suffix = '';
          }
          $full_value = $prefix . $value . $suffix;
        }
        if (strlen($full_value) or !$strict) {
          $tokenized_string = str_replace($token_string, $full_value, $tokenized_string);
        }
      }
    }

    // Match any remaining single tokens, e.g. [organism_genus]
    if (preg_match_all('/\[[^\[\]]+\]/', $tokenized_string, $matches)) {
      foreach ($matches[0] as $token_string) {
        $key = substr($token_string, 1, strlen($token_string)-2);
        $key = $this->firstMatchedToken($key, $token_values);
        $value = $token_values[$key] ?? '';
        if (strlen($value) or !$strict) {
          $tokenized_string = str_replace($token_string, $value, $tokenized_string);
        }
      }
    }
    return $tokenized_string;
  }

  /**
   * Finds any tokens in the passed string that are not valid.
   * This can be used to validate user input on a form.
   *
   * @param string $tokenized_string
   *   A string containing tokens to validate.
   * @param array $valid_tokens
   *   An array whose keys are valid tokens, without brackets.
   *   Array values are not used here.
   *
   * @return array
   *   A list of all tokens not present in the $valid_tokens array.
   *   An empty array indicates that validation has passed.
   */
  public function validateTokens(string $tokenized_string, array $valid_tokens): array {
    $invalid_tokens = [];
    // We only need to examine the inner tokens
    if (preg_match_all('/\[([^\[\]]+)\]/', $tokenized_string, $matches)) {
      foreach ($matches[1] as $token_string) {
        $parts = explode('|', $token_string);
        foreach ($parts as $token) {
          if (!array_key_exists($token, $valid_tokens)) {
            $invalid_tokens[] = $token;
          }
        }
      }
    }
    return $invalid_tokens;
  }

  /**
   * Determine the first token in a '|' delimited string of tokens
   * that has defined value in the $values array.
   *
   * @param string $token_string
   *   One or more tokens delimited by "|"
   * @param array $values
   *   Associative array of key value pairs where keys correspond to the tokens.
   *
   * @return string
   *   The first matching token that is present as a key in $values
   *   and is not null. Returns an empty string if none of the
   *   tokens have defined values.
   */
  protected function firstMatchedToken(string $token_string, array $values): string {
    $tokens = explode('|', $token_string);
    foreach ($tokens as $token) {
      if (array_key_exists($token, $values) and !is_null($values[$token])) {
        return $token;
      }
    }
    return '';
  }

}
