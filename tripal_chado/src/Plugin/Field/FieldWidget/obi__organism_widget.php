<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type widget.
 *
 * @FieldWidget(
 *   id = "obi__organism_widget",
 *   label = @Translation("Chado Organism Reference Widget"),
 *   description = @Translation("A chado organism reference widget"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class obi__organism_widget extends TripalWidgetBase {


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

    $element['value'] = $element + [
      '#type' => 'select',
      '#options' => $organisms,
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
    ];

    return $element + parent::formElement($items, $delta, $element, $form, $form_state);
  }
}