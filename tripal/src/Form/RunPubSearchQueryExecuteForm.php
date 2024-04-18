<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class RunPubSearchQueryExecuteForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_run_publication_search_query_execute_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $html = 'Execute a job for importing publication using the importer? Or maybe link to a ImporterForm - need to check back code';
    $form['heading'] = [
      '#markup' => $html
    ];

    $public = \Drupal::database();
    $pub_importers_query = $public->select('tripal_pub_library_query','tpi');
    $pub_importers_count = $pub_importers_query->countQuery()->execute()->fetchField();

    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_queries = $pub_library_manager->getSearchQueries();
    $pub_queries_count = count($pub_queries);

    return $form;
  }


  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $public = \Drupal::database();
    $user_input = $form_state->getUserInput();
    $trigger_element = $form_state->getTriggeringElement();
    // if (stripos($trigger_element['#name'],'delete-') !== FALSE) {
    //   $pub_library_query_id = explode('delete-',$trigger_element['#name'])[1];
    //   $url = Url::fromUri('internal:/admin/tripal/loaders/publications/delete_publication_search_query/' . $pub_library_query_id);
    //   $form_state->setRedirectUrl($url);
    // }
  }

}