<?php

class chado_linker__featureloc extends TripalField {

  /**
   * @see TripalField::fieldDefaults()
   */
  public static function fieldDefaults() {
    return array(
      'label' => t('Aligned Locations'),
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
   * @see TripalField::can_attach()
   */
  protected function setCanAttach() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    // If  this is not the feature table then we don't want to attach.
    if ($table_name == 'feature') {
      $this->can_attach = TRUE;
      return;
    }
    $this->can_attach = FALSE;
  }

  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $this->field_name = 'featureloc';
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

    $schema = chado_get_schema('featureloc');
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_linker__featureloc',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => 'featureloc',
        'chado_column' => $pkey,
        'base_table' => 'feature',
        'semantic_web' => 'SO:position_of',
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
      'label' => 'Aligned Locations',
      'description' => 'The locations on other genomic sequences where this
        record has been aligned.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__featureloc_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'chado_linker__featureloc_formatter',
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
      'chado_linker__featureloc_widget' => array(
        'label' => t('Aligned Locations'),
        'field types' => array('chado_linker__featureloc')
      ),
    );
  }

  /**
   * @see TripalField::formatterInfo()
   */
  public static function formatterInfo() {
    return array(
      'chado_linker__featureloc_formatter' => array(
        'label' => t('Aligned Locations'),
        'field types' => array('chado_linker__featureloc'),
        'settings' => array(
        ),
      ),
    );
  }

  /**
   * @see TripalField::formatter_settings_summary()
   */
  public function formatter_settings_summary($field, $instance,
      $view_mode) {

  }

  /**
   * @see TripalField::formatter_settings_form()
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {



  }

  /**
   * @see TripalField::formatterView()
   */
  static function formatterView(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $rows = array();
    $headers = array('Aligned Feature' ,'Feature Type', 'Alignment Location');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }

      $alignment = $item['value'];
      $feature_name = $alignment['name'];
      $feature_loc = '';
      $strand = '.';
      if ($alignment['strand'] == -1) {
        $strand = '-';
      }
      elseif ($alignment['strand'] == 1) {
         $strand = '+';
      }
      // if this is a match then make the other location
      if(array_key_exists('right_feature', $alignment)){
        $rstrand = '.';
        if ($alignment['right_strand'] == -1) {
          $rstrand = '-';
        }
        elseif ($alignment['right_strand'] == 1) {
          $rstrand = '+';
        }
        $feature_loc = $feature['name'] .":". ($alignment['fmin'] + 1) . ".." . $alignment['fmax'] . " " . $strand;
        $feature_loc .= "<br>" . $alignment['name'] .":". ($alignment['right_fmin'] + 1) . ".." . $alignment['right_fmax'] . " " . $rstrand;
      }
      else {
        $feature_loc = $alignment['name'] .":". ($alignment['fmin'] + 1) . ".." . $alignment['fmax'] . " " . $strand;
      }

      $rows[] = array(
        $feature_name,
        $alignment['type'],
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
      'empty' => 'This record is not aligned to any locations.',
    );

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => theme_table($table),
      );
    }
  }

  /**
   * @see TripalField::load()
   */
  static function load($field, $entity, $details = array()) {
    $record = $details['record'];
    $settings = $field['settings'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    $options = array(
      'return_array' => TRUE,
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

    // Get alignments as child
    $cfeaturelocs = $feature->featureloc->feature_id;
    if (!$cfeaturelocs) {
      $cfeaturelocs = array();
    }
    // Get alignment as parent
    $pfeaturelocs = $feature->featureloc->srcfeature_id;
    if (!$pfeaturelocs) {
      $pfeaturelocs = array();
    }

    // Get matched alignments (those with an itermediate 'match' or 'EST_match', etc
    $mfeaturelocs = self::get_matched_alignments($feature);
    $feature->matched_featurelocs = $mfeaturelocs;

    // Combine all three alignments into a single array for printing together in
    // a single list
    $alignments = array();
    foreach ($pfeaturelocs as $featureloc) {
      // if type is a 'match' then ignore it. We will handle those below
      if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
        continue;
      }
      $alignments[] = array(
        'name' => $featureloc->feature_id->name,
        'type' => $featureloc->feature_id->type_id->name,
        'fmin' => $featureloc->fmin,
        'fmax' => $featureloc->fmax,
        'phase' => $featureloc->phase,
        'strand' => $featureloc->strand,
      );
    }
    foreach ($cfeaturelocs as $featureloc) {
      // if type is a 'match' then ignore it. We will handle those below
      if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
        continue;
      }
      $alignments[] = array(
        'name' => $featureloc->srcfeature_id->name,
        'type' => $featureloc->srcfeature_id->type_id->name,
        'fmin' => $featureloc->fmin,
        'is_fmin_partial' => $featureloc->is_fmin_partial,
        'fmax' => $featureloc->fmax,
        'is_fmax_partial' => $featureloc->is_fmax_partial,
        'phase' => $featureloc->phase,
        'strand' => $featureloc->strand,
      );
    }
    // in matching features, the left feature is always the feature
    // provided to this function.
    foreach ($mfeaturelocs as $featureloc) {
      // get more information about the right feature
      $select = array('feature_id' => $featureloc->right_srcfeature_id);
      $rfeature = chado_generate_var('feature', $select);
      // now add to the list
      $alignments[] = array(
        'name' => $rfeature->name,
        'type' => $rfeature->type_id->name,
        'fmin' => $featureloc->left_fmin,
        'is_fmin_partial' => $featureloc->left_is_fmin_partial,
        'fmax' => $featureloc->left_fmax,
        'is_fmax_partial' => $featureloc->left_is_fmax_partial,
        'phase' => $featureloc->left_phase,
        'strand' => $featureloc->left_strand,
        'right_fmin' => $featureloc->right_fmin,
        'right_is_fmin_partial' => $featureloc->right_is_fmin_partial,
        'right_fmax' => $featureloc->right_fmax,
        'right_is_fmax_partial' => $featureloc->right_is_fmax_partial,
        'right_phase' => $featureloc->right_phase,
        'right_strand' => $featureloc->right_strand,
      );
    }
    $i = 0;
    foreach ($alignments as $alignment) {
      $entity->{$field_name}['und'][$i]['value'] = $alignment;
      $i++;
    }
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
   static public function get_matched_alignments($feature) {

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
   * @see TripalField::widgetForm()
   */
   public static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

}
/**
 * Callback function for validating the chado_linker_featureloc_widget.
 */
function chado_linker__featureloc_widget_validate($element, &$form_state) {

}
/**
 * Validation function for the chado_linker_featureloc_formatter_settings_form.
 */
function chado_linker__featureloc_formatter_settings_form_validate(&$form, &$form_state) {

  // Place here as an example for validating the settings form.
}

