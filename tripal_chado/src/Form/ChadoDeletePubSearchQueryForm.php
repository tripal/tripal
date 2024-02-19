<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class ChadoDeletePubSearchQueryForm extends FormBase {

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
  public function buildForm(array $form, FormStateInterface $form_state, $pub_import_id = null) {
    $public = \Drupal::database();
    $publication = $public->select('tripal_pub_import', 'tpi')->fields('tpi')->condition('pub_import_id', $pub_import_id, '=')->execute()->fetchObject();
    $form['are_you_sure'] = [
      '#markup' => 'Are you sure you want to delete "' . $publication->name . '"?<br />'
    ];

    $form['pub_import_id'] = [
      '#type' => 'hidden',
      '#value' => $pub_import_id 
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

    $pub_import_id = $user_input['pub_import_id'];

    $public->delete('tripal_pub_import')
    ->condition('pub_import_id', $pub_import_id, '=')
    ->execute();

    $url = Url::fromUri('internal:/admin/tripal/loaders/publications/manage_publication_search_queries');
    $form_state->setRedirectUrl($url);
            
    $messenger = \Drupal::messenger();
    $messenger->addMessage("Publication importer has been deleted.");
  }  
}