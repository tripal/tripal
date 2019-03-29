<?php

class TripalNucFASTADownloader extends TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.  It
   * should be a short identifier. Use the $full_label for a more
   * descriptive label.
   */
  static public $label = 'FASTA';

  /**
   * A more verbose label that better describes the formatter.
   */
  static public $full_label = 'Nucleotide FASTA';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'fna';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {
    $lines = [];
    $site = !property_exists($entity, 'site_id') ? 'local' : $entity->site_id;
    $bundle_name = $entity->bundle;

    // Holds the list of sequence identifiers that will be used to build the
    // definition line.
    $identifiers = [
      'identifier' => '',
      'name' => '',
      'accession' => '',
    ];
    // Holds the list of non identifiers that will be used in the definitino
    // line.
    $others = [];
    // Holds the sequence string for the FASTA item.
    $residues = '';

    // Iterate through all of the fields and build the definition line and
    // the sequence string.
    foreach ($this->fields[$site][$bundle_name] as $field_id => $info) {
      $field = $info['field'];
      $instance = $info['instance'];
      $field_name = $field['field_name'];
      $accession = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];

      // If this field really is not attched to the entity then skip it.
      if (!property_exists($entity, $field_name)) {
        continue;
      }

      // If we only have one element then this is good.
      if (count($entity->{$field_name}['und']) == 1) {

        $value = $entity->{$field_name}['und'][0]['value'];

        // Add in the unique identifier for this sequence to the defline.
        if ($accession == 'data:0842') {
          $identifiers['identifier'] = $value;
        }
        // Add in the non-unique name for this sequence to the defline.
        else {
          if ($accession == 'schema:name') {
            $identifiers['name'] = $value;
          }
          // Add in the local site accession for this sequence to the defline.
          else {
            if ($accession == 'data:2091') {
              $identifiers['accession'] = $value;
            }
            // Add in the sequence coordinataes to the defline.
            else {
              if ($accession == 'data:2012') {
                $others[$instance['label']] = $value["data:3002"] . ':' . $value["local:fmin"] . '-' . $value["local:fmax"] . $value["data:0853"];
              }
              // Get the nuclotide sequence.
              else {
                if ($accession == 'data:2044') {
                  $residues = $entity->{$field_name}['und'][0]['value'];
                }
                // Skip the protein sequence if it exists.
                else {
                  if ($accession == 'data:2976') {
                    // do nothing.
                  }
                  // Add in the organism.
                  else {
                    if ($accession == 'OBI:0100026') {
                      //$others[$instance['label']] = strip_tags($value['rdfs:label']);
                    }
                    // All other fields add them to the others list.
                    else {
                      if (!is_array($value)) {
                        $others[$instance['label']] = $value;
                      }
                      else {
                        // TODO: What to do with fields that are arrays?
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      else {
        // TODO: What to do with fields that have multiple values?
      }
    }

    // First add the definition line.
    if (count(array_keys($identifiers)) == 0) {
      $defline = ">Unknown feature identifier. The data collection must have a name or accession field";
      $lines[] = $defline;
    }
    else {
      $defline = ">";
      $defline .= $identifiers['identifier'] ? $identifiers['identifier'] . ' ' : '';
      $defline .= $identifiers['name'] ? $identifiers['name'] . ' ' : '';
      $defline .= $identifiers['accession'] ? $identifiers['accession'] . ' ' : '';

      foreach ($others as $k => $v) {
        if ($v) {
          // If the value has non alpha-numeric characters then wrap it in
          // quotes.
          if (preg_match('/[^\w]/', $v)) {
            $defline .= $k . ':"' . $v . '"; ';
          }
          else {
            $defline .= $k . ':' . $v . '; ';
          }
        }
      }
      $lines[] = $defline;
    }

    // Now add the residues.
    if ($residues) {
      $sequence = explode('|', wordwrap($residues, 50, "|", TRUE));
      foreach ($sequence as $line) {
        $lines[] = $line;
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