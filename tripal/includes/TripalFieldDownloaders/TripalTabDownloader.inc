<?php

class TripalTabDownloader extends TripalFieldDownloader {
  /**
   * Sets the label shown to the user describing this formatter.  It
   * should be a short identifier. Use the $full_label for a more
   * descriptive label.
   */
  static public $label = 'TAB';

  /**
   * A more verbose label that better describes the formatter.
   */
  static public $full_label = 'Tab delimeted';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'txt';

  /**
   * @see TripalFieldDownloader::isFieldSupported()
   */
  public function isFieldSupported($field, $instance) {
    $is_supported = parent::isFieldSupported($field, $instance);

    // For now all fields are supported.
    return TRUE;
  }

  /**
   * @see TripalFieldDownloader::formatEntity()
   */
   protected function formatEntity($entity) {
     $bundle_name = $entity->bundle;
     $site = !property_exists($entity, 'site_id') ? 'local' : $entity->site_id;
     $row = array();

     // Iterate through all of the printable fields and add the value to
     // the row.
     foreach ($this->printable_fields as $accession => $label) {

       // If this field is not present for this entity then add an empty
       // element and move on.
       if (!array_key_exists($accession, $this->fields2terms[$site][$bundle_name]['by_accession'])) {
         $row[] = '';
         continue;
       }

       // Get the field from the class variables.
       $field_id = $this->fields2terms[$site][$bundle_name]['by_accession'][$accession];
       $field = $this->fields[$site][$bundle_name][$field_id]['field'];
       $instance = $this->fields[$site][$bundle_name][$field_id]['instance'];
       $field_name = $field['field_name'];

       // If we only have one item for this value then add it.
       if (count($entity->{$field_name}['und']) == 1) {
         $value = $entity->{$field_name}['und'][0]['value'];

         // If the single element is not an array then this is good.
         if (!is_array($value)) {
           $row[] = $value;
         }
         else {
           if (array_key_exists('rdfs:label', $entity->{$field_name}['und'][0]['value'])) {
             $label = $entity->{$field_name}['und'][0]['value']['rdfs:label'];
             $row[] = strip_tags($label);
           }
           else {
             $row[] = '';
           }
           // TODO: What to do with fields that are arrays?
         }
       }
       // If we have multiple items then deal with that.
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
    foreach ($this->printable_fields as $accession => $label) {
      $row[] = $label;
    }
    return array(implode("\t", $row));
  }
}
