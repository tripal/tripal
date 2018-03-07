<?php

class TripalCSVDownloader extends TripalFieldDownloader {
  /**
   * Sets the label shown to the user describing this formatter.  It
   * should be a short identifier. Use the $full_label for a more
   * descriptive label.
   */
  static public $label = 'CSV';

  /**
   * A more verbose label that better describes the formatter.
   */
  static public $full_label = 'Comma separated';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'csv';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {
    $row = array();
    $bundle_name = $entity->bundle;
    
    // Grab the headers to ensure we tab the fields properly.
    $header =  $this->header_array;
    if (!empty($header)) { 
      // Count the number of items in the header and build an array with that many items.
      $header_count = count($header);
      $row = array_fill(0, $header_count, NULL);
    }
    // Determine if the entity is remote or local.
    if (strpos($bundle_name, 'bio_data_') !== 0) {
      $external = TRUE;
    }
    else {
      $external = FALSE;
    }

    $bundle_collections = $this->collection_bundles;

    foreach ($bundle_collections as $bundle_collection) {
      $bundle = $bundle_collection->bundle_name;
      if ($bundle_name == $bundle) {
        $fields = unserialize($bundle_collection->fields);
      }
    }

    foreach ($fields as $field) {
      if ($external) {
        $field_name = $entity->$field['und'][0]['label'];
        $value = $entity->$field['und'][0]['value'];
        $accession = $entity->$field['und'][0]['accession'];
        if (!empty($header)) {
          //Now find the accession in the header array and place it where it needs to go.
          $position = array_search($accession, $header);
          $row[$position] = $value;
        }
      }
      else {
        $field_info = field_info_field_by_id($field);
        $field_name = $field_info['field_name'];
        $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
        // Build the accession incase someone renames a cord field name.
        $accession = $instance['settings']['term_vocabulary'].$instance['settings']['term_accession'];

        if (count($entity->{$field_name}['und']) == 1) {
          $value = $entity->{$field_name}['und'][0]['value'];

          if (!empty($header)) { 
            //Now find the accession in the header array and place it where it needs to go.
            $position = array_search($accession, $header);
            // If the single element is not an array then this is good.
            if (!is_array($value)) {
              $row[$position] = $value;
            }
            else {
              if (array_key_exists('rdfs:label', $entity->{$field_name}['und'][0]['value'])) {
                $row[$position] = $tabs . strip_tags($entity->{$field_name}['und'][0]['value']['rdfs:label']);
              }
              else {
                $row[$position] = '';
              }
              // TODO: What to do with fields that are arrays?
            }

          } 
        }
        else {
          $row[] = '';
          // TODO: What to do with fields that have multiple values?
        }
      }
    }
    return array(implode("\t", $row));
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {
    $bundle_collections = $this->collection_bundles;
    return $this->processTabHeader($bundle_collections);
  }

}
