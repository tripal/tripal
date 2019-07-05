<?php

/**
 * ChadoPrefixExtender
 *
 * A query extender that for select queries. By extending the
 * SelectQueryExtender class, we can make sure that chado tables
 *
 * @see https://www.drupal.org/docs/7/api/database-api/dynamic-queries/extenders
 */
class ChadoPrefixExtender extends SelectQueryExtender {

  /**
   * A static cache for Chado tables.
   *
   * @var array
   */
  protected static $chado_tables = [];

  /**
   * A replacement for db_select when querying Chado.
   *
   * Use this function instead of db_select when querying Chado tables.
   *
   * @param string|\SelectQuery $table
   *   The base table for this query. May be a
   *   string or another SelectQuery object. If a query object is passed, it
   *   will be used as a subselect.
   * @param string $alias
   *   The alias for the base table of this query.
   * @param array $options
   *   An array of options to control how the query
   *   operates.
   *
   * @return \SelectQuery
   *   A new SelectQuery object for this connection.
   *
   * @ingroup tripal_chado_query_api
   */
  public static function select($table, $alias = NULL, array $options = []) {
    // Since the table could also be a SelectQuery object, we should verify that
    // it is a string first.
    if (is_string($table)) {
      $table = static::getTable($table);
    }

    // If the alias is null, determine a safe alias. db_select fails to generate
    // a safe alias when the table name is prefixed with "public.".
    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    // Create a select query
    $query = db_select($table, $alias, $options);
    return $query->extend('ChadoPrefixExtender');
  }

  /**
   * @param $type
   * @param $table
   * @param null $alias
   * @param null $condition
   * @param array $arguments
   *
   * @return $this
   *
   * @see SelectQueryInterface::addJoin()
   */
  public function addJoin($type, $table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = static::getTable($table);

    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    $this->query->addJoin($type, $table, $alias, $condition, $arguments);

    return $this;
  }

  /**
   * Overwrites the join to prefix table names.
   *
   * @param string $table
   *   Table to join.
   * @param string $alias
   *   Alias for joined table.
   * @param string $condition
   *   Operation for joining.
   * @param array $arguments
   *   Additional arguments.
   *
   * @return $this
   *   The current object.
   *
   * @ingroup tripal_chado_query_api
   */
  public function join($table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = static::getTable($table);

    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    $this->query->join($table, $alias, $condition, $arguments);

    return $this;
  }

  /**
   * Overwrites the innerJoin to prefix table names.
   *
   * @param string $table
   *   Table to join.
   * @param string $alias
   *   Alias for joined table.
   * @param string $condition
   *   Operation for joining.
   * @param array $arguments
   *   Additional arguments.
   *
   * @return $this
   *   The current object.
   *
   * @ingroup tripal_chado_query_api
   */
  public function innerJoin($table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = static::getTable($table);

    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    $this->query->innerJoin($table, $alias, $condition, $arguments);

    return $this;
  }

  public function leftJoin($table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = static::getTable($table);

    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    $this->query->leftJoin($table, $alias, $condition, $arguments);

    return $this;
  }

  /**
   * Overwrites the rightJoin to prefix table names.
   *
   * @param string $table
   *   Table to join.
   * @param string $alias
   *   Alias for joined table.
   * @param string $condition
   *   Operation for joining.
   * @param array $arguments
   *   Additional arguments.
   *
   * @return $this
   *   The current object.
   *
   * @ingroup tripal_chado_query_api
   */
  public function rightJoin($table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = static::getTable($table);

    if (is_null($alias)) {
      $alias = static::makeAlias($table);
    }

    $this->query->rightJoin($table, $alias, $condition, $arguments);

    return $this;
  }

  /**
   * Checks if a table is a chado table.
   *
   * @param string $table The table name.
   *
   * @return bool
   */
  public static function isChadoTable($table) {
    if (empty(static::$chado_tables)) {
      static::$chado_tables = chado_get_table_names(TRUE);
    }

    return in_array(strtolower($table), static::$chado_tables);
  }

  /**
   * If the table name has a schema name as a prefix, replace it with the
   * correct schema name.
   *
   * @param string $table
   *   The table name.
   *
   * @return string
   *   The table with the correct prefix.
   *
   */
  public static function getTable($table) {
    $chado_schema_name = chado_get_schema_name('chado');
    $drupal_schema_name = chado_get_schema_name('drupal');

    // No schema was provided.
    if (strpos($table, '.') === FALSE) {
      // If this is a chado table, add the chado prefix. Otherwise, add the
      // public prefix.
      if (static::isChadoTable($table)) {
        $table = $chado_schema_name . ".{$table}";
      }
      else {
        $table = $drupal_schema_name . ".{$table}";
      }
    }

    // Now that the schema has been set, we can replace it with the correct
    // name. Note that schema names can be altered by developers so we need to
    // to run the following function to obtain the final name.
    $table = static::getRealSchema($table);

    return $table;
  }

  /**
   * Allows altered schema names to be replaces correctly.
   *
   * @param string $table
   *     The table name with a prefix such as "chado." or "public."
   *
   * @return mixed
   *    The table name with the correct prefix.
   */
  public static function getRealSchema($table) {

    if (strpos($table, 'public.') === 0) {
      $replace = chado_get_schema_name('drupal') . '.';
      return str_replace('public.', $replace, $table);
    }

    if (strpos($table, 'chado.') === 0) {
      $replace = chado_get_schema_name('chado') . '.';
      return str_replace('chado.', $replace, $table);
    }

    return $table;
  }

  /**
   * Create a safe alias.
   *
   * @param string $table Table name.
   *
   * @return string
   *    The safe alias.
   */
  public static function makeAlias($table) {
    return str_replace('.', '_', $table);
  }
}
