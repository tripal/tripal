<?php

class data__sequence_record_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Sequences';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_record'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $label_term = 'rdfs:label';
    $description_term = 'schema:description';
    $type_term = 'rdfs:type';
    $sequence_term = chado_get_semweb_term('feature', 'residues');
    $seq_coords_term = 'data:2012';
    $seq_length_term = chado_get_semweb_term('feature', 'seqlen');
    $seq_md5sum_term = chado_get_semweb_term('feature', 'md5checksum');
    $description_term = 'schema:description';
    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');
    $phase_term = chado_get_semweb_term('featureloc', 'phase');
    $fasta_defline = 'local:fasta_definition';

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }

      $num_bases = 50;
      $residues = '<pre class="residues-formatter">';
      $residues .= ">" . $item['value'][$fasta_defline] . "<br>";
      $residues .= wordwrap($item['value'][$sequence_term], $num_bases, "<br>", TRUE);
      $residues .= '</pre>';

      $element[$delta] = [
        '#type' => 'item',
        '#title' => $item['value'][$label_term],
        '#description' => $item['value'][$description_term],
        '#markup' => $residues,
      ];
    }

    if (count($element) == 0) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => 'There are no sequences for this feature.',
      ];
    }
  }
}
