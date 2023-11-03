<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class ChadoNewPublicationForm extends FormBase {

  private $form_state_previous_user_input = null;
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_new_publication_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $pub_import_id = null) {
    
    dpm('Pub Import ID:' . $pub_import_id);
    if ($pub_import_id != null) {
      $public = \Drupal::database();
      // This is the edit version of the form, we need to lookup the current pub_import_id
      $publication = $public->select('tripal_pub_import', 'tpi')->fields('tpi')->condition('pub_import_id', $pub_import_id, '=')->execute()->fetchObject();
      $criteria = unserialize($publication->criteria);


      // THIS DOES NOT WORK!!!!!! DRUPAL and Symphony!!!!!
      // $form_state->setUserInput($criteria['form_state_user_input']);

      // Alternative and not what I want
      // Add the previously saved user input into the instantiated object   
      $this->form_state_previous_user_input = $criteria['form_state_user_input'];

      // Let's add a hidden field called form_mode to tell the form submit process that this is an edit instead of creation
      $form['mode'] = [
        '#type' => 'hidden',
        '#value' => 'edit'
      ];

      // Save the pub_import_id into a hidden field to be used if the form is ever submitted
      $form['pub_import_id'] = [
        '#type' => 'hidden',
        '#value' => $pub_import_id
      ];
    }

    $form_state_values = $form_state->getValues();
    // dpm($form_state_values);



    $html = "<ul class='action-links'>";
    $html .= '  <li>' . Link::fromTextAndUrl('Return to manage pub search queries', Url::fromUri('internal:/admin/tripal/loaders/publications/manage_pub_search_queries'))->toString() . '</li>';
    $html .= '</ul>';
    $form['new_publication_link'] = [
      '#type' => 'markup',
      '#markup' => $html
    ];
    unset($html);

    // Show the list of importers available for user to select
    $form = $this->form_elements_importer_selection($form, $form_state);

    // If the button_next was clicked, it will exist in the form_state_values
    if (isset($form_state_values['button_next']) || $pub_import_id != null) {
      // Once clicked, hide the 'next' button by changing type to hidden
      $form['button_next']['#type'] = 'hidden';

      // Disable the click radio options
      $form['plugin_id']['#attributes'] = array('onclick' => 'return false;');


      // add the elements for the specific importer (below function initialized plugin and calls form function)
      $form = $this->form_elements_specific_importer($form, $form_state);
      dpm('We should show the elements fields required for this importer');


      // add the common elements (like search criteria)
      $form = $this->form_elements_common($form, $form_state);
      
      // handle previous user input
      if ($pub_import_id != 'null') {
        dpm('IT CAME HERE');
        // dpm($this->form_state_previous_user_input);
        dpm($form);
        $this->form_elements_load_previous_user_input($this->form_state_previous_user_input, $form['pub_parser']);
        dpm($form);
      }
    }

    return $form;
  }

  public function form_elements_load_previous_user_input(&$input, &$form_element) {
    foreach ($input as $key => $value) {
      if (!is_array($input[$key])) {
        dpm($key . ' and ' . $value);
        $form_element[$key]['#default_value'] = $value;
      }
      else {
        $this->form_elements_load_previous_user_input($input[$key], $form_element[$key]);
      }
    }
  }


  public function form_elements_common($form, FormStateInterface &$form_state) {
    $form_state_values = $form_state->getValues();
    // dpm($form_state_values);
    //@todo get these values
    $disabled = '';
    $do_contact = '';

    $form['pub_parser']['loader_name'] = [
      '#title' => t('Loader Name'),
      '#type' => 'textfield',
      '#description' => t("Please provide a name for this loader setup"),
      '#required' => TRUE,
      '#weight' => 0,
    ];
    $form['pub_parser']['disabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Disabled'),
      '#description' => t('Check to disable this importer.'),
      '#default_value' => $disabled,
      '#weight' => 0,
    ];
    $form['pub_parser']['do_contact'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Contact'),
      '#description' => t('Check to create an entry in the contact table for each author of'
         . ' a matching publication during import. This allows storage of additional information'
         . ' such as affilation, etc. Otherwise, only authors\' names are retrieved'),
      '#default_value' => $do_contact,
      '#weight' => 0,
    ];

    $num_criteria = 1; // default criteria row count
    $trigger = $form_state->getTriggeringElement()['#name'];
    $user_input = $form_state->getUserInput();

    if (isset($user_input['num_criteria'])) {
      $num_criteria = $user_input['num_criteria'];
    }
    elseif (isset($this->form_state_previous_user_input['num_criteria'])) {
      $num_criteria = $this->form_state_previous_user_input['num_criteria'];
    }

    // dpm($user_input);
    if ($trigger == 'add') {
      // Increment the num_criteria which should regenerate the form with an additional criteria row
      $num_criteria += 1;
      $user_input['num_criteria'] = $num_criteria;
      $form_state->setUserInput($user_input);
      
    }
    elseif ($trigger == 'remove') {
      // Increment the num_criteria which should regenerate the form with an additional criteria row
      $num_criteria -= 1;
      $user_input['num_criteria'] = $num_criteria;
      $form_state->setUserInput($user_input);
    }

    // Add the form for the criteria
    $form['pub_parser']['num_criteria'] = [
      '#type' => 'hidden',
      '#default_value' => $num_criteria,
    ];


    $criteria = [];


    $form = $this->tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

    $form['pub_parser']['criteria_debug'] = [
      '#markup' => '<div id="tripal-pub-importer-criteria-debug-section"></div><br />',
      '#weight' => 52,
    ];

    // Add the submit buttons
    $form['pub_parser']['save'] = [
      '#type' => 'submit',
      '#value' => t('Save Importer'),
      '#weight' => 51,
    ];
    $form['pub_parser']['test'] = [
      '#type' => 'submit',
      '#value' => t('Test Importer'),
      '#weight' => 51,
    ];
    $form['pub_parser']['delete'] = [
      '#type' => 'submit',
      '#value' => t('Delete Importer'),
      '#attributes' => ['style' => 'float: right;'],
      '#weight' => 51,
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
      '#weight' => 50, // arbitrary heavier number so table is below most options
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
          // '#type' => 'button',
          '#type' => 'submit',
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
          // '#submit' => [$this, 'tripal_pub_importer_form_ajax_button_submit'],
          // '#validate' => ['tripal_pub_importer_form_ajax_button_validate'], 
          // '#limit_validation_errors' => [],
        ];
      }


      $row["add-$i"] = [
        // '#type' => 'button',
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
        // '#submit' => [$this,'tripal_pub_importer_form_ajax_button_submit'],
        // '#validate' => ['tripal_pub_importer_form_ajax_button_validate'],
        // '#limit_validation_errors' => [],
      ];
      
    }
    $form['pub_parser']['table'][$i] = $row;
    return $form;
  }  

  public function form_elements_specific_importer($form, FormStateInterface $form_state) {
    // Add elements only after a plugin has been selected.
    $plugin_id = $form_state->getValue(['plugin_id']);
    if ($plugin_id) {

      // Instantiate the selected plugin
      // Pub Parse Manager is found in tripal module: tripal/tripal/src/TripalPubParser/PluginManagers/TripalPubParserManager.php
      $pub_parser_manager = \Drupal::service('tripal.pub_parser');
      $plugin = $pub_parser_manager->createInstance($plugin_id, []);

      // The selected plugin defines form elements specific
      // to itself.
      $form = $plugin->form($form, $form_state);
    }
    return $form;
  }

  public function form_elements_importer_selection($form, FormStateInterface $form_state) {
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

    $form['#prefix'] = '<div id="pub_importer_main_form">';
    $form['#suffix'] = '</div>';

    $default_value = NULL;
    if ($this->form_state_previous_user_input != null) {
      $default_value = $this->form_state_previous_user_input['plugin_id'];
    }

    // RISH: This is the radio buttons which lists the types of publication / sources eg NIH PubMed database
    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => $default_value,
      // '#ajax' => [
      //   'callback' =>  [$this, 'formAjaxCallback'], // calls function within this class: function formAjaxCallback
      //   'wrapper' => 'edit-parser',
      // ],
    ];

    $form['button_next'] = [
      '#type' => 'button',
      '#value' => 'Next'
    ];

    // Doug: A placeholder for the form elements for the selected plugin,
    // to be populated by the AJAX callback.

    // RISH: This is the container that will hold the specific fields for a specific 'plugin' which represents the 
    //       publication / sources eg NIH PubMed database form elements
    $form['pub_parser'] = [
      '#prefix' => '<span id="edit-pub_parser">',
      '#suffix' => '</span>',
    ];

    return $form;

  }


  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // $form_state_values = $form_state->getValues();
    // dpm($form_state_values);
    $public = \Drupal::database();
    $user_input = $form_state->getUserInput();
    dpm($user_input);
    $trigger = $form_state->getTriggeringElement()['#name'];
    
    dpm($trigger);
    if ($trigger == 'op') {
      $op = $user_input['op'];
      dpm($op);
      if ($op == 'Save Importer') {
        // tripal_pub_import table columns are: pub_import_id, name, criteria, disabled, do_contact

        // Translate the submitted data into a variable which can be serialized into a criteria column
        // of the tripal_pub_import table
        dpm($user_input);
        $disabled = $user_input['disabled'];
        if ($disabled == null) {
          $disabled = 0;
        }
        $do_contact = $user_input['do_contact'];
        if ($do_contact == null) {
          $do_contact = 0;
        }
        $criteria_column_array = [
          'remote_db' => explode('tripal_pub_parser_', $user_input['plugin_id'])[1],
          // 'days' => $user_input['days'],
          'num_criteria' => $user_input['num_criteria'],
          'loader_name' => $user_input['loader_name'],
          'disabled' => $disabled,
          'do_contact' => $do_contact,
          'pub_import_id' => $user_input['pub_import_id'],
          'criteria' => [],
          'form_state_user_input' => NULL, // used for edit form
        ];

        // Save form_state_user_input (for use with the edit version of this form)
        // This removes the requirement to retranslate the saved data which could become unmaintainable
        // Remove any data from user_input that is not necessary or can confuse logic processing
        $form_mode = $user_input['mode'];
        unset($user_input['op']); // used to determine if it was a save or delete
        // unset($user_input['form_build_id']);
        // unset($user_input['form_token']);
        unset($user_input['mode']); // was used to determine if it is new or edit
        $criteria_column_array['form_state_user_input'] = $user_input;

        $criteria_count = 1;
        // $user_input['table'] is the criteria rows from the submitted form
        // Go through each row of criteria
        foreach ($user_input['table'] as $criteria_row_submitted) {
          $is_phrase = $criteria_row_submitted['is_phrase-' . $criteria_count];
          if ($is_phrase == null) {
            $is_phrase = 0;
          }
          $criteria_column_array['criteria'][$criteria_count] = [
            'search_terms' => $criteria_row_submitted['search_terms-' . $criteria_count],
            'scope' => $criteria_row_submitted['scope-' . $criteria_count],
            'is_phrase' => $is_phrase,
            'operation' => $criteria_row_submitted['operation-' . $criteria_count],
          ];
          $criteria_count++;
        }


        // Load the plugin
        $plugin_id = $user_input['plugin_id'];
        if ($plugin_id) {
          // Instantiate the selected plugin
          // Pub Parse Manager is found in tripal module: tripal/tripal/src/TripalPubParser/PluginManagers/TripalPubParserManager.php
          $pub_parser_manager = \Drupal::service('tripal.pub_parser');
          $plugin = $pub_parser_manager->createInstance($plugin_id, []);

          // The selected plugin defines form elements specific
          // to itself.
          $plugin->form_submit($form, $form_state, $criteria_column_array);
        }


        $criteria_column_serialized = serialize($criteria_column_array);

        $db_fields = [
          'name' => $user_input['loader_name'],
          'criteria' => $criteria_column_serialized,
          'disabled' => $disabled,
          'do_contact' => $do_contact
        ];

        $messenger = \Drupal::messenger();

        // If form_mode is not edit, then it is a new importer
        if ($form_mode != "edit") {
          $public->insert('tripal_pub_import')->fields($db_fields)->execute();
          
          $messenger->addMessage("Importer successfully added!");
          
        }
        // If form_mode is 'edit', this is an update to the database
        else {
          $public->update('tripal_pub_import', 'tpi')
            ->fields($db_fields)
            ->condition('pub_import_id', $user_input['pub_import_id'])
            ->execute();
          $messenger->addMessage("Importer successfully edited!");
        }
        
        $form_state->setRebuild(TRUE); // @TODO change this to false after developing this form

      }
      
      
    }
    else {
      $form_state->setRebuild(TRUE);
    }
  }

}