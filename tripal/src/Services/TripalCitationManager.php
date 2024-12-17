<?php

namespace Drupal\tripal\Services;


/**
 * The citation manager class is used to generate citations for
 * publications of various types. The primary function is
 * generateCitation(), and input consists of two parts, a
 * citation template containing various tokens in a particular
 * order with appropriate punctuation, and an associative array
 * of key => value pairs that will be used to replace the
 * tokens in the template.
 *
 * This class can also provide default templates for several
 * types of publications.
 *
 */
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
   *   used to replace the tokens.
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
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      // The next five templates implement nearly equivalent citations as done by
      // Tripal 3 as found in tripal_chado/api/modules/tripal_chado.pub.api.inc
      'Journal Article' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      'Review' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].][ [Publisher].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      "Research Support, Non-U.S. Gov't" =>
        '[[Authors].][ [Title].][ [Journal Name].][ [Publication Date|Year]].',
      'Letter' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      'Conference Proceedings' =>
        '[[Authors].][ [Title].][ [Conference Name|Series Name|Series Abbreviation].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      // The publication importer also supports "Book" and "Book Chapter",
      // so include those.
      'Book' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].][ [Publisher].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
      'Book Chapter' =>
        '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].][ [Publisher].]'
        . '[ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].',
    ];
    if (array_key_exists($pub_type, $templates)) {
      return $templates[$pub_type];
    }
    else {
      return $templates['default'];
    }
  }
}
