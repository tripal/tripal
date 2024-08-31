<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado publication widget.
 *
 * @FieldWidget(
 *   id = "chado_pub_widget_default",
 *   label = @Translation("Chado Publication Widget"),
 *   description = @Translation("The default publication widget."),
 *   field_types = {
 *     "chado_pub_type_default"
 *   }
 * )
 */
class ChadoPubWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the field settings.
    $field_definition = $items[$delta]->getFieldDefinition();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $linker_fkey_column = $storage_settings['linker_fkey_column']
      ?? $storage_settings['base_column'] ?? 'pub_id';
    $property_definitions = $items[$delta]->getFieldDefinition()->getFieldStorageDefinition()->getPropertyDefinitions();

    // Get the list of publications.
    $pubs = [];
    $chado = \Drupal::service('tripal_chado.database');

    // In addition to getting a sorted list of pubs, include
    // the pubprop rdfs:type when it is present, e.g.
    // genome assembly or genome annotation.
    $sql = 'SELECT P.pub_id, P.title FROM {1:pub} P
      ORDER BY LOWER(P.title)';
    $results = $chado->query($sql, []);

    while ($pub = $results->fetchObject()) {
      $pubs[$pub->pub_id] = $pub->title;
      // Change the non-user-friendly 'null' publication.
      if ($pubs[$pub->pub_id] == '') {
        $pubs[$pub->pub_id] = '-- Unknown --';  // This will sort to the top.
      }
    }
    natcasesort($pubs);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $linker_id = $item_vals['linker_id'] ?? 0;
    $link = $item_vals['link'] ?? 0;
    $pub_id = $item_vals['pub_id'] ?? 0;

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
      '#options' => $pubs,
      '#default_value' => $pub_id,
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
    $this->saveInitialValues($delta, $linker_id, $linker_fkey_column, $form_state);

    return $elements;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    return $this->massageLinkingFormValues('pub_id', $values, $form_state);
  }

}
