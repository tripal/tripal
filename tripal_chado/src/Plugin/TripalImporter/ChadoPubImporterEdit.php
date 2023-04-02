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

// cf. src/Entity/ChadoTermMapping.php
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

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    // Always call the parent form to ensure Chado is handled properly.
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
    // Always call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

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

    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => NULL,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'],
        'wrapper' => 'edit-parser',
      ],
    ];

    // A placeholder for the plugin form elements populated
    // by the AJAX callback.
    $form['pub_parser'] = [
      '#prefix' => '<span id="edit-pub_parser">',
      '#suffix' => '</span>',
    ];

    // The remainder of the form is only populated if
    // plugin_id has been selected. The plugin base class
    // and the selected plugin can each add form elements.
    $form = $this->formPlugin($form, $form_state);

    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function formSubmit($form, &$form_state) {
    dpm('Editor Submit not implemented'); //@@@
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
      $pub_parser_manager = \Drupal::service('tripal.pub_parser');
      $plugin = $pub_parser_manager->createInstance($plugin_id, []);

      // The plugin manager defines form elements used by
      // all pub_parser plugins.
      $form = $pub_parser_manager->form($form, $form_state);

      // The selected plugin defines form elements specific
      // to itself.
      $form = $plugin->form($form, $form_state);
    }

    return $form;
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
