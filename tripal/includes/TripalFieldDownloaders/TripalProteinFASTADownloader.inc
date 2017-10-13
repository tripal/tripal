<?php
class TripalProteinFASTADownloader extends TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'Protein FASTA';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'faa';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {
    $lines = array();

    // Get the list of all fields that have been attached to the entity
    $instances = field_info_instances('TripalEntity', $entity->bundle);
    $available_fields = array();
    foreach ($instances as $field_name => $instance) {
      if ($instance['field_name'] == 'entity_id') {
        continue;
      }
      $available_fields[$instance['field_name']] = $instance;
    }

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

          // We need to make sure we have some fields for the definition line.
          // those may or may not have been included, so we should add them.
          $defline = '>';
          $found_identifier = FALSE;
          if (property_exists($entity, 'data__identifier')) {
            $found_identifier = TRUE;
            $defline .= $entity->{'data__identifier'}['und'][0]['value'] . ' ';
          }
          if (property_exists($entity, 'schema__name')) {
            $found_identifier = TRUE;
            $defline .= $entity->{'schema__name'}['und'][0]['value'] . ' ';
          }
          if (property_exists($entity, 'data__accession')) {
            $found_identifier = TRUE;
            $defline .= $entity->{'data__accession'}['und'][0]['value'] . ' ';
          }
          if (!$found_identifier) {
            $defline .= "Unknown feature identifier. Please add a name field to the data collection";
          }
          if (property_exists($entity, 'data__sequence_coordinates')) {
            $location = strip_tags(drupal_render(field_view_field('TripalEntity', $entity, 'data__sequence_coordinates'))) . '; ';
            $location = preg_replace('/\&nbsp\;/', ' ', $location);
            $defline .= $location;
          }
          // Add to the defnition line values from any single valued fields.
          foreach ($available_fields as $fname => $instance) {
            if (count($entity->{$fname}['und']) == 1) {
              if (!is_array($entity->{$fname}['und'][0]['value'])) {
                // Skip the identifier fields and the residues fields.
                if (!in_array($fname, array('data__identifier',
                  'schema__name', 'data__sequence', $field_name))) {
                  $fvalue = $entity->{$fname}['und'][0]['value'];
                  if ($fvalue) {
                    $defline .= $instance['label'] . ': ' . $fvalue . '; ';
                  }
                }
              }
              else {
                if (array_key_exists('rdfs:label', $entity->{$fname}['und'][0]['value'])) {
                  $defline .= $instance['label'] . ': ' . strip_tags($entity->{$fname}['und'][0]['value']['rdfs:label']) . '; ';
                }
              }
            }
          }
          $defline = rtrim($defline, '; ');

          // Now add the residues.
          $lines[] = $defline;
          $residues = explode('|', wordwrap($value, 50, "|", TRUE));
          foreach ($residues as $line) {
            $lines[] = $line;
          }
        }
        else {
          // TODO: What to do with fields that are arrays?
        }
      }
      else {
        // TODO: What to do with fields that have multiple values?
      }
    }
    return $lines;
  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {

  }
}