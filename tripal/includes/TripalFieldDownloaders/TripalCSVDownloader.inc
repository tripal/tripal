<?php

class TripalCSVDownloader extends TripalFieldDownloader {
  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'CSV (comma separated)';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'csv';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {
    $row = array();
    foreach ($this->fields as $field_id) {
      // Check is the field is remote or not.
      if (!is_numeric($field)) {
        $field_name = $entity->$field_id['field_name'];

      }
      else {
        $field = field_info_field_by_id($field_id);
        $field_name = $field['field_name'];
      }
      if (!property_exists($entity, $field_name)) {
        continue;
      }

      // If we only have one element then this is good.
      if (count($entity->{$field_name}['und']) == 1) {
        $value = $entity->{$field_name}['und'][0]['value'];
        // If the single element is not an array then this is good.
        if (!is_array($value)) {
          $row[] = '"' . $value . '"';
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
    return array(implode(',', $row));
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {
    $row = array();
    foreach ($this->fields as $field_id) {
      // If the $field is numeric it's a field id from the local site but
      // if it is not then it's a remote field that needs to be handled differently.
      if (!is_numeric($field)) {
        $fake_tripal_entity = $this->getRemoteEntity($remote_id, $site_id, $remote_id);
        $field_name = $fake_tripal_entity[$field_id]['field_name'];
        $row[] = '"' . $field_name . '"';
      }
      else {
        $field = field_info_field_by_id($field_id);
        $field_name = $field['field_name'];
        $instance = field_info_instance('TripalEntity', $field_name, $this->bundle_name);
        $row[] = '"' . $instance['label'] . '"';
      }

      
    }
    return array(implode(',', $row));
  }
}