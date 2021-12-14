<?php

namespace Drupal\tripal_chado\Form;

use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Link;

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
  
    // get details about this table entry
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = :table_id";
    $results = db_query($sql, [':table_id' => $table_id]);
    $entry = $results->fetchObject();
  
    // if this is a materialized view then don't allow editing with this function
    if ($entry->mview_id) {
      drupal_set_message("This custom table is a materialized view. Please use the " . Link::fromTextAndUrl('Materialized View', Url::fromUserInput('admin/tripal/storage/chado/mviews')) . " interface to delete it.", 'error');
      drupal_goto("admin/tripal/storage/chado/custom_tables");
      return [];
    }
  
  
    $form = [];
    $form['table_id'] = [
      '#type' => 'value',
      '#value' => $table_id,
    ];
  
    $form['sure'] = [
      '#type' => 'markup',
      '#markup' => '<p>Are you sure you want to delete the "' . $entry->table_name . '" custom table?</p>',
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
    // drupal_set_message(print_r($values, true));
    
    $action = $values['op'];
    $table_id = $values['table_id'];
  
    if (strcmp($action, 'Delete') == 0) {
      $result = chado_delete_custom_table($table_id);
      if($result == TRUE) {
        drupal_set_message(t("Custom table successfully deleted"));
      }
      else {
        drupal_set_message(t("An error occurred when trying to delete custom table. Check the report logs."));
      }
    }
    else {
      drupal_set_message(t("No action performed."));
    }
    // drupal_goto("admin/tripal/storage/chado/custom_tables");
    $response = new RedirectResponse(\Drupal\Core\Url::fromUserInput('/admin/tripal/storage/chado/custom_tables')->toString());
    $response->send();
  }  
}




?>