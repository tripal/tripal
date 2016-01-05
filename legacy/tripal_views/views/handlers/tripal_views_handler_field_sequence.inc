<?php
/**
 * @file
 * Contains tripal_views_handler_field_sequence
 */

/**
 * Handles display of sequence data.  If will aggregate sequences that need
 * to be aggregated (e.g. coding sequences) and provide
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_field_sequence extends views_handler_field {

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['display'] = array(
      '#type' => 'fieldset',
      '#title' => 'Format Output',
      '#description' => t('Alter the way a sequence is displayed')
    );
    $default_num_bases_per_line = '50';
    if ($this->options['display']['num_bases_per_line']) {
       $default_num_bases_per_line = $this->options['display']['num_bases_per_line'];
    }
    $default_output_format = 'raw';
    if ($this->options['display']['output_format']) {
       $default_ouput_format = $this->options['display']['output_format'];
    }

    $form['display']['num_bases_per_line'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of bases per line'),
      '#description' => t('Specify the number of bases per line. An HTML <br> tag ' .
        'will be inserted after the number of bases indicated. If no value is ' .
        'provided. The sequence will be one long string (default)'),
      '#default_value' => $default_num_bases_per_line,
    );
    $form['display']['derive_from_parent'] = array(
      '#type' => 'checkbox',
      '#title' => t('Derive sequence from parent'),
      '#description' => t('Rather than use the sequence from the \'residues\' of this feature, you may ' .
        'derive the sequence from the parent features to which it is aligned. This is useful in the case that the feature ' .
        'does not have sequence associated with it and we need to get it through it\'s alignment. ' .
        'Note: this will slow queries with large numbers of results on the page.'),
      '#default_value' => $this->options['display']['derive_from_parent'],
    );
    $form['display']['aggregate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Aggregate sub features'),
      '#description' => t('If the feature has sub features (e.g. CDS of an mRNA) then check this '.
        'box to filter the sequence to only include the sub features.  Gaps between sub features will be '.
        'excluded from the sequence.  This is useful for obtaining a complete CDS from an mRNA '.
        'without intronic sequence'),
      '#default_value' => $this->options['display']['aggregate'],
    );
    $form['display']['output_format'] = array(
      '#type' => 'radios',
      '#title' => t('Output format'),
      '#options' => array(
         'raw' => 'Raw sequence data (no formatting)',
         'fasta_html' => 'FASTA in HTML format',
         'fasta_txt'  => 'FASTA in text format',
      ),
      '#description' => t('Choose an output format.  Raw output cannot be used when the sequence is derived from the parent.'),
      '#default_value' => $default_ouput_format,
    );
  }

  /**
   * {@inheritdoc}
   */
  function query() {
    parent::query();

    // if we are going to get the sequence from the parent then
    // we will need to do more queries in the render function
    // and we must have the feature_id to do those
    if ($this->options['display']['derive_from_parent']) {
      $this->ensure_my_table();
      $this->query->add_field($this->table, 'feature_id');
      $this->query->add_field($this->table, 'name');
    }
  }

  /**
   * {@inheritdoc}
   *
   * Prior to display of results we want to format the sequence
   */
  function render($values) {
    $residues = '';

    // get the number of bases to show per line
    $num_bases_per_line = $this->options['display']['num_bases_per_line'];
    $output_format = $this->options['display']['output_format'];

    // get the residues from the feature.residues column
    $field = $this->field_alias;

    // get the feature id
    $feature_id = $values->feature_feature_id;
    $feature_name = $values->feature_name;

    // the upstream and downstream values get set by the
    // tripal_views_handlers_filter_sequence.inc
    $upstream = $_SESSION['upstream'];
    $downstream = $_SESSION['downstream'];
    if (!$upstream) {
       $upstream = 0;
    }
    if (!$downstream) {
       $downstream = 0;
    }

    $derive_from_parent = $this->options['display']['derive_from_parent'];
    $aggregate = $this->options['display']['aggregate'];

    $residues = tripal_feature_get_formatted_sequence($feature_id, $feature_name,
      $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
      $upstream, $downstream);
 /*
    // if we need to get the sequence from the parent but there is no aggregation
    // then do so now.
    if ($this->options['display']['derive_from_parent']) {

      // execute our prepared statement
      if (tripal_core_is_sql_prepared('sequence_by_parent')) {
        $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
        $parents = chado_query($sql, $upstream, $downstream, $feature_id);
      }

      while ($parent = d-b_f-etch_object($parents)) {
        $seq = '';  // initialize the sequence for each parent

        // if we are to aggregate then we will ignore the feature returned
        // by the query above and rebuild it using the sub features
        if ($this->options['display']['aggregate']){

          // now get the sub features that are located on the parent.
          $sql = "EXECUTE sub_features (%d, %d)";
          $children = chado_query($sql, $feature_id, $parent->srcfeature_id);
          $sql = "EXECUTE count_sub_features (%d, %d)";
          $num_children = d-b_f-etch_object(chado_query($sql, $feature_id, $parent->srcfeature_id));

          // iterate through the sub features and concat their sequences. They
          // should already be in order.
          $types = array();
          $i = 0;
          while($child = d-b_f-etch_object($children)) {
            // keep up with the types
            if (!in_array($child->type_name,$types)) {
              $types[] = $child->type_name;
            }

            $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";

            // if the first sub feature we need to include the upstream bases
            if ($i == 0 and $parent->strand >= 0) {
              // -------------------------- ref
              //    ....---->  ---->
              //     up    1       2
              $q = chado_query($sql, $upstream, 0, $child->feature_id);
            }
            elseif ($i == 0 and $parent->strand < 0) {
              // -------------------------- ref
              //    ....<----  <----
              //    down  1       2
              $q = chado_query($sql, 0, $downstream, $child->feature_id);
            }
            // if the last sub feature we need to include the downstream bases
            elseif ($i == $num_children->num_children - 1 and $parent->strand >= 0) {
              // -------------------------- ref
              //        ---->  ---->....
              //          1       2 down
              $q = chado_query($sql, 0, $downstream, $child->feature_id);
            }
            elseif ($i == $num_children->num_children - 1 and $parent->strand < 0) {
              // -------------------------- ref
              //        <----  <----....
              //          1       2  up
              $q = chado_query($sql, $upstream, 0, $child->feature_id);
            }

            // for internal sub features we don't want upstream or downstream bases
            else {
              $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
              $q = chado_query($sql, 0, 0, $child->feature_id);
            }

            while ($subseq = d-b_f-etch_object($q)){
              // concatenate the sequences of all the sub features
              if($subseq->srcfeature_id == $parent->srcfeature_id){
                $seq .= $subseq->residues;
              }
            }
            $i++;
          }
        }
        // if this isn't an aggregate then use the parent residues
        else {
           $seq = $parent->residues;
        }

        // get the reverse compliment if feature is on the reverse strand
        $dir = 'forward';
        if ($parent->strand < 0) {
          $seq = trpial_feature_reverse_complement($seq);
          $dir = 'reverse';
        }

        // now format for display
        if ($output_format == 'fasta_html') {
           $seq = wordwrap($seq, $num_bases_per_line, "<br>", TRUE);
        }
        elseif ($output_format == 'fasta_txt') {
           $seq = wordwrap($seq, $num_bases_per_line, "\n", TRUE);
        }
        $residues .= ">$feature_name ($parent->typename) $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax ." ($dir). ";
        if (count($types) > 0) {
          $residues .= "Excludes all bases but those of type(s): " . implode(', ',$types) . ". " ;
        }
        if ($parent->upstream > 0) {
           $residues .= "Includes " . $parent->upstream . " bases upstream.  ";
        }
        if ($parent->downstream > 0) {
           $residues .= "Includes " . $parent->downstream . " bases downstream.  ";
        }
        if (!$seq) {
          $residues .= "No sequence available\n<br>";
        }
        else {
          if ($output_format == 'fasta_html') {
            $residues .= "<br>";
          }
          $residues .= "\n" . $seq . "\n";
          if ($output_format == 'fasta_html') {
            $residues .= "<br>";
          }
        }
      }
    }
    // if we are not getting the sequence from the parent sequence then
    // use what comes through from the feature record
    else {
      $residues = $values->$field;
      if ($output_format == 'fasta_html') {
         $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);
      }
      elseif ($output_format == 'fasta_txt') {
         $residues = wordwrap($residues, $num_bases_per_line, "\n", TRUE);
      }
    }

    // format the residues for display
    if($residues and $num_bases_per_line){
      if ($output_format == 'fasta_html') {
         $residues = '<span style="font-family: monospace;">' . $residues . '</span>';
      }
    } */
    return $residues;
  }

}
