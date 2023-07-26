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

    // Default values can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing phylotree
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks from non submit form elements
    // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
    //    form elements and the form is being rebuilt
    //
    // set form field defaults
    $name_re = '';
    $match = '';

    $form_state_values = $form_state->getValues();

if (0) { //@@@
    // If we are reconstructing the form from a failed validation or ajax callback
    // then use the $form_state['values'] values.
    if (array_key_exists('tree_name', $form_state_values)) {
dpm('CP41 tree_name defined');
//@@@ in parent form      $tree_name = $form_state_values['tree_name'];
      $leaf_type = $form_state_values['leaf_type'];
//      $analysis_id = $form_state_values['analysis_id'];
      $comment = $form_state_values['description'];
      $dbxref = $form_state_values['dbxref'];
    }

    // If we are rebuilding the form from after submission (from ajax call) then
    // the values are in the $form_state_values['input'] array.
    if (array_key_exists('input', $form_state_values) and !empty($form_state_values['input'])) {
dpm('CP42 input defined');
      $tree_name = $form_state_values['input']['tree_name'];
      $leaf_type = $form_state_values['input']['leaf_type'];
//      $analysis_id = $form_state_values['input']['analysis_id'];
      $comment = $form_state_values['input']['description'];
      $dbxref = $form_state_values['input']['dbxref'];
    }
dpm('CP43 ');
} //@@@

    $form['tree_parser']['name_re'] = [
      '#title' => t('Feature Name Regular Expression'),
      '#type' => 'textfield',
      '#description' => t('The tree nodes will be automatically associated with
          features, or in the case of taxonomic trees, with organisms. However,
          if the nodes in the tree file are not exactly as the names of features
          or organisms but have enough information to uniquely identify them,
          then you may provide a regular expression that the importer will use to
          extract the appropriate names from the node names. For example, remove
          a prefix ABC_ with %example', ['%example' => '^ABC_(.*)$']),
      '#default_value' => $name_re,
    ];
    $form['tree_parser']['match'] = [
      '#title' => t('Use Unique Feature Name'),
      '#type' => 'checkbox',
      '#description' => t('If this is a phylogenetic (non taxonomic) tree and the nodes ' .
        'should match the unique name of the feature rather than the name of the feature, ' .
        'then check this box. If unchecked, the loader will try to match the feature ' .
        'using the feature name.'),
      '#default_value' => $match,
    ];

    return $form;
  }

  public function formValidate($form, &$form_state) {
dpm('TripalTreeParserNewick PLUGIN formValidate called'); //@@@
    $values = $form_state->getValues();
    $schema = $values['schema_name'];
    $options = [
      'name' => trim($values['tree_name'] ?? ''),
      'description' => trim($values['description'] ?? ''),
      // When leaf_type is not specified on the form, default to 'taxonomy'
      // for taxonomic (species) trees. In Tripal3 this had to be typed in.
      'leaf_type' => $values['leaf_type'] ?? 'taxonomy',
      'format' => 'newick',
      'dbxref' => trim($values['dbxref'] ?? ''),
      'match' => $values['match'] ?? '',
      'name_re' => $values['name_re'] ?? '',
      'load_later' => $values['load_later'] ?? '',
    ];

    // check the regular expression to make sure it is valid
    if ($options['name_re']) {
      @ $result_re = preg_match('/' . $options['name_re'] . '/', NULL);
      if (!$result_re) {
        $form_state->setErrorByName('name_re',
            t('The entered regular expression %re is not valid', ['%re' => $options['name_re']]));
      }
    }

    $errors = [];
    $warnings = [];

//@@@    chado_validate_phylotree('insert', $options, $errors, $warnings, $schema);

    // Now set form errors if any errors were detected.
    if (count($errors) > 0) {
      foreach ($errors as $field => $message) {
        if ($field == 'name') {
          $field = 'tree_name';
        }
        $form_state->setErrorByName($field, $message);
      }
    }
    // Add any warnings if any were detected
    // n.b. chado_validate_phylotree() does not currently return any warnings.
    if (count($warnings) > 0) {
      foreach ($warnings as $field => $message) {
        $form_state->setErrorByName($field, $message);
      }
    }
  }

  public function formSubmit($form, &$form_state) {
dpm('TripalTreeParserNewick plugin formSubmit() called'); //@@@
  }

  public function run(array $criteria) {
dpm('TripalTreeParserNewick plugin run() called'); //@@@
  }

}
