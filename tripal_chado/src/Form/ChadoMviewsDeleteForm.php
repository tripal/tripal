<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal_chado\Services\ChadoMView;

class ChadoMviewsDeleteForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_mviews_delete_form';
  }


  /**
   * Just a simple form for confirming deletion of a custom table
   *
   */
  public function buildForm(array $form, FormStateInterface $form_state, $mview_id = null) {

    $mview = ChadoMView::loadMview($mview_id);

    $form = [];
    $form['mview_id'] = [
      '#type' => 'value',
      '#value' => $mview_id,
    ];

    $form['sure'] = [
      '#type' => 'markup',
      '#markup' => '<p>Are you sure you want to delete the "' . $mview->tableName() .
      '" materialized view in the "' . $mview->chadoSchema() . '" schema?</p>',
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => 'Delete',
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

    if (strcmp($action, 'Delete') == 0) {
      $mview = ChadoMView::loadMview($mview_id);
      $success = $mview->destroy();
      if($success == TRUE) {
        \Drupal::messenger()->addMessage(t("The materialized view was successfully deleted"));
      }
      else {
        \Drupal::messenger()->addError(t("An error occurred when trying to delete materialized view. Check the report logs."));
      }
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