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
    $row_with_tabs = array();
    $bundle_name = $entity->bundle;
    
    // Grab the headers to ensure we tab the fields properly.
    $header =  $this->header_array;

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
        // Add the tabs so it's in the right place.
        if (!empty($header)) {
          $tabs_needed = array_search($accession, $header);
          // The number of existing rows needs to be taken into account
          // because all rows will be combined in the end into one line.
          $row_count = count($row);
          $tabs_needed = $tabs_needed - $row_count;
          
          if ($tabs_needed > 0) {
            for ($i = 0; $i <= $tabs_needed; $i++) {
              $tabs .= "\t";
            }
          }
          else {
            $tabs = '';
          }
        }
        if (!is_array($value)) {
          $row[] = $tabs . $value;
        }
      }
      else {
        $field_info = field_info_field_by_id($field);
        $field_name = $field_info['field_name'];
        
        if (count($entity->{$field_name}['und']) == 1) {
          $value = $entity->{$field_name}['und'][0]['value'];
          // Add the tabs so it's in the right place.
          if (!empty($header)) {
            $tabs_needed = array_search($field_name, $header);
            // The number of existing rows needs to be taken into account
            // because all rows will be combined in the end into one line.
            $row_count = count($row);
            $tabs_needed = $tabs_needed - $row_count;
            for ($i = 0; $i <= $tabs_needed; $i++) {
              $tabs .= "\t";
            }
          } 
          // If the single element is not an array then this is good.
          if (!is_array($value)) {
            $row[] = $tabs . $value;
          }
          else {
            if (array_key_exists('rdfs:label', $entity->{$field_name}['und'][0]['value'])) {
              $row[] = $tabs . strip_tags($entity->{$field_name}['und'][0]['value']['rdfs:label']);
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
    }
    return array(implode('', $row));
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {
    $row = array();
    $accession_ids = array();
    $bundle_collections = $this->collection_bundles;

    foreach ($bundle_collections as $bundle_collection) {
      $collection_id = $bundle_collection->collection_bundle_id;
      $bundle_name = $bundle_collection->bundle_name;
      $fields = unserialize($bundle_collection->fields);
      $entity_ids = unserialize($bundle_collection->ids);
      $site_id = $bundle_collection->site_id;

      // Determine if the entity is remote or local.
      if (strpos($bundle_name, 'bio_data_') !== 0) {
        $external = TRUE;
      }
      else {
        $external = FALSE;
      }

      foreach ($fields as $field) {
        if ($external) {
          $fake_tripal_entity = $this->getRemoteEntity($entity_ids, $site_id, $field, $bundle_name);
          $field_name = $fake_tripal_entity->$field['und'][0]['field_name'];
          $accession = $fake_tripal_entity->$field['und'][0]['accession'];
          if (!empty($accession_ids) && !empty($row)) {
            // If the accession is already in the accession_ids list skip.
            if (array_search($accession, $accession_ids)) {
              continue;
            }
            else {
              $row[] = $field_name;
              $accession_ids[] = $accession;
            }
          } 
          else {
            $row[] = $field_name;
            $accession_ids[] = $accession;
          }
        }
        else {
          $field_info = field_info_field_by_id($field);
          //$field_name is the accession info
          $field_name = $field_info['field_name'];
          $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
          if (!empty($accession_ids) && !empty($row)) {
            // If the accession is already in the accession_ids list skip.
            if (array_search($accession, $accession_ids)) {
              continue;
            }
            else {
              $accession_ids[] = $field_name;
              $row[] = $instance['label'];
            }
          } 
          else {
            $accession_ids[] = $field_name;
            $row[] = $instance['label'];
          }
        }
      }
    }
    $this->header_array = $row;
    return array(implode("\t", $row));
  }

}
