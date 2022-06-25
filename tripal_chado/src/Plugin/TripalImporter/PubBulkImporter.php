<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;

/**
 * Bulk Publication Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "chado_pub_bulk",
 *    label = @Translation("Chado Bulk Publication Importer"),
 *    description = @Translation("Create and modify importers that can connect to and retrieve publications from remote databases."),
 *    file_types = {},
 *    upload_description = @Translation(""),
 *    upload_title = @Translation("File Upload"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import"),
 *    file_upload = False,
 *    file_load = False,
 *    file_remote = False,
 *    file_required = False,
 *    cardinality = 0,
 *    menu_path = "",
 *    callback = "tripal_pub_importers_list",
 *    callback_module = "tripal_chado",
 *    callback_path = "includes/loaders/tripal_chado.pub_importers.inc",
 *  )
 */
class PubBulkImporter extends ChadoImporterBase {

  /**
   * {@inheritdoc}
   */
  public function form($form, $form_state) {

  }
  /**
   * {@inheritdoc}
   */
  public function formValidate($form, $form_state) {

  }
  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, $form_state) {

  }
  /**
   * {@inheritdoc}
   */
  public function run() {

  }
  /**
   * {@inheritdoc}
   */
  public function postRun() {
  }
}
