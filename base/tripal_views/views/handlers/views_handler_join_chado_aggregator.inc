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
    
    // Determine the postgresql version
    $postgresql_version = pg_version();
    $this->postgresql_version = $postgresql_version['client'];

    // If version is 9.0+ then indicate
    // Needed to apply sorting for aggregated fields
    if (intval($postgresql_version['client']) >= 9) {
      $this->postgresql_9up = TRUE;
    }

  }

  /**
   * Creates SQL including aggregation query used in join
   */
  function join($table, &$query) {
    $output = array();

    // Create the table SQL (used in join) -------
    // query creating one-to-one table using array_agg
    $table_desc = module_invoke_all('chado_'.$this->definition['table'].'_schema');
    $select_fields[ $this->definition['table'] ] = $table_desc['fields'];
    
    // Add joins to tables with a foreign key in this table
    // (ie: add join to cvterm if this table has a type_id
    $joins = array();
    foreach($table_desc['foreign keys'] as $defn) {
      if ($defn['table'] != $this->left_table) {
        foreach( $defn['columns'] as $left => $right) {
          $left = $this->definition['table'] .'.'. $left;
          $right = $defn[table] .'.'. $right;
          $joins[] = "LEFT JOIN $defn[table] $defn[table] ON $left=$right";
        }
        
        // Fields to be selected from joined table
        $join_table = module_invoke_all('chado_'.$defn['table'].'_schema');
        $select_fields[ $defn['table'] ] = $join_table['fields'];
      } 
    }
    
    // Determine Order BY's for aggregates
    $order_by = array();
    foreach ($this->sort as $s) {
      $order_by[] = $s['table'].'.'.$s['field'].' '.$s['order'];
    }
    
    
    // Fields to be selected
    foreach ($select_fields as $table => $table_fields) {
      foreach ($table_fields as $fname => $f) {
        $alias = '';
        if ($table != $this->definition['table']) {
          $alias = $table .'_';
        }
        
        if ($fname != $this->definition['field']) {
          // Add sort to aggregate field if postgreSQL 9.0+
          if ($this->postgresql_9up && !empty($order_by)) {
            $fields[] = 'array_agg('.$table.'.'.$fname.' ORDER BY '.implode(',',$order_by).') as '.$alias.$fname;
          } else {
            $fields[] = 'array_agg('.$table.'.'.$fname.') as '.$alias.$fname;
          }
          $composite_field_parts[] = "'".$alias.$fname."::' ||".$table.'.'.$fname;
        } else {
          $fields[] = $fname;
          $composite_field_parts[] = "'".$alias.$fname."::' ||".$table.'.'.$fname;
        }
      }
    }
    
    // composite field 
    // (combines all other fields before aggregating)
    // Add sort to aggregate field if postgreSQL 9.0+
    if ($this->postgresql_9up && !empty($order_by)) {
      $composite_field = "array_agg('{'||".implode(" || ',' || ",$composite_field_parts)."||'}' ORDER BY ".implode(',',$order_by).") as all";
    } else {
      $composite_field = "array_agg('{'||".implode(" || ',' || ",$composite_field_parts)."||'}') as all";
    }
    $fields[] = $composite_field;
    
    // SQL to use in the join
    $sql = 'SELECT '.implode(', ',$fields)
      .' FROM '.$this->definition['table']
      .' '.implode(' ',$joins);
 
    if (!empty($this->filter)) {
      $sql .= ' WHERE '.implode(', ', $this->filter);
    }
    
    $sql .= ' GROUP BY '.$this->definition['field'];
    
    // Create the join (full SQL) ----------------
    $output[] = $this->create_single_join(
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
    
    return implode("\n",$output);
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