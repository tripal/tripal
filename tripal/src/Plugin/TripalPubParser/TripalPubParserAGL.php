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
 *  @TripalImporter(
 *    id = "tripal_pub_parser_agl",
 *    label = @Translation("Agricola"),
 *    description = @Translation("Retrieves and parses data from the USDA National Library Agricola database."),
 *  )
 */
class TripalPubParserAGL extends TripalPubParserBase {

  public function formSubmit($form, $form_state) {
  }

  public function form($form, $form_state) {
  }

  public function formValidate($form, $form_state) {
  }

  public function run(array $criteria) {
  }

}