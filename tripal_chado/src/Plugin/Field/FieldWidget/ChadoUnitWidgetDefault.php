<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Data Source widget.
 *
 * @FieldWidget(
 *   id = "chado_unit_widget_default",
 *   label = @Translation("Chado Unit Widget Default"),
 *   description = @Translation("The default unit widget which allows curators to enter unit on the Gene Map content edit page."),
 *   field_types = {
 *     "chado_unit_type_default"
 *   }
 * )
 */
class ChadoUnitWidgetDefault extends ChadoWidgetBase {
  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state)  {

    $unit_vals = [];
     
    $chado = \Drupal::service('tripal_chado.database');

    $query = $chado->select( 'cvterm', 'cvt' );
    $query->leftJoin ('cv', 'cv', 'cvt.cv_id = cv.cv_id ');
    $query->addField( 'cvt', 'name', 'cvt_name' );
    $query->addField( 'cvt', 'cvterm_id', 'unittype_id' );
    $query->condition('cv.name', 'featuremap_units', '=');
    $results = $query->execute();

    while ( $unit_rec = $results->fetchObject() ) {
      $unit_vals[$unit_rec->unittype_id] = $unit_rec->cvt_name;
    }

    $item_vals = $items[$delta]->getValue() ;
    $record_id = $item_vals['record_id'] ?? 0 ;
    $unittype_id = $item_vals['unittype_id'] ?? 0 ;

    $elements = [];

    $elements['record_id'] = [
      '#default_value' => $record_id,
      '#type' => 'value',
    ];

    $elements['unittype_id'] = $element + [
      '#type' => 'select',
      '#description' =>  t("Select map unit from dropdown."),
      '#options' => $unit_vals,
      '#default_value' => $unittype_id,
      '#empty_option' => '-- Select --',
    ];

    return $elements;
  }  
  
  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    // Remove any empty values that don't have a unit type
     foreach ($values as $delta => $item) {
       if ($item['unittype_id'] == '') {
         unset($values[$delta]);
       }
     }
     return $values;
   }

}