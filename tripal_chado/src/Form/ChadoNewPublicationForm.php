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
    // $html = "IGNITION";
    // $form['header'] = [
    //     '#type' => 'markup',
    //     '#markup' => $html
    // ];
    // unset($html); 

    $form = $this->form_elements_importer_selection($form);

    return $form;
  }

  public function form_elements_importer_selection($form) {
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

    $form['#prefix'] = '<div id="pub_importer_main_form">';
    $form['#suffix'] = '</div>';

    // RISH: This is the radio buttons which lists the types of publication / sources eg NIH PubMed database
    $form['plugin_id'] = [
      '#title' => t('Select a source of publications'),
      '#type' => 'radios',
      '#description' => t("Choose one of the sources above for loading publications."),
      '#required' => TRUE,
      '#options' => $plugins,
      '#default_value' => NULL,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'], // calls function within this class: function formAjaxCallback
        'wrapper' => 'edit-parser',
      ],
    ];

    // Doug: A placeholder for the form elements for the selected plugin,
    // to be populated by the AJAX callback.

    // RISH: This is the container that will hold the specific fields for a specific 'plugin' which represents the 
    //       publication / sources eg NIH PubMed database form elements
    $form['pub_parser'] = [
      '#prefix' => '<span id="edit-pub_parser">',
      '#suffix' => '</span>',
    ];

    return $form;

  }


  /**
   * {@inheritDoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
  }

}