<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller routines for the chado DB xref autocomplete.
 */
class ChadoDbxrefAutocompleteController extends ControllerBase {
  /**
   * Controller method, autocomplete db:accession. Not case sensitive.
   *
   * @param Request request
   *
   * @param int $count
   *   Desired number of matching names to suggest.
   *   Default to 5 items.
   *   Zero will disable autocomplete.
   *
   * @param int $db_id
   *   Limit the match of term to the DB with this db_id.
   *   Zero, the default, will return matches to any DB.
   *
   * @return Json Object
   *   Matching dbxref rows where each row is formatted as string:
   *   db.name:dbxref.accession and is the value for the object
   *   keys label and value.
   */
  public function handleAutocomplete(Request $request, int $count = 5, int $db_id = 0) {
    // Array to hold matching dbxref accessions.
    $response = [];

    if ($request->query->get('q')) {
      // Get typed in string input from the URL.
      $string = trim($request->query->get('q'));

      if (strlen($string) > 0 && $count > 0) {
        // Proceed to autocomplete when string is at least a character
        // long and result count is set to a value greater than 0.

        // If there is a colon in the string, extract the left part as the
        // DB name and the right part as the accession.
        $db_name = '';
        if (preg_match('/^([^:]*):(.*)$/', $string, $matches)) {
          $db_name = strtolower($matches[1]);
          $string = $matches[2];
        }

        // Transform string as a case-insensitive search keyword pattern.
        $keyword = strtolower($string) . '%';

        // Query dbxref (joins: dbxref - accession and db - name) for names matching
        // the keyword pattern and return each row in the format specified.
        // Tables indicate schema sequence number #1 to use default schema.
        $sql = "
          SELECT xr.accession, db.name
          FROM {1:dbxref} AS xr
            LEFT JOIN {1:db} AS db USING(db_id)
          WHERE LOWER(xr.accession) LIKE :keyword";
        $args = [':keyword' => $keyword, ':limit' => $count];

        // Limit terms to selected DB when this is specified.
        if ($db_id) {
          $sql .= " AND xr.db_id = :db_id";
          $args[':db_id'] = $db_id;
        }

        // If user typed a DB: prefix, limit by that.
        if ($db_name) {
          $sql .= " AND LOWER(db.name) = :db_name";
          $args[':db_name'] = $db_name;
        }

        $sql .= " ORDER BY xr.accession ASC LIMIT :limit";

        // Prepare Chado database connection and execute sql query
        $connection = \Drupal::service('tripal_chado.database');
        $results = $connection->query($sql, $args);

        // Compose response result.
        if ($results) {
          foreach ($results as $record) {
            $term = $record->name . ':' . $record->accession;
            $response[] = [
              'value' => $term, // Value returned and value displayed by textfield.
              'label' => $term  // Value shown in the list of options.
            ];
          }
        }
      }
    }

    return new JsonResponse($response);
  }

  /**
   * Fetch the dbxref.dbxref_id given a dbxref name (db.name:dbxref.accession)
   * value returned by the handler method above.
   *
   * @param string $accession
   *   String value returned by autocomplete handler method. Case sensitive.
   *
   * @param string $db_id
   *   Optional db_id of a database. Can be used if user bypassed autocomplete
   *   and entered just a dbxref accession manually. Case sensitive.
   *
   * @return integer
   *   Id number corresponding to chado.dbxref_id field of the matching accession
   *   or 0 if no match or multiple matches were found.
   */
  public static function getDbxrefId(string $accession, $db_id = null): int {
    $id = 0;

    if (strlen($accession) > 0) {

      // If there is a colon in the accession, extract the left part as
      // the DB name and the right part as the accession.
      $db_name = '';
      if (preg_match('/^([^:]*):(.*)$/', $accession, $matches)) {
        $db_name = $matches[1];
        $accession = $matches[2];
      }

      $sql = "
        SELECT xr.dbxref_id FROM {1:dbxref} AS xr
          LEFT JOIN {1:db} AS db USING(db_id)
        WHERE xr.accession = :keyword";
      $args = [':keyword' => $accession];

      if ($db_name) {
        $sql .= " AND db.name = :db_name";
        $args[':db_name'] = $db_name;
      }
      elseif ($db_id) {
        $sql .= " AND db.db_id = :db_id";
        $args[':db_id'] = $db_id;
      }

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection
        ->query($sql, $args)
        ->fetchAll();

      if (count($result) == 1) {
        $id = $result[0]->dbxref_id;
      }
    }
    return $id;
  }

  /**
   * Given a dbxref id number, return the matching dbxref record using
   * the format db.name:dbxref.accession
   *
   * @param integer $id
   *   Dbxref id number to match.
   *
   * @return string
   *   Dbxref record in db.name:dbxref.accession format.
   */
  public static function formatDbxref(int $id) {
    $term = null;

    if ($id > 0) {
      $sql = "
        SELECT CONCAT(db.name, ':', xr.accession)
        FROM {1:dbxref} AS xr
          LEFT JOIN {1:db} AS db USING(db_id)
        WHERE xr.dbxref_id = :dbxref_id
        LIMIT 1
      ";
      $args = [':dbxref_id' => $id];

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection->query($sql, $args);

      if ($result) {
        $accession = $result->fetchField();
      }
    }

    return $accession;
  }
}
