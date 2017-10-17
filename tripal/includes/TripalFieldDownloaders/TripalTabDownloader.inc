<?php

class TripalTabDownloader extends TripalFieldDownloader {
  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'Tab delimeted';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'txt';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {
    $row = array();
    foreach ($this->fields as $field_id) {
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];

      if (!property_exists($entity, $field_name)) {
        continue;
      }

      // If we only have one element then this is good.
      if (count($entity->{$field_name}['und']) == 1) {
        $value = $entity->{$field_name}['und'][0]['value'];
        // If the single element is not an array then this is good.
        if (!is_array($value)) {
          $row[] = $value;
        }
        else {
          if (array_key_exists('rdfs:label', $entity->{$field_name}['und'][0]['value'])) {
            $row[] = strip_tags($entity->{$field_name}['und'][0]['value']['rdfs:label']);
          }
          else {
            $row[] = '';
          }
          // TODO: What to do with fields that are arrays?
        }
      }
      else {
        $row[] = '';
        // TODO: What to do with fields that have multiple values?
      }
    }
    return array(implode("\t", $row));
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {
    $row = array();
    foreach ($this->fields as $field_id) {
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $instance = field_info_instance('TripalEntity', $field_name, $this->bundle_name);
      $row[] = $instance['label'];
    }
    return array(implode("\t", $row));
  }
}