<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado contact widget.
 *
 * @FieldWidget(
 *   id = "chado_contact_widget_default",
 *   label = @Translation("Chado Contact Widget"),
 *   description = @Translation("The default contact widget."),
 *   field_types = {
 *     "chado_contact_type_default",
 *     "chado_contact_by_role_type_default"
 *   }
 * )
 */
class ChadoContactWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'contact_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();

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
      if ($contact_name == 'null') {
        $contact_name = '-- Unknown --';  // This will sort to the top.
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
    $contact_id = $item_vals[$linker_fkey_column] ?? 0;

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
    // pass the foreign key name through the form for massageFormValues()
    $elements['linker_fkey_column'] = [
      '#type' => 'value',
      '#default_value' => $linker_fkey_column,
    ];
    $elements[$linker_fkey_column] = $element + [
      '#type' => 'select',
      '#options' => $contacts,
      '#default_value' => $contact_id,
      '#empty_option' => '-- Select --',
    ];

    // If there is a type_id and the value is not already set, then we want to
    // use the cvterm of the field as the default.
    if (array_key_exists('linker_type_id', $property_definitions)) {

      if (empty($item['linker_type_id'])) {
        $termIdSpace = $this->getFieldSetting('termIdSpace');
        $termAccession = $this->getFieldSetting('termAccession');

        $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
        $idSpace = $idSpace_manager->loadCollection($termIdSpace);
        $term = $idSpace->getTerm($termAccession);

        $item['linker_type_id'] = $term->getInternalId();
      }

      $elements['linker_type_id'] = [
        '#type' => 'value',
        '#default_value' => $item['linker_type_id'],
      ];
    }

    // If there is a rank and it is not already set,
    // then we want to use 0 as the default.
    if (array_key_exists('linker_rank', $property_definitions)) {
      $default_value = $item_vals['linker_rank'] ?? 0;
      $elements['linker_rank'] = [
        '#type' => 'value',
        '#default_value' => $default_value,
      ];
    }

    // If there is a pub_id and it is not already set, then we want to use
    // the null pub which has an id of 1.
    if (array_key_exists('linker_pub_id', $property_definitions)) {
      $default_value = $item_vals['linker_pub_id'] ?? 1;
      $elements['linker_pub_id'] = [
        '#type' => 'value',
        '#default_value' => $default_value,
      ];
    }

    // Save initial values to allow later handling of the "Remove" button
    $storage = $form_state->getStorage();
    if (!($storage['initial_values'][$delta] ?? FALSE)) {
      $storage['initial_values'][$delta] = [
        'linker_id' => $linker_id,
        'linker_fkey_column' => $linker_fkey_column,
        $linker_fkey_column => $contact_id,
      ];
      $form_state->setStorage($storage);
    }

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Handle any empty values.
    $retained_records = [];
    $next_delta = 0;
    foreach ($values as $val_key => $value) {
      $retained_records[$val_key] = $value['contact_id'];
      // Foreign key is usually contact_id, but not always.
      $linker_fkey_column = $value['linker_fkey_column'];
      if ($value[$linker_fkey_column] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no contact_id, this means
          // we need to pass in this record to chado storage to
          // have the linker record be deleted there. To do this,
          // we need to have the correct primitive type for this
          // field, so change from empty string to zero.
          $values[$val_key][$linker_fkey_column] = 0;
        }
        else {
          unset($values[$val_key]);
        }
      }
      if ($next_delta <= $val_key) {
        $next_delta = $val_key + 1;
      }
    }

    // If there were any values in the initial values that are not
    // present in the current form state, then the "Remove" button
    // was clicked. We need to include these in the values array
    // so that chado storage is informed to delete them in chado.
    $storage_values = $form_state->getStorage();
    $initial_values = $storage_values['initial_values'];
    foreach ($initial_values as $delta => $initial_value) {
      $linker_fkey_column = $initial_value['linker_fkey_column'];
      $contact_id = $initial_value[$linker_fkey_column];
      if ($contact_id and !in_array($contact_id, $retained_records)) {
        // This delta was removed from the original form. Add back a
        // value so that chado storage knows to remove the chado record.
        $values[$next_delta]['linker_id'] = $initial_value['linker_id'];
        $values[$next_delta][$linker_fkey_column] = 0;
        $next_delta++;
      }
    }

    // Reset the weights
    $i = 0;
    foreach ($values as $val_key => $value) {
      $values[$val_key]['_weight'] = $i;
      $i++;
    }

    return $values;
  }
}
