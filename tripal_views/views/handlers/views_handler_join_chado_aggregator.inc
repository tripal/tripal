<?php

module_load_include('inc', 'views', 'includes/base');
module_load_include('inc', 'views', 'includes/handlers');

/**
 * @file
 * Handler to allow joins between records via a linking table
 *
 * Example Usage:
 *   To join the analysisprop table to the analysis table,
 *   Use the following code in the analysisprop hook_views_data:
 *   @code
        $data['analysisprop']['table']['join']['analysis'] = array(
          'left_field' => 'analysis_id',
          'field' => 'analysis_id',
          'handler' => 'views_handler_join_chado_aggregator',
          'pre-aggregated' => TRUE | FALSE //whether the table is already aggregated (contains arrays)
          'table_aggregated' => CURRENT | LEFT //the table which has many records for each record in the other
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

    $opt = array(
      'table' => $this->definition['table'],
      'field' => $this->definition['field'],
      'left_table' => $this->definition['left_table'],
      'left_field' => $this->definition['left_field'],
      'table_aggregated' => $this->definition['table_aggregated'],
      'sort' => $this->sort,
      'filter' => $this->filter,
      'postgresql_9up' => $this->postgresql_9up,
    );

    $output = $this->aggregate_join($query, $opt);
    return implode("\n", $output);

  }

  function aggregate_join (&$query, $opt) {
    // Create the table SQL (used in join) -------
    // query creating one-to-one table using array_agg

    // Only aggregate each field if it the join table hadn't been pre-aggregated
    // Example where it might be pre-aggregated: Materialized view
    if (!$this->definition['pre-aggregated']) {

      $sql = $this->get_aggregate_sql_for_table_field($opt);

      // Create the join (full SQL) ----------------
      $output[] = $this->create_single_join(
        $query,
        array(
          'table' => $opt['table'],
          'field' => $opt['field'],
          'table_sql' => $sql,
          'is_drupal' => FALSE,
        ),
        array(
          'table' => $opt['left_table'],
          'field' => $opt['left_field'],
        ),
        'LEFT'
      );

    // Otherwise the table has been pre-aggregated
    // Then only need to do a regular join with any in where
    }
    else {

      // Create the join

      $current_table_spec = array(
        'table' => $opt['table'],
        'field' => $opt['field'],
        'is_drupal' => FALSE,
      );
      $left_table_spec = array(
        'table' => $opt['left_table'],
        'field' => $opt['left_field'],
      );

      switch ($opt['table_aggregated']) {
        default:
        case 'CURRENT':
          $current_table_spec['pre-aggregated'] = TRUE;
        break;
        case 'LEFT':
          $left_table_spec['pre-aggregated'] = TRUE;
        break;
      }

      $output[] = $this->create_single_join(
        $query,
        $current_table_spec,
        $left_table_spec,
        'LEFT'
      );
    }

    return $output;
  }

  /**
   * Create the SQL needed to aggregate a table
   */
  function get_aggregate_sql_for_table_field($opt) {

    // Determine Order BY's for aggregates
    $order_by = array();
    if (!is_array($opt['sort'])) {
      $opt['sort'] = array();
    }
    foreach ($opt['sort'] as $s) {
      $order_by[] = $s['table'] . '.' . $s['field'] . ' ' . $s['order'];
    }

    // get table description (if defined via schema api)
    $table_desc = tripal_core_get_chado_table_schema($opt['table']);
    $select_fields[ $opt['table'] ] = $table_desc['fields'];

    if (!empty($table_desc)) {
      // Add joins to tables with a foreign key in this table
      // (ie: add join to cvterm if this table has a type_id
      $joins = array();
      foreach ($table_desc['foreign keys'] as $defn) {
        if ($defn['table'] != $opt['left_table']) {
          foreach ( $defn['columns'] as $left => $right) {
            $left = $opt['table'] . '.' . $left;
            $right = $defn['table'] . '.' . $right;
            $joins[] = "LEFT JOIN $defn[table] $defn[table] ON $left=$right";
          }

          // Fields to be selected from joined table
          $join_table = tripal_core_get_chado_table_schema($defn['table']);
          $select_fields[ $defn['table'] ] = $join_table['fields'];
        }
      }

      // Fields to be selected
      foreach ($select_fields as $table => $table_fields) {
        foreach ($table_fields as $fname => $f) {
          $alias = '';
          if ($table != $opt['table']) {
            $alias = $table . '_';
          }

          if ($fname != $opt['field']) {
            // Add sort to aggregate field if postgreSQL 9.0+
            if ($opt['postgresql_9up'] && !empty($order_by)) {
              $fields[] = 'array_agg(' . $table . '.' . $fname . ' ORDER BY ' . implode(',', $order_by) . ') as ' . $alias . $fname;
            }
            else {
              $fields[] = 'array_agg(' . $table . '.' . $fname . ') as '. $alias . $fname;
            }
            $composite_field_parts[] = "'" . $alias . $fname . "::' ||" . $table . '.' . $fname;
          }
          else {
            $fields[] = $fname;
            $composite_field_parts[] = "'" . $alias . $fname . "::' ||" . $table . '.' . $fname;
          }
        }
      }

    // There is no definition in schema api
    // then use postgresql select
    }
    else {

      // No known foreign key reelationships
      $joins = array();

      // Fields to be selected
      $sql = "SELECT
                attname as column,
                format_type(atttypid, atttypmod) as datatype
              FROM pg_attribute, pg_type
              WHERE typname='nd_genotype_experiment'
                AND attrelid=typrelid
                AND attname NOT IN ('cmin','cmax','ctid','oid','tableoid','xmin','xmax')";
      $previous_db = tripal_db_set_active('chado');
      $resource = db_query($sql);
      tripal_db_set_active($previous_db);
      while ($r = db_fetch_object($resource)) {
        $table = $opt['table'];
        $alias = ''; //no alias needed if table is current table (only option if no schema api definition)
        $fname = $r->column;

        if ($fname != $opt['field']) {
          // Add sort to aggregate field if postgreSQL 9.0+
          if ($opt['postgresql_9up'] && !empty($order_by)) {
            $fields[] = 'array_agg(' . $table . '.' . $fname . ' ORDER BY ' . implode(',', $order_by) . ') as ' . $alias . $fname;
          }
          else {
            $fields[] = 'array_agg(' . $table . '.' . $fname . ') as ' . $alias . $fname;
          }
          $composite_field_parts[] = "'" . $alias . $fname . "::' ||" . $table . '.' . $fname;
        }
        else {
          $fields[] = $fname;
          $composite_field_parts[] = "'" . $alias . $fname . "::' ||" . $table . '.' . $fname;
        }
      }

    }

    // composite field
    // (combines all other fields before aggregating)
    // Add sort to aggregate field if postgreSQL 9.0+
    if ($opt['postgresql_9up'] && !empty($order_by)) {
      $composite_field = "array_agg('{'||" . implode(" || ',' || ", $composite_field_parts) . "||'}' ORDER BY " . implode(',', $order_by) . ") as all";
    }
    else {
      $composite_field = "array_agg('{'||" . implode(" || ',' || ", $composite_field_parts) . "||'}') as all";
    }
    $fields[] = $composite_field;

    // SQL to use in the join
    $sql = 'SELECT ' . implode(', ', $fields)
      .' FROM ' . $opt['table']
      .' ' . implode(' ', $joins);

    if (!empty($opt['filter'])) {
      $sql .= ' WHERE ' . implode(', ', $opt['filter']);
    }

    $sql .= ' GROUP BY ' . $opt['field'];

    return $sql;
  }

  /**
   * Creates SQL for a single join based on parameters
   * Join will be: <type> JOIN (<query creating one-to-one table using array_agg>) <table alias>
   *                  ON <qualified left field>=<qualified right field>
   */
  function create_single_join(&$query, $right_spec, $left_spec, $join_type) {

    if ($right_spec['table']) {
      $right = $query->get_table_info($right_spec['table']);
      if (!$right['alias']) {
        $right['alias'] = $right_spec['table'];
      }
      $right_field = "$right[alias].$right_spec[field]";

      // Add any() around field if already aggregated
      if ($right_spec['pre-aggregated']) {
        $right_field = "any(" . $right_field . ")";
      }

      // Add drupal { } around table
      if ($right_spec['is_drupal']) {
        $right_table = '{' . $right_spec['table'] . '}';
      }
      else {
        $right_table = $right_spec['table'];
      }
    }

    if ($left_spec['table']) {
      $left = $query->get_table_info($left_spec['table']);
      if (!$left['alias']) {
        $left['alias'] = $left_spec['table'];
      }
      $left_field = "$left[alias].$left_spec[field]";
    }
    else {
      // This can be used if left_field is a formula or something. It should be used only *very* rarely.
      $left_field = $this->left_spec['field'];
    }

    // Add any() around field if already aggregated
    if ($left_spec['pre-aggregated']) {
      $left_field = "any(" . $left_field . ")";
    }

    // Concatenate parts together to form join sql
    if (!empty($right_spec['table_sql'])) {
      $output = " $join_type JOIN ($right_spec[table_sql]) $right[alias] ON $left_field = $right_field";
    }
    else {
      $output = " $join_type JOIN $right_spec[table] $right[alias] ON $left_field = $right_field";
    }

    return $output;
  }

}
