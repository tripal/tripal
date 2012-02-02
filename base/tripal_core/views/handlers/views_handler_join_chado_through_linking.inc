<?php

/**
 * Handler to allow joins between records via a linking table
 *
 * Example Usage:
 *   To join the analysis table to the feature table through the analysisfeature table,
 *   (ie: get analysis fields to show up in a feature view)
 *   Use the following code in the analysis hook_views_data:
 *   @code
        $data['analysis']['table']['join']['feature'] = array(
          'linking' => array(
            'table' => 'analysisfeature',
            'left_field' => 'feature_id',
            'field' => 'analysis_id',
          ),
          'left_field' => 'feature_id',
          'field' => 'analysis_id',
          'handler' => 'views_handler_join_chado_through_linking'
        ); 
 *   @endcode
 *
 * NOTE: If the right table is in the drupal schema rather then the chado schema 
 *  (ie: node, chado_feature) then add the following to the above join description:
 *  @code
    'table_is_drupal' => TRUE
 *  @endcode
 *  This will ensure the drupal table is surrounded by { } and as such any database
 *  prefixes are added correctly. If the left table is in the drupal schema it should already
 *  be defined by a previous join (or the From clause).
 */
class views_handler_join_chado_through_linking extends views_join {

  // PHP 4 doesn't call constructors of the base class automatically from a
  // constructor of a derived class. It is your responsibility to propagate
  // the call to constructors upstream where appropriate.
  function construct($table = NULL, $left_table = NULL, $left_field = NULL, $field = NULL, $extra = array(), $type = 'LEFT', $added = NULL) {
    parent::construct($table, $left_table, $left_field, $field, $extra, $type);
  }
  
  /**
   * Creates SQL for both joins table => linking_table and linking_table => left_table
   * NOTE: Uses fields in definition as passed in from hook_views_data join definition
   */
  function join($table, &$query) {
    $output = '';
    $joins = array();

    $joins[] = $this->create_single_join(
      $query,
      array(
        'table' => $this->definition['linking']['table'],
        'field' => $this->definition['linking']['left_field'],
        'is_drupal' => FALSE,
      ),
      array(
        'table' => $this->definition['left_table'],
        'field' => $this->definition['left_field'],
      ),
      'LEFT'
    );
    
    $joins[] = $this->create_single_join(
      $query,
      array(
        'table' => $this->definition['table'],
        'field' => $this->definition['field'],
        'is_drupal' => $this->definition['table_is_drupal'],
      ),
      array(
        'table' => $this->definition['linking']['table'],
        'field' => $this->definition['linking']['field'],
      ),
      'LEFT'
    );
    
    $output .= implode("\n",$joins);
    return $output;
  }
  
  /**
   * Creates SQL for a single join based on parameters
   */
  function create_single_join (&$query, $right_spec, $left_spec, $join_type) {
    
    if ($right_spec['table']) {
      $right = $query->get_table_info($right_spec['table']);
      if (!$right['alias']) { $right['alias'] = $right_spec['table']; }
      $right_field = "$right[alias].$right_spec[field]";

      if ($right_spec['is_drupal']) {
        $right_table = '{'.$right_spec['table'].'}';
      } else {
        $right_table = $right_spec['table'];
      }
    }   

    if ($left_spec['table']) {
      $left = $query->get_table_info($left_spec['table']);
      if (!$left['alias']) { $left['alias'] = $left_spec['table']; }
      $left_field = "$left[alias].$left_spec[field]";
    } else {
      // This can be used if left_field is a formula or something. It should be used only *very* rarely.
      $left_field = $this->left_spec['field'];
    }
    
    $output = " $join_type JOIN $right_table $right[alias] ON $left_field = $right_field";
     
    return $output;
  }
 
}