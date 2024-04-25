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
    $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $default_value = "";
    // dpm($form_state);
    $form['query_id'] = [
        '#title' => t('Query ID'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#value' => $default_value,
        '#description' => t("Required to import the publications based on query id"),
    ];

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