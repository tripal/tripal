<?php

namespace Drupal\tripal\TripalPubParser\PluginManagers;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Provides a tripal importer plugin manager.
 */
class TripalPubParserManager extends DefaultPluginManager {

  /**
   * Constructs a new publication parser manager.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param string $plugin_interface
   *   The interface each plugin should implement.
   * @param string $plugin_definition_annotation_name
   *   The name of the annotation that contains the plugin definition.
   */
  public function __construct(
      \Traversable $namespaces
      ,CacheBackendInterface $cache_backend
      ,ModuleHandlerInterface $module_handler
  ) {
    parent::__construct(
        "Plugin/TripalPubParser"
        ,$namespaces
        ,$module_handler
        ,'Drupal\tripal\TripalPubParser\Interfaces\TripalPubParserInterface'
        ,'Drupal\tripal\TripalPubParser\Annotation\TripalPubParser'
    );
    $this->alterInfo("tripal_pub_parser_info");
    $this->setCacheBackend($cache_backend, "tripal_pub_parser_plugins");
  }

  // Plugins can add form elements specific to their parser.
  // Elements common to all parser plugins are defined here.
  // All elements need to be under the 'pub_parser' array index since
  // a placeholder exists for this to be updated by the Ajax callback.
  public function form($form, $form_state) {
    //@todo get these values
    $disabled = '';
    $do_contact = '';

    $form['pub_parser']['loader_name'] = [
      '#title' => t('Loader Name'),
      '#type' => 'textfield',
      '#description' => t("Please provide a name for this loader setup"),
      '#required' => TRUE,
    ];
    $form['pub_parser']['disabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Disabled'),
      '#description' => t('Check to disable this importer.'),
      '#default_value' => $disabled,
    ];
    $form['pub_parser']['do_contact'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Contact'),
      '#description' => t('Check to create an entry in the contact table for each author of'
         . ' a matching publication during import. This allows storage of additional information'
         . ' such as affiliation, etc. Otherwise, only authors\' names are retrieved'),
      '#default_value' => $do_contact,
    ];

    // Add the form for the criteria
    $num_criteria = 1;
    $criteria = [];
    $form = $this->tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

    // Add the submit buttons
    $form['pub_parser']['save'] = [
      '#type' => 'submit',
      '#value' => t('Save Importer'),
    ];
    $form['pub_parser']['test'] = [
      '#type' => 'submit',
      '#value' => t('Test Importer'),
    ];
    $form['pub_parser']['delete'] = [
      '#type' => 'submit',
      '#value' => t('Delete Importer'),
      '#attributes' => ['style' => 'float: right;'],
    ];

    // Add a placeholder for the section where the test results will appear
    $form['pub_parser']['results'] = [
      '#markup' => '<div id="tripal-pub-importer-test-section"></div>',
    ];

