<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalTextTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence widget.
 *
 * @FieldWidget(
 *   id = "chado_synonym_widget_default",
 *   label = @Translation("Chado Alias Widget"),
 *   description = @Translation("The default chado synonym widget."),
 *   field_types = {
 *     "chado_synonym_default"
 *   }
 * )
 */
class ChadoSynonymWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */

public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $field_settings = $field_definition->getSettings();


    // Get the default values.
    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $synonym_pk_id = $item_vals['synonym_pk_id'] ?? 0;
    $pub_pk_id = $item_vals['pub_pk_id'] ?? 0;
    $linker_pk_id = $item_vals['linker_pk_id'] ?? 0;
    $default_name = $item_vals['name'] ?? '';
    $synonym_sgml = $default_name;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['base_fk_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['synonym_pk_id'] = [
      '#type' => 'value',
      '#default_value' => $synonym_pk_id,
    ];
    $elements['synonym_fk_id'] = [
      '#type' => 'value',
      '#default_value' => $synonym_pk_id,
    ];
    $elements['pub_pk_id'] = [
      '#type' => 'value',
      '#default_value' => $pub_pk_id,
    ];
    $elements['pub_fk_id'] = [
      '#type' => 'value',
      '#default_value' => $pub_pk_id,
    ];
    $elements['linker_pk_id'] = [
      '#type' => 'value',
      '#value' => $linker_pk_id,
    ];
    $elements['name'] = $element + [
      '#type' => 'textarea',
      '#default_value' => $default_name,
      '#title' => '',
      '#description' => '',
      '#rows' => '',
      '#required' => FALSE,
    ];
    return $elements;
  }


  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    $biodb = \Drupal::service('tripal_chado.database');
    $sql_cvterm = "SELECT cvterm_id from {1:cvterm} where name = :synonym";
    $results = $biodb->query($sql_cvterm, [':synonym' => 'synonym']);
    foreach ($results as $record){
      $type_id = $record->cvterm_id;
    }
    
    $uniquename='assigned_synonym';
    $sql_pub = "SELECT pub_id from {1:pub} where uniquename = :uniquename and type_id = :type_id";
    $results = $biodb->query($sql_pub, [':uniquename' => $uniquename, ':type_id' => $type_id]);
    foreach ($results as $record){
      $pub_id = $record->pub_id;
    }
    if (!isset($pub_id)){
      $pub_id = $biodb->insert('pub')->fields(['uniquename' => $uniquename,'type_id' => $type_id])->execute();
    }

    // Remove any empty values that aren't mapped to a record id.
    foreach ($values as $val_key => $value) {
      # if i need to change the format for name to be sgml, i can do it here
      $values[$val_key]['synonym_sgml'] = $value['name'];
      $values[$val_key]['syn_type_id'] = $type_id;
      $values[$val_key]['pub_id'] = $pub_id;
    }
    return $values;
  }

}

