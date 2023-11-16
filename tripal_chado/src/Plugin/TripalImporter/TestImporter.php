<?php
namespace Drupal\tripal_chado\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * GFF3 Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "test_disable_button",
 *    label = @Translation("Test Disable Button"),
 *    description = @Translation("A test importer which should NOT BE COMMITTED that allows testing of this functionality via the UI."),
 *    file_types = {"txt"},
 *    upload_description = @Translation("Please provide a plain text file of any format really as we don't intend to use it."),
 *    upload_title = @Translation("Random Text File"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import"),
 *    file_upload = False,
 *    file_load = True,
 *    file_remote = False,
 *    file_local = True,
 *    file_required = False,
 *    cardinality = 1,
 *    use_button = True,
 *    submit_disabled = True,
 *    menu_path = "",
 *    callback = "",
 *    callback_module = "",
 *    callback_path = "",
 *  )
 */
class TestImporter extends ChadoImporterBase {

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    // Create a select field that will update the contents
    // of the textbox below.
    $form['example_select'] = [
      '#type' => 'select',
      '#title' => 'How do you feel about Tripal?',
      '#options' => [
        '1' => "It's Amazing!",
        '2' => "Powerful but challenging",
        '3' => "Just let me test the PR, ok!?!",
      ],
      '#empty_option' => ' - Select - ',
      '#ajax' => [
        // Callback is a method in this class.
        'callback' => [$this, 'myAjaxCallback'],
        // Refresh the whole importer form in order to capture the button.
        'wrapper' => 'tripal-admin-form-tripalimporter',
      ],
    ];

    // Create a textbox that will be updated
    // when the user selects an item from the select box above.
    $form['output'] = [
      '#type' => 'textfield',
      '#size' => '60',
      '#disabled' => TRUE,
      '#value' => '',
    ];

    // If there's a value submitted for the select list let's set the textfield value.
    if ($selectedValue = $form_state->getValue('example_select')) {

      // Set the form state storage to indicate
      // the submit button can now be enabled!
      $storage = $form_state->getStorage();
      $storage['disable_TripalImporter_submit'] = FALSE;
      $form_state->setStorage($storage);

      // Also reply to the reviewer with peace and gratitude.
      if ($selectedValue == 1) {
        $form['output']['#value'] = "That's Great!";
      }
      if ($selectedValue == 2) {
        $form['output']['#value'] = "True Enough. You are surrounded by friends!";
      }
      if ($selectedValue == 3) {
        $form['output']['#value'] = "Ok ;-) We can do that!";
      }
    }

    return $form;
  }

  /**
   * Ajax Callback triggered when you select from the select list.
   */
  public function myAjaxCallback(array &$form, $form_state) {
    // Fix the id of the form.
    // Drupal appends a unique code to id's after AJAX submission for some reason.
    // This line resets that so the AJAX can fire a second time and still find the wrapper ;-)
   $form['#attributes']['id'] = 'tripal-admin-form-tripalimporter';

    // Return the prepared textfield.
    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {}

  /**
   * @see TripalImporter::formSubmit()
   */
  public function formSubmit($form, &$form_state) {}

  /**
   * @see TripalImporter::run()
   */
  public function run() {}

  /**
   * {@inheritdoc}
   */
  public function postRun() {}
}
