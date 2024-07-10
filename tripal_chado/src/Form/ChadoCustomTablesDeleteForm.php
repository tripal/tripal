<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\tripal_chado\Services\ChadoCustomTable;

class ChadoCustomTablesDeleteForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_custom_tables_delete_form';
  }


  /**
   * Just a simple form for confirming deletion of a custom table
   *
   */
  public function buildForm(array $form, FormStateInterface $form_state, $table_id = null) {

    $custom_tables = \Drupal::service('tripal_chado.custom_tables');
    $custom_table = $custom_tables->loadById($table_id);

    $form = [];
    $form['table_id'] = [
      '#type' => 'value',
      '#value' => $table_id,
    ];

    $form['sure'] = [
      '#markup' => '<p>Are you sure you want to delete the "' . $custom_table->getTableName() .
        '" custom table in the "' . $custom_table->getChadoSchema() . '" schema?</p>',
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
    $table_id = $values['table_id'];

    if (strcmp($action, 'Delete') == 0) {
      $custom_tables = \Drupal::service('tripal_chado.custom_tables');
      $custom_table = $custom_tables->loadById($table_id);
      $success = $custom_table->delete();
      if($success == TRUE) {
        \Drupal::messenger()->addMessage(t("Custom table successfully deleted"));
      }
      else {
        \Drupal::messenger()->addError(t("An error occurred when trying to delete custom table. Check the report logs."));
      }
    }
    else {
      \Drupal::messenger()->addMessage(t("No action performed."));
    }
    // drupal_goto("admin/tripal/storage/chado/custom_tables");
    $response = new RedirectResponse(\Drupal\Core\Url::fromUserInput('/admin/tripal/storage/chado/custom_tables')->toString());
    $response->send();
  }
}




?>
