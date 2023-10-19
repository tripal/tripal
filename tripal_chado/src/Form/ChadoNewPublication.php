<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


class ChadoNewPublicationForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_new_publication_form';
  }

  /**
   * {@inheritDoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // The link to add a new publication
    $html = "<ul class='action-links'>";
    $html .= '  <li>' . Link::fromTextAndUrl('New Importer', Url::fromUri('internal:/admin/tripal/loaders/publications/new_publication'))->toString() . '</li>';
    $html .= '</ul>';
    $form['new_publication_link'] = [
        '#type' => 'markup',
        '#markup' => $html
    ];
    unset($html); 


    return $form;
  }




  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
  }

}