<?php
namespace Drupal\tripal_chado\Commands;

use Drush\Commands\DrushCommands;
use Symfony\Component\Console\Helper\Table;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Drush command specific to checking the cv/db/cvterm/dbxref records in a
 * specific chado schema against the expected terms in the Tripal Content Terms
 * YAML.
 *
 * DO NOT ADD ADDITION DRUSH COMMANDS TO THIS CLASS.
 */
class ChadoCheckTermsAgainstYaml extends DrushCommands {

  protected $chado_schema;

  protected ChadoConnection $chado;

  protected string $red_format = "\033[31;40m\033[1m %s \033[0m";
  protected string $yellow_format = "\033[1;33;40m\033[1m %s \033[0m";

  /**
   * Checks a given chado install for any inconsistencies between its cvterms
   * and what Tripal expects.
   *
   * @command tripal-chado:trp-check-terms
   * @aliases trp-check-terms
   * @option chado_schema
   *   The name of the chado schema to check.
   * @option auto-expand
   *   Indicates that you always want to show specifics of any errors or warnings.
   * @option auto-fix
   *   Indicates that you always want us to attempt to fix any issues without
   *   the need for us to prompt.
   * @option no-fix
   *   Indicates that you do not want us to offer to fix anything.
   * @usage drush trp-check-terms --chado_schema=chado_prod
   *   Checks the terms stored in chado_prod.cvterm for consistency.
   */
  public function chadoCheckTermsAreAsExpected($options = ['chado_schema' => NULL, 'auto-expand' => FALSE, 'auto-fix' => FALSE, 'no-fix' => FALSE]) {

    if (!$options['chado_schema']) {
      throw new \Exception(dt('The --chado_schema argument is required.'));
    }
    $this->chado_schema = $options['chado_schema'];

    // We're going to use symphony tables to summarize what this command finds.
    // The headers are: YAML Term, CD, DB, CVTERM, DBXREF
    // Each row will be a term and each cell will either be an existing id
    // or use the ` - ` string to indicate that it isn't found.
    // @see chadoCheckTerms_printSummaryTable() to see how it will be printed.
    $summary_rows = [];

    // We are also going to keep track of the issues so we can offer to fix them
    // in some cases.
    $problems = [
      'error' => [],
      'warning' => [],
    ];
    $solutions = [
      'error' => [],
      'warning' => [],
    ];

    $this->chado = \Drupal::service('tripal_chado.database');
    $this->chado->setSchemaName($options['chado_schema']);

    $this->output()->writeln('');
    $this->output()->writeln('Using the Chado Content Terms YAML specification to determine what Tripal expects.');
    $this->output()->writeln('');

    $config_factory = \Drupal::service('config.factory');

    $id = 'chado_content_terms';
    $config_key = 'tripal.tripal_content_terms.' . $id;
    $config = $config_factory->get($config_key);
    if (!$config) {
      $this->io()->error('Unable to access the configuration for tripal content terms!');
      return FALSE;
    }

    $this->output()->writeln("  Finding term definitions for $id term collection.");
    $vocabs = $config->get('vocabularies');
    if (!$vocabs) {
      $this->io()->error('Tripal content terms configuration did not have an array of vocabularies!');
      return FALSE;
    }

    foreach ($vocabs as $vocab_info) {

      // Reset for the new vocab.
      $summary_term = NULL;
      $summary_cv = NULL;
      $summary_dbs = [];
      $summary_cvterm = NULL;
      $summary_dbxref = NULL;

      [$summary_cv, $existing_cv] = $this->chadoCheckTerms_checkVocab(
        $vocab_info,
        $problems,
        $solutions
      );

      [$summary_dbs, $defined_ispaces] = $this->chadoCheckTerms_checkIdSpaces(
        $vocab_info,
        $problems,
        $solutions
      );

      // Now for each term in this vocabulary...
      $vocab_info['terms'] = (array_key_exists('terms', $vocab_info)) ? $vocab_info['terms'] : [];
      foreach ($vocab_info['terms'] as $term_info) {

        $summary_term = $term_info['name'] . ' (' . $term_info['id'] . ')';
        $term_info['label'] = $summary_term;

        // Extract the parts of the id.
        [$term_db, $term_accession] = explode(':', $term_info['id']);
        $term_info['idspace'] = $term_db;
        $term_info['accession'] = $term_accession;
        $term_info['cv_name'] = $vocab_info['name'];

        // Check the term id space was defined in the id spaces block
        // Note: if a id space was defined but not found in the database
        // it will still be in the $defined_idspaces array but the value
        // will be NULL.
        if (!array_key_exists($term_db, $defined_ispaces)) {

          $summary_dbs[$idspace_info['name']] = sprintf($this->red_format, ' X ');

          // ERROR:
          // The YAML-defined term includes an ID Space that was not defined in the ID Spaces section for this vocabulary.
          // @see chadoCheckTerms_reportProblem_missingDbYaml().
          $problems['error']['missingDbYaml'][$term_db][] = [
            'missing-db-name' => $term_db,
            'defined-dbs' => $defined_ispaces,
            'term' => $summary_term,
            'vocab' => $vocab_info['name'],
          ];
          // No solution for this one... instead the developer of the module needs to fix their YAML ;-p
          $solutions['error']['missingDbYaml'] = [];
        }

        [$summary_cvterm, $summary_dbxref] = $this->chadoCheckTerms_checkTerm(
          $term_info,
          $problems,
          $solutions
        );

        // Now add the details of what we found for this term to the summary table.
        $summary_rows[] = [
          'term' => $summary_term,
          'cv' => $summary_cv,
          'db' => $summary_dbs[$term_db],
          'cvterm' => $summary_cvterm,
          'dbxref' => $summary_dbxref,
        ];
      }
    }

    // Finally tell the user the summary state of things.
    $this->chadoCheckTerms_printSummaryTable($summary_rows);

    // Now we can start reporting more detail if they want.
    // First ERRORS:
    $this->io()->title('Errors');
    $this->output()->writeln('Differences are categorized as errors if they are likely to cause failures when preparing this chado instance or to cause Tripal to be unable to find the term reliably.');

    $has_errors = (array_key_exists('error', $problems) && count($problems['error']) > 0);

    if (!$has_errors) {
      $this->io()->success('There are no errors associated with this chado instance!');
    }

    $show_errors = $this->askOrRespectOptions(
      'Would you like more details regarding the errors we found?',
      $options,
      'auto-expand',
      $has_errors,
      TRUE
    );
    if ($show_errors) {

      // missingDbYaml
      if (array_key_exists('missingDbYaml', $problems['error'])) {
        $this->chadoCheckTerms_reportProblem_missingDbYaml(
          $problems['error']['missingDbYaml'],
          $solutions['error']['missingDbYaml'],
          $options
        );
      }

      // term
      if (array_key_exists('term', $problems['error'])) {
        $solutions['error']['term'] = (array_key_exists('term', $solutions['error'])) ? $solutions['error']['term'] : [];
        $this->chadoCheckTerms_reportProblem_terms(
          $problems['error']['term'],
          $solutions['error']['term'],
          $options
        );
      }
    }

    $this->output()->writeln('');

    // Then WARNINGS:
    $this->io()->title('Warnings');
    $this->output()->writeln('Differences are categorized as warnings if they are in non-critical parts of the terms, vocabularies and references. These can be safely ignored but you may also want to use this opprotinuity to update your version of these terms.');

    $has_warnings = (array_key_exists('warning', $problems) && count($problems['warning']) > 0);
    if (!$has_warnings) {
      $this->io()->success('There are no warnings associated with this chado instance!');
    }

    $show_warnings = $this->askOrRespectOptions(
      'Would you like more details regarding the warnings we found?',
      $options,
      'auto-expand',
      $has_warnings,
      TRUE
    );
    if ($show_warnings) {

      // Small differences between the expected and found chado.cv record.
      if (array_key_exists('cv', $problems['warning'])) {
        $this->chadoCheckTerms_reportProblem_eccentricCv(
          $problems['warning']['cv'],
          $solutions['warning']['cv'],
          $options
        );
      }

      $this->output()->writeln('');

      // Small differences between the expected and found chado.db record.
      if (array_key_exists('db', $problems['warning'])) {
        $this->chadoCheckTerms_reportProblem_eccentricDb(
          $problems['warning']['db'],
          $solutions['warning']['db'],
          $options
        );
      }

      $this->output()->writeln('');

      // Small differences between the expected and found chado.cvterm record.
      if (array_key_exists('cvterm', $problems['warning'])) {
        $this->chadoCheckTerms_reportProblem_eccentricCVTerm(
          $problems['warning']['cvterm'],
          $solutions['warning']['cvterm'],
          $options
        );
      }

      $this->output()->writeln('');
    }
  }

