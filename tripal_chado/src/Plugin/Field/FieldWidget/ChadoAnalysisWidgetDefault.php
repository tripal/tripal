<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado analysis widget.
 *
 * @FieldWidget(
 *   id = "chado_analysis_widget_default",
 *   label = @Translation("Chado Analysis Widget"),
 *   description = @Translation("The default analysis widget."),
 *   field_types = {
 *     "chado_analysis_type_default"
 *   }
 * )
 */
class ChadoAnalysisWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'analysis_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();

    // Get the list of analyses.
    $analyses = [];
    $chado = \Drupal::service('tripal_chado.database');

    // In addition to getting a sorted list of analyses, include
    // the analysisprop rdfs:type when it is present, e.g.
    // genome assembly or genome annotation.
    $sql = 'SELECT A.analysis_id, A.name, TYPE.value FROM {1:analysis} A
      LEFT JOIN (
        SELECT AP.analysis_id, AP.value FROM {1:analysisprop} AP
        LEFT JOIN {1:cvterm} T ON AP.type_id=T.cvterm_id
        LEFT JOIN {1:cv} CV ON T.cv_id=CV.cv_id
        WHERE T.name=:cvterm
        AND CV.name=:cv
      ) AS TYPE
      ON A.analysis_id=TYPE.analysis_id
      ORDER BY LOWER(A.name)';
    $results = $chado->query($sql, [':cvterm' => 'type', ':cv' => 'rdfs']);

    while ($analysis = $results->fetchObject()) {
      $type_text = $analysis->value ? ' (' . $analysis->value . ')' : '';
      $analyses[$analysis->analysis_id] = $analysis->name . $type_text;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $analysis_id = $item_vals['analysis_id'] ?? 0;

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
      '#options' => $analyses,
      '#default_value' => $analysis_id,
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

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Handle any empty values.
    foreach ($values as $val_key => $value) {
      // Foreign key is analysis_id
      $linker_fkey_column = $value['linker_fkey_column'];
      if ($value[$linker_fkey_column] == '') {
        if ($value['record_id']) {
          // If there is a record_id, but no analysis_id, this means
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