    return $form;
  }

  /**
   * A helper function for the importer setup form that adds the criteria to
   * the form that belongs to the importer.
   *
   * @param $form
   *   The form
   * @param $form_state
   *   The form state
   * @param $num_criteria
   *   The number of criteria that exist for the importer
   * @param $criteria
   *   An array containing the criteria
   *
   * @return
   *  A form array
   *
   * @ingroup tripal_pub
   */
  private function tripal_pub_importer_setup_add_criteria_fields($form, &$form_state, $num_criteria, $criteria) {

    // choices array
    $scope_choices = [
      'any' => 'Any Field',
      'abstract' => 'Abstract',
      'author' => 'Author',
      'id' => 'Accession',
      'title' => 'Title',
      'journal' => 'Journal Name',
    ];

    $first_op_choices = [
      '' => '',
      'NOT' => 'NOT',
    ];
    $op_choices = [
      'AND' => 'AND',
      'OR' => 'OR',
      'NOT' => 'NOT',
    ];

    $headers = ['Operation', 'Scope', 'Search Terms', '', ''];

    // Add the table to the form
    $form['pub_parser']['table'] = [
      '#type' => 'table',
      '#header' => $headers,
      '#prefix' => '<div id="tripal-pub-importer-setup">',
      '#suffix' => '</div>',
    ];

    for ($i = 1; $i <= $num_criteria; $i++) {
      $row = [];
      $search_terms = '';
      $scope = '';
      $is_phrase = '';
      $operation = '';

      // if we have criteria supplied from the database then use them as the initial defaults
      if ($criteria) {
        if (array_key_exists('criteria', $criteria)) {
          if (array_key_exists($i, $criteria['criteria'])) {
            $search_terms = $criteria['criteria'][$i]['search_terms'];
            $scope = $criteria['criteria'][$i]['scope'];
            $is_phrase = $criteria['criteria'][$i]['is_phrase'];
            $operation = $criteria['criteria'][$i]['operation'];
          }
        }
      }

      // if the criteria come from the session
      if (array_key_exists('tripal_pub_import', $_SESSION)) {
        $search_terms = isset($_SESSION['tripal_pub_import']['criteria'][$i]['search_terms']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'] : $search_terms;
        $scope = isset($_SESSION['tripal_pub_import']['criteria'][$i]['scope']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['scope'] : $scope;
        $is_phrase = isset($_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'] : $is_phrase;
        $operation = isset($_SESSION['tripal_pub_import']['criteria'][$i]['operation']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['operation'] : $operation;
      }

      // If the form_state has variables then use those. This happens when an
      // error occurs on the form, or the form is resubmitted using AJAX.
      $operation = $form_state->getValue("operation-$i") ?? $operation;
      $scope = $form_state->getValue("scope-$i") ?? $scope;
      $search_terms = $form_state->getValue("search_terms-$i") ?? $search_terms;
      $is_phrase = $form_state->getValue("is_phrase-$i") ?? $is_phrase;

      // $row['#attributes'] = ['vertical-align' => 'top'];  // Align vertically to top - @todo this doesn't work
      $row["operation-$i"] = [
        '#type' => 'select',
        '#options' => $i==1?$first_op_choices:$op_choices,
        '#default_value' => $operation,
      ];
      $row["scope-$i"] = [
        '#type' => 'select',
        '#description' => t('Please select the fields to search for this term.'),
        '#description_display' => 'after',
        '#options' => $scope_choices,
        '#default_value' => $scope,
      ];
      $row["search_terms-$i"] = [
        '#type' => 'textfield',
        '#description' => t('<span style="white-space: normal">Please provide a list of words for searching. You may use
          conjunctions such as "AND" or "OR" to separate words if they are expected in
          the same scope, but do not mix ANDs and ORs. Check the "Is Phrase" checkbox to use conjunctions as part of the text to search</span>'),
        '#description_display' => 'after',
        '#default_value' => $search_terms,
        '#required' => TRUE,
        '#maxlength' => 2048,
      ];
      $row["is_phrase-$i"] = [
        '#type' => 'checkbox',
        '#title' => t('Is Phrase?'),
        '#default_value' => $is_phrase,
      ];

      // If last row of the table
      if ($i == $num_criteria) {
        if ($i > 1) {
          $row["remove-$i"] = [
            '#type' => 'button',
            '#name' => 'remove',
            '#value' => t('Remove'),
            '#ajax' => [
              'callback' => 'tripal_pub_setup_form_ajax_update',
              'wrapper' => 'tripal-pub-importer-setup',
              'effect' => 'fade',
              'method' => 'replace',
              'prevent' => 'click',
            ],
            // When this button is clicked, the form will be validated and submitted.
            // Therefore, we set custom submit and validate functions to override the
            // default form submit. In the validate function we set the form_state to
            // rebuild the form so that the submit function never actually gets called,
            // but we need it or Drupal will run the default validate anyway.
            // We also set #limit_validation_errors to empty so fields that are
            // required that don't have values won't generate warnings.
            
            // RISH REMOVED FOR TESTING (9/23/2023)
            // '#submit' => ['tripal_pub_setup_form_ajax_button_submit'],
            // '#validate' => ['tripal_pub_setup_form_ajax_button_validate'], 
            // '#limit_validation_errors' => [],
          ];
        }
        $row["add-$i"] = [
          '#type' => 'button',
          '#name' => 'add',
          '#value' => t('Add'),
          '#ajax' => [
            'callback' => 'tripal_pub_setup_form_ajax_update',
            'wrapper' => 'tripal-pub-importer-setup',
            'effect' => 'fade',
            'method' => 'replace',
            'prevent' => 'click',
          ],
          // When this button is clicked, the form will be validated and submitted.
          // Therefore, we set custom submit and validate functions to override the
          // default form submit. In the validate function we set the form_state to
          // rebuild the form so that the submit function never actually gets called,
          // but we need it or Drupal will run the default validate anyway.
          // we also set #limit_validation_errors to empty so fields that
          // are required that don't have values won't generate warnings.
          
          //@to-do this submit function is not being called - why?

          // RISH REMOVED FOR TESTING (9/23/2023)
          // '#submit' => ['tripal_pub_setup_form_ajax_button_submit'],
          // '#validate' => ['tripal_pub_setup_form_ajax_button_validate'],
          // '#limit_validation_errors' => [],
        ];
      }
      $form['pub_parser']['table'][$i] = $row;
    } // for $i

    return $form;
  }

  /**
   * This function is used to rebuild the form if an ajax call is made via a
   * button. The button causes the form to be submitted. We don't want this so we
   * override the validate and submit routines on the form button. Therefore,
   * this function only needs to tell Drupal to rebuild the form
   *
   * @ingroup tripal_pub
   */
  public function tripal_pub_setup_form_ajax_button_validate($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, "tripal_pub_setup_form_ajax_button_validate() called, not yet implemented");
    $form_state->setRebuild(TRUE);
  }

  /**
   * This function is just a dummy to override the default form submit on ajax
   * calls for buttons
   *
   * @ingroup tripal_pub
   */
  public function tripal_pub_setup_form_ajax_button_submit($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, "tripal_pub_setup_form_ajax_button_submit() called, not yet implemented");
    // do nothing
  }

  /**
   * This function received ajax calls from the add button in the criteria table
   */
  public function tripal_pub_setup_form_ajax_update($form, &$form_state) {
    // dpm('hmmm');
    $response = new AjaxResponse();
    // $response->addCommand(new ReplaceCommand('#tripal-pub-importer-setup', $form['pub_parser']['table']));

    return $response;
  }
}