  /**
   * Checks that the vocabulary metadata in the YAML matches this chado instance.
   *
   * @param array $vocab_info
   * @param array $problems
   * @param array $solutions
   * @return array
   *   - summary_cv: the value to print in the summary table
   *   - existing_cv: the cv object selected from the database or NULL if there wasn't one.
   */
  protected function chadoCheckTerms_checkVocab(array $vocab_info, array &$problems, array &$solutions) {

    // Check if the cv record for this vocabulary exists.
    $query = $this->chado->select('1:cv', 'cv')
      ->fields('cv', ['cv_id', 'definition'])
      ->condition('cv.name', $vocab_info['name']);
    $existing_cv = $query->execute()->fetchObject();
    if ($existing_cv) {
      $summary_cv = $existing_cv->cv_id;

      // Check if the definition matches our expectations and warn if not.
      if ($existing_cv->definition != $vocab_info['label']) {
        $summary_cv = sprintf($this->yellow_format, $existing_cv->cv_id);

        // WARNING:
        // @see chadoCheckTerms_reportProblem_eccentricCv().
        $problems['warning']['cv'][$existing_cv->cv_id][] = [
          'column' => 'cv.definition',
          'property' => 'label',
          'YOURS' => $existing_cv->definition,
          'EXPECTED' => $vocab_info['label'],
          'vocab-name' => $vocab_info['name'],
        ];
        $solutions['warning']['cv'][$existing_cv->cv_id]['definition'] = $vocab_info['label'];
      }
    } else {
      $summary_cv = ' - ';
    }

    return [$summary_cv, $existing_cv];
  }

  /**
   * Checks that the id space metadata in the YAML matches this chado instance.
   *
   * @param array $vocab_info
   * @param array $problems
   * @param array $solutions
   * @return array
   *   - summary_dbs: an array where the key is the id space name and the value
   *       summarizes its status.
   *   - defined_idspaces: an array where the key is the id space name and the
   *       value is the db_id found or NULL if not.
   */
  protected function chadoCheckTerms_checkIdSpaces(array $vocab_info, array &$problems, array &$solutions) {

    $summary_dbs = [];
    $defined_ispaces = [];
    foreach ($vocab_info['idSpaces'] as $idspace_info) {

      // Check if the db record for this id space exists.
      $query = $this->chado->select('1:db', 'db')
        ->fields('db', ['db_id', 'description', 'urlprefix', 'url'])
        ->condition('db.name', $idspace_info['name']);
      $existing_db = $query->execute()->fetchObject();
      if ($existing_db) {
        $summary_dbs[$idspace_info['name']] = $existing_db->db_id;
        $defined_ispaces[$idspace_info['name']] = $existing_db->db_id;

        // Now check the db description, url prefix and url match what we expect and warn if not.
        if ($existing_db->description != $idspace_info['description']) {

          $summary_dbs[$idspace_info['name']] = sprintf($this->yellow_format, $existing_db->db_id);

          // WARNING:
          // @see chadoCheckTerms_reportProblem_eccentricDb().
          $problems['warning']['db'][$existing_db->db_id][] = [
            'idspace-name' => $idspace_info['name'],
            'column' => 'db.description',
            'property' => 'idSpace.description',
            'YOURS' => $existing_db->description,
            'EXPECTED' => $idspace_info['description'],
          ];
          $solutions['warning']['db'][$existing_db->db_id]['description'] = $idspace_info['description'];
        }
        if ($existing_db->urlprefix != $idspace_info['urlPrefix']) {

          $summary_dbs[$idspace_info['name']] = sprintf($this->yellow_format, $existing_db->db_id);

          // WARNING:
          // @see chadoCheckTerms_reportProblem_eccentricDb().
          $problems['warning']['db'][$existing_db->db_id][] = [
            'idspace-name' => $idspace_info['name'],
            'column' => 'db.urlprefix',
            'property' => 'idSpace.urlPrefix',
            'YOURS' => $existing_db->urlprefix,
            'EXPECTED' => $idspace_info['urlPrefix'],
          ];
          $solutions['warning']['db'][$existing_db->db_id]['urlprefix'] = $idspace_info['urlPrefix'];
        }
        if ($existing_db->url != $vocab_info['url']) {

          $summary_dbs[$idspace_info['name']] = sprintf($this->yellow_format, $existing_db->db_id);

          // WARNING:
          // @see chadoCheckTerms_reportProblem_eccentricDb().
          $problems['warning']['db'][$existing_db->db_id][] = [
            'message' => $vocab_info['url'] . ': The db.url for this vocabulary in your chado instance does not match what is in the YAML.',
            'idspace-name' => $idspace_info['name'],
            'column' => 'db.url',
            'property' => 'vocabulary.url',
            'YOURS' => $existing_db->url,
            'EXPECTED' => $vocab_info['url'],
          ];
          $solutions['warning']['db'][$existing_db->db_id]['url'] = $vocab_info['url'];
        }
      } else {
        $summary_dbs[$idspace_info['name']] = ' - ';
        $defined_ispaces[$idspace_info['name']] = NULL;
      }
    }

    return [$summary_dbs, $defined_ispaces];
  }

  /**
   * Checks that the term metadata in the YAML matches this chado instance.
   *
   * @param array $term_info
   * @param array $problems
   * @param array $solutions
   * @return array
   *   - summary_cvterm: the value to print in the summary table
   *   - summary_dbxref: the value to print in the summary table
   */
  protected function chadoCheckTerms_checkTerm(array $term_info, array &$problems, array &$solutions) {
    $summary_cvterm = ' ? ';
    $unique_cvterm = NULL;
    $summary_dbxref = ' ? ';

    // Do an extra trim on the yaml values just to make sure.
    foreach($term_info as $key => $value) {
      $term_info[$key] = trim($value);
    }

    // First check that cvterm.name, cvterm.cv, dbxref.accession
    // and dbxref.db all match that which is expected.
    $query = $this->chado->select('1:cvterm', 'cvt')
      ->fields('cvt', ['cvterm_id', 'name', 'definition'])
      ->condition('cvt.name', $term_info['name']);
    $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $query->condition('cv.name', $term_info['cv_name']);
    $query->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
    $query->condition('dbx.accession', $term_info['accession']);
    $query->fields('dbx', ['dbxref_id', 'accession']);
    $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->condition('db.name', $term_info['idspace']);
    $terms = $query->execute()->fetchAll();
    if ($terms && count($terms) == 1) {
      $summary_cvterm = $terms[0]->cvterm_id;
      $summary_dbxref = $terms[0]->dbxref_id;

      // This term is great so no need to continue looking.
      return [$summary_cvterm, $summary_dbxref];
    }

    // If not, then select the cvterm...
    // ... assuming the cvterm.name and cvterm.cv match
    $cv_matches = TRUE;
    $query = $this->chado->select('1:cvterm', 'cvt')
      ->fields('cvt', ['cvterm_id', 'name', 'definition', 'dbxref_id'])
      ->condition('cvt.name', $term_info['name']);
    $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $query->addField('cv', 'name', 'cv_name');
    $query->condition('cv.name', $term_info['cv_name']);
    $query->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
    $query->addField('dbx', 'accession', 'term_accession');
    $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->addField('db', 'name', 'term_idspace');
    $cvterms = $query->execute()->fetchAllAssoc('dbxref_id');

    // ... only looking for the matching cvterm.name.
    if (!$cvterms) {
      $query = $this->chado->select('1:cvterm', 'cvt')
        ->fields('cvt', ['cvterm_id', 'name', 'definition', 'dbxref_id'])
        ->condition('cvt.name', $term_info['name']);
      $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
      $query->addField('cv', 'name', 'cv_name');
      $query->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
      $query->addField('dbx', 'accession', 'term_accession');
      $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
      $query->addField('db', 'name', 'term_idspace');
      $cvterms = $query->execute()->fetchAllAssoc('dbxref_id');
      $cv_matches = FALSE;
    }

    // Also, independently select the dbxref...
    // ... assuming the dbxref.accession and dbxref.db match
    $db_matches = TRUE;
    $query = $this->chado->select('1:dbxref', 'dbx')
      ->fields('dbx', ['dbxref_id', 'accession'])
      ->condition('dbx.accession', $term_info['accession']);
    $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->addField('db', 'name', 'db_name');
    $query->condition('db.name', $term_info['idspace']);
    $dbxrefs = $query->execute()->fetchAllAssoc('dbxref_id');

    // ... only looking for the matching dbxref.accession.
    if (!$dbxrefs) {
      $query = $this->chado->select('1:dbxref', 'dbx')
        ->fields('dbx', ['dbxref_id', 'accession'])
        ->condition('dbx.accession', $term_info['accession']);
      $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
      $query->addField('db', 'name', 'db_name');
      $dbxrefs = $query->execute()->fetchAllAssoc('dbxref_id');
      $db_matches = FALSE;
    }

    // Also grab the first element of cvterms + dbxrefs.
    // We will use this after checking that there is only one element.
    $first_cvterm = reset($cvterms);
    $first_dbxref = reset($dbxrefs);

    // Then we can check a number of cases:
    // CASE: there just is not a cvterm or dbxref.
    if (!$cvterms && !$dbxrefs) {
      $summary_cvterm = ' - ';
      $summary_dbxref = ' - ';
    }

    // CASE: There is only 1 cvterm with matching cv but no dbxref
    //       In this case the cvterm must be connected to the wrong dbxref.
    if (count($cvterms) == 1 && $cv_matches && !$dbxrefs) {
      $summary_dbxref = ' - ';
      $summary_cvterm = sprintf($this->red_format, $first_cvterm->cvterm_id);
      $unique_cvterm = $first_cvterm;

      // ERROR:
      // Cvterm must be connected to the wrong dbxref.
      // @see chadoCheckTerms_reportProblem_terms()
      $problems['error']['term'][ $term_info['id'] ][] = [
        'term-name' => $term_info['name'],
        'term-id' => $term_info['id'],
        'category' => 'wrong_dbxref',
        'message' => 'Wrong or Missing dbxref',
        'error-column' => 'cvterm.dbxref_id',
        'YOURS' => $unique_cvterm->term_idspace . ':' . $unique_cvterm->term_accession,
        'EXPECTED' => $term_info['id'],
      ];
      // @todo suggest a fix.
    }

    // CASE: There is only 1 dbxref with matching db but no cvterm
    if (count($dbxrefs) == 1 && $db_matches && !$cvterms) {
      $summary_cvterm = ' - ';
      $summary_dbxref = $first_dbxref->dbxref_id;
    }

    // CASE: all match but are not connected.
    if (count($cvterms) == 1 && $cv_matches && count($dbxrefs) == 1 && $db_matches) {
      $summary_cvterm = sprintf($this->red_format, $first_cvterm->cvterm_id);
      $summary_dbxref = $first_dbxref->dbxref_id;
      $unique_cvterm = $first_cvterm;

      // ERROR:
      // Broken connection between cvterm + dbxref!
      // @see chadoCheckTerms_reportProblem_terms()
      $problems['error']['term'][ $term_info['id'] ][] = [
        'term-name' => $term_info['name'],
        'term-id' => $term_info['id'],
        'category' => 'wrong_dbxref',
        'message' => 'Broken Connection between cvterm + dbxref',
        'error-column' => 'cvterm.dbxref_id',
        'YOURS' => $unique_cvterm->term_idspace . ':' . $unique_cvterm->term_accession,
        'EXPECTED' => $term_info['id'],
      ];
      // @todo suggest fix.
    }

    // At this point we have one match and the other is either missing or not unique.
    // Single dbxref match but missing or non-unique cvterm.

    // CASE: cvterm.name, dbxref.accession, dbxref.db match + are connected.
    //       only cvterm.cv is not matching and may need to be updated.
    if ($db_matches && $summary_cvterm == ' ? ' && array_key_exists($first_dbxref->dbxref_id, $cvterms)) {
      $summary_cvterm = sprintf($this->red_format, $cvterms[$first_dbxref->dbxref_id]->cvterm_id);
      $summary_dbxref = $first_dbxref->dbxref_id;
      $unique_cvterm = $cvterms[$first_dbxref->dbxref_id];

      // ERROR:
      // cv doesn't match but the cvterm is connected to the right dbxref
      // so we are pretty sure this connection is valid.
      // @see chadoCheckTerms_reportProblem_terms()
      $problems['error']['term'][$term_info['id']][] = [
        'term-name' => $term_info['name'],
        'term-id' => $term_info['id'],
        'category' => 'wrong_cv',
        'message' => 'Wrong cv (cvterm validated by dbxref)',
        'error-column' => 'cvterm.cv_id',
        'YOURS' => $unique_cvterm->cv_name,
        'EXPECTED' => $term_info['cv_name'],
      ];
      // @todo suggest fix.
    }

    // If we still don't know what to do with the cvterm...
    if ($db_matches && $summary_cvterm == ' ? ') {

      // Now we want to determine if there are any other cvterms connected to
      // the single perfect dbxref we found. If there are then that is a concern
      // but if not then it turns out this dbxref is without error.
      $query = $this->chado->select('1:dbxref', 'dbx')
        ->condition('dbx.accession', $term_info['accession']);
      $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
      $query->condition('db.name', $term_info['idspace']);
      $query->join('1:cvterm', 'cvt', 'cvt.dbxref_id = dbx.dbxref_id');
      $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
      $query->addExpression("cvt.name||' ('||cv.name||')'", 'Term');
      $connected_cvterms = $query->execute()->fetchCol();

      // CASE: dbxref.accession, dbxref.db match but they are connected to
      //       a different cvterm.
      if ($connected_cvterms) {

        $summary_dbxref = sprintf($this->red_format, $first_dbxref->dbxref_id);

        // ERROR:
        // Dbxref is connected to other cvterms and not it's correct one!
        // @see chadoCheckTerms_reportProblem_terms()
        $problems['error']['term'][$term_info['id']][] = [
          'term-name' => $term_info['name'],
          'term-id' => $term_info['id'],
          'category' => 'wrong_cvterm',
          'message' => 'Dbxref is connected to the wrong cvterm(s)',
          'error-column' => 'dbxref>cvterm.dbxref_id',
          'YOURS' => implode(', ', $connected_cvterms),
          'EXPECTED' => $term_info['name'] . ' (' . $term_info['cv_name'] . ')',
        ];
        // @todo suggest a fix.
      }
      else {
        // CASE: dbxref.accession, dbxref.db match but there is no matching cvterm.
        $summary_dbxref = $first_dbxref->dbxref_id;
      }
    }

    // CASE: cvterm.name, cvterm.cv, and dbxref.accession match + are connected.
    //       only dbxref.db is not matching and may need to be updated.
    if ($cv_matches && $summary_dbxref == ' ? ' && array_key_exists($first_cvterm->dbxref_id, $dbxrefs)) {

      $summary_cvterm = $first_cvterm->cvterm_id;
      $summary_dbxref = sprintf($this->red_format, $dbxrefs[$first_cvterm->dbxref_id]);
      $unique_cvterm = $first_cvterm;

      // ERROR:
      // db doesn't match but the dbxref is connected to a good cvterm.
      // so this connection might be valid...
      // @see chadoCheckTerms_reportProblem_terms()
      $problems['error']['term'][$term_info['id']][] = [
        'term-name' => $term_info['name'],
        'term-id' => $term_info['id'],
        'category' => 'wrong_db',
        'message' => 'Wrong db but dbxref connected to right cvterm',
        'error-column' => 'dbxref.db_id',
        'YOURS' => $first_dbxref->db_name,
        'EXPECTED' => $term_info['db_name'],
      ];
      // @todo suggest fix.
    }
    // CASE: cvterm.name and cvterm.cv match but they are connected to
    //       a different dbxref.
    elseif ($cv_matches && $summary_dbxref == ' ? ') {
      $summary_cvterm = sprintf($this->red_format, $first_cvterm->cvterm_id);
      $summary_dbxref = ' - ';
      $unique_cvterm = $first_cvterm;

      // ERROR:
      // cvterm is attached to the wrong dbxref!
      // @see chadoCheckTerms_reportProblem_terms()
      $problems['error']['term'][$term_info['id']][] = [
        'term-name' => $term_info['name'],
        'term-id' => $term_info['id'],
        'category' => 'wrong_dbxref',
        'message' => 'Dbxref is missing and cvterm is attached to wrong dbxref',
        'error-column' => 'cvterm.dbxref_id',
        'YOURS' => $unique_cvterm->term_idspace . ':' . $unique_cvterm->term_accession,
        'EXPECTED' => $term_info['id'],
      ];
      // @todo suggest a fix
    }

    // Try to catch a few final cases that slip through...
    // If we never did find any cvterms then it is just missing.
    if ($summary_cvterm == ' ? ' && !$cvterms) {
      $summary_cvterm = ' - ';
    }
    // If we never did find any dbxrefs then it is just missing.
    if ($summary_dbxref == ' ? ' && !$dbxrefs) {
      $summary_dbxref = ' - ';
    }
    // If we never did find any meaningful connections with the multiple
    // dbxrefs we found then they were false positives.
    if ($summary_dbxref == ' ? ' AND count($dbxrefs) > 1) {
      $summary_dbxref = ' - ';
    }
    // At this point we feel we have checked all the possibilities with the
    // cvterms we selected earlier so they are likely false positives
    // if the cv didn't match.
    if ($summary_cvterm == ' ? ' && !$cv_matches) {
      $summary_cvterm = ' - ';
    }

    // Report missed cases.
    if ($summary_cvterm == ' ? ') {
      $this->io()->error('We missed a case with the cvterm for ' . $term_info['label'] . '. These are the cvterms we have to work with: ' . print_r($cvterms, TRUE));
    }
    if ($summary_dbxref == ' ? ') {
      $this->io()->error('We missed a case with the dbxref for ' . $term_info['label'] . '. These are the dbxrefs we have to work with: ' . print_r($dbxrefs, TRUE));
    }

    // Finally we can check the cvterm definition if we have found one!
    if ($unique_cvterm) {
      if (array_key_exists('description', $term_info) && ($unique_cvterm->definition !== $term_info['description'])) {

        // WARNING:
        // The term definition does not match what we expected.
        // @see chadoCheckTerms_reportProblem_eccentricCVTerm()
        $problems['warning']['cvterm'][$unique_cvterm->cvterm_id][] = [
          'column' => 'cvterm.definition',
          'property' => 'term.description',
          'YOURS' => $unique_cvterm->definition,
          'EXPECTED' => $term_info['description'],
          'term-name' => $term_info['name'],
          'term-id' => $term_info['id'],
        ];
        $solutions['warning']['cvterm'][$unique_cvterm->cvterm_id]['definition'] = $term_info['description'];
      }
    }

    return [$summary_cvterm, $summary_dbxref];
  }

  /**
   * Prints a beautiful summary table showing the status of all terms.
   *
   * @param array $summary_rows
   * @return void
   *   No need to return as we are printing directly.
   */
  protected function chadoCheckTerms_printSummaryTable(array $summary_rows) {

    $summary_headers = [
      'term' => 'YAML Term',
      'cv' => 'CV',
      'db' => 'DB',
      'cvterm' => 'CVTERM',
      'dbxref' => 'DBXREF',
    ];

    $this->output()->writeln('');
    $this->output()->writeln('The following table summarizes the terms.');
    $this->io()->table($summary_headers, $summary_rows);
    $this->output()->writeln('Legend:');
    $this->output()->writeln(sprintf($this->yellow_format, ' YELLOW ') . ' Indicates there are some mismatches between the existing version and what we expected but it\'s minor.');
    $this->output()->writeln(sprintf($this->red_format, '  RED   ') . ' Indicates there is a serious mismatch which will cause the prepare to fail on this chado instance.');
    $this->output()->writeln('    -      Indicates this one is missing but that is not a concern as it will be added when you run prepare.');
    $this->output()->writeln('');

  }

  /**
   * Asks the user if the options specified by option key is not TRUE.
   *
   * @param string $ask_message
   *  A message to show to the user if we need to ask them whether we should continue.
   * @param array $options
   *  The options provided to the drush command.
   * @param string $option_key
   *  The key of the option to check.
   *  Should be either 'auto-expand' or 'auto-fix'.
   * @param boolean $worth_continuing
   *  Indicates if there is any point asking the user or checking options. For
   *  example, when the point is to decide whether to show more detail, if there
   *  are no details recorded then there is no point continueing ;-p
   * @param boolean $default
   *  The default option when asking the user to confirm. It should be true
   *  for non-destructive processes and false otherwise.
   * @return boolean
   *  Yes or no in response to the question posed by the message.
   */
  private function askOrRespectOptions(string $ask_message, array $options, string $option_key, bool $worth_continuing, bool $default) {

    if (!$worth_continuing) {
      return FALSE;
    }

    if (array_key_exists($option_key, $options) && $options[$option_key]) {
      $response = TRUE;
    }
    else {
      $response = $this->io()->confirm($ask_message, $default);
    }

    return $response;
  }

  /**
   * Updates records in chado based on an array of records.
   *
   * @param string $table_name
   *  The name of the chado table to be updated.
   * @param string $pkey
   *  The name of the primary key of the table to be updated.
   * @param array $records
   *  An array of the following format:
   *   - [primary key of the table]: an array of columns to update where each
   *     is of the form:
   *      - [column]: [value to update it to]
   * @return void
   */
  protected function updateChadoTermRecords(string $table_name, string $pkey, array $records) {

    foreach ($records as $id => $values) {
      $query = $this->chado->update('1:' . $table_name)
        ->fields($values)
        ->condition($pkey, $id);
      $query->execute();
    }
  }

  /**
   * Reports errors and potential solutions for the "missingDbYaml" error type.
   *
   * Trigger Example: Imagine there is a term defined whose id is DATUM:12345
   *   but the vocabulary this term is in either
   *   1. has a number of ID Spaces defined but none of them have the
   *      idSpaces[name] of 'DATUM' (case sensitive match required).
   *   2. does not have any id spaces defined.
   *
   * @param array $problems
   *  An array describing instances with this type of error with the following format:
   *    - [YAML ID Sapce name]: an array of reports where a term had the ID Space
   *      indicated by the key despite that ID Space not being defined in the YAML.
   *      Each report has the following structure:
   *        - missing-db-name:
   *        - defined-dbs:
   *        - term:
   *        - vocab:
   * @param array $solutions
   *  There are currently no easy suggested solutions for this but the parameter
   *  is here in case we decide to be more helpful later ;-p
   *
   * @return void
   *   This function interacts through command-line input/output directly and
   *   as such, does not need to return anything to the parent Drush command.
   */
  protected function chadoCheckTerms_reportProblem_missingDbYaml($problems, $solutions, $options) {

    $this->io()->section('YAML Issues: Missing ID Space definitions.');
    $num_detected = count($problems);
    $this->output()->writeln("We have detected $num_detected ID Space(s) missing from your YAML file. You will want to contact the developers to let them know the following output:");
    $list = [];
    foreach ($problems as $idspace => $terms_with_issues) {
      foreach ($terms_with_issues as $prob_deets) {
        if (count($prob_deets['defined-dbs']) > 0) {
          $list[] = sprintf(
            "Term %s: Missing '%s' ID Space from defined ID Spaces for '%s' vocabulary. Defined ID Spaces include %s.",
            $prob_deets['term'],
            $prob_deets['missing-db-name'],
            $prob_deets['vocab'],
            implode(
              ', ',
              array_keys($prob_deets['defined-dbs'])
            )
          );
        }
        else {
          $list[] = sprintf(
            "Term %s: Missing '%s' ID Space from defined ID Spaces for '%s' vocabulary. There were no ID Spaces at all defined for this vocabulary.",
            $prob_deets['term'],
            $prob_deets['missing-db-name'],
            $prob_deets['vocab']
          );
        }
      }
    }
    $this->io()->listing($list);
  }

  /**
   * Reports errors and potential solutions for the "term" error type.
   *
   * Trigger Examples:
   *   Imagine a term defined with a name of 'Location' and an id of 'NCIT:C25341'
   *
   * @param array $problems
   *  An array describing instances with this type of error with the following format:
   *    - [YAML Term ID]: an array of reports where each report has the
   *      following structure:
   *        - term-name:
   *        - term-id:
   *        - category:
   *        - message:
   *        - error-column
   *        - YOURS
   *        - EXPECTED
   * @param array $solutions
   *  There are currently no easy suggested solutions for this but the parameter
   *  is here in case we decide to be more helpful later ;-p
   *
   * @return void
   *   This function interacts through command-line input/output directly and
   *   as such, does not need to return anything to the parent Drush command.
   */
  protected function chadoCheckTerms_reportProblem_terms($problems, $solutions, $options) {


    $this->io()->section('Term (cvterm/dbxref) Issues.');
    $num_detected = count($problems);
    $this->output()->writeln("We have detected $num_detected Term(s) with a key deviation from what is expected. Specifically:");

    $table = new Table($this->output());
    $table->setHeaders(['TERM', 'MESSAGE', 'COLUMN', 'EXPECTED', 'YOURS']);
    // Set the yours/expected columns to wrap at 50 characters each.
    $table->setColumnMaxWidth(4, 50);
    $table->setColumnMaxWidth(5, 50);

    $rows = [];
    foreach ($problems as $id => $terms_with_issues) {
      foreach ($terms_with_issues as $prob_deets) {
        $rows[] = [
          $prob_deets['term-name'] . ' (' . $prob_deets['term-id'] . ')',
          $prob_deets['message'],
          $prob_deets['error-column'],
          $prob_deets['EXPECTED'],
          $prob_deets['YOURS'],
        ];
      }
    }
    $table->addRows($rows);
    $table->render();
  }

  /**
   * Reports warnings and potential solutions for the "cv" warning type.
   *
   * Trigger Example: Imagine there is a vocabulary defined whose
   *   1. definition in the YAML is different from in your chado instance
   *
   * @param array $problems
   *  An array describing instances with this type of warning with the following format:
   *    - [Existing cv_id]: an array of reports describing how this cv differs
   *      in your chado instance from what is defined in the YAML.
   *      Each report has the following structure:
   *        - vocab-name: the name of the vocabulary in the YAML which must
   *          match the cv in your chado instance.
   *        - column: the chado column showing a difference
   *        - property: the yaml property being compared
   *        - YOURS: the value in your chado instance
   *        - THEIRS: the value in the YAML
   * @param array $solutions
   *  An array describing possible solutions with the following format:
   *    - [Existing cv_id]: an array of columns in the cv table to update.
   *      Each entry has the following structure:
   *        - [column name]: [value in YAML]
   *
   * @return void
   *   This function interacts through command-line input/output directly and
   *   as such, does not need to return anything to the parent Drush command.
   */
  protected function chadoCheckTerms_reportProblem_eccentricCv($problems, $solutions, $options) {

    $this->io()->section('Small differences in vocabulary definitions.');
    $num_detected = count($problems);
    $this->output()->writeln("We have detected $num_detected vocabularies in your chado instance that differ from those defined in the YAML in small ways. More specifically:");

    $table = new Table($this->output());
    $table->setHeaders(['VOCAB','PROPERTY', 'COLUMN', 'EXPECTED', 'YOURS']);
    // Set the yours/expected columns to wrap at 50 characters each.
    $table->setColumnMaxWidth(3, 50);
    $table->setColumnMaxWidth(4, 50);

    $rows = [];
    foreach ($problems as $cv_id => $specific_issues) {
      foreach ($specific_issues as $prob_deets) {
        $rows[] = [
          $prob_deets['vocab-name'],
          $prob_deets['property'],
          $prob_deets['column'],
          $prob_deets['EXPECTED'],
          $prob_deets['YOURS'],
        ];
      }
    }
    $table->addRows($rows);
    $table->render();

    $offer_fix = !$options['no-fix'];
    $fix = $this->askOrRespectOptions(
      'Would you like us to update the descriptions of your chado cvs to match our expectations?',
      $options,
      'auto-fix',
      $offer_fix,
      FALSE
    );
    if ($fix) {
      $this->updateChadoTermRecords('cv', 'cv_id', $solutions);
      $this->io()->success('Vocabularies have been updated to match our expectations.');
    }
  }

  /**
   * Reports warnings and potential solutions for the "db" warning type.
   *
   * Trigger Example: Imagine there is a ID Space defined whose
   *   1. definition in the YAML is different from in your chado instance
   *
   * @param array $problems
   *  An array describing instances with this type of warning with the following format:
   *    - [Existing db_id]: an array of reports describing how this db differs
   *      in your chado instance from what is defined in the YAML.
   *      Each report has the following structure:
   *        - idspace-name: the name of the id space in the YAML which must
   *          match the cv in your chado instance.
   *        - column: the chado column showing a difference
   *        - property: the yaml property being compared
   *        - YOURS: the value in your chado instance
   *        - THEIRS: the value in the YAML
   * @param array $solutions
   *  An array describing possible solutions with the following format:
   *    - [Existing db_id]: an array of columns in the db table to update.
   *      Each entry has the following structure:
   *        - [column name]: [value in YAML]
   *
   * @return void
   *   This function interacts through command-line input/output directly and
   *   as such, does not need to return anything to the parent Drush command.
   */
  protected function chadoCheckTerms_reportProblem_eccentricDb($problems, $solutions, $options) {

    $this->io()->section('Small differences in ID Space entries.');
    $num_detected = count($problems);
    $this->output()->writeln("We have detected $num_detected ID Spaces in your chado instance that differ from those defined in the YAML in small ways. More specifically:");

    $table = new Table($this->output());
    $table->setHeaders(['ID SPACE', 'PROPERTY', 'COLUMN', 'EXPECTED', 'YOURS']);
    // Set the yours/expected columns to wrap at 50 characters each.
    $table->setColumnMaxWidth(3, 50);
    $table->setColumnMaxWidth(4, 50);

    $rows = [];
    foreach ($problems as $db_id => $specific_issues) {
      foreach ($specific_issues as $prob_deets) {
        $rows[] = [
          $prob_deets['idspace-name'],
          $prob_deets['property'],
          $prob_deets['column'],
          $prob_deets['EXPECTED'],
          $prob_deets['YOURS'],
        ];
      }
    }
    $table->addRows($rows);
    $table->render();

    $offer_fix = !$options['no-fix'];
    $fix = $this->askOrRespectOptions(
      'Would you like us to update the non-critical db columns to match our expectations?',
      $options,
      'auto-fix',
      $offer_fix,
      FALSE
    );
    if ($fix) {
      $this->updateChadoTermRecords('db', 'db_id', $solutions);
      $this->io()->success('ID Spaces have been updated to match our expectations.');
    }
  }

  /**
   * Reports warnings and potential solutions for the "cvterm" warning type.
   *
   * Trigger Example: Imagine there is a cvterm defined whose
   *   1. definition in the YAML is different from in your chado instance
   *
   * @param array $problems
   *  An array describing instances with this type of warning with the following format:
   *    - [Existing cvterm_id]: an array of reports describing how this cvterm differs
   *      in your chado instance from what is defined in the YAML.
   *      Each report has the following structure:
   *        - term-name: the name of the term in the YAML which must
   *          match the cvterm in your chado instance.
   *        - term-id: the full id of the term in the YAML which must
   *          match the connected dbxref in your database.
   *        - column: the chado column showing a difference
   *        - property: the yaml property being compared
   *        - YOURS: the value in your chado instance
   *        - THEIRS: the value in the YAML
   * @param array $solutions
   *  An array describing possible solutions with the following format:
   *    - [Existing cvterm_id]: an array of columns in the cvterm table to update.
   *      Each entry has the following structure:
   *        - [column name]: [value in YAML]
   *
   * @return void
   *   This function interacts through command-line input/output directly and
   *   as such, does not need to return anything to the parent Drush command.
   */
  protected function chadoCheckTerms_reportProblem_eccentricCVTerm($problems, $solutions, $options) {

    $this->io()->section('Small differences in Term entries.');
    $num_detected = count($problems);
    $this->output()->writeln("We have detected $num_detected Terms in your chado instance that differ from those defined in the YAML in small ways. More specifically:");

    $table = new Table($this->output());
    $table->setHeaders(['TERM NAME', 'TERM ACCESSION', 'PROPERTY', 'COLUMN', 'EXPECTED', 'YOURS']);
    // Set the yours/expected columns to wrap at 50 characters each.
    $table->setColumnMaxWidth(4, 50);
    $table->setColumnMaxWidth(5, 50);

    $rows = [];
    foreach ($problems as $cvterm_id => $specific_issues) {
      foreach ($specific_issues as $prob_deets) {
        $rows[] = [
          $prob_deets['term-name'],
          $prob_deets['term-id'],
          $prob_deets['property'],
          $prob_deets['column'],
          $prob_deets['EXPECTED'],
          $prob_deets['YOURS'],
        ];
      }
    }
    $table->addRows($rows);
    $table->render();

    $offer_fix = !$options['no-fix'];
    $fix = $this->askOrRespectOptions(
      'Would you like us to update the non-critical cvterm columns to match our expectations?',
      $options,
      'auto-fix',
      $offer_fix
    );
    if ($fix) {
      $this->updateChadoTermRecords('cvterm', 'cvterm_id', $solutions);
      $this->io()->success('Terms have been updated to match our expectations.');
    }
  }
}
