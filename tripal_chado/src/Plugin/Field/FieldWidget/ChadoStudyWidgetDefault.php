<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado study widget.
 *
 * @FieldWidget(
 *   id = "chado_study_widget_default",
 *   label = @Translation("Chado Study Widget"),
 *   description = @Translation("The default study widget."),
 *   field_types = {
 *     "chado_study_type_default"
 *   }
 * )
 */
class ChadoStudyWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $settings = $field_definition->getSettings();
    $storage_settings = $settings['storage_plugin_settings'];
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'study_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();
    $field_term = $settings['termIdSpace'] . ':' . $settings['termAccession'];

    // Get the list of studies. Include contacts because that has a not null constraint.
    $studys = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('study', 's');
    $query->leftJoin('contact', 'c', 's.contact_id = c.contact_id');
    $query->fields('s', ['study_id', 'name']);
    $query->addField('c', 'name', 'contact_name');
    $query->orderBy('name', 'contact_name');
    $results = $query->execute();
    while ($study = $results->fetchObject()) {
      $studys[$study->study_id] = $study->name;
    }
    natcasesort($studys);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $study_id = $item_vals['study_id'] ?? 0;

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
    // pass the field cv term through the form for massageFormValues()
    $elements['field_term'] = [
      '#type' => 'value',
      '#default_value' => $field_term,
    ];
    $elements[$linker_fkey_column] = $element + [
      '#type' => 'select',
      '#options' => $studys,
      '#default_value' => $study_id,
      '#empty_option' => '-- Select --',
    ];

    // If there are any additional columns present in the linker table,
    // use a default of 1 which will work for type_id or rank.
    // or pub_id. Any existing value will pass through as the default.
    foreach ($property_definitions as $property => $definition) {
      if (($property != 'linker_id') and preg_match('/^linker_/', $property)) {
        $default_value = $item_vals[$property] ?? 1;
        $elements[$property] = [
          '#type' => 'value',
          '#default_value' => $default_value,
        ];
      }
    }

    // Save some initial values to allow later handling of the "Remove" button
    $this->saveInitialValues($delta, $field_term, $linker_id, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    return $this->massageLinkingFormValues('study_id', $values, $form_state);
  }
}
