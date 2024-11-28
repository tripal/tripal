<?php

namespace Drupal\tripal\Services;

use Exception;


class TripalCitationManager {

  /**
   * The Tripal Token Parser service.
   *
   * @var \Drupal\tripal\Services\TripalTokenParser $token_parser
   */
  protected $token_parser = NULL;

  /**
   * TripalCitationManager constructor.
   *
   * @param \Drupal\tripal\Services\TripalTokenParser
   *   The token parser service.
   */
  public function __construct(TripalTokenParser $token_parser) {
    $this->token_parser = $token_parser;
  }

  /**
   * Generate citation
   *
   * @param string $format
   *   A token string defining fields used to generate the citation.
   *   Tokens are text enclosed in square brackets, e.g. "[Title]"
   *   Tokens may be "doubled" inside another set of square brackets to
   *   indicate a prefix or suffix that is only added if the token has
   *   a value. For example, a journal may not have issue numbers.
   *   Thus, for "[ [Volume]][([Issue])][:[Pages].]", if there is
   *   no issue number, then the parentheses will not be included.
   * @param array $values
   *   An associative array defining the publication properties
   */
  public function generateCitation(string $format, array $values) {
    $citation = $this->token_parser->replaceTokens($format, $values);
    return $citation;
  }

  /**
   * Returns a default token string suitable for citation
   * generation of the specified publication type
   *
   * @param string $pub_type
   *   The publication type.
   *
   */
  public function getDefaultCitationTemplate(string $pub_type) {
    $templates = [
      'default' =>
        '[[Authors].][ [Title].][ [Publication Date|Year].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation]][ [Volume]][([Issue])][:[Pages].]',
      // These five templates implement equivalent citations as done by Tripal 3
      // as found in tripal_chado/api/modules/tripal_chado.pub.api.inc
      'Journal Article' =>
        '[[Authors].][ [Title].][ [Publication Date|Year].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation]][ [Volume]][([Issue])][:[Pages].]',
      'Review' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation]][ [Publisher].][ [Publication Date|Year].][ [Volume]][([Issue])][:[Pages].]',
      "Research Support, Non-U.S. Gov't" =>
        '[[Authors].][ [Title].][ [Journal Name]][ [Publication Date|Year].]',
      'Letter' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation]][ [Publication Date|Year].][ [Volume]][([Issue])][:[Pages].]',
      'Conference Proceedings' =>
        '[[Authors].][ [Title].][ [Conference Name|Series Name|Series Abbreviation]][ [Publication Date|Year].][ [Volume]][([Issue])][:[Pages].]',
    ];
    if (array_key_exists($pub_type, $templates)) {
      return $templates[$pub_type];
    }
    else {
      return $templates['default'];
    }
  }
}
