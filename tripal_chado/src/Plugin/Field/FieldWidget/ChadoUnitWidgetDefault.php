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
   * This array provides a list of term accessions used to populate the
   * select list for this widget.
   *
   * Each element should be a string of the format [IDSpace]:[Accession]
   * and each term must already exist in the database.
   *
   * @var string[]
   */
  public array $term_options = [
    'local:cM',
    'local:bp',
    'local:bin_unit',
    'local:marker_order',
    'local:undefined',
  ];

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state)  {

    $unit_vals = [];

    $chado = \Drupal::service('tripal_chado.database');

    $query = $chado->select( 'cvterm', 'cvt' );
    $query->leftJoin ('cv', 'cv', 'cvt.cv_id = cv.cv_id ');
    $query->leftJoin ('dbxref', 'dbx', 'cvt.dbxref_id = dbx.dbxref_id');
    $query->leftJoin ('db', 'db', 'db.db_id = db.db_id');
    $query->addField( 'cvt', 'name', 'cvt_name');
    $query->addField( 'cvt', 'cvterm_id', 'unittype_id');

    // Use a condition groups to build a section of where clause as follows:
    // ((db.name = :term1db AND dbx.accession = :term1acc) OR (db.name = :term2db AND dbx.accession = :term2acc))
    $or_group = $query->orConditionGroup();
    foreach ($this->term_options as $term) {
      list($db,$accession) = explode(':', $term);
      $term_group = $query->andConditionGroup()
        ->condition('db.name', $db, '=')
        ->condition('dbx.accession', $accession, '=');
      $or_group->condition($term_group);
    }
    $query->condition($or_group);

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
