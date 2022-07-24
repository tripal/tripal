<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal_chado\Services\ChadoMView;

class ChadoMviewPopulateForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_mviews_populate_form';
  }


  /**
   * Just a simple form for confirming deletion of a custom table
   *
   */
  public function buildForm(array $form, FormStateInterface $form_state, $mview_id = null) {

    $mview = ChadoMView::load($mview_id);

    $form = [];
    $form['mview_id'] = [
      '#type' => 'value',
      '#value' => $mview_id,
    ];

    $form['sure'] = [
      '#type' => 'markup',
      '#markup' => '<p>Please confirm you want to populate the "' . $mview->tableName() .
      '" materialized view in the "' . $mview->chadoSchema() . '" schema?</p>',
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => 'Populate',
    ];
    $form['cancel'] = [
      '#type' => 'submit',
      '#value' => 'Cancel',
    ];
    return $form;
  }

  /**
   * form submit hook for the tripal_custom_tables_delete_form form.
   *
   * @param $form
   * @param $form_state
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();

    $action = $values['op'];
    $mview_id = $values['mview_id'];

    if (strcmp($action, 'Populate') == 0) {
      $mview = ChadoMView::load($mview_id);
      $current_user = \Drupal::currentUser();
      $args = [$mview_id];
      tripal_add_job("Populate materialized view '$mview->tableName()'", 'tripal_chado',
          'chado_populate_mview', $args, $current_user->id());
    }
    else {
      \Drupal::messenger()->addMessage(t("No action performed."));
    }
    // drupal_goto("admin/tripal/storage/chado/custom_tables");
    $response = new RedirectResponse(\Drupal\Core\Url::fromUserInput('/admin/tripal/storage/chado/mviews')->toString());
    $response->send();
  }
}




?>