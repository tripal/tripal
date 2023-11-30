<?php

namespace Drupal\tripal\Plugin\TripalPubLibrary;

use Drupal\tripal\TripalPubLibrary\TripalPubLibraryBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Agricola publication library @TODO
 *
 *  @TripalPubLibrary(
 *    id = "tripal_pub_library_agricola",
 *    label = @Translation("National Agricultural Library Agricola database"),
 *    description = @Translation("Retrieves and parses data from the USDA National Agricultural Library Agricola database"),
 *  )
 */
class TripalPubLibraryAgricola extends TripalPubLibraryBase {

  public function formSubmit($form, &$form_state) {
  }

  public function form($form, &$form_state) {
    // Add form elements specific to this parser.
    $form['pub_library']['year_start'] = [
      '#title' => t('Earliest year of publication'),
      '#type' => 'textfield',
      '#description' => t('Filter returned publications for those that have been published no earlier than this year'),
      '#required' => FALSE,
      '#size' => 5,
    ];
    $form['pub_library']['year_stop'] = [
      '#title' => t('Latest year of publication'),
      '#type' => 'textfield',
      '#description' => t('Filter returned publications for those that have been published no later than this year'),
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
