<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;


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
 *    file_required = True,
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

    $query_id = "";
    $build_args = $form_state->getBuildInfo();
    if ($build_args['args'][1] != NULL) {
      $query_id = $build_args['args'][1];
    }
    // dpm($form_state);
    $form['query_id'] = [
        '#title' => t('Query ID'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#value' => $query_id,
        '#description' => t("Required to import the publications based on query id"), 
    ];

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

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $chado = \Drupal::service('tripal_chado.database');

    $form_state_values = $form_state->getValues();

    $organism_id = $form_state_values['organism_id'];

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

  }  
}