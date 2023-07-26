<?php

namespace Drupal\tripal\Plugin\TripalTreeParser;

use Drupal\tripal\TripalTreeParser\TripalTreeParserBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Newick tree parser
 *
 *  @TripalTreeParser(
 *    id = "tripal_tree_parser_newick",
 *    label = @Translation("Newick tree format"),
 *    description = @Translation("Parses data in the Newick tree file format"),
 *  )
 */
class TripalTreeParserNewick extends TripalTreeParserBase {

  public function form($form, &$form_state) {
    // Add form elements specific to this tree parser.
    // Currently there are none.

    return $form;
  }

  public function formValidate($form, &$form_state) {

  }

  public function formSubmit($form, &$form_state) {

  }

  public function run(array $criteria) {

  }

}
