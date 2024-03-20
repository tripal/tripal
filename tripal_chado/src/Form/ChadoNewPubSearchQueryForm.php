<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Render\Markup;
use Drupal\Core\StringTranslation\TranslatableMarkup;


class ChadoNewPubSearchQueryForm extends FormBase {
  private $pub_import_id = null;
  private $form_state_previous_user_input = null;

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_new_pub_search_query_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $pub_import_id = null) {
    if ($pub_import_id != null) {
      // used to keep track of whether this is a new query or edit query
      $this->pub_import_id = $pub_import_id; 
      $public = \Drupal::database();

      // This is the edit version of the form, we need to lookup the current pub_import_id
      $pub_library_manager = \Drupal::service('tripal.pub_library');
      $publication = $pub_library_manager->getSearchQuery($pub_import_id);
      $criteria = unserialize($publication->criteria);


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

    // If performing a test we need to change the state etc to make sure the form appears correctly
    if (isset($_SESSION['tripal_pub_import'])) {
      if ($_SESSION['tripal_pub_import']['perform_test'] == 1) {
        $this->form_state_previous_user_input = $_SESSION['tripal_pub_import']['perform_test_user_input'];
        $form_state_values['button_next'] = "Next";
      }
    }

    $html = "<ul class='action-links'>";
    $html .= '  <li>' . 
      Link::fromTextAndUrl(
        'Return to manage pub search queries', 
        Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries')
      )->toString() . '</li>';
    $html .= '</ul>';
    $form['new_publication_link'] = [
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

      // add the common elements (like search criteria)
      $form = $this->form_elements_common($form, $form_state);
      
      // handle previous user input
      if ($pub_import_id != 'null') {
        $this->form_elements_load_previous_user_input(
          $this->form_state_previous_user_input, $form['pub_library']
        );
      }

      // If the test button was clicked - run the TripalPubLibrary Plugin specific test function
      if (isset($_SESSION['tripal_pub_import'])) {
        if ($_SESSION['tripal_pub_import']['perform_test'] == 1) {
          $plugin_id = $form['plugin_id']['#default_value'];
          if ($plugin_id) {
            // Instantiate the selected plugin
            // Pub Library Manager is found in tripal module: 
            // tripal/tripal/src/TripalPubLibrary/PluginManagers/TripalPubLibraryManager.php
            $pub_library_manager = \Drupal::service('tripal.pub_library');
            $plugin = $pub_library_manager->createInstance($plugin_id, []);

            // The selected plugin defines a test specific to itself.
            $criteria_column_array = $_SESSION['tripal_pub_import']['perform_test_criteria_array'];

            // Perform a retrieve aka test lookup (retrieve 5 items, page 0)
            $results = $plugin->retrieve($criteria_column_array, 5, 0);

            // On successful results, it should return array with keys total_records, search_str, pubs(array)
            $headers = ['', 'Publication', 'Authors'];
            $form['test_results_table'] = [
              '#type' => 'table',
              '#header' => $headers,
              '#prefix' => '<div id="test_results_table">',
              '#suffix' => '</div>',
              '#weight' => 1000, // arbitrary heavier number so table is below most options
            ];

            if ($results != NULL) {

              $form['test_results_count_info'] = [
                '#markup' => '<h1>Test results</h1><div>Found ' . $results['total_records'] . 
                  ' publications.' . ($results['total_records']>5?' Showing the first 5 publications.':'') . '</div>',
                '#weight' => 998
              ];
              
              $form['test_results_search_string'] = [
                '#markup' => 'Search String: ' .  $results['search_str'],
                '#weight' => 999,
              ];  

              $index = 0;
              foreach ($results['pubs'] as $pubs_row) {
                $index++;
                $row["index"] = [
                  '#markup' => $index,
                ];
                $row["publication"] = [
                  '#markup' => $pubs_row['Title'],
                ];
                $row["authors"] = [
                  '#markup' => $pubs_row['Authors'] ?? '',
                ];              
                $form['test_results_table'][$index - 1] = $row;                           
              }
            }

            // Set the session variable perform_test back to 0 since the test has finished
            $_SESSION['tripal_pub_import']['perform_test'] = 0;
            $_SESSION['tripal_pub_import']['perform_test_criteria_array'] = [];
          }
        }
      }
    }
    return $form;
  }

  /**
   * Recursive function to find values from user_input and add it back to the #default_value
   * key for the specific form element
   */
  public function form_elements_load_previous_user_input(&$input, &$form_element) {
    if (isset($input)) {
      foreach ($input as $key => $value) {
        if (!is_array($input[$key])) {
          $form_element[$key]['#default_value'] = $value;
        }
        else {
          $this->form_elements_load_previous_user_input($input[$key], $form_element[$key]);
        }
      }
    }
  }


  public function form_elements_common($form, FormStateInterface &$form_state) {
    $form_state_values = $form_state->getValues();

    $disabled = '';
    $do_contact = '';

    $form['pub_library']['loader_name'] = [
      '#title' => t('Loader Name'),
      '#type' => 'textfield',
      '#description' => t("Please provide a name for this loader setup"),
      '#required' => TRUE,
      '#weight' => -50,
    ];
    $form['pub_library']['disabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Disabled'),
      '#description' => t('Check to disable this importer.'),
      '#default_value' => $disabled,
      '#weight' => -49,
    ];
    $form['pub_library']['do_contact'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Contact'),
      '#description' => t('Check to create an entry in the contact table for each author of'
         . ' a matching publication during import. This allows storage of additional information'
         . ' such as affilation, etc. Otherwise, only authors\' names are retrieved'),
      '#default_value' => $do_contact,
      '#weight' => -48,
    ];

    $num_criteria = 1; // default criteria row count
    $trigger = @$form_state->getTriggeringElement()['#name'];
    $user_input = $form_state->getUserInput();

    if (isset($user_input['num_criteria'])) {
      $num_criteria = $user_input['num_criteria'];
    }
    elseif (isset($this->form_state_previous_user_input['num_criteria'])) {
      $num_criteria = $this->form_state_previous_user_input['num_criteria'];
    }

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
    $form['pub_library']['num_criteria'] = [
      '#type' => 'hidden',
      '#default_value' => $num_criteria,
    ];


    $criteria = [];


    $form = $this->tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

    $form['pub_library']['criteria_debug'] = [
      '#markup' => '<div id="tripal-pub-importer-criteria-debug-section"></div><br />',
      '#weight' => 52,
    ];

    // Add the submit buttons
    $form['pub_library']['save'] = [
      '#type' => 'submit',
      '#value' => t('Save Search Query'),
      '#weight' => 51,
    ];


    $form['pub_library']['test'] = [
      '#type' => 'submit',
      '#value' => t('Test Search Query'),
      '#weight' => 51,
    ];

    if($this->pub_import_id != null) {
      $form['pub_library']['delete'] = [
        '#type' => 'submit',
        '#value' => t('Delete Search Query'),
        '#attributes' => ['style' => 'float: right;'],
        '#weight' => 51,
      ];
    }

    // Add a placeholder for the section where the test results will appear
    $form['pub_library']['results'] = [
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
    $form['pub_library']['table'] = [
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
        the same scope, but do not mix ANDs and ORs. Check the "Is Phrase" checkbox to use conjunctions 
        as part of the text to search</span>'),
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
        ];
      }


      $row["add-$i"] = [
        // '#type' => 'button',
        '#type' => 'submit',
        '#name' => 'add',
        '#value' => t('Add'),
      ];
      
    }
    $form['pub_library']['table'][$i] = $row;
    return $form;
  }  

  public function form_elements_specific_importer($form, FormStateInterface $form_state) {
    // Add elements only after a plugin has been selected.
    $plugin_id = $form_state->getValue(['plugin_id']);
    if (!$plugin_id) {
      $plugin_id = $form['plugin_id']['#default_value'];
    }
    if ($plugin_id) {
      // Instantiate the selected plugin
      // Pub Library Manager is found in tripal module: 
      // tripal/tripal/src/TripalPubLibrary/PluginManagers/TripalPubLibraryManager.php
      $pub_library_manager = \Drupal::service('tripal.pub_library');
      $plugin = $pub_library_manager->createInstance($plugin_id, []);

      // The selected plugin defines form elements specific
      // to itself.
      $form = $plugin->form($form, $form_state);
    }
    return $form;
  }

  public function form_elements_importer_selection($form, FormStateInterface $form_state) {
    // Retrieve a sorted list of available pub library plugins.
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_library_defs = $pub_library_manager->getDefinitions();
    $plugins = [];
    foreach ($pub_library_defs as $plugin_id => $def) {
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

    // This is the radio buttons which lists the types of publication / sources eg NIH PubMed database
    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => $default_value,
    ];

    $form['button_next'] = [
      '#type' => 'button',
      '#value' => 'Next'
    ];

    // This is the container that will hold the specific fields for a specific 'plugin' which represents the 
    // publication / sources eg NIH PubMed database form elements
    $form['pub_library'] = [
      '#prefix' => '<span id="edit-pub_library">',
      '#suffix' => '</span>',
    ];
    return $form;
  }


  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $public = \Drupal::database();
    $user_input = $form_state->getUserInput();
    $form_mode = NULL;
    if (isset($user_input['mode'])) {
      $form_mode = $user_input['mode'];
    }
    $trigger = $form_state->getTriggeringElement()['#name'];
    
    if ($trigger == 'op') {
      $op = $user_input['op'];
      if ($op == 'Save Search Query') {
        $_SESSION['tripal_pub_import']['perform_test'] = 0;
        // tripal_pub_library_query table columns are: pub_library_query_id, name, criteria, disabled, do_contact

        // Translate the submitted data into a variable which can be serialized into a criteria column
        // of the tripal_pub_library_query table
        $criteria_column_array = $this->criteria_convert_to_array($form, $form_state);

        // Load the plugin and initialize an instance to perform it's unique form_submit function
        // This will run plugin specific form submit operations that can alter the criteria database column
        // which stores the specific plugin importer settings (basically all the form data)
        $plugin_id = $user_input['plugin_id'];
        $pub_library_manager = NULL;
        $plugin = NULL;
        if ($plugin_id) {
          // Instantiate the selected plugin
          // Pub Library Manager is found in tripal module: 
          // tripal/tripal/src/TripalPubLibrary/PluginManagers/TripalPubLibraryManager.php
          $pub_library_manager = \Drupal::service('tripal.pub_library');
          $plugin = $pub_library_manager->createInstance($plugin_id, []);

          // The selected plugin defines form elements specific
          // to itself.
          $plugin->form_submit($form, $form_state, $criteria_column_array);
        }

        $criteria_column_serialized = serialize($criteria_column_array);

        $db_fields = [
          'name' => $user_input['loader_name'],
          'criteria' => $criteria_column_serialized,
          'disabled' => $criteria_column_array['disabled'],
          'do_contact' => $criteria_column_array['do_contact'],
        ];

        $messenger = \Drupal::messenger();

        // If form_mode is not edit, then it is a new importer
        if ($form_mode != "edit") {
          $pub_library_manager->addSearchQuery($db_fields);
          $messenger->addMessage("Importer successfully added!");
          $url = Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries');
          $form_state->setRedirectUrl($url);
        }

        // If form_mode is 'edit', this is an update to the database
        else {
          $pub_library_manager->updateSearchQuery($user_input['pub_import_id'], $db_fields);
          $messenger->addMessage("Importer successfully edited!");
          $url = Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries');
          $form_state->setRedirectUrl($url);
        }
        
        $form_state->setRebuild(FALSE);

      }
      else if ($op == 'Delete Search Query') {
        $pub_import_id = $user_input['pub_import_id'];
        $url = Url::fromUri('internal:/admin/tripal/loaders/publications/delete_publication_search_query/' . $pub_import_id);
        $form_state->setRedirectUrl($url);
      }
      else if ($op == 'Test Search Query') {
        // This session variable gets checked when the form reloads so you can find the code 
        // in the buildForm function
        $_SESSION['tripal_pub_import']['perform_test'] = 1;

        // Translate the submitted data into a variable which can be serialized into a criteria column
        // of the tripal_pub_library_query table
        $criteria_column_array = $this->criteria_convert_to_array($form, $form_state);

        // Load the plugin and initialize an instance to perform it's unique form_submit function
        // This will run plugin specific form submit operations that can alter the criteria database column
        // which stores the specific plugin importer settings (basically all the form data)
        $plugin_id = $user_input['plugin_id'];
        if ($plugin_id) {
          // Instantiate the selected plugin
          // Pub Library Manager is found in tripal module: 
          // tripal/tripal/src/TripalPubLibrary/PluginManagers/TripalPubLibraryManager.php
          $pub_library_manager = \Drupal::service('tripal.pub_library');
          $plugin = $pub_library_manager->createInstance($plugin_id, []);

          // The selected plugin defines form elements specific
          // to itself.
          $plugin->form_submit($form, $form_state, $criteria_column_array);
        }
        $_SESSION['tripal_pub_import']['perform_test_criteria_array'] = $criteria_column_array;

        // Older code before 1/5/2024
        // $_SESSION['tripal_pub_import']['perform_test_criteria_array'] = $this->criteria_convert_to_array($form, $form_state);
        $_SESSION['tripal_pub_import']['perform_test_user_input'] = $form_state->getUserInput();
      }
    }
    else {
      $_SESSION['tripal_pub_import']['perform_test'] = 0; // stop perform test from running
      $form_state->setRebuild(TRUE);
    }
  }

  /**
   * This function accepts the form state and converts the data into a criteria array
   * This criteria array is serialized and saved in the tripal_pub_library_query table as a row if Save Importer is clicked
   * This array will be given to the plugin test function to perform a test if Test Importer is clicked
   */
  public function criteria_convert_to_array($form, FormStateInterface $form_state) {
    $user_input = $form_state->getUserInput();
    
    $disabled = $user_input['disabled'];
    if ($disabled == null) {
      $disabled = 0;
    }
    $do_contact = $user_input['do_contact'];
    if ($do_contact == null) {
      $do_contact = 0;
    }
    $pub_import_id = NULL;
    if (isset($user_input['pub_import_id'])) {
      $pub_import_id = $user_input['pub_import_id'];
    }
    $criteria_column_array = [
      'remote_db' => explode('tripal_pub_library_', $user_input['plugin_id'])[1],
      // 'days' => $user_input['days'],
      'num_criteria' => $user_input['num_criteria'],
      'loader_name' => $user_input['loader_name'],
      'disabled' => $disabled,
      'do_contact' => $do_contact,
      'pub_import_id' => $pub_import_id,
      'criteria' => [],
      'form_state_user_input' => NULL, // used for edit form
    ];

    // Save form_state_user_input (for use with the edit version of this form)
    // This removes the requirement to retranslate the saved data which could become unmaintainable
    // Remove any data from user_input that is not necessary or can confuse logic processing
    
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

    return $criteria_column_array;
  }

}