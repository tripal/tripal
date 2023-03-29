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
 *    id = "tripal_pub_parser_pmid",
 *    label = @Translation("PMID"),
 *    description = @Translation("Retrieves and parses data from the NIH PubMed database."),
 *  )
 */
class TripalPubParserPMID extends TripalPubParserBase {

  public function formSubmit($form, $form_state) {
  }

  public function form($form, $form_state) {
  }

  public function formValidate($form, $form_state) {
  }

  public function run(array $criteria) {
  }

}
