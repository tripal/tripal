<?php

namespace Drupal\tripal\Plugin\TripalPubParser;

use Drupal\tripal\TripalPubParser\TripalPubParserBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Link;
use Drupal\Core\Url;
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
    $api_key_description = t('Tripal imports publications using NCBI\'s ')
      . Link::fromTextAndUrl('EUtils API',
          Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25500/'))->toString()
      . t(', which limits users and programs to a maximum of 3 requests per second without an API key. '
          . 'However, NCBI allows users and programs to an increased maximum of 10 requests per second if '
          . 'they provide a valid API key. This is particularly useful in speeding up large publication imports. '
          . 'For more information on NCBI API keys, please ')
      . Link::fromTextAndUrl(t('see here'),
          Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25497/#chapter2.Coming_in_December_2018_API_Key', [
            'attributes' => [
              'target' => 'blank',
            ]]))->toString()
      . '.';

    $form['pub_parser']['ncbi_api_key'] = [
      '#title' => t('(Optional) NCBI API key:'),
      '#type' => 'textfield',
      '#description' => $api_key_description,
      '#required' => FALSE,
//to-do add ajax callback to populate?
      '#size' => 20,
    ];
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
