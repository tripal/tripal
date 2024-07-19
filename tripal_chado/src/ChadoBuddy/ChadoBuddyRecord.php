<?php

namespace Drupal\tripal_chado\ChadoBuddy;

/**
 * Chado Buddy Record
 *
 * Each chado record returned by a ChadoBuddy service will be in the form of an
 * instance of this class.
 */
class ChadoBuddyRecord {

  /**
   * The base chado table that this record was retrieved from.
   *
   * This is the table that would be the FROM in the query rather then
   * any tables included via joins.
   * @var string
   */
  protected string $base_table;

  /**
   * The name of the chado schema this record was retrieved from.
   * @var string
   */
  protected string $schema_name;

}
