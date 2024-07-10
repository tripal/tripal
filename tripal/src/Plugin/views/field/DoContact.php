<?php

namespace Drupal\tripal\Plugin\views\field;

use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;

/**
 * Views field plugin to display 'do_contact'.
 *
 * @ingroup views_field_handlers
 * 
 * @ViewsField("do_contact")
 */
class DoContact extends FieldPluginBase {

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
  public function render(ResultRow $values) {
    // Since our primary column is weight, we can get its value without
    // supplying the 2nd argument into the ::getValue() method.
    $value = $this->getValue($values);

    $do_contact = $value;
    if ($do_contact <= 0) {
      $do_contact = 'No';
    }
    else {
      $do_contact = 'Yes';
    }

    // To retrieve a value of an additional field, just use the construction as
    // below. The 'units' key of $this->additional_fields is the name of
    // additional field whose value we intend to retrieve from $values. In fact
    // $this->additional_fields['units'] will get us alias of the additional
    // field 'units' under which it was included into the SELECT query.
    // $units = $this->getValue($values, $this->additional_fields['units']);


    // Now it all reduces to just pretty-printing. This is
    // the actual content Views will display for our field.
    return $do_contact;
  }

}