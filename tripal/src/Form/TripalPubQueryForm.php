<?php
/**
 * @file
 * Contains \Drupal\tripal\Form\TripalImporterForm.
 */
namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;

/**
 * Provides a publication query form object.
 */
class TripalPubQueryForm implements FormInterface {
  /**
   * {@inheritdoc}
   */
  public function getFormID() {
    return 'tripal_admin_form_tripal_pub_query';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $plugin_id = NULL) {

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
        'callback' =>  [$this, 'sourceAjaxCallback'],
        'wrapper' => 'edit-parser',
      ],
    ];

    // Provides a placeholder for the form elements for the selected plugin,
    // to be populated by the AJAX callback.
    $form['pub_parser'] = [
      '#prefix' => '<span id="edit-pub_parser">',
      '#suffix' => '</span>',
    ];

    // The placeholder will only be populated if a plugin, i.e.
    // $form['plugin_id'], has been selected. Both the plugin base
    // class and the selected plugin can each add form elements.
    $form = $this->formPlugin($form, $form_state);

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {

    // Convert the validation code into the D8/9 equivalent
    $form_values = $form_state->getValues();
    $plugin_id = $form_values['plugin_id'];
    $pub_parser_manager = \Drupal::service('tripal.pub_parser');
    $pub_parser = $pub_parser_manager->createInstance($plugin_id);
    $pub_parser_def = $pub_parser_manager->getDefinitions()[$plugin_id];

    // Now allow the query form to do validation of its form additions.
    $pub_parser->formValidate($form, $form_state);
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
  public function sourceAjaxCallback($form, &$form_state) {

    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-pub_parser', $form['pub_parser']));

    return $response;
  }

  /**
   * This function is used to rebuild the form if an ajax call is made via a
   * button. The button causes the form to be submitted. We don't want this so we
   * override the validate and submit routines on the form button. Therefore,
   * this function only needs to tell Drupal to rebuild the form
   *
   * @ingroup tripal_pub
   */
  public function tripal_pub_setup_form_ajax_button_validate($form, &$form_state) {
$trigger = $form_state->getTriggeringElement()['#name'];
dpm($trigger, "tripal_pub_setup_form_ajax_button_validate() called, not yet implemented");
    $form_state->setRebuild(TRUE);
  }

  /**
   * This function is just a dummy to override the default form submit on ajax
   * calls for buttons
   *
   * @ingroup tripal_pub
   */
  public function tripal_pub_setup_form_ajax_button_submit($form, &$form_state) {
$trigger = $form_state->getTriggeringElement()['#name'];
dpm($trigger, "tripal_pub_setup_form_ajax_button_submit() called, not yet implemented");
    // do nothing
  }

}
