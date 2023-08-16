<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
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
 *     "chado_analysis_default"
 *   }
 * )
 */
class ChadoAnalysisWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

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
      ORDER BY A.name';
    $results = $chado->query($sql, [':cvterm' => 'type', ':cv' => 'rdfs']);

    while ($analysis = $results->fetchObject()) {
      $type_text = $analysis->value ? ' (' . $analysis->value . ')' : '';
      $analyses[$analysis->analysis_id] = $analysis->name . $type_text;
    }

    $item_vals = $items[$delta]->getValue();
    $record_id = $item_vals['record_id'] ?? 0;
    $analysis_id = $item_vals['analysis_id'] ?? 0;

    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $elements['analysis_id'] = $element + [
      '#type' => 'select',
      '#options' => $analyses,
      '#default_value' => $analysis_id,
      '#placeholder' => $this->getSetting('placeholder'),
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }

}
