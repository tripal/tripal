<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal string type widget.
 *
 * @FieldWidget(
 *   id = "chado_linker_contact_widget_default",
 *   label = @Translation("Chado Contact"),
 *   description = @Translation("Add a linked Chado contact to the content type."),
 *   field_types = {
 *     "chado_linker_contact_default"
 *   }
 * )
 */
class ChadoLinkerContactWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of contacts.
    $contacts = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('contact', 'c');
    $query->leftJoin('cvterm', 'cvt', 'c.type_id = cvt.cvterm_id');
    $query->fields('c', ['contact_id', 'name', 'description']);
    $query->addField('cvt', 'name', 'contact_type');
    $query->orderBy('name', 'contact_type');
    $results = $query->execute();
    while ($contact = $results->fetchObject()) {
      $contact_name = $contact->name;
      // Change the non-user-friendly 'null' contact, which is specified by chado.
      if ($contact->name == 'null') {
        $contact->name = '-- Unknown --';  // This will sort to the top.
      }
      if ($contact->contact_type) {
        $contact_name .= ' (' . $contact->contact_type . ')';
      }
      $contacts[$contact->contact_id] = $contact_name;
    }
    natcasesort($contacts);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $contact_id = $item_vals['contact_id'] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['linker_id'] = [
      '#type' => 'value',
      '#default_value' => $linker_id,
    ];
    $elements['link'] = [
      '#type' => 'value',
      '#default_value' => $link,
    ];
//@todo add in type_id and rank here
    $elements['contact_id'] = $element + [
      '#type' => 'select',
      '#options' => $contacts,
      '#default_value' => $contact_id,
      '#placeholder' => $this->getSetting('placeholder'),
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    $storage_settings = $this->getFieldSetting('storage_plugin_settings');
// @to-do handle type_id and rank when present
    $linker_table = $storage_settings['linker_table'];
//    $rank_term = $this->sanitizeKey($mapping->getColumnTermId($linker_table, 'rank'));

    // Remove any empty values.
    foreach ($values as $val_key => $value) {
      if ($value['contact_id'] == '') {
        // If there is a record_id, then we need to delete the
        // existing linker record in chado. How to do this, though? @@@
        // This will be called for both validate and update, we can't
        // delete during the validate call
        if ($value['record_id']) {
          // We need to pass in this record to chado storage to
          // be deleted there, but also we need to have the
          // correct primitive type for this field, so change
          // from empty string to zero.
          $values[$val_key]['contact_id'] = 0;
        }
        else {
          unset($values[$val_key]);
        }
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      $values[$val_key]['_weight'] = $i;
//      $values[$val_key][$rank_term] = $i;
      $i++;
    }

    return $values;
  }
}
