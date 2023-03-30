<?php

namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\tripal\TripalPubParser\Interfaces\TripalPubParserInterface;
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
   * A brief description for this loader. This description will be
   * presented to the site user.
   */
  public static $description = 'Import publications into Chado';

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    $chado = \Drupal::service('tripal_chado.database');
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    // Retrieve a list of available pub parser plugins.
    $pub_parser_manager = \Drupal::service('tripal.pub_parser');
    $pub_parser_defs = $pub_parser_manager->getDefinitions();
    $plugins = [];
    foreach ($pub_parser_defs as $plugin_id => $def) {
      $plugin_key = $def['label']->getUntranslatedString();
      $plugin_value = $def['select_text']->getUntranslatedString();
      $plugins[$plugin_key] = $plugin_value;
    }
    asort($plugins);

    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => 'NAL',
    ];

    return $form;
  }

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
