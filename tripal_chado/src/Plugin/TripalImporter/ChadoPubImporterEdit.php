<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalPubParser\Interfaces\TripalPubParserInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Render\Markup;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * ChadoPubImporterEdit implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_pub_loader_edit",
 *    label = @Translation("Add a Publication Importer"),
 *    description = @Translation("Add or edit Chado Publication Importers"),
 *    button_text = @Translation("Save Publication Loader"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    file_upload = False,
 *    file_load = False,
 *    file_remote = False,
 *    file_required = False,
 *    cardinality = 1,
 *    menu_path = "",
 *    callback = "",
 *    callback_module = "",
 *    callback_path = "",
 *  )
 */
class ChadoPubImporterEdit extends ChadoImporterBase {

  /**
   * The name of this loader. This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Publication Loader Editor';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_pub_loader_edit';

  /**
   * A brief description for this loader. This description will be
   * presented to the site user.
   */
  public static $description = 'Define a publication importer';


  public static $form_instance = null;

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    // $this->form_instance = $this;
    ChadoPubImporterEdit::$form_instance = $this;

    // Call the parent form to provide the Chado schema selector.
    $form = parent::form($form, $form_state);

    $form = $this->newLoaderForm($form, $form_state);

    return $form;
  }

  /**
   * Form for creating or editing a publication loader.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function newLoaderForm($form, &$form_state) {
    // this is called in form(), can delete it here
    //    // Call the parent form to provide the Chado schema selector.
    //    $form = parent::form($form, $form_state);

    // Retrieve a sorted list of available pub parser plugins.
    $pub_parser_manager = \Drupal::service('tripal.pub_parser');
    $pub_parser_defs = $pub_parser_manager->getDefinitions();
    $plugins = [];
    foreach ($pub_parser_defs as $plugin_id => $def) {
      $plugin_key = $def['id'];
      $plugin_value = $def['label']->render();
      $plugins[$plugin_key] = $plugin_value;
    }
    asort($plugins);
    dpm('Generate form elements');

    $form['#prefix'] = '<div id="pub_importer_main_form">';
    $form['#suffix'] = '</div>';

    // RISH: This is the radio buttons which lists the types of publication / sources eg NIH PubMed database
    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => NULL,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'], // calls function within this class: function formAjaxCallback
        'wrapper' => 'edit-parser',
      ],
    ];

    // Doug: A placeholder for the form elements for the selected plugin,
    // to be populated by the AJAX callback.

    // RISH: This is the container that will hold the specific fields for a specific 'plugin' which represents the 
    //       publication / sources eg NIH PubMed database form elements
    $form['pub_parser'] = [
      '#prefix' => '<span id="edit-pub_parser">',
      '#suffix' => '</span>',
    ];

    // The placeholder will only be populated if a plugin, i.e.
    // $form['plugin_id'], has been selected. Both the plugin base
    // class and the selected plugin can each add form elements.

    // RISH: I think this the part that actually adds the additional form elements for the specific 'plugin' example PubMed 
    // I think this somehow gets executed on the ajax callback and loads the form elements
    $form = $this->formPlugin($form, $form_state);
    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function formValidate($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, 'ChadoPubImporterEdit.php Editor Validate not implemented'); //@@@
  }

  /**
   * {@inheritDoc}
   */
  public function formSubmit($form, &$form_state) {
    dpm('Form Submit never fires');
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, 'ChadoPubImporterEdit.php Editor Submit not implemented'); //@@@
    // Disable the parent submit
    // $form_state->setRebuild(True);
  }

  /**
   * {@inheritDoc}
   */
  public function run() {
  }

  /**
   * {@inheritDoc}
   */
  public function postRun() {
  }

  /**
   * Retrieves form elements from a plugin.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  private function formPlugin($form, &$form_state) {

    // Add elements only after a plugin has been selected.
    $plugin_id = $form_state->getValue(['plugin_id']);
    if ($plugin_id) {

      // Instantiate the selected plugin
      // Pub Parse Manager is found in tripal module: tripal/tripal/src/TripalPubParser/PluginManagers/TripalPubParserManager.php
      $pub_parser_manager = \Drupal::service('tripal.pub_parser');
      $plugin = $pub_parser_manager->createInstance($plugin_id, []);

      // The plugin manager defines form elements used by
      // all pub_parser plugins.

      // RISH: AJAX callbacks don't seem to work here due to a serialization issue
      // $form = $pub_parser_manager->form($form, $form_state);

      // RISH: Move common elements to this ChadoPubImporterEdit class
      $form = $this->form_common_elements($form, $form_state);

      // The selected plugin defines form elements specific
      // to itself.
      $form = $plugin->form($form, $form_state);
    }

    return $form;
  }

  // Plugins can add form elements specific to their parser.
  // Elements common to all parser plugins are defined here.
  // All elements need to be under the 'pub_parser' array index since
  // a placeholder exists for this to be updated by the Ajax callback.
  public function form_common_elements($form, $form_state) {
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
         . ' such as affilation, etc. Otherwise, only authors\' names are retrieved'),
      '#default_value' => $do_contact,
    ];

    // Add the form for the criteria
    $num_criteria = 1;
    $form['pub_parser']['num_criteria'] = [
      '#type' => 'hidden',
      '#default_value' => 1,
    ];
    $criteria = [];
    $form = $this->tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $form['pub_parser']['num_criteria']['#default_value'], $criteria);

    $form['pub_parser']['criteria_debug'] = [
      '#markup' => '<div id="tripal-pub-importer-criteria-debug-section"></div><br />',
    ];

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

    $headers = ['Operation', 'Scope', 'Search Terms', '', '', ''];
    // Add the table to the form
    $form['pub_parser']['table'] = [
      '#type' => 'table',
      '#header' => $headers,
      '#prefix' => '<div id="tripal-pub-importer-setup">',
      '#suffix' => '</div>',
    ];

    for ($i = 1; $i <= $num_criteria; $i++) {
      $form = $this->tripal_pub_importer_add_criteria_fields_row($form, $form_state, $i, $num_criteria, $criteria);
    } // for $i

    return $form;
  }

  private function tripal_pub_importer_add_criteria_fields_row($form, &$form_state, $i, $num_criteria, $criteria) {
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
          // '#ajax' => [
          //   'callback' => [$this, 'tripal_pub_importer_form_ajax_update'],
          //   'wrapper' => 'tripal-pub-importer-setup',
          //   'effect' => 'fade',
          //   // 'method' => 'replace',
          //   // 'prevent' => 'click',
          // ],
          // When this button is clicked, the form will be validated and submitted.
          // Therefore, we set custom submit and validate functions to override the
          // default form submit. In the validate function we set the form_state to
          // rebuild the form so that the submit function never actually gets called,
          // but we need it or Drupal will run the default validate anyway.
          // We also set #limit_validation_errors to empty so fields that are
          // required that don't have values won't generate warnings.
          
          // RISH REMOVED FOR TESTING (9/23/2023)
          '#submit' => [ChadoPubImporterEdit::$form_instance, 'tripal_pub_importer_form_ajax_button_submit'],
          // '#validate' => ['tripal_pub_importer_form_ajax_button_validate'], 
          // '#limit_validation_errors' => [],
        ];
      }


      $row["add-$i"] = [
        '#type' => 'submit',
        '#name' => 'add',
        '#value' => t('Add'),
        // '#ajax' => [
        //   'callback' => [$this, 'tripal_pub_importer_form_ajax_update'],
        //   // 'wrapper' => 'tripal-pub-importer-setup',
        //   'wrapper' => 'pub_importer_main_form',
        //   'effect' => 'fade',
        //   // 'method' => 'replace',
        //   // 'prevent' => 'click',
        // ],
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default form submit. In the validate function we set the form_state to
        // rebuild the form so that the submit function never actually gets called,
        // but we need it or Drupal will run the default validate anyway.
        // we also set #limit_validation_errors to empty so fields that
        // are required that don't have values won't generate warnings.
        
        //@to-do this submit function is not being called - why?

        // RISH REMOVED FOR TESTING (9/23/2023)
        '#submit' => [ChadoPubImporterEdit::$form_instance,'tripal_pub_importer_form_ajax_button_submit'],
        // '#validate' => ['tripal_pub_importer_form_ajax_button_validate'],
        // '#limit_validation_errors' => [],
      ];
      
    }
    $form['pub_parser']['table'][$i] = $row;
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
  public function tripal_pub_importer_form_ajax_button_validate($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, "tripal_pub_importer_form_ajax_button_validate() called, not yet implemented");
    // $form_state->setRebuild(TRUE);
  }

  /**
   * This function is just a dummy to override the default form submit on ajax
   * calls for buttons
   *
   * @ingroup tripal_pub
   */
  public function tripal_pub_importer_form_ajax_button_submit($form, &$form_state) {
    $trigger = $form_state->getTriggeringElement()['#name'];
    dpm($trigger, "tripal_pub_importer_form_ajax_button_submit() called, not yet implemented");
    // do nothing
  }

  /**
   * This function received ajax calls from the add button in the criteria table
   */
  public function tripal_pub_importer_form_ajax_update(&$form, &$form_state) {

    // $user_input = $form_state->getUserInput();
    // dpm($user_input);

    $trigger = $form_state->getTriggeringElement()['#name'];
    $response = new AjaxResponse();
    // https://www.drupal.org/docs/drupal-apis/ajax-api/core-ajax-callback-commands

    // $debug_output = "";
    // $debug_output .= $trigger . ' occurred.<br />';
    // $debug_output .= 'keys ' . print_r(array_keys($form['pub_parser']['table']), true) . '<br />';
    // $debug_output .= 'rows ' . json_encode($form['pub_parser']['table']['#rows']) . '<br />';
    // $response->addCommand(new ReplaceCommand('#tripal-pub-importer-criteria-debug-section', $debug_output));
    
    // dpm(array_keys($form['pub_parser']['table']));
    // If add was clicked
    if ($trigger == 'add') {
      $num_criteria = $form['pub_parser']['num_criteria']['#default_value'];

      // We need to remove the add and remove buttons maybe from all previous rows
      for ($i = 1; $i < $num_criteria + 1; $i++) {
        // dpm($form['pub_parser']['table'][$i]);
        unset($form['pub_parser']['table'][$i]["add-$i"]);
      }

      $form['pub_parser']['num_criteria']['#default_value'] = $num_criteria + 1;
      $form = $this->tripal_pub_importer_add_criteria_fields_row($form, $form_state, $num_criteria + 1, $num_criteria + 1, NULL);
    }
    else if ($trigger == 'remove')  {
      // $input = $form_state->getUserInput();
      // dpm($input);
      // array_pop($input['table']);
      // $form_state->setUserInput($input);
      $num_criteria = $form['pub_parser']['num_criteria']['#default_value'];
      //unset($form['pub_parser']['table'][2]);
    }
    

    $response->addCommand(new ReplaceCommand('#edit-pub_parser', $form['pub_parser']));
    return $response;
    // return $form;
  }

  /**
   * Ajax callback for the ChadoPubImporter::form() function.
   * This adds form elements appropriate for the selected parser plugin.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function formAjaxCallback($form, &$form_state) {
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-pub_parser', $form['pub_parser']));
    return $response;
  }



}
