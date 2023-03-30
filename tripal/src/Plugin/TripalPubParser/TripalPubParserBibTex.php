<?php

namespace Drupal\tripal\Plugin\TripalPubParser;

use Drupal\tripal\TripalPubParser\TripalPubParserBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Agricola publication parser
 *
 *  @TripalPubParser(
 *    id = "tripal_pub_parser_bibtex",
 *    label = @Translation("Upload a BibTex format file"),
 *    description = @Translation("Parses data from the an uploaded BibTex file"),
 *  )
 */
class TripalPubParserBibTex extends TripalPubParserBase {

  public function formSubmit($form, &$form_state) {
  }

  public function form($form, &$form_state) {
    // Add form elements specific to this parser.
    return $form;
  }

  public function formValidate($form, &$form_state) {
  }

  public function run(array $criteria) {
  }

}
