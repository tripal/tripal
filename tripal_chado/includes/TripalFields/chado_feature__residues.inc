<?php

class chado_feature__residues extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Sequences';

  // The default description for this field.
  public static $default_description = 'A field for managing nucleotide and protein residues.';

  // Add any default settings elements.  If you override the fieldSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );


  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $this->field_name = 'feature__residues';
  }

  /**
   * @see TripalField::create_info()
   */
  function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_feature__residues',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'residues',
        'semantic_web' => tripal_get_chado_semweb_term($table_name, 'residues'),
      ),
    );
  }
  /**
   * @see TripalField::createInstanceInfo()
   */
  function createInstanceInfo() {
    if (!$this->can_attach) {
      return;
    }
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'entity_type' => $this->entity_type,
      'bundle' => $this->bundle->name,
      'label' => 'Sequences',
      'description' => 'All available sequences for this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_feature__residues_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'chado_feature__residues_formatter',
          'settings' => array(),
        ),
      ),
    );
  }

  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_feature__residues_widget' => array(
        'label' => t('Sequence'),
        'field types' => array('chado_feature__residues'),
      ),
    );
  }
  /**
   * @see TripalField::formatterInfo()
   */
  static function formatterInfo() {
    return array(
      'chado_feature__residues_formatter' => array(
        'label' => t('Sequences'),
        'field types' => array('chado_feature__residues'),
      ),
    );
  }
  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {


    $element[0] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => '',
    );

    $num_bases = 50;
    foreach ($items as $delta => $item) {
      // If there are no residues then skip this one.
      if (!is_array($item['value']) or !array_key_exists('residues', $item['value'])) {
        continue;
      }

      $residues = $item['value']['residues'];
      $label = $item['value']['label'];
      $defline = $item['value']['defline'];

      $content = '<p>' . $label . '<p>';
      $content .= '<pre class="residues-formatter">';
      $content .= '>' . $defline . "<br>";
      $content .= wordwrap($residues, $num_bases, "<br>", TRUE);
      $content .= '</pre>';

      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field defaults.
    $residues = '';
    if (count($items) > 0 and array_key_exists('feature__residues', $items[0])) {
      $residues = $items[0]['feature__residues'];
    }
    if (array_key_exists('values', $form_state)) {
      //$residues = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'feature__residues');
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['feature__residues'] = array(
      '#type' => 'textarea',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#default_value' => $residues,
      '#delta' => $delta,
//      '#element_validate' => array('chado_feature__residues_widget_validate'),
      '#cols' => 30,
    );
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $field_name = $this->field['field_name'];
    $feature = $details['record'];
    $num_seqs = 0;


    // We don't want to get the sequence for traditionally large types. They are
    // too big,  bog down the web browser, take longer to load and it's not
    // reasonable to print them on a page.
    if(strcmp($feature->type_id->name,'scaffold') == 0 or
       strcmp($feature->type_id->name,'chromosome') == 0 or
       strcmp($feature->type_id->name,'supercontig') == 0 or
       strcmp($feature->type_id->name,'pseudomolecule') == 0) {
      $entity->{$field_name}['und'][$num_seqs]['value'] = array(
        '@type' => 'SO:0000110',
        'type' => 'sequence_feature',
        'label' => 'Residues',
        'defline' => ">This sequence is too large for this display.",
        'residues' => '',
      );
      $entity->{$field_name}['und'][$num_seqs]['feature__residues'] = '';
    }
    else {
      $feature = chado_expand_var($feature,'field','feature.residues');
      if ($feature->residues) {
        $entity->{$field_name}['und'][$num_seqs]['value'] = array(
          '@type' => 'SO:0000110',
          'type' => 'sequence_feature',
          'label' => 'Raw Sequence',
          'defline' => tripal_get_fasta_defline($feature, '', NULL, '', strlen($feature->residues)),
          'residues' => $feature->residues,
        );
        $entity->{$field_name}['und'][$num_seqs]['feature__residues'] = $feature->residues;
      }
      else {
        $entity->{$field_name}['und'][$num_seqs]['value'] = array();
        $entity->{$field_name}['und'][$num_seqs]['feature__residues'] = '';
      }
    }
    $num_seqs++;

    // Add in the protein sequences. It's faster to provide the SQL rather than
    // to use chado_generate_var based on the type.
    $sql = "
      SELECT F.*
      FROM {feature_relationship} FR
        INNER JOIN {feature} F on FR.subject_id = F.feature_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
      WHERE
        FR.object_id = :feature_id and
        CVT.name = 'polypeptide' and
        RCVT.name = 'derives_from'
      ORDER BY FR.rank ASC
    ";
    $results = chado_query($sql, array(':feature_id' => $feature->feature_id));
    while ($protein = $results->fetchObject()) {
      if ($protein->residues) {
        $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
          '@type' => 'SO:0000104',
          'type' => 'polypeptide',
          'label' => 'Protein Sequence',
          'defline' => tripal_get_fasta_defline($protein, '', NULL, '', strlen($protein->residues)),
          'residues' => $protein->residues,
        );
      }
    }


    // Add in sequences from alignments.
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'srcfeature_id' => array(
          'type_id' => 1
        ),
        'feature_id' => array(
          'type_id' => 1
        ),
      ),
    );
    $feature = chado_expand_var($feature, 'table', 'featureloc', $options);
    $featureloc_sequences = self::get_featureloc_sequences($feature->feature_id, $feature->featureloc->feature_id);

    // Add in the coding sequences. It's faster to provide the SQL rather than
    // to use chado_generate_var based on the type.
    $sql = "
      SELECT F.*
      FROM {feature_relationship} FR
        INNER JOIN {feature} F on FR.subject_id = F.feature_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
        INNER JOIN {cvterm} RCVT on RCVT.cvterm_id = FR.type_id
        INNER JOIN {featureloc} FL on FL.feature_id = F.feature_id
      WHERE
        FR.object_id = :feature_id and
        CVT.name = 'CDS' and
        RCVT.name = 'part_of'
      ORDER BY FR.rank ASC
    ";
    $results = chado_query($sql, array(':feature_id' => $feature->feature_id));
    $coding_seq = '';
    while ($CDS = $results->fetchObject()) {
      if ($CDS->residues) {
        $coding_seq .= $CDS->residues;
      }
    }
    if ($coding_seq) {
      $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
        '@type' => 'SO:0000316',
        'type' => 'coding_sequence',
        'label' => 'Coding sequence (CDS)',
        'defline' => tripal_get_fasta_defline($feature, 'CDS', NULL, '', strlen($coding_seq)),
        'residues' => $coding_seq,
      );
    }

    foreach($featureloc_sequences as $src => $attrs){
      // the $attrs array has the following keys
      //   * id:  a unique identifier combining the feature id with the cvterm id
      //   * type: the type of sequence (e.g. mRNA, etc)
      //   * location:  the alignment location
      //   * defline: the definition line
      //   * formatted_seq: the formatted sequences
      //   * featureloc:  the feature object aligned to
      $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
        'residues' => $attrs['residues'],
        '@type' => 'SO:0000110',
        'type' => 'sequence_feature',
        'defline' => tripal_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', strlen($attrs['residues'])),
        'label' => 'Sequence from alignment at ' . $attrs['location'],
      );


      // check to see if this alignment has any CDS. If so, generate a CDS sequence
      $cds_sequence = tripal_get_feature_sequences(
        array(
          'feature_id' => $feature->feature_id,
          'parent_id' => $attrs['featureloc']->srcfeature_id->feature_id,
          'name' => $feature->name,
          'featureloc_id' => $attrs['featureloc']->featureloc_id,
        ),
        array(
          'derive_from_parent' => 1, // CDS are in parent-child relationships so we want to use the sequence from the parent
          'aggregate' => 1, // we want to combine all CDS for this feature into a single sequence
          'sub_feature_types' => array('CDS'), // we're looking for CDS features
          'is_html' => 0
        )
      );

      if (count($cds_sequence) > 0) {
        // the tripal_get_feature_sequences() function can return multiple sequences
        // if a feature is aligned to multiple places. In the case of CDSs we expect
        // that one mRNA is only aligned to a single location on the assembly so we
        // can access the CDS sequence with index 0.
        if ($cds_sequence[0]['residues']) {
          $entity->{$field_name}['und'][$num_seqs++]['value'] = array(
            'residues' => $cds_sequence[0]['residues'],
            '@type' => 'SO:0000316',
            'type' => 'coding_sequence',
            'defline' => tripal_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', $cds_sequence[0]['length']),
            'label' => 'Coding sequence (CDS) from alignment at  ' . $attrs['location'],
          );
        }
      }
    }
  }

  /**
   *
   * @param unknown $feature_id
   * @param unknown $featurelocs
   * @return multitype:|Ambigous <multitype:, an>
   */
  private static function get_featureloc_sequences($feature_id, $featurelocs) {

    // if we don't have any featurelocs then no point in continuing
    if (!$featurelocs) {
      return array();
    }

    // get the list of relationships (including any aggregators) and iterate
    // through each one to find information needed to color-code the reference sequence
    $relationships = self::get_aggregate_relationships($feature_id);
    if (!$relationships) {
      return array();
    }


    // iterate through each of the realtionships features and get their
    // locations
    foreach ($relationships as $rindex => $rel) {
      // get the featurelocs for each of the relationship features
      $rel_featurelocs = self::get_featurelocs($rel->subject_id, 'as_child', 0);
      foreach ($rel_featurelocs as $rfindex => $rel_featureloc) {
        // keep track of this unique source feature
        $src = $rel_featureloc->src_feature_id . "-" . $rel_featureloc->src_cvterm_id;

        // copy over the results to the relationship object.  Since there can
        // be more than one feature location for each relationship feature we
        // use the '$src' variable to keep track of these.
        $rel->featurelocs = new stdClass();
        $rel->featurelocs->$src = new stdClass();
        $rel->featurelocs->$src->src_uniquename = $rel_featureloc->src_uniquename;
        $rel->featurelocs->$src->src_cvterm_id  = $rel_featureloc->src_cvterm_id;
        $rel->featurelocs->$src->src_cvname     = $rel_featureloc->src_cvname;
        $rel->featurelocs->$src->fmin           = $rel_featureloc->fmin;
        $rel->featurelocs->$src->fmax           = $rel_featureloc->fmax;
        $rel->featurelocs->$src->src_name       = $rel_featureloc->src_name;

        // keep track of the individual parts for each relationship
        $start = $rel->featurelocs->$src->fmin;
        $end   = $rel->featurelocs->$src->fmax;
        $type  = $rel->subject_type;
        $rel_locs[$src]['parts'][$start][$type]['start'] = $start;
        $rel_locs[$src]['parts'][$start][$type]['end']   = $end;
        $rel_locs[$src]['parts'][$start][$type]['type']  = $type;
      }
    }

    // the featurelocs array provided to the function contains the locations
    // where this feature is found.   We want to get the sequence for each
    // location and then annotate it with the parts found from the relationships
    // locations determiend above.
    $floc_sequences = array();
    foreach ($featurelocs as $featureloc) {

      // build the src name so we can keep track of the different parts for each feature
      $src = $featureloc->srcfeature_id->feature_id . "-" . $featureloc->srcfeature_id->type_id->cvterm_id;

      // orient the parts to the beginning of the feature sequence
      if (!empty($rel_locs[$src]['parts'])) {
        $parts = $rel_locs[$src]['parts'];
        $rparts = array();  // we will fill this up if we're on the reverse strand

        foreach ($parts as $start => $types) {
          foreach ($types as $type_name => $type) {
            if ($featureloc->strand >= 0) {
              // this is on the forward strand.  We need to convert the start on the src feature to the
              // start on this feature's sequence
              $parts[$start][$type_name]['start'] = $parts[$start][$type_name]['start'] - $featureloc->fmin;
              $parts[$start][$type_name]['end']   = $parts[$start][$type_name]['end'] - $featureloc->fmin;
              $parts[$start][$type_name]['type']  = $type_name;
            }
            else {
              // this is on the reverse strand.  We need to swap the start and stop and calculate from the
              // begining of the reverse sequence
              $size = ($featureloc->fmax - $featureloc->fmin);
              $start_orig = $parts[$start][$type_name]['start'];
              $end_orig = $parts[$start][$type_name]['end'];
              $new_start = $size - ($end_orig - $featureloc->fmin);
              $new_end = $size - ($start_orig - $featureloc->fmin);

              $rparts[$new_start][$type_name]['start'] = $new_start;
              $rparts[$new_start][$type_name]['end']   = $new_end;
              $rparts[$new_start][$type_name]['type']  = $type_name;
            }
          }
        }

        // now sort the parts
        // if we're on the reverse strand we need to resort
        if ($featureloc->strand >= 0) {
          usort($parts, 'chado_feature__residues_sort_rel_parts_by_start');
        }
        else {
          usort($rparts, 'chado_feature__residues_sort_rel_parts_by_start');
          $parts = $rparts;
        }

        $floc_sequences[$src]['id'] = $src;
        $floc_sequences[$src]['type'] = $featureloc->feature_id->type_id->name;
        $args = array(':feature_id' => $featureloc->srcfeature_id->feature_id);
        $start = $featureloc->fmin + 1;
        $size = $featureloc->fmax - $featureloc->fmin;

        // TODO: fix the hard coded $start and $size
        // the $start and $size variables are hard-coded in the SQL statement
        // because the db_query function places quotes around all placeholders
        // (e.g. :start & :size) and screws up the substring function
        $sql = "
          SELECT substring(residues from $start for $size) as residues
          FROM {feature}
          WHERE feature_id = :feature_id
        ";
        $sequence = chado_query($sql, $args)->fetchObject();
        $residues = $sequence->residues;
        if ($featureloc->strand < 0) {
          $residues = tripal_reverse_compliment_sequence($residues);
        }
        $strand = '.';
        if ($featureloc->strand == 1) {
          $strand = '+';
        }
        elseif ($featureloc->strand == -1) {
          $strand = '-';
        }
        $floc_sequences[$src]['location'] = tripal_get_location_string($featureloc);
        $floc_sequences[$src]['defline'] = tripal_get_fasta_defline($featureloc->feature_id, '', $featureloc, '', strlen($residues));
        $floc_sequences[$src]['featureloc'] = $featureloc;
        $floc_sequences[$src]['residues'] = $residues;
        //$floc_sequences[$src]['formatted_seq'] =  tripal_feature_color_sequence($residues, $parts, $floc_sequences[$src]['defline']);
      }
    }
    return $floc_sequences;
  }

  /**
   * Get features related to the current feature to a given depth. Recursive function.
   *
   * @param $feature_id
   * @param $substitute
   * @param $levels
   * @param $base_type_id
   * @param $depth
   *
   * @ingroup tripal_feature
   */
  private static function get_aggregate_relationships($feature_id, $substitute=1,
      $levels=0, $base_type_id=NULL, $depth=0) {

    // we only want to recurse to as many levels deep as indicated by the
    // $levels variable, but only if this variable is > 0. If 0 then we
    // recurse until we reach the end of the relationships tree.
    if ($levels > 0 and $levels == $depth) {
      return NULL;
    }

    // first get the relationships for this feature
    return self::get_relationships($feature_id, 'as_object');

  }

  /**
   * Get the relationships for a feature.
   *
   * @param $feature_id
   *   The feature to get relationships for
   * @param $side
   *   The side of the relationship this feature is (ie: 'as_subject' or 'as_object')
   *
   * @ingroup tripal_feature
   */
  private static function get_relationships($feature_id, $side = 'as_subject') {
    // get the relationships for this feature.  The query below is used for both
    // querying the object and subject relationships
    $sql = "
    SELECT
      FS.name as subject_name, FS.uniquename as subject_uniquename,
      CVTS.name as subject_type, CVTS.cvterm_id as subject_type_id,
      FR.subject_id, FR.type_id as relationship_type_id, FR.object_id, FR.rank,
      CVT.name as rel_type,
      FO.name as object_name, FO.uniquename as object_uniquename,
      CVTO.name as object_type, CVTO.cvterm_id as object_type_id
    FROM {feature_relationship} FR
     INNER JOIN {cvterm} CVT  ON FR.type_id    = CVT.cvterm_id
     INNER JOIN {feature} FS  ON FS.feature_id = FR.subject_id
     INNER JOIN {feature} FO  ON FO.feature_id = FR.object_id
     INNER JOIN {cvterm} CVTO ON FO.type_id    = CVTO.cvterm_id
     INNER JOIN {cvterm} CVTS ON FS.type_id    = CVTS.cvterm_id
  ";
    if (strcmp($side, 'as_object')==0) {
      $sql .= " WHERE FR.object_id = :feature_id";
    }
    if (strcmp($side, 'as_subject')==0) {
      $sql .= " WHERE FR.subject_id = :feature_id";
    }
    $sql .= " ORDER BY FR.rank";

    // get the relationships
    $results = chado_query($sql, array(':feature_id' => $feature_id));


    // iterate through the relationships, put these in an array and add
    // in the Drupal node id if one exists
    $i=0;
    $esql = "
        SELECT entity_id
        FROM {chado_entity}
        WHERE data_table = 'feature' AND record_id = :feature_id";
    $relationships = array();
    while ($rel = $results->fetchObject()) {
      $entity = db_query($esql, array(':feature_id' => $rel->subject_id))->fetchObject();
      if ($entity) {
        $rel->subject_entity_id = $entity->entity_id;
      }
      $entity = db_query($esql, array(':feature_id' => $rel->object_id))->fetchObject();
      if ($entity) {
        $rel->object_entity_id = $entity->entity_id;
      }
      $relationships[$i++] = $rel;
    }
    return $relationships;
  }

  /**
   * Load the locations for a given feature
   *
   * @param $feature_id
   *   The feature to look up locations for
   * @param $side
   *   Whether the feature is the scrfeature, 'as_parent', or feature, 'as_child'
   * @param $aggregate
   *   Whether or not to get the locations for related features
   *
   * @ingroup tripal_feature
   */
  private static function get_featurelocs($feature_id, $side = 'as_parent', $aggregate = 1) {

    $sql = "
    SELECT
       F.name, F.feature_id, F.uniquename,
       FS.name as src_name, FS.feature_id as src_feature_id, FS.uniquename as src_uniquename,
       CVT.name as cvname, CVT.cvterm_id,
       CVTS.name as src_cvname, CVTS.cvterm_id as src_cvterm_id,
       FL.fmin, FL.fmax, FL.is_fmin_partial, FL.is_fmax_partial,FL.strand, FL.phase
     FROM {featureloc} FL
       INNER JOIN {feature} F   ON FL.feature_id = F.feature_id
       INNER JOIN {feature} FS  ON FS.feature_id = FL.srcfeature_id
       INNER JOIN {cvterm} CVT  ON F.type_id     = CVT.cvterm_id
       INNER JOIN {cvterm} CVTS ON FS.type_id    = CVTS.cvterm_id
   ";
    if (strcmp($side, 'as_parent')==0) {
      $sql .= "WHERE FL.srcfeature_id = :feature_id ";
    }
    if (strcmp($side, 'as_child')==0) {
      $sql .= "WHERE FL.feature_id = :feature_id ";
    }

    $flresults = chado_query($sql, array(':feature_id' => $feature_id));

    // copy the results into an array
    $i=0;
    $featurelocs = array();
    while ($loc = $flresults->fetchObject()) {
      // if a drupal node exists for this feature then add the nid to the
      // results object

      $loc->feid = tripal_get_chado_entity_id('feature', $loc->feature_id);
      $loc->seid = tripal_get_chado_entity_id('feature', $loc->src_feature_id);
      // add the result to the array
      $featurelocs[$i++] = $loc;
    }

    // Add the relationship feature locs if aggregate is turned on
    if ($aggregate and strcmp($side, 'as_parent')==0) {
      // get the relationships for this feature without substituting any children
      // for the parent. We want all relationships
      $relationships = tripal_feature_get_aggregate_relationships($feature_id, 0);
      foreach ($relationships as $rindex => $rel) {
        // get the featurelocs for each of the relationship features
        $rel_featurelocs = tripal_feature_load_featurelocs($rel->subject_id, 'as_child', 0);
        foreach ($rel_featurelocs as $findex => $rfloc) {
          $featurelocs[$i++] = $rfloc;
        }
      }
    }

    usort($featurelocs, 'chado_feature__residues_sort_locations');
    return $featurelocs;
  }
}

/**
 * Callback function for validating the chado_feature__residues_widget.
 */
function chado_feature__residues_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // Remove any white spaces.
  //$residues = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'feature__residues');
  if ($residues) {
    $residues = preg_replace('/\s/', '', $residues);
    tripal_chado_set_field_form_values($field_name, $form_state, $residues, 0, 'feature__residues');
  }
}

/**
 * Used to sort the list of relationship parts by start position
 *
 * @ingroup tripal_feature
 */
function chado_feature__residues_sort_rel_parts_by_start($a, $b) {
  foreach ($a as $type_name => $details) {
    $astart = $a[$type_name]['start'];
    break;
  }
  foreach ($b as $type_name => $details) {
    $bstart = $b[$type_name]['start'];
    break;
  }
  return strnatcmp($astart, $bstart);
}
/**
 * Used to sort the feature locs by start position
 *
 * @param $a
 *   One featureloc record (as an object)
 * @param $b
 *   The other featureloc record (as an object)
 *
 * @return
 *   Which feature location comes first
 *
 * @ingroup tripal_feature
 */
function chado_feature__residues_sort_locations($a, $b) {
  return strnatcmp($a->fmin, $b->fmin);
}