<?php

namespace Drupal\tripal\Plugin\TripalPubParser;

use Drupal\tripal\TripalPubParser\TripalPubParserBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * PubMed publication parser
 *
 *  @TripalPubParser(
 *    id = "tripal_pub_parser_pubmed",
 *    label = @Translation("NIH PubMed database"),
 *    description = @Translation("Retrieves and parses publication data from the NIH PubMed database"),
 *  )
 */
class TripalPubParserPubmed extends TripalPubParserBase {

  public function formSubmit($form, &$form_state) {
  }

  public function form($form, &$form_state) {
    // Add form elements specific to this parser.
    $form['pub_parser']['days'] = [
      '#title' => t('Days since record modified'),
      '#type' => 'textfield',
      '#description' => t('Limit the search to include pubs that have been added no more than this many days before today'),
      '#required' => FALSE,
      '#size' => 5,
    ];
    return $form;
  }

  public function formValidate($form, &$form_state) {
  }

  public function run(array $criteria) {
  }

}
