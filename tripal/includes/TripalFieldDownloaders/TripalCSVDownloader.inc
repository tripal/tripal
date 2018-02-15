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
    $bundle_name = $entity->bundle;

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
        if (!is_array($value)) {
          $row[] = $value;
        }
      }
      else {
        $field_info = field_info_field_by_id($field);
        $field_name = $field_info['field_name'];
        
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
    }
    return array(implode("\t", $row));
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {
    $row = array();
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
          $row[] = $field_name;
        }
        else {
          $field_info = field_info_field_by_id($field);
          $field_name = $field_info['field_name'];
          $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
          $row[] = $instance['label'];
        }
      }
    }
    return array(implode("\t", $row));
  }

}
