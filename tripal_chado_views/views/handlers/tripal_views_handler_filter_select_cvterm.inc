<?php
/**
 * @file
 * Contains tripal_views_handler_filter_select_cvterm
 */

/**
 * This Handler provides a select list for the type field
 *
 *  NOTE: This handler only works when applied to the type_id field in the
 *  base_table of this view.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_select_cvterm extends tripal_views_handler_filter_select_string {

  /**
   * Provide the options used in the select list.
   * Override this function in extended handlers to easily change option list.
   *
   * @return
   *   An array of options where the key is the value of this field in the
   *   database
   */
  function get_select_options() {

    // If the admin has set the "Show All" option then we want to show all the
    // cvterms regardless of whether they are used in the base table or not.
    if (isset($this->options['show_all']) AND $this->options['show_all'] == TRUE) {

      // Get a list of cvs currently used.

      // If the filter is for a field in the cvterm table (weird, I know but
      // we can't assume that tripal admin won't do this) then we only need
      // to make one-hop to the cv table.
      if ($this->table == 'cvterm') {

        $return = $this->get_select_option_where($this->table);
        $where_clauses = $return['where_clauses'];
        $arguments = $return['arguments'];
        $base_where = '';
        if (!empty($where_clauses)) {
          $base_where = implode(' AND ', $where_clauses);
        }

        // Using a "Loose Index Scan" to get a list of all the cvs used
        // in the cvterm table (ie: all the cv's with at least one term).
        // See https://wiki.postgresql.org/wiki/Loose_indexscan
        $sql = "
          WITH RECURSIVE t AS (
            SELECT MIN(cv_id) AS col FROM {!table}
              " . ($base_where == '' ? '' : "WHERE " . $base_where) . "
            UNION ALL
            SELECT (SELECT MIN(cv_id) FROM {!table} WHERE cv_id > col " . ($base_where == '' ? '' : " AND " . $base_where) . ")
              FROM t WHERE col IS NOT NULL
          )
          SELECT cv_id, name
            FROM {cv}
            WHERE cv_id IN (SELECT col FROM t where col IS NOT NULL)
            ORDER BY cv.name ASC";
        $sql = format_string($sql, ['!table' => $this->table]);
      }
      // Otherwise, (most often the case) we need to make two-hops
      // to the cv table through the cvterm table.
      else {

        // There are actually two sets of conditions we care about and of course
        // they are placed in different places in the query :p.
        // 1. Restrictions on the cvterm table. This lets users specify: only
        // show these exact types.
        $return = $this->get_select_option_where('cvterm');
        $where_clauses = $return['where_clauses'];
        $cvterm_args = $return['arguments'];
        $cvterm_where = '';
        if (!empty($where_clauses)) {
          $cvterm_where = implode(' AND ', $where_clauses);
        }
        // 2. Restrictions on the filter table Since those affect which types
        // have been used.
        $return = $this->get_select_option_where($this->table);
        $where_clauses = $return['where_clauses'];
        $base_args = $return['arguments'];
        $base_where = '';
        if (!empty($where_clauses)) {
          $base_where = implode(' AND ', $where_clauses);
        }
        // We only supply one set or arguments those so merge the two.
        $arguments = array_merge($cvterm_args, $base_args);

        // Using a "Loose Index Scan" to get a list of all the cvs used
        // in the table the drop-down filter is from.
        // See https://wiki.postgresql.org/wiki/Loose_indexscan
        $sql = "
          WITH RECURSIVE t AS (
            SELECT MIN(cvterm.cv_id) AS col
              FROM {!table} filter_table
              LEFT JOIN {cvterm} ON filter_table.!field=cvterm.cvterm_id
              " . ($base_where == '' ? '' : "WHERE " . $base_where) . "
            UNION ALL
            SELECT (
                SELECT MIN(cv_id)
                FROM {!table} filter_table
                LEFT JOIN {cvterm} ON filter_table.!field=cvterm.cvterm_id
                WHERE cv_id > col " . ($base_where == '' ? '' : " AND " . $base_where) . "
              )
              FROM t WHERE col IS NOT NULL
          )
          SELECT cvterm_id, name
            FROM {cvterm}
            WHERE cv_id IN (SELECT col FROM t where col IS NOT NULL) " . ($cvterm_where == '' ? '' : " AND " . $cvterm_where) . "
            ORDER BY cvterm.name ASC";
        $sql = format_string($sql, [
          '!table' => $this->table,
          '!field' => $this->field,
        ]);
      }
      $resource = chado_query($sql, $arguments);

      // Now actually gerenate the select list
      // based on the results from the above query.
      $cvterms = [];
      foreach ($resource as $r) {
        $cvterms[$r->cvterm_id] = $r->name;
      }

    }
    // Otherwise, show the user the much smaller list of all cvterms used in
    // the base table.
    else {

      // There are actually two sets of conditions we care about and of course
      // they are placed in different places in the query :p.
      // 1. Restrictions on the cvterm table. This lets users specify: only
      // show these exact types.
      $return = $this->get_select_option_where('cvterm');
      $where_clauses = $return['where_clauses'];
      $cvterm_args = $return['arguments'];
      $cvterm_where = '';
      if (!empty($where_clauses)) {
        $cvterm_where = implode(' AND ', $where_clauses);
      }
      // 2. Restrictions on the filter table Since those affect which types
      // have been used.
      $return = $this->get_select_option_where($this->table);
      $where_clauses = $return['where_clauses'];
      $base_args = $return['arguments'];
      $base_where = '';
      if (!empty($where_clauses)) {
        $base_where = implode(' AND ', $where_clauses);
      }
      // We only supply one set or arguments those so merge the two.
      $arguments = array_merge($cvterm_args, $base_args);

      // Using a "Loose Index Scan" to get a list of all the cvterms used
      // in the base table. See https://wiki.postgresql.org/wiki/Loose_indexscan
      $sql = "
        WITH RECURSIVE t AS (
          SELECT MIN(!field) AS col FROM {!table}
            " . ($base_where == '' ? '' : "WHERE " . $base_where) . "
          UNION ALL
          SELECT (
            SELECT MIN(!field)
            FROM {!table}
            WHERE !field > col " . ($base_where == '' ? '' : " AND " . $base_where) . "
          )
          FROM t WHERE col IS NOT NULL
        )
        SELECT cvterm_id, name
          FROM {cvterm}
          WHERE cvterm_id IN (SELECT col FROM t where col IS NOT NULL) " . ($cvterm_where == '' ? '' : " AND " . $cvterm_where) . "
          ORDER BY cvterm.name ASC";
      $sql = format_string($sql, [
        '!table' => $this->table,
        '!field' => $this->field,
      ]);

      $resource = chado_query($sql, $arguments);
      $cvterms = [];

      // Add an "- Any - " option to allow a type to not be set by default.
      if ($this->options['select_optional']) {
        $cvterms['All'] = '- Any -';
      }

      // Now actually gerenate the select list
      // based on the results from the above query.
      foreach ($resource as $r) {
        $cvterms[$r->cvterm_id] = $r->name;
      }
    }

    return $cvterms;

  }

  /**
   * For the SQL generating the options, determine the WHERE clauses
   *
   * @return
   *   An array of full qualified where clauses (ie: table.myfield = 'fred')
   */
  function get_select_option_where($table = NULL, $generic_placeholder = TRUE) {
    return parent::get_select_option_where($table, $generic_placeholder);
  }
  /*
      // build a where clause that will filter the list in the drop box
      // using fields that are not exposed and that are for the table
      // from whcih the values in the drop box will be slected and
      // we only want to use non-exposed fields because these are not
      // available to the user to edit--they're fixed.
      $filters = (is_array($this->view->filter)) ? $this->view->filter : array();
      foreach ($filters as $filter_name => $details) {
         // we only want to include non-exposed filters
         if ($details->options['exposed'] == FALSE) {
            $value = $details->value;
            if (is_array($details->value) AND isset($details->value['value'])) {
              $value = $details->value['value'];
            }
  
            $field = $details->field;
            if (($this->table == $this->view->base_table) AND ($details->field == 'type_id')) {
              $field = 'cvterm_id';
            }
  
            if (is_array($value)) {
              // we only want to filter on the table we're getting the list from
              if (strcmp($details->table, $this->table)==0 AND !empty($value)) {
                $where[] = "$field IN (" . implode(', ', $value) . ')';
              }
            }
            else {
              // we only want to filter on the table we're getting the list from
              if (strcmp($details->table, 'cvterm')==0 AND !empty($value)) {
                $where[] = "$field $details->operator " . $value;
              }
            }
         }
      }
  
      return $where;
    }
  */
  /**
   * {@inheritdoc}
   */
  function option_definition() {
    return parent::option_definition();
  }

  /**
   * {@inheritdoc}
   */
  function expose_form(&$form, &$form_state) {
    parent::expose_form($form, $form_state);
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  function expose_submit($form, &$form_state) {
    parent::expose_submit($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  function expose_options() {
    parent::expose_options();
  }

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
  }

}
