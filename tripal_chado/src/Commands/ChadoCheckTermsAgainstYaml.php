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

  /**
   * Checks a given chado install for any inconsistencies between its cvterms
   * and what Tripal expects.
   *
   * @command tripal-chado:trp-check-terms
   * @aliases trp-check-terms
   * @options chado_schema
   *   The name of the chado schema to check.
   * @usage drush trp-check-terms --chado_schema=chado_prod
   *   Checks the terms stored in chado_prod.cvterm for consistency.
   */
  public function chadoCheckTermsAreAsExpected($options = ['chado_schema' => NULL]) {
    $red = "\033[31;40m\033[1m %s \033[0m";
    $yellow = "\033[1;33;40m\033[1m %s \033[0m";

    if (!$options['chado_schema']) {
      throw new \Exception(dt('The --chado_schema argument is required.'));
    }
    $this->chado_schema = $options['chado_schema'];

    /// We're going to use symphony tables to summarize what this command finds.
    // As such, I'm going to setup the header now and then compile the rows as I go.
    // Each row will be a term and the whole table will be printed at once at the end.
    $summary_headers = [
      'term' => 'YAML Term',
      'cv' => 'CV',
      'db' => 'DB',
      'cvterm' => 'CVTERM',
      'dbxref' => 'DBXREF',
    ];
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

    $chado = \Drupal::service('tripal_chado.database');
    $chado->setSchemaName($options['chado_schema']);
    $this->chado = $chado;

    $this->output()->writeln('');
    $this->output()->writeln('Using the Chado Content Terms YAML specification to determine what Tripal expects.');
    $this->output()->writeln('');

    $config_factory = \Drupal::service('config.factory');

    $id = 'chado_content_terms';
    $config_key = 'tripal.tripal_content_terms.' . $id;
    $config = $config_factory->get($config_key);
    if ($config) {
      $this->output()->writeln("  Finding term definitions for $id term collection.");
      $vocabs = $config->get('vocabularies');
      if ($vocabs) {
        foreach ($vocabs as $vocab_info) {

          // Reset for the new vocab.
          $summary_term = NULL;
          $summary_cv = NULL;
          $summary_dbs = [];
          $summary_cvterm = NULL;
          $summary_dbxref = NULL;

          // Check if the cv record for this vocabulary exists.
          $query = $chado->select('1:cv', 'cv')
            ->fields('cv', ['cv_id', 'definition'])
            ->condition('cv.name', $vocab_info['name']);
          $existing_cv = $query->execute()->fetchObject();
          if ($existing_cv) {
            $summary_cv = $existing_cv->cv_id;

            // Check if the definition matches our expectations and warn if not.
            if ($existing_cv->definition != $vocab_info['label']) {
              $summary_cv = sprintf($yellow, $existing_cv->cv_id);

              // WARNING:
              // @see chadoCheckTermsAreAsExpected_eccentricCv().
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

          // Now look for connected ID Spaces...
          $defined_ispaces = [];
          foreach ($vocab_info['idSpaces'] as $idspace_info) {

            // Check if the db record for this id space exists.
            $query = $chado->select('1:db', 'db')
              ->fields('db', ['db_id', 'description', 'urlprefix', 'url'])
              ->condition('db.name', $idspace_info['name']);
            $existing_db = $query->execute()->fetchObject();
            if ($existing_db) {
              $summary_dbs[$idspace_info['name']] = $existing_db->db_id;
              $defined_ispaces[$idspace_info['name']] = $existing_db->db_id;

              // Now check the db description, url prefix and url match what we expect and warn if not.
              if ($existing_db->description != $idspace_info['description']) {

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // @see chadoCheckTermsAreAsExpected_eccentricDb().
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

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // @see chadoCheckTermsAreAsExpected_eccentricDb().
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

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // @see chadoCheckTermsAreAsExpected_eccentricDb().
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

          // Now for each term in this vocabulary...
          $vocab_info['terms'] = (array_key_exists('terms', $vocab_info)) ? $vocab_info['terms'] : [];
          foreach ($vocab_info['terms'] as $term_info) {

            $summary_term = $term_info['name'] . ' (' . $term_info['id'] . ')';

            // Extract the parts of the id.
            [$term_db, $term_accession] = explode(':', $term_info['id']);

            // Check the term id space was defined in the id spaces block
            // Note: if a id space was defined but not found in the database
            // it will still be in the $defined_idspaces array but the value
            // will be NULL.
            if (!array_key_exists($term_db, $defined_ispaces)) {

              $summary_dbs[$idspace_info['name']] = sprintf($red, ' X ');

              // ERROR:
              // The YAML-defined term includes an ID Space that was not defined in the ID Spaces section for this vocabulary.
              // @ see chadoCheckTermsAreAsExpected_missingDbYaml().
              $problems['error']['missingDbYaml'][$term_db][] = [
                'missing-db-name' => $term_db,
                'defined-dbs' => $defined_ispaces,
                'term' => $summary_term,
                'vocab' => $vocab_info['name'],
              ];
              // No solution for this one... instead the developer of the module needs to fix their YAML ;-p
              $solutions['error']['missingDbYaml'] = [];
            }

            // First check that cvterm.name, cvterm.cv, dbxref.accession
            // and dbxref.db all match that which is expected.

            // If not, then select the cvterm...
            // ... assuming the cvterm.name and cvterm.cv match
            // @todo implement this.

            // ... only looking for the matching cvterm.name.
            // @todo implement this.

            // Also, indendantly select the dbxref...
            // ... assuming the dbxref.accession and dbxref.db match
            // @todo implement this.

            // ... only looking for the matching dbxref.accession.
            // @todo implement this.

            // Then we can check a number of cases:
            // CASE: cvterm.name, dbxref.accession, dbxref.db match + are connected.
            //       only cvterm.cv is not matching and may need to be updated.
            // @todo implement this.

            // CASE: cvterm.name, cvterm.cv, and dbxref.accession match + are connected.
            //       only dbxref.db is not matching and may need to be updated.
            // @todo implement this.

            // CASE: all match but are not connected.
            // @todo implement this.

            // CASE: it's just missing which is not actually a problem.
            // @todo implement this.

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
      }
    }

    // Finally tell the user the summary state of things.
    $this->output()->writeln('');
    $this->output()->writeln('The following table summarizes the terms.');
    $this->io()->table($summary_headers, $summary_rows);
    $this->output()->writeln('Legend:');
    $this->output()->writeln(sprintf($yellow, ' YELLOW ') . ' Indicates there are some mismatches between the existing version and what we expected but it\'s minor.');
    $this->output()->writeln(sprintf($red, '  RED   ') . ' Indicates there is a serious mismatch which will cause the prepare to fail on this chado instance.');
    $this->output()->writeln('    -      Indicates this one is missing but that is not a concern as it will be added when you run prepare.');
    $this->output()->writeln('');

    // Now we can start reporting more detail if they want.
    // First ERRORS:
    $this->io()->title('Errors');
    $this->output()->writeln('Differences are categorized as errors if they are likely to cause failures when preparing this chado instance or to cause Tripal to be unable to find the term reliably.');

    if (array_key_exists('error', $problems) && count($problems['error']) > 0) {
      $show_errors = $this->io()->confirm('Would you like to see more details about the errors?');
      if ($show_errors) {

        // missingDbYaml
        if (array_key_exists('missingDbYaml', $problems['error'])) {
          $this->chadoCheckTermsAreAsExpected_missingDbYaml(
            $problems['error']['missingDbYaml'],
            $solutions['error']['missingDbYaml']
          );
        }
      }
    }
    else {
      $this->io()->success('There are no errors associated with this chado instance!');
    }
    $this->output()->writeln('');

    // Then WARNINGS:
    $this->io()->title('Warnings');
    $this->output()->writeln('Differences are categorized as warnings if they are in non-critical parts of the terms, vocabularies and references. These can be safely ignored but you may also want to use this opprotinuity to update your version of these terms.');
    if (array_key_exists('warning', $problems) && count($problems['warning']) > 0) {
      $show_warnings = $this->io()->confirm('Would you like to see more details about the warnings?');
      if ($show_warnings) {

        // Small differences between the expected and found chado.cv record.
        if (array_key_exists('cv', $problems['warning'])) {
          $this->chadoCheckTermsAreAsExpected_eccentricCv(
            $problems['warning']['cv'],
            $solutions['warning']['cv']
          );
        }

        $this->output()->writeln('');

        // Small differences between the expected and found chado.db record.
        if (array_key_exists('db', $problems['warning'])) {
          $this->chadoCheckTermsAreAsExpected_eccentricDb(
            $problems['warning']['db'],
            $solutions['warning']['db']
          );
        }

        $this->output()->writeln('');
      }
    }
    else {
      $this->io()->success('There are no warning associated with this chado instance!');
    }
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
  protected function chadoCheckTermsAreAsExpected_missingDbYaml($problems, $solutions = []) {

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
  protected function chadoCheckTermsAreAsExpected_eccentricCv($problems, $solutions) {

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

    $fix = $this->io()->confirm('Would you like us to update the descriptions of your chado cvs to match our expectations?');
    if ($fix) {
      $this->updateChadoTermRecords('cv', 'cv_id', $solutions);
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
  protected function chadoCheckTermsAreAsExpected_eccentricDb($problems, $solutions) {

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

    $fix = $this->io()->confirm('Would you like us to update the descriptions of your chado dbs to match our expectations?');
    if ($fix) {
      $this->updateChadoTermRecords('db', 'db_id', $solutions);
    }
  }
}
