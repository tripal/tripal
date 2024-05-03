<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Render\Markup;


/**
 * Chado Pub Search Query Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "pub_search_query_loader",
 *    label = @Translation("Pub Search Query Loader"),
 *    description = @Translation("Import a Pub Search Query file into Chado"),
 *    file_types = {"fasta","txt","fa","aa","pep","nuc","faa","fna"},
 *    upload_description = @Translation("Please provide a file."),
 *    upload_title = @Translation("File"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import Pub Search Query"),
 *    file_upload = False,
 *    file_remote = False,
 *    file_local = False,
 *    file_required = False,
 *  )
 */
class PubSearchQueryImporter extends ChadoImporterBase {

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    // $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);
    $form_state_values = $form_state->getValues();
    // dpm($form_state_values);


    $query_id = "";
    $build_args = $form_state->getBuildInfo();
    if ($build_args['args'][1] != NULL) {
      $query_id = $build_args['args'][1];
    }
    // dpm($form_state);
    $form['query_id'] = [
        '#title' => t('Query ID'),
        '#type' => 'hidden',
        '#required' => TRUE,
        '#value' => $query_id,
        '#description' => t("Required to import the publications based on query id"), 
    ];

    // If query_id is unset, we need to display library options and an autocomplete for the search query
    if ($query_id == "") {
      // Get list of database/libraries
      $pub_library_manager = \Drupal::service('tripal.pub_library');
      $pub_library_defs = $pub_library_manager->getDefinitions();
      $plugins = [];
      foreach ($pub_library_defs as $plugin_id => $def) {
        $plugin_key = $def['id'];
        $plugin_value = $def['label']->render();
        $plugins[$plugin_key] = $plugin_value;
      }
      asort($plugins);
      foreach ($plugins as $plugin_key => $plugin) {
        $library_options[$plugin_key] = $plugin;
      }

      $form['database'] = [
        '#title' => t('Database'),
        '#type' => 'select',
        '#required' => TRUE,
        '#options' => $library_options,
        '#description' => 'The database of the search query',
        // '#ajax' => [
        //   // 'callback' => '::database_on_change', // don't forget :: when calling a class method.
        //   'callback' => [$this, 'database_on_change'], //alternative notation
        //   'disable-refocus' => FALSE, // Or TRUE to prevent re-focusing on the triggering element.
        //   'event' => 'change',
        //   'wrapper' => 'edit-output', // This element is updated with this AJAX callback.
        //   'progress' => [
        //     'type' => 'throbber',
        //     'message' => 'Verifying entry...',
        //   ],
        // ],
        //
        '#ajax' => [
          'callback' =>  [$this::class, 'database_on_change'],
          'wrapper' => 'edit-output',
        ],
      ];

      // For debug purposes when a user selects from the database / library select list
      // $form['output'] = [
      //   '#type' => 'hidden',
      //   '#size' => '60',
      //   '#disabled' => TRUE,
      //   '#value' => 'Hello, Drupal!!1',      
      //   '#prefix' => '<div id="edit-output">',
      //   '#suffix' => '</div>',
      // ]; 
      
      $form['search_query_name'] = [
        '#title' => t('Search query name'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#autocomplete_path' => 'admin/tripal/autocomplete/pubsearchqueryname',
        '#autocomplete_route_name' => 'tripal.pubsearchqueryname_autocomplete',
        '#autocomplete_query_parameters' => ['db' => 'dummyval'],
        '#description' => t("The search query name"),
        '#prefix' => '<div id="edit-search-query-name">',
        '#suffix' => '</div>',
        // '#ajax' => [
        //   'callback' => [$this::class, 'search_query_name_on_change'],
        //   'wrapper' => 'pub-query-details',
        //   'event' => 'autocompleteclose',
        //   // 'event' => 'change'
        // ],
      ];

      // $form['test_click'] = [
      //   '#type' => 'textfield',
      //   '#value' => 'OK',
      //   '#ajax' => [
      //     'callback' => [$this::class, 'test_click_on_change'],
      //     'wrapper' => 'edit-test-click',
      //     'event' => 'click',
      //     // 'event' => 'change'
      //   ],
      // ];

      $form['button_view_query_details'] = [
        '#type' => 'button',
        '#button_type' => 'button',
        '#value' => 'Preview query details'
        // '#ajax' => [
        //   'callback' => [$this::class, 'test_click_on_change'],
        //   'wrapper' => 'pub-query-details',
        //   'event' => 'click',
        //   // 'event' => 'change'
        // ],
      ];

      // $form['button_view_query_details'] = [
      //   '#markup' => Markup::create('<div class="button">Preview query details</div>'),

      // ];

      if (isset($form_state_values['op'])) {
        $op = $form_state_values['op'];
        if ($op = 'Preview query details') {
          $query_id = -1;
          if ($form_state_values['query_id'] != "") {
            $query_id = $form_state_values['query_id'];
          }
          else {
            $search_query_name = $form_state_values['search_query_name'];
            $start_bracket_pos = strrpos($search_query_name, '(');
            $right_string = substr($search_query_name, $start_bracket_pos);
            $right_string = ltrim($right_string, '(');
            $query_id = rtrim($right_string, ')');
          }
          $headers = [
            'Importer Name',
            'Database',
            'Search String',
            'Disabled',
            'Create Contact',
          ];
          $form['pub_query_details'] = [
            '#type' => 'table',
            '#header' => $headers,
            '#prefix' => '<div id="pub_manager_table">',
            '#suffix' => '</div>',
          ];

          $public = \Drupal::database();
          $query = $public->select('tripal_pub_library_query','tpi')->fields('tpi')->condition('pub_library_query_id', $query_id, '=');
          // $pub_importers_count = $query->countQuery()->execute()->fetchField();
          // dpm($pub_importers_count);
          $results = $query->execute();
          foreach ($results as $pub_query) {
            $criteria_column_array = unserialize($pub_query->criteria);
    
            $search_string = "";
            foreach ($criteria_column_array['criteria'] as $criteria_row) {
              $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
            }
    
            $disabled = $criteria_column_array['disabled'];
            if ($disabled <= 0) {
              $disabled = 'No';
            }
            else {
              $disabled = 'Yes';
            }
    
            $do_contact = $criteria_column_array['do_contact'];
            if ($do_contact <= 0) {
              $do_contact = 'No';
            }
            else {
              $do_contact = 'Yes';
            }
    
            $row = [];
    
            // This should contain edit test and import pubs links @TODO

            $row['col-1'] = [
              '#markup' => $pub_query->name
            ];
            $row['col-2'] = [
              '#markup' => $criteria_column_array['remote_db']
            ];
    
            // Search string
            $row['col-3'] = [
              '#markup' => $search_string
            ];
    
            // Disabled
            $row['col-4'] = [
              '#markup' => $disabled
            ];
    
            // Create contact
            $row['col-5'] = [
              '#markup' => $do_contact
            ];
    
    
            $form['pub_query_details'][] = $row;
          }
        }
      }
      


    }

    // If the query id is set, display the data
    if ($build_args['args'][1] != NULL) {
      $public = \Drupal::service('database');
      $row = $public->select('tripal_pub_library_query', 'tpi')
        ->fields('tpi')
        ->condition('pub_library_query_id', $query_id, '=')
        ->execute()->fetchObject();
      $criteria_column_array = unserialize($row->criteria);
      // Get search string from the criteria data
      $search_string = "";
      foreach ($criteria_column_array['criteria'] as $criteria_row) {
        $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
      }
      // Get the database from the criteria data
      $db_string = $criteria_column_array['remote_db'];
      $markup = "<h4>Search Query Details</h4>";
      $markup .= "<p>Name: " . $row->name . "</p>";
      $markup .= "<p>Database: " . $db_string . "</p>";
      $markup .= "<p>Search string: " . $search_string . "</p>";
      $form['query_info'] = [
        '#markup' => $markup
      ];
    }

    return $form;
  }

  public static function test_click_on_change(array &$form, $form_state) {
    $user_input = $form_state->getUserInput();

    $response = new AjaxResponse();

    $response->addCommand(new ReplaceCommand('#pub-query-details', 'WOW'));
    //$response->addCommand(new ReplaceCommand('#pub-query-details', $form['pub_query_details']));
    // $response->addCommand(new InvokeCommand('#pub-query-details', 'html', ['OK']));
    
    return $response;

    // return $form['pub_query_details'];

  }

  public static function database_on_change(array &$form, $form_state) {
    $user_input = $form_state->getUserInput();

    // database / library value when changed
    $database = $user_input['database'];

    $response = new AjaxResponse();

    // Used for debugging purposes
    // $form['output']['#value'] = "Interesting";
    // $response->addCommand(new ReplaceCommand('#edit-output', $database));

    // This adjusts the autocomplete path for search query name
    $autocomplete_path = $form['search_query_name']['#autocomplete_path'];
    $autocomplete_path_parts = explode('db=', $autocomplete_path);
    $autocomplete_path = base_path() . $autocomplete_path_parts[0]. '?db=' . $database;
    $response->addCommand(new InvokeCommand('#edit-search-query-name input', 'attr', ['data-autocomplete-path', $autocomplete_path]));
    
  
    return $response;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    // $chado = \Drupal::service('tripal_chado.database');

    // $form_state_values = $form_state->getValues();

    // $organism_id = $form_state_values['organism_id'];

  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {
    $arguments = $this->arguments['run_args'];

    $organism_id = $arguments['organism_id'];

    // $this->loadFasta($file_path, $organism_id, $type, $re_name, $re_uname, $re_accession,
    //   $db_id, $rel_type, $re_subject, $parent_type, $method, $analysis_id,
    //   $match_type);
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {

  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    $form_state->setRebuild(TRUE);
  }  
}