<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
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
 *     "chado_contact_default"
 *   }
 * )
 */
class ChadoContactWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of contacts.
    $contacts = [];
    $chado = \Drupal::service('tripal_chado.database');

    // In addition to getting a sorted list of contacts, include
    // the contact type if present, e.g.
    // Person or Institute
    $sql = 'SELECT C.contact_id, C.name, T.name AS type FROM {1:contact} C
      LEFT JOIN {1:cvterm} T ON C.type_id=T.cvterm_id
      ORDER BY LOWER(C.name)';
    $results = $chado->query($sql, []);

    while ($contact = $results->fetchObject()) {
      // Change the non-user-friendly 'null' contact, which is specified by chado.
      if ($contact->name == 'null') {
        $contact->name = '-- Unknown --';  // This will sort to the top.
      }
      $type_text = $contact->type ? ' (' . $contact->type . ')' : '';
      $contacts[$contact->contact_id] = $contact->name . $type_text;
    }
    natcasesort($contacts);

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $contact_id = $item_vals['contact_id'] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['contact_id'] = $element + [
      '#type' => 'select',
      '#options' => $contacts,
      '#default_value' => $contact_id,
      '#placeholder' => $this->getSetting('placeholder'),
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }

}
