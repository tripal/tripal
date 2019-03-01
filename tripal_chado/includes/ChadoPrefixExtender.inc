<?php

/**
 * SelectQueryExtender to use chado_replace_table_prefix().
 *
 *   Note that extenders can only be called on an existing db_select().
 * Therefore the table name in the initial select must be prefixed prior to
 * initialization. Read more about extenders:
 * https://www.drupal.org/docs/7/api/database-api/dynamic-queries/extenders.
 */
class ChadoPrefixExtender extends SelectQueryExtender {

  /**
   * Overwrites the join to prefix table names.
   *
   * @param string $table
   *   Table to join.
   * @param null $alias
   *   Alias for joined table.
   * @param null $condition
   *   Operation for joining.
   * @param array $arguments
   *   Additional arguments.
   *
   * @return $this
   */
  public function join($table, $alias = NULL, $condition = NULL, $arguments = []) {
    if(strpos($table, '[') !== 0 && strpos($table, ']') !== strlen($table)) {
      $table = "{{$table}}";
    }
    $table = chado_replace_table_prefix($table);
    $this->query->join($table, $alias, $condition, $arguments);
    return $this;
  }

}
