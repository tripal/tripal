<?php
/*
 * There are several ways that sequences can be displayed.  They can come from the
 * feature.residues column,  they can come from an alignment with another feature,
 * they can come from a protein sequence that has relationship with this sequence,
 * or they can come from sub children (e.g. CDS coding sequences).
 *
 * This template will show all types depending on the data available.
 *
 */

$feature = $variables['node']->feature;

// number of bases per line in FASTA format
$num_bases = 50;

// we don't want to get the sequence for traditionally large types. They are
// too big,  bog down the web browser, take longer to load and it's not
// reasonable to print them on a page.
$residues = '';
if (strcmp($feature->type_id->name, 'scaffold') != 0 and
  strcmp($feature->type_id->name, 'chromosome') != 0 and
  strcmp($feature->type_id->name, 'supercontig') != 0 and
  strcmp($feature->type_id->name, 'pseudomolecule') != 0) {
  $feature = chado_expand_var($feature, 'field', 'feature.residues');
  $residues = $feature->residues;
}

// get the sequence derived from alignments
$feature = $variables['node']->feature;
$featureloc_sequences = $feature->featureloc_sequences;

if ($residues or count($featureloc_sequences) > 0) {

  $sequences_html = '';  // a variable for holding all sequences HTML text
  $list_items = []; // a list to be used for theming of content on this page

  // ADD IN RESIDUES FOR THIS FEATURE
  // add in the residues if they are present
  if ($residues) {
    $list_items[] = '<a href="#residues">' . $feature->type_id->name . ' sequence</a>';

    // format the sequence to break every 50 residues
    $sequences_html .= '<a name="residues"></a>';
    $sequences_html .= '<div id="residues" class="tripal_feature-sequence-item">';
    $sequences_html .= '<p><b>' . $feature->type_id->name . ' sequence</b></p>';
    $sequences_html .= '<pre class="tripal_feature-sequence">';
    $sequences_html .= '>' . tripal_get_fasta_defline($feature, '', NULL, '', strlen($feature->residues)) . "<br>";
    $sequences_html .= wordwrap($feature->residues, $num_bases, "<br>", TRUE);
    $sequences_html .= '</pre>';
    $sequences_html .= '<a href="#sequences-top">back to top</a>';
    $sequences_html .= '</div>';

  }

  // ADD IN RELATIONSHIP SEQUENCES (e.g. proteins)
  // see the explanation in the tripal_feature_relationships.tpl.php
  // template for how the 'all_relationships' is provided. It is this
  // variable that we use to get the proteins.
  $all_relationships = $feature->all_relationships;
  $object_rels = $all_relationships['object'];
  $has_coding_seq = 0;
  $coding_seq = '';
  foreach ($object_rels as $rel_type => $rels) {
    foreach ($rels as $subject_type => $subjects) {
      foreach ($subjects as $subject) {

        // add in protein sequence if it has residues
        if ($rel_type == 'derives from' and $subject_type == 'polypeptide') {
          $protein = $subject->record->subject_id;
          $protein = chado_expand_var($protein, 'field', 'feature.residues');

          if ($protein->residues) {
            $list_items[] = '<a href="#residues">protein sequence</a>';
            $sequences_html .= '<a name="protein-' . $protein->feature_id . '"></a>';
            $sequences_html .= '<div id="protein-' . $protein->feature_id . '" class="tripal_feature-sequence-item">';
            $sequences_html .= '<p><b>protein sequence of ' . $protein->name . '</b></p>';
            $sequences_html .= '<pre class="tripal_feature-sequence">';
            $sequences_html .= '>' . tripal_get_fasta_defline($protein, '', NULL, '', strlen($protein->residues)) . "<br>";
            $sequences_html .= wordwrap($protein->residues, $num_bases, "<br>", TRUE);
            $sequences_html .= '</pre>';
            $sequences_html .= '<a href="#sequences-top">back to top</a>';
            $sequences_html .= '</div>';
          }
        }

        // If the CDS has sequences then concatenate those. The objects
        // should be returned in order of rank
        if ($rel_type == 'part of' and $subject_type == 'CDS') {
          $cds = $subject->record->subject_id;
          $cds = chado_expand_var($cds, 'field', 'feature.residues');
          if ($cds->residues) {
            $has_coding_seq = 1;
            $coding_seq .= $cds->residues;
          }
        }

        // add any other sequences that are related through a relationship
        // and that have values in the 'residues' column

      }
    }
  }

  // CODING SEQUENCES FROM RELATIONSHIPS
  // add in any CDS sequences.
  if ($has_coding_seq) {
    $list_items[] = '<a href="#coding_sequence">coding sequence </a>';
    $sequences_html .= '<a name="coding_sequence"></a>';
    $sequences_html .= '<div id="coding_sequence" class="tripal_feature-sequence-item">';
    $sequences_html .= '<p><b>coding sequence</b></p>';
    $sequences_html .= '<pre class="tripal_feature-sequence">';
    $sequences_html .= wordwrap($coding_seq, $num_bases, "<br>", TRUE);
    $sequences_html .= '</pre>';
    $sequences_html .= '<a href="#sequences-top">back to top</a>';
    $sequences_html .= '</div>';
  }

  /* ADD IN ALIGNMENT SEQUENCES FOR THIS FEATURE
   * For retreiving the sequence from an alignment we would typically make a call to
   * chado_expand_var function.  For example, to retrieve all
   * of the featurelocs in order to get the sequences needed for this template, the
   * following function call would be made:
   *
   *   $feature = chado_expand_var($feature,'table','featureloc');
   *
   * Then all of the sequences would need to be retreived from the alignments and
   * formatted for display below.  However, to simplify this template, this has already
   * been done by the tripal_feature module and the sequences are made available in
   * the variable:
   *
   *   $feature->featureloc_sequences
   */
  if (count($featureloc_sequences) > 0) {
    foreach ($featureloc_sequences as $src => $attrs) {
      // the $attrs array has the following keys
      //   * id:  a unique identifier combining the feature id with the cvterm id
      //   * type: the type of sequence (e.g. mRNA, etc)
      //   * location:  the alignment location
      //   * defline: the definition line
      //   * formatted_seq: the formatted sequences
      //   * featureloc:  the feature object aligned to
      $list_items[] = '<a href="#' . $attrs['id'] . '">' . $feature->type_id->name . ' from alignment at  ' . $attrs['location'] . "</a>";
      $sequences_html .= '<a name="' . $attrs['id'] . '"></a>';
      $sequences_html .= '<div id="' . $attrs['id'] . '" class="tripal_feature-sequence-item">';
      $sequences_html .= '<p><b>' . $feature->type_id->name . ' from alignment at  ' . $attrs['location'] . '</b></p>';
      $sequences_html .= $attrs['formatted_seq'];
      $sequences_html .= '<a href="#sequences-top">back to top</a>';
      $sequences_html .= '</div>';
    }

    // check to see if this alignment has any CDS. If so, generate a CDS sequence
    $cds_sequence = tripal_get_feature_sequences(
      [
        'feature_id' => $feature->feature_id,
        'parent_id' => $attrs['featureloc']->srcfeature_id->feature_id,
        'name' => $feature->name,
        'featureloc_id' => $attrs['featureloc']->featureloc_id,
      ],
      [
        'width' => $num_bases,
        // FASTA sequence should have $num_bases chars per line
        'derive_from_parent' => 1,
        // CDS are in parent-child relationships so we want to use the sequence from the parent
        'aggregate' => 1,
        // we want to combine all CDS for this feature into a single sequence
        'sub_feature_types' => ['CDS'],
        // we're looking for CDS features
        'is_html' => 1,
      ]
    );
    if (count($cds_sequence) > 0) {
      // the tripal_get_feature_sequences() function can return multiple sequences
      // if a feature is aligned to multiple places. In the case of CDSs we expect
      // that one mRNA is only aligned to a single location on the assembly so we
      // can access the CDS sequence with index 0.
      if ($cds_sequence[0]['residues']) {
        $list_items[] = '<a href="#coding_' . $attrs['id'] . '">coding sequence from alignment at  ' . $attrs['location'] . "</a>";
        $sequences_html .= '<a name="ccoding_' . $attrs['id'] . '"></a>';
        $sequences_html .= '<div id="coding_' . $attrs['id'] . '" class="tripal_feature-sequence-item">';
        $sequences_html .= '<p><b>Coding sequence (CDS) from alignment at  ' . $attrs['location'] . '</b></p>';
        $sequences_html .= '<pre class="tripal_feature-sequence">';
        $sequences_html .= '>' . tripal_get_fasta_defline($feature, '', $attrs['featureloc'], 'CDS', $cds_sequence[0]['length']) . "<br>";
        $sequences_html .= $cds_sequence[0]['residues'];
        $sequences_html .= '</pre>';
        $sequences_html .= '<a href="#sequences-top">back to top</a>';
        $sequences_html .= '</div>';
      }
    }
  }
  ?>

    <div class="tripal_feature-data-block-desc tripal-data-block-desc">The
        following sequences are available for this feature:
    </div>
  <?php

  // first add a list at the top of the page that can be formatted as the
  // user desires.  We use the theme_item_list function of Drupal to create
  // the list rather than hard-code the HTML here.  Instructions for how
  // to create the list can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_item_list/7
  print '<a name="sequences-top"></a>';
  print theme_item_list([
    'items' => $list_items,
    'title' => '',
    'type' => 'ul',
    'attributes' => [],
  ]);

  $message = 'Administrators, sequences will appear on this page if:
    <br><br><b>For any feature type:</b>
    <ul>
      <li>This feature has residues stored in the "residues" field of the feature table of Chado.</li>
      <li>This feature is aligned to another feature (e.g. scaffold, or chromosome). In this case, the
          sequence underlying the alignment will be shown.</li>
    </ul>
    <br><b>For gene models:</b>
    <ul>
      <li>This feature has a "polypeptide" (protein) feature associated via the "feature_relationship" table of Chado with a
          relationship of type "derives from" and the protein feature has residues. Typically, a protein
          is associated with an mRNA feature and protein sequences will appear on the mRNA page.</li>
      <li>This feature has one or more CDS features associated via the "feature_relationship" table of Chado with a
          relationship of type "part of". If the CDS features have residues then those will be concatenated
          and presented as a sequence. Typically, CDSs are associated with an mRNA feature and CDS sequences
          will appear on the mRNA page.</li>
      <li>This feature is aligned to another feature (e.g. scaffold, or chromosome) and this feature has
          one or more CDS features associated.  The CDS sequenes underlying the alignment will be
          shown.</li>
    </ul>
    </p>';
  print tripal_set_message($message, TRIPAL_INFO, ['return_html' => 1]);

  // now print the sequences
  print $sequences_html;
}