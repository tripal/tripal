<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;


class DeletePubSearchQueryForm extends ConfirmFormBase {

  /**
   * ID of the publication query.
   *
   * @var int $pub_library_query_id
   */
  protected $pub_library_query_id;

  /**
   * Name of the publication query.
   *
   * @var string $pub_library_query_name
   */
  protected $pub_library_query_name;

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_new_pub_search_query_form';
  }

  /**
   * Build form.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @param ?int $pub_library_query_id
   *
   * @return array
   */
  public function buildForm(array $form, FormStateInterface $form_state, $pub_library_query_id = NULL) {
    $this->pub_library_query_id = $pub_library_query_id;

    // Lookup the name of the publication query from the supplied ID
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_query = $pub_library_manager->getSearchQuery($this->pub_library_query_id);
    $this->pub_library_query_name = $pub_query->name;

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t(
      'Are you sure you want to delete the publication importer %name?',
      ['%name' => $this->pub_library_query_name]
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->t('You are deleting a publication importer specification. This action cannot be undone.');
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    return new Url('tripal.data_loaders.publication_loaders.manage_publication_search_queries');
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pub_library_manager->deleteSearchQuery($this->pub_library_query_id);
    $this->messenger()->addMessage($this->t(
      'The publication importer %name has been deleted',
      ['%name' => $this->pub_library_query_name])
    );
    $form_state->setRedirectUrl($this->getCancelUrl());
  }

}
