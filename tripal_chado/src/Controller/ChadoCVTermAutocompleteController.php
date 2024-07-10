<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller routines for the chado CV Term autocomplete.
 */
class ChadoCVTermAutocompleteController extends ControllerBase {
  /**
   * Controller method, autocomplete cvterm name.
   *
   * @param Request request
   *
   * @param int $count
   *   Desired number of matching names to suggest.
   *   Default to 5 items.
   *   Zero will disable autocomplete.
   *
   * @param int $cv_id
   *   Limit the match of term to the CV with this cv_id.
   *   Zero, the default, will return matches to any CV.
   *
   * @return Json Object
   *   Matching cvterm rows where each row is formatted as string:
   *   cvterm.name (db.name:dbxref.accession) and is the value for
   *   the object keys label and value.
   */
  public function handleAutocomplete(Request $request, int $count = 5, int $cv_id = 0) {
    // Array to hold matching cvterm names.
    $response = [];

    if ($request->query->get('q')) {
      // Get typed in string input from the URL.
      $string = trim($request->query->get('q'));

      if (strlen($string) > 0 && $count > 0) {
        // Proceed to autocomplete when string is at least a character
        // long and result count is set to a value greater than 0.

        // Transform string as a case-insensitive search keyword pattern.
        $keyword = strtolower($string) . '%';

        // Query cvterm (joins: dbxref - accession and db - dn name) for names matching
        // the keyword pattern and return each row in the format specified.
        // Tables indicate schema sequence number #1 to use default schema.
        $sql = "
          SELECT ct.name AS term, db.name AS dbname, dx.accession
          FROM {1:cvterm} AS ct
            LEFT JOIN {1:dbxref} AS dx USING(dbxref_id)
            LEFT JOIN {1:db} USING(db_id)
          WHERE LOWER(ct.name) LIKE :keyword";
        $args = [':keyword' => $keyword, ':limit' => $count];
        // Limit terms to selected CV when this is specified.
        if ($cv_id) {
          $sql .= " AND ct.cv_id = :cv_id";
          $args[':cv_id'] = $cv_id;
        }
        $sql .= " ORDER BY ct.name ASC LIMIT :limit";

        // Prepare Chado database connection and execute sql query by providing value
        // for :keyword placeholder text.
        $connection = \Drupal::service('tripal_chado.database');
        $results = $connection->query($sql, $args);

        // Compose response result.
        if ($results) {
          foreach ($results as $record) {
            $term = $record->term . ' (' . $record->dbname . ':' . $record->accession . ')';
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
   * Fetch the cvterm.cvterm_id given a cvterm name (db.name:dbxref.accession)
   * value returned by the handler method above.
   *
   * @param string $term
   *   String value returned by autocomplete handler method.
   *
   * @param string $cv_name
   *   Optional name of a controlled vocabulary. Can be used if user
   *   bypassed autocomplete and entered just a CV term name manually.
   *
   * @return integer
   *   Id number corresponding to chado.cvterm_id field of the matching term
   *   or 0 if no match was found.
   */
  public static function getCVtermId(string $term, $cv_name = ''): int {
    $id = 0;

    if (strlen($term) > 0) {
      $sql = "
        SELECT ct.cvterm_id FROM {1:cvterm} AS ct
          LEFT JOIN {1:dbxref} AS dx USING(dbxref_id)
          LEFT JOIN {1:db} USING(db_id)
        WHERE CONCAT(ct.name, ' (', db.name, ':', dx.accession, ')') = :term
      ";

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection
        ->query($sql, [':term' => $term])
        ->fetchAll();

      if(count($result) == 1) {
        $id = $result[0]->cvterm_id;
      }

      // If no match, and if a disambiguating CV was specified,
      // try again using only that CV. This happens if the user
      // types in the term and doesn't let the autocomplete
      // append the (DB:accession).
      else if ($cv_name) {
        $sql = "
          SELECT ct.cvterm_id FROM {1:cvterm} AS ct
            LEFT JOIN {1:cv} AS cv USING(cv_id)
          WHERE ct.name = :term AND cv.name = :cvname
        ";

        $result = $connection
          ->query($sql, [':term' => $term, ':cvname' => $cv_name])
          ->fetchAll();

        if(count($result) == 1) {
          $id = $result[0]->cvterm_id;
        }
      }
    }
    return $id;
  }

  /**
   * Given a cvterm id number, return the matching cvterm record using
   * the format cvterm name (db.name:dbxref.accession).
   *
   * @param integer $id
   *   Cvterm id number to match.
   *
   * @return string
   *   Cvterm record in cvterm name (db.name:dbxref.accession) format.
   */
  public static function formatCVterm(int $id) {
    $term = null;

    if ($id > 0) {
      $sql = "
        SELECT CONCAT(ct.name, ' (', db.name, ':', dx.accession, ')')
        FROM {1:cvterm} AS ct
          LEFT JOIN {1:dbxref} AS dx USING(dbxref_id)
          LEFT JOIN {1:db} USING(db_id)
        WHERE ct.cvterm_id = :cvterm_id
        LIMIT 1
      ";

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection->query($sql, [':cvterm_id' => $id]);

      if($result) {
        $term = $result->fetchField();
      }
    }

    return $term;
  }
}
