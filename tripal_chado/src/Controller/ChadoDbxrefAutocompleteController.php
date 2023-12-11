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
   * Controller method, autocomplete db:accession.
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

        // Transform string as a case-insensitive search keyword pattern.
        $keyword = strtolower($string) . '%';

        // Query dbxref (joins: dbxref - accession and db - name) for names matching
        // the keyword pattern and return each row in the format specified.
        // Tables indicate schema sequence number #1 to use default schema.
        $sql = "
          SELECT dbxref.accession, db.name,
          FROM {1:dbxref} AS xr
            LEFT JOIN {1:db} AS db USING(db_id)
          WHERE LOWER(xr.accession) LIKE :keyword";
        $args = [':keyword' => $keyword, ':limit' => $count];
        // Limit terms to selected DB when this is specified.
        if ($db_id) {
          $sql .= " AND xr.db_id = :db_id";
          $args[':db_id'] = $db_id;
        }
        $sql .= " ORDER BY xr.accession ASC LIMIT :limit";

        // Prepare Chado database connection and execute sql query by providing value
        // for :keyword placeholder text.
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
   *   String value returned by autocomplete handler method.
   *
   * @param string $db_name
   *   Optional name of a database. Can be used if user
   *   bypassed autocomplete and entered just a dbxref accession manually.
   *
   * @return integer
   *   Id number corresponding to chado.dbxref_id field of the matching accession
   *   or 0 if no match was found.
   */
  public static function getDbxrefId(string $accession, $db_name = ''): int {
    $id = 0;

    if (strlen($accession) > 0) {
      $sql = "
        SELECT xr.dbxref_id FROM {1:dbxref} AS xr
          LEFT JOIN {1:db} AS db USING(db_id)
        WHERE CONCAT(db.name, ':', xr.accession) = :accession
      ";

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection
        ->query($sql, [':accession' => $accession])
        ->fetchAll();

      if(count($result) == 1) {
        $id = $result[0]->dbxref_id;
      }

      // If no match, and if a disambiguating DB was specified,
      // try again using only that DB. This happens if the user
      // types in the accession and doesn't let the autocomplete
      // prefix with the DB:.
      else if ($db_name) {
        $sql = "
          SELECT xr.dbxref_id FROM {1:dbxref} AS xr
            LEFT JOIN {1:db} AS db USING(db_id)
          WHERE xr.accession = :accession AND db.name = :dbname
        ";

        $result = $connection
          ->query($sql, [':accession' => $accession, ':dbname' => $db_name])
          ->fetchAll();

        if(count($result) == 1) {
          $id = $result[0]->dbxref_id;
        }
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

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection->query($sql, [':dbxref_id' => $id]);

      if($result) {
        $accession = $result->fetchField();
      }
    }

    return $accession;
  }
}
