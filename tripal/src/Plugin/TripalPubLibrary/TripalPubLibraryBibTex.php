<?php

namespace Drupal\tripal\Plugin\TripalPubLibrary;

use Drupal\tripal\TripalPubLibrary\TripalPubLibraryBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Agricola publication library
 *
 *  @TripalPubLibrary(
 *    id = "tripal_pub_library_bibtex",
 *    label = @Translation("Upload a BibTex format file"),
 *    description = @Translation("Parses data from the an uploaded BibTex file"),
 *  )
 */
class TripalPubLibraryBibTex extends TripalPubLibraryBase {

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
