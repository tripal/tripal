<?php

namespace Drupal\tripal\Plugin\views\field;

use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;
use Drupal\Core\Form\FormStateInterface;


/**
 * Views field plugin to display 'criteria'.
 *
 * @ingroup views_field_handlers
 * 
 * @ViewsField("criteria")
 */
class Criteria extends FieldPluginBase {



  /**
   * {@inheritdoc}
   */
  public function query() {
    parent::query();

    // We actually do not even have to introduce the additional 'units' column
    // ourselves because 'additional fields' property of field definition, in
    // fact, is magical one - whatever addtional columns are defined there get
    // automatically into the SELECT query in FieldPluginBase::query() method.
  }

  /**
   * {@inheritdoc}
   */
  public function defineOptions() {
    $options = parent::defineOptions();

    $options['data_element'] = ['default' => []];

    return $options;
  }  
  
  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);
    $form['data_element'] = [
      '#type' => 'select',
      '#title' => $this->t('Data element'),
      '#description' => $this->t('Element within criteria column to return'),
      '#options' => [
        'Search String' => 'Search String',
        'Database' => 'Database'
      ],
      '#default_value' => $this->options['data_element'],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function render(ResultRow $values) {
    // Since our primary column is weight, we can get its value without
    // supplying the 2nd argument into the ::getValue() method.
    $value = $this->getValue($values);
    $criteria_column_array = unserialize($value);

    $output = "";
    $data_element = $this->options['data_element'];

    switch ($data_element) {
      case 'Search String': 
        $search_string = "";
        foreach ($criteria_column_array['criteria'] as $criteria_row) {
          $search_string .= $criteria_row['operation'] . ' (' . $criteria_row['scope'] . ': ' . $criteria_row['search_terms'] . ') ';
        }
        $output .= $search_string;
        break;
      case 'Database':
        $db_string = $criteria_column_array['remote_db'];
        $output .= $db_string;
        break;
      default:
        $output .= "A data element was not selected for this field, please configure the field options for this element.";
    }
  
    // To retrieve a value of an additional field, just use the construction as
    // below. The 'units' key of $this->additional_fields is the name of
    // additional field whose value we intend to retrieve from $values. In fact
    // $this->additional_fields['units'] will get us alias of the additional
    // field 'units' under which it was included into the SELECT query.
    // $units = $this->getValue($values, $this->additional_fields['units']);


    // Now it all reduces to just pretty-printing. This is
    // the actual content Views will display for our field.
    return $output;
  }

}