<?php

/**
 * Handler to allow joins between records via a linking table
 *
 * Example Usage:
 *   To join the analysisprop table to the analysis table,
 *   Use the following code in the analysisprop hook_views_data:
 *   @code
        $data['analysisprop']['table']['join']['analysis'] = array(
          'left_field' => 'analysis_id',
          'field' => 'analysis_id',
          'handler' => 'views_handler_join_chado_aggregator'
        );
 *   @endcode
 */
class views_handler_join_chado_aggregator extends views_join {

  // PHP 4 doesn't call constructors of the base class automatically from a
  // constructor of a derived class. It is your responsibility to propagate
  // the call to constructors upstream where appropriate.
  function construct($table = NULL, $left_table = NULL, $left_field = NULL, $field = NULL, $extra = array(), $type = 'LEFT', $added = NULL) {
    parent::construct($table, $left_table, $left_field, $field, $extra, $type);
  }

  /**
   * Creates SQL including aggregation query used in join
   */
  function join($table, &$query) {
    $output = '';
    $joins = array();

    // Create the table SQL (used in join) -------
    // query creating one-to-one table using array_agg
    $table_desc = module_invoke_all('chado_'.$this->definition['table'].'_schema');

    // Fields to be selected
    $fields = array();
    foreach ($table_desc['fields'] as $fname => $f) {
      if ($fname != $this->definition['field']) {
        $fields[] = 'array_agg('.$fname.') as '.$fname;
        $composite_field_parts[] = "'".$fname."::' ||".$fname;
      } else {
        $fields[] = $fname;
      }
    }
    
    // composite field 
    // (combines all other fields before aggregating)
    $composite_field = "array_agg('{'||".implode(" || ',' || ",$composite_field_parts)."||'}') as all";
    $fields[] = $composite_field;
    
    // SQL to use in the join
    $sql = 'SELECT '.implode(', ',$fields).' FROM '.$this->definition['table'].' GROUP BY '.$this->definition['field'];
   
    // Create the join (full SQL) ----------------
    $joins[] = $this->create_single_join(
      $query,
      array(
        'table' => $this->definition['table'],
        'field' => $this->definition['field'],
        'table_sql' => $sql,
        'is_drupal' => FALSE,
      ),
      array(
        'table' => $this->definition['left_table'],
        'field' => $this->definition['left_field'],
      ),
      'LEFT'
    );
    
    $output .= implode("\n",$joins);
    return $output;
  }
  
  /**
   * Creates SQL for a single join based on parameters
   * Join will be: <type> JOIN (<query creating one-to-one table using array_agg>) <table alias> 
   *                  ON <qualified left field>=<qualified right field>
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
    
    $output = " $join_type JOIN ($right_spec[table_sql]) $right[alias] ON $left_field = $right_field";
     
    return $output;
  }
 
}