<?php

/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_linker__featureloc_info() {
  return array(
    'label' => t('Alignments'),
    'description' => t('Locations on landmark sequences where the feature is aligned.'),
    'default_widget' => 'chado_linker__featureloc_widget',
    'default_formatter' => 'chado_linker__featureloc_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_linker__featureloc_widget_info() {
  return array(
    'label' => t('Alignment Settings'),
    'field types' => array('chado_linker__featureloc')
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_linker__featureloc_formatter_info() {
  return array(
    'label' => t('Alignments'),
    'field types' => array('chado_linker__featureloc'),
    'settings' => array(
    ),
  );
}

/**
 * Implements hook_formatter_settings_summary.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_summary() for each field
 * to specify.
 *
 */
function chado_linker__featureloc_formatter_settings_summary($field, $instance,
    $view_mode) {

}

/**
 * Provides a settings form for the formatter.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_form() for each field
 * to specify.
 */
function chado_linker__featureloc_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {



}

/**
 * Validation function for the chado_linker_featureloc_formatter_settings_form.
 */
function chado_linker__featureloc_formatter_settings_form_validate(&$form, &$form_state) {

  // Place here as an example for validating the settings form.
}

/**
 *
 */
function chado_linker__featureloc_formatter(&$element, $entity_type, $entity,
    $field, $instance, $langcode, $items, $display) {

  // Get the settings
  $settings = $display['settings'];

  foreach ($items as $delta => $item) {
    $alignments = $item['featurelocs'];
    if (count($alignments) > 0){

      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Aligned Feature' ,'Feature Type', 'Alignment Location');

      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();

      foreach ($alignments as $alignment){
        $feature_name = $alignment->name;
        if (property_exists($alignment, 'nid')) {
          $feature_name = l($feature_name, "node/" . $alignment->nid);
        }
        $feature_loc = '';
        $strand = '.';
        if ($alignment->strand == -1) {
          $strand = '-';
        }
        elseif ($alignment->strand == 1) {
           $strand = '+';
        }
        // if this is a match then make the other location
        if(property_exists($alignment, 'right_feature')){
          $rstrand = '.';
          if ($alignment->right_strand == -1) {
            $rstrand = '-';
          }
          elseif ($alignment->right_strand == 1) {
            $rstrand = '+';
          }
          $feature_loc = $feature->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand;
          $feature_loc .= "<br>" . $alignment->name .":". ($alignment->right_fmin + 1) . ".." . $alignment->right_fmax . " " . $rstrand;
        }
        else {
          $feature_loc = $alignment->name .":". ($alignment->fmin + 1) . ".." . $alignment->fmax . " " . $strand;
        }

        $rows[] = array(
          $feature_name,
          $alignment->type,
          $feature_loc
        );
      }

      // the $table array contains the headers and rows array as well as other
      // options for controlling the display of the table.  Additional
      // documentation can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $table = array(
        'header' => $headers,
        'rows' => $rows,
        'attributes' => array(
          'id' => 'tripal_feature-table-alignments',
          'class' => 'tripal-data-table'
        ),
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => array(),
        'empty' => '',
      );

      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
      );
    }
  }
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 */
function chado_linker__featureloc_load($field, $entity, $base_table, $record) {
  $field_name = $field['field_name'];

  $options = array(
    'return_array' => 1,
    'include_fk' => array(
      'srcfeature_id' => array(
        'type_id' => 1,
      ),
      'feature_id' => array(
        'type_id' => 1
      ),
    )
  );
  $feature = chado_expand_var($record, 'table', 'featureloc', $options);

  // get alignments as child
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
    $cfeaturelocs = array();
  }
  // get alignment as parent
  $pfeaturelocs = $feature->featureloc->srcfeature_id;
  if (!$pfeaturelocs) {
    $pfeaturelocs = array();
  }

  // get matched alignments (those with an itermediate 'match' or 'EST_match', etc
  $mfeaturelocs = chado_linker__featureloc_get_matched_alignments($feature);
  $feature->matched_featurelocs = $mfeaturelocs;

  // combine all three alignments into a single array for printing together in
  // a single list
  $alignments = array();
  foreach ($pfeaturelocs as $featureloc) {
    // if type is a 'match' then ignore it. We will handle those below
    if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
      continue;
    }
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->name = $featureloc->feature_id->name;
    $alignment->type = $featureloc->feature_id->type_id->name;
    $alignment->fmin = $featureloc->fmin;
    $alignment->fmax = $featureloc->fmax;
    $alignment->phase = $featureloc->phase;
    $alignment->strand = $featureloc->strand;
    $alignments[] = $alignment;
    if (property_exists($featureloc->feature_id, 'nid')) {
      $alignment->nid = $featureloc->feature_id->nid;
    }
  }
  foreach ($cfeaturelocs as $featureloc) {
    // if type is a 'match' then ignore it. We will handle those below
    if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
      continue;
    }
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->name = $featureloc->srcfeature_id->name;
    $alignment->type = $featureloc->srcfeature_id->type_id->name;
    $alignment->fmin = $featureloc->fmin;
    $alignment->is_fmin_partial = $featureloc->is_fmin_partial;
    $alignment->fmax = $featureloc->fmax;
    $alignment->is_fmax_partial = $featureloc->is_fmax_partial;
    $alignment->phase = $featureloc->phase;
    $alignment->strand = $featureloc->strand;
    $alignments[] = $alignment;
    if (property_exists($featureloc->srcfeature_id, 'nid')) {
      $alignment->nid = $featureloc->srcfeature_id->nid;
    }
  }
  // in matching features, the left feature is always the feature
  // provided to this function.
  foreach ($mfeaturelocs as $featureloc) {
    // get more information about the right feature
    $select = array('feature_id' => $featureloc->right_srcfeature_id);
    $rfeature = chado_generate_var('feature', $select);
    // now add to the list
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->right_feature = $rfeature;
    $alignment->name = $rfeature->name;
    $alignment->type = $rfeature->type_id->name;
    $alignment->fmin = $featureloc->left_fmin;
    $alignment->is_fmin_partial = $featureloc->left_is_fmin_partial;
    $alignment->fmax = $featureloc->left_fmax;
    $alignment->is_fmax_partial = $featureloc->left_is_fmax_partial;
    $alignment->phase = $featureloc->left_phase;
    $alignment->strand = $featureloc->left_strand;
    $alignment->right_fmin = $featureloc->right_fmin;
    $alignment->right_is_fmin_partial = $featureloc->right_is_fmin_partial;
    $alignment->right_fmax = $featureloc->right_fmax;
    $alignment->right_is_fmax_partial = $featureloc->right_is_fmax_partial;
    $alignment->right_phase = $featureloc->right_phase;
    $alignment->right_strand = $featureloc->right_strand;
    $alignments[] = $alignment;
    if (property_exists($rfeature, 'nid')) {
      $alignment->nid = $rfeature->nid;
    }
  }
  $entity->{$field_name}['und'][0]['featurelocs'] = $alignments;
}
/**
 * This function is for features that align through an intermediate such
 * as 'EST_match' or 'match'.  This occurs in the case where two sequences
 * align but where one does not align perfectly.  Some ESTs may be in a contig
 * but not all of the EST.  Portions may overhang and not be included in the
 * consensus if quality is bad.
 * For example:
 *    Feature 1: Contig --------------------
 *    Feature 2: EST_match           -------
 *    Feature 3: EST                 ---------
 *
 * The feature provided to the function will always be the feature 1.  The
 * featureloc columns prefixed with 'right' (e.g. right_fmin) belong to the
 * alignment of feature 3 with feature 2
 *
 * Features may align to more than one feature and are not matches. We do
   * not want to include these, so we have to filter on the SO terms:
   * match, or %_match
   *
   * @ingroup tripal_feature
   */
 function chado_linker__featureloc_get_matched_alignments($feature) {

   $sql = "
     SELECT
       FL1.featureloc_id    as left_featureloc_id,
       FL1.srcfeature_id    as left_srcfeature_id,
       FL1.feature_id       as left_feature_id,
       FL1.fmin             as left_fmin,
       FL1.is_fmin_partial  as left_is_fmin_partial,
       FL1.fmax             as left_fmax,
       FL1.is_fmax_partial  as left_is_fmax_partial,
       FL1.strand           as left_strand,
       FL1.phase            as left_phase,
       FL1.locgroup         as left_locgroup,
       FL1.rank             as left_rank,
       FL2.featureloc_id    as right_featureloc_id,
       FL2.srcfeature_id    as right_srcfeature_id,
       FL2.feature_id       as right_feature_id,
       FL2.fmin             as right_fmin,
       FL2.is_fmin_partial  as right_is_fmin_partial,
       FL2.fmax             as right_fmax,
       FL2.is_fmax_partial  as right_is_fmax_partial,
       FL2.strand           as right_strand,
       FL2.phase            as right_phase,
       FL2.locgroup         as right_locgroup,
       FL2.rank             as right_rank
     FROM {feature} F1
       INNER JOIN {featureloc} FL1 on FL1.srcfeature_id = F1.feature_id
       INNER JOIN {feature} F2 on FL1.feature_id = F2.feature_id
       INNER JOIN {featureloc} FL2 on FL2.feature_id = F2.feature_id
       INNER JOIN {cvterm} CVT2 on F2.type_id = CVT2.cvterm_id
     WHERE
       F1.feature_id = :feature_id  AND
       (CVT2.name = 'match' or CVT2.name like '%_match')
     ORDER BY FL1.fmin
   ";

   $results = chado_query($sql, array(':feature_id' => $feature->feature_id));

   // iterate through the results and add them to our featurelocs array
   $featurelocs = array();
   while ($fl = $results->fetchObject()) {
     // ignore featurelocs where the left and right srcfeature is the same
     if (strcmp($fl->left_srcfeature_id, $fl->right_srcfeature_id) == 0) {
       continue;
     }
     $featurelocs[] = $fl ;
   }
   return $featurelocs;
 }

/**
 * Implements hook_ws_formatter().
 */
function chado_linker__featureloc_ws_formatter(&$element, $entity_type, $entity,
    $field, $instance, $items) {

}

/**
 *  Implements hook_widget().
 */
function chado_linker__featureloc_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

}
/**
 * Callback function for validating the chado_linker_featureloc_widget.
 */
function chado_linker__featureloc_widget_validate($element, &$form_state) {

}