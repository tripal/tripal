<?php

namespace Drupal\tripal_chado\Plugin\ChadoPubImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * GFF3 Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_pub_loader",
 *    label = @Translation("Chado Publication Loader"),
 *    description = @Translation("Imports publications into Chado"),
 *    file_types = {"bib", "bibtex"},
 *    upload_description = @Translation("Please provide the GFF3 file."),
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
   * {@inheritDoc}
   */
  public function formSubmit($form, $form_state) {
  }

  /**
   * {@inheritDoc}
   */

  public function postRun() {
  }

  /**
   * {@inheritDoc}
   */

  public function formValidate($form, $form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function run() {
  }


}