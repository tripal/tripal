<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Tripal organism widget.
 *
 * @FieldWidget(
 *   id = "chado_organism_widget_default",
 *   label = @Translation("Chado Organism Reference Widget"),
 *   description = @Translation("A chado organism reference widget"),
 *   field_types = {
 *     "chado_organism_type_default"
 *   }
 * )
 */
class ChadoOrganismWidgetDefault extends ChadoWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // Get the list of organisms.
    $organisms = [];
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('organism', 'o');
    $query->leftJoin('cvterm', 'cvt', 'o.type_id = cvt.cvterm_id');
    $query->fields('o', ['organism_id', 'genus', 'species', 'infraspecific_name']);
    $query->fields('cvt', ['name']);
    $query->orderBy('genus');
    $query->orderBy('species');
    $results = $query->execute();

    while ($organism = $results->fetchObject()) {
      $org_name = $organism->genus . ' ' . $organism->species;

      if ($organism->name) {
        $org_name .= ' ' . $organism->name;
      }
      if ($organism->infraspecific_name) {
        $org_name .= ' ' . $organism->infraspecific_name;
      }
      $organisms[$organism->organism_id] = $org_name;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $organism_id = $item_vals['organism_id'] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['organism_id'] = $element + [
      '#type' => 'select',
      '#options' => $organisms,
      '#default_value' => $organism_id,
      '#placeholder' => $this->getSetting('placeholder'),
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }
}
