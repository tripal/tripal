<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * ChadoPubImporter implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_pub_loader",
 *    label = @Translation("Chado Publication Loader"),
 *    description = @Translation("Imports publications into Chado"),
 *    file_types = {"bib", "bibtex"},
 *    upload_description = @Translation("Please provide the data file."),
 *    upload_title = @Translation("BibTex File"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import Publications"),
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
class ChadoPubImporter extends ChadoImporterBase {

  /**
   * The name of this loader. This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Publication Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_pub_loader';

  /**
   * {@inheritDoc}
   */
  public function formSubmit($form, &$form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function postRun() {
  }

  /**
   * {@inheritDoc}
   */
  public function formValidate($form, &$form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function run() {
  }

}
