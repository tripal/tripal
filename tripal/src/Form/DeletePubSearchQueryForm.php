<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class DeletePubSearchQueryForm extends FormBase {

  private $form_state_previous_user_input = null;
  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_new_pub_search_query_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $pub_library_query_id = null) {
    $public = \Drupal::database();
    $publication = $public->select('tripal_pub_library_query', 'tpi')->fields('tpi')->condition('pub_library_query_id', $pub_library_query_id, '=')->execute()->fetchObject();
    $form['are_you_sure'] = [
      '#markup' => 'Are you sure you want to delete "' . $publication->name . '"?<br />'
    ];

    $form['pub_library_query_id'] = [
      '#type' => 'hidden',
      '#value' => $pub_library_query_id 
    ];    

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => 'Confirm'
    ];
    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // $form_state_values = $form_state->getValues();
    $public = \Drupal::database();
    $user_input = $form_state->getUserInput();
    // $trigger = $form_state->getTriggeringElement()['#name'];

    $pub_library_query_id = $user_input['pub_library_query_id'];

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_library_manager->deleteSearchQuery($pub_library_query_id);

    $url = Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries');
    $form_state->setRedirectUrl($url);
            
    $messenger = \Drupal::messenger();
    $messenger->addMessage("Publication importer has been deleted.");
  }  
}