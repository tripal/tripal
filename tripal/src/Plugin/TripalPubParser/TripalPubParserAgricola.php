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
 *    id = "tripal_pub_parser_agricola",
 *    label = @Translation("Agricola"),
 *    select_text = @Translation("National Agricultural Library Agricola database"),
 *    description = @Translation("Retrieves and parses data from the USDA National Agricultural Library Agricola database."),
 *  )
 */
class TripalPubParserAgricola extends TripalPubParserBase {

  public function formSubmit($form, $form_state) {
  }

  public function form($form, $form_state) {
  }

  public function formValidate($form, $form_state) {
  }

  public function run(array $criteria) {
  }

}
