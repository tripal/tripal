<?php
/**
 * @file
 * Contains tripal_views_handler_filter_select_id
 */

/**
 * This Handler provides a select list for the type field
 *
 *  NOTE: This handler only works when applied to the type_id field in the
 * base_table of this view.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_select_id extends tripal_views_handler_filter_select_string {

  /**
   * {@inheritdoc}
   */
  function init(&$view, &$options) {
    parent::init($view, $options);

    if (preg_match('/(\w+)_id/', $this->field, $matches)) {
      $this->parent_table = $matches[1];
    }
  }

  /**
   * Provide the options used in the select list.
   * Override this function in extended handlers to easily change option list.
   *
   * @return
   *   An array of options where the key is the value of this field in the
   *   database
   */
  function get_select_options() {

    // @TODO: Make name field configurable.
    $name_field = 'common_name';

    // First check that this table has a name field.
    $table_desc = chado_get_schema($this->parent_table);
    if (!isset($table_desc['fields'][$name_field])) {
      return [];
    }

    // If the "Show All" options is set then show all the "names" from
    // the table referenced by the foreign key constraint.
    if (isset($this->options['show_all']) AND $this->options['show_all'] == TRUE) {

      // We still want to use any hidden fitlers on the parent table
      // but the arguments will need to be field names rather than
      // generic placeholders so we need to tell get_select_option_where() that.
      $return = $this->get_select_option_where($this->parent_table, FALSE);
      $args = $return['arguments'];

      // Simply grab all the values from the table referenced by
      // the foreign key constraint. Since we use the id as the key of
      // the options there is no need to use DISTRINCT in the query.
      $resource = chado_select_record($this->parent_table, [
        $this->field,
        $name_field,
      ], $args);
      $options = [];
      foreach ($resource as $r) {
        $options[$r->{$this->field}] = $r->{$name_field};
      }
    }
    // Otherwise, only show those that are actually used in the base table.
    else {

      $return = $this->get_select_option_where($this->parent_table);
      $where_clauses = $return['where_clauses'];
      $arguments = $return['arguments'];
      $where = '';
      if (!empty($where_clauses)) {
        $where = implode(' AND ', $where_clauses);
      }


      // Using a "Loose Index Scan" to get a list of all the unique values for
      // the name in the table referenced by the foreign key constraint.
      // See https://wiki.postgresql.org/wiki/Loose_indexscan
      $sql = "WITH RECURSIVE t AS (
            SELECT MIN(filter_table.!id_field) AS col
              FROM {!filter_table} filter_table
              LEFT JOIN {!foreign_table} foreign_table ON filter_table.!id_field=foreign_table.!id_field
              " . ($where == '' ? '' : "WHERE " . $where) . "
            UNION ALL
            SELECT (
                SELECT MIN(filter_table.!id_field)
                FROM {!filter_table} filter_table
                LEFT JOIN {!foreign_table} foreign_table ON filter_table.!id_field=foreign_table.!id_field
                WHERE filter_table.!id_field > col " . ($where == '' ? '' : " AND " . $where) . "
              )
              FROM t WHERE col IS NOT NULL
          )
          SELECT !id_field as id, !name_field as name
            FROM {!foreign_table}
            WHERE !id_field IN (SELECT col FROM t where col IS NOT NULL)
            ORDER BY !name_field ASC";
      $sql = format_string($sql, [
        '!filter_table' => $this->table,
        '!foreign_table' => $this->parent_table,
        '!id_field' => $this->field,
        '!name_field' => $name_field,
      ]);

      $resource = chado_query($sql, $arguments);
      $options = [];

      if ($this->options['select_optional']) {
        $options['All'] = '- Any -';
      }

      foreach ($resource as $r) {
        $options[$r->id] = $r->name;
      }
    }

    return $options;

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
