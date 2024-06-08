<?php
namespace Drupal\tripal_chado\Commands;

use Drush\Commands\DrushCommands;

/**
 * Drush commands
 */
class ChadoFixingCommands extends DrushCommands {


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
              $problems['warning']['cv'][$existing_cv->cv_id][] = [
                'message' => $vocab_info['name'] . ': The cv.definition for this vocabulary in your chado instance does not match what is in the YAML.',
                'YOURS' => $existing_cv->definition,
                'EXPECTED' => $vocab_info['label'],
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
                $problems['warning']['db'][$existing_db->db_id][] = [
                  'message' => $idspace_info['name'] . ': The db.description for this ID Space in your chado instance does not match what is in the YAML.',
                  'YOURS' => $existing_db->description,
                  'EXPECTED' => $idspace_info['description'],
                ];
                $solutions['warning']['db'][$existing_db->db_id]['description'] = $idspace_info['description'];
              }
              if ($existing_db->urlprefix != $idspace_info['urlPrefix']) {

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                $problems['warning']['db'][$existing_db->db_id][] = [
                  'message' => $idspace_info['name'] . ': The db.urlprefix for this ID Space in your chado instance does not match what is in the YAML.',
                  'YOURS' => $existing_db->urlprefix,
                  'EXPECTED' => $idspace_info['urlPrefix'],
                ];
                $solutions['warning']['db'][$existing_db->db_id]['urlprefix'] = $idspace_info['urlPrefix'];
              }
              if ($existing_db->url != $vocab_info['url']) {

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                $problems['warning']['db'][$existing_db->db_id][] = [
                  'message' => $vocab_info['url'] . ': The db.url for this vocabulary in your chado instance does not match what is in the YAML.',
                  'YOURS' => $existing_db->url,
                  'EXPECTED' => $vocab_info['url'],
                ];
                $solutions['warning']['db'][$existing_db->db_id]['urlprefix'] = $vocab_info['url'];
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
            if (!array_key_exists($term_db, $defined_ispaces)) {

              $summary_dbs[$idspace_info['name']] = sprintf($red, ' X ');

              // ERROR:
              $problems['error']['missing-db'][$term_db][] = [
                'message' => $summary_term . ': The YAML-defined term includes an ID Space that was not defined in the ID Spaces section for this vocabulary.',
                'missing-db-name' => $term_db,
                'defined-dbs' => $defined_ispaces,
                'term' => $summary_term,
                'vocab' => $vocab_info['name'],
              ];
              // No solution for this one... instead the developer of the module needs to fix their YAML ;-p
            }

            // Check if the cvterm record for this term exists.
            $query = $chado->select('1:cvterm', 'cvt')
              ->fields('cvt', ['cvterm_id', 'name', 'definition', 'dbxref_id', 'cv_id'])
              ->condition('cvt.name', $term_info['name']);
            $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
            $query->condition('cv.name', $vocab_info['name']);
            $existing_cvterm = $query->execute()->fetchObject();
            if ($existing_cvterm) {
              $summary_cvterm = $existing_cvterm->cvterm_id;

              // Now check the term definition.
              $term_info['description'] = (array_key_exists('description', $term_info)) ? $term_info['description'] : '';
              if ($existing_cvterm->definition != $term_info['description']) {

                $summary_cvterm = sprintf($yellow, $existing_cvterm->cvterm_id);

                // WARNING:
                $problems['warning']['cvterm'][$existing_cvterm->cvterm_id][] = [
                  'message' => $summary_term . ': The cvterm.definition for this term in your chado instance does not match what is in the YAML.',
                  'YOURS' => $existing_cvterm->definition,
                  'EXPECTED' => $term_info['description'],
                ];
                $solutions['warning']['cvterm'][$existing_cvterm->cvterm_id]['definition'] = $term_info['description'];
              }

              // Now get the dbxref record for this cvterm.
              $query = $chado->select('1:dbxref', 'dbx')
                ->fields('dbx', ['dbxref_id', 'accession'])
                ->condition('dbx.dbxref_id', $existing_cvterm->dbxref_id);
              $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
              $query->addField('db', 'name', 'db_name');
              $existing_dbxref = $query->execute()->fetchObject();
              if ($existing_dbxref) {
                $summary_dbxref = $existing_dbxref->dbxref_id;

                // Check the term id space matches the dbxref>db.name
                if ($existing_dbxref->db_name != $term_db) {

                  $summary_dbxref = sprintf($red, $existing_dbxref->dbxref_id);

                  // ERROR:
                  $problems['error']['dbxref'][$existing_dbxref->dbxref_id][] = [
                    'message' => 'The dbxref.db_id>db.name for this term in your chado instance does not match what is in the YAML.',
                    'YOURS' => $existing_dbxref->db_name,
                    'EXPECTED' => $term_db,
                    'term' => $summary_term,
                  ];
                  // We only have a solution to suggest if the db was defined and existed already.
                  if (array_key_exists($term_db, $defined_ispaces)) {
                    $solutions['error']['dbxref'][$existing_dbxref->dbxref_id]['db_id'] = $defined_ispaces[$term_db];
                  }
                }
                // Check the term accession matches the dbxref.accession
                if ($existing_dbxref->accession != $term_accession) {

                  $summary_dbxref = sprintf($red, $existing_dbxref->dbxref_id);

                  // ERROR:
                  $problems['error']['dbxref'][$existing_dbxref->dbxref_id][] = [
                    'message' => 'The dbxref.accession for this term in your chado instance does not match what is in the YAML.',
                    'YOURS' => $existing_dbxref->accession,
                    'EXPECTED' => $term_accession,
                    'term' => $summary_term,
                  ];
                  $solutions['error']['dbxref'][$existing_dbxref->dbxref_id]['accession'] = $term_accession;
                }
              } else {
                $summary_dbxref = sprintf($red, $existing_cvterm->dbxref_id);

                // ERROR:
                // NO DBXREF Record associated with the existing cvterm! This should not be possible due to database contraints so your Chado is Very Broken!
                $problems['error']['missing-dbxref'][$existing_cvterm->cvterm_id][] = [
                  'dbxref_id' => $existing_cvterm->dbxref_id,
                  'term' => $summary_term,
                ];
                // We only have a solution to suggest if the db was defined and existed already.
                if (array_key_exists($term_db, $defined_ispaces)) {
                  $solutions['error']['missing-dbxref'][$existing_cvterm->cvterm_id] = [
                    'dbxref_id' => $existing_cvterm->dbxref_id,
                    'db_id' => $defined_ispaces[$term_db],
                    'accession' => $term_accession,
                  ];
                }
              }
            } else {
              $summary_cvterm = ' - ';
              // Check if the accession exists as defined in case there is a
              // mismatch between term name and accession.
              $query = $chado->select('1:dbxref', 'dbx')
                ->fields('dbx', ['dbxref_id', 'accession'])
                ->condition('dbx.accession', $term_accession);
              $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
              $query->addField('db', 'name', 'db_name');
              $query->condition('db.name', $term_db);
              $existing_dbxrefs = $query->execute()->fetchAll();

              if ($existing_dbxrefs) {
                foreach ($existing_dbxrefs as $existing_dbxref) {

                  // Get information about any attached terms.
                  $query = $chado->select('1:cvterm', 'cvt')
                    ->fields('cvt', ['name'])
                    ->condition('cvt.dbxref_id', $existing_dbxref->dbxref_id);
                  $query->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
                  $query->addField('cv', 'name', 'cv_name');

                  // If there are any terms associated this may be a
                  // cv error rather then a dbxref one...
                  $has_terms = FALSE;
                  $attached_terms = [];
                  foreach ($query->execute() as $existing_cvterm) {
                    $has_terms = TRUE;

                    // Check if the attached term name matches ours.
                    if ($existing_cvterm->name == $term_info['name']) {
                      $has_terms = TRUE;
                    }
                    // Otherwise just keep track of them.
                    else {
                      $attached_terms[] = $existing_cvterm;
                    }
                  }

                  if ($has_terms) {

                    // @todo report term with wrong cv.

                    // @todo report possible terms which maybe should be attached to this dbxref.

                  }
                  else {
                    $summary_dbxref = sprintf($red, ' X (' . $existing_dbxref->dbxref_id . ')');

                    // @todo check here if we have an existing term above.
                    // maybe it's just missing a connection?

                    // ERROR:
                    $problems['error']['dbxref-not-attached'][$existing_dbxref->dbxref_id][] = [
                      'dbxref-id' => $existing_dbxref->dbxref_id,
                      'dbxref-accession' => $existing_dbxref->accession,
                      'dbxref-dbname' => $existing_dbxref->db_name,
                      'term' => $summary_term,
                    ];
                    // We don't have a solution for this (grimaces).
                  }
                }
              } else {
                // Otherwise it's just missing which is not a concern really.
                $summary_dbxref = ' - ';
                $summary_dbs[$term_db] = ' - ';
              }
            }

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
    $show_errors = $this->io()->confirm('Would you like to see more details about the errors?');
    if ($show_errors) {

      // missing-db
      if (array_key_exists('missing-db', $problems['error'])) {
        $this->io()->section('YAML Issues: Missing ID Space definitions.');
        $num_detected = count($problems['error']['missing-db']);
        $this->output()->writeln("We have detected $num_detected problems with your YAML file. You will want to contact the developers to let them know the following output:");
        $list = [];
        foreach($problems['error']['missing-db'] as $idspace => $terms_with_issues) {
          foreach ($terms_with_issues as $prob_deets) {
            $list[] = sprintf(
              "Term %s: Missing '%s' ID Space from defined ID Spaces for '%s' vocabulary. Defined ID Spaces include %s.",
              $prob_deets['term'],
              $prob_deets['missing-db-name'],
              $prob_deets['vocab'],
              implode(', ', array_keys($prob_deets['defined-dbs'])
            ));
          }
        }
        $this->io()->listing($list);
      }

      // dbxref
      if (array_key_exists('dbxref', $problems['error'])) {
        $this->io()->section('Term Accessions with unexpected values.');
        $num_detected = count($problems['error']['dbxref']);
        $this->output()->writeln("We have detected $num_detected serious problems with existing dbxref. These are highlighted red and show the existing id above.");
        foreach($problems['error']['dbxref'] as $dbxref_id => $specific_issues) {
          $list = [];
          $term = NULL;
          foreach ($specific_issues as $prob_deets) {
            $term = $prob_deets['term'];
            $list[] = sprintf(
              "%s\n     Your chado instance has '%s' but Tripal expects '%s'",
              $prob_deets['message'],
              $prob_deets['YOURS'],
              $prob_deets['EXPECTED']
            );
          }
          $this->output()->writeln('Term: ' . $term);
          $this->io()->listing($list);
        }
      }

      // missing-dbxref
      if (array_key_exists('missing-dbxref', $problems['error'])) {
        $this->io()->section('Referential Integrity Issues.');
        $num_detected = count($problems['error']['dbxref']);
        $this->output()->writeln("We have detected $num_detected serious problems with referential integrity!");
        $this->output()->writeln("More specifically, the following existing cvterms have a dbxref_id indicated but that record does not exist in your chado instance.");
        $this->output()->writeln("This should not be possible due to database contraints so your Chado is Very Broken!");
        $list = [];
        foreach($problems['error']['missing-dbxref'] as $cvterm_id => $specific_issues) {
          $term = NULL;
          foreach ($specific_issues as $prob_deets) {
            $term = $prob_deets['term'];
            $list[] = sprintf(
              'Term %s (id: %s) has a dbxref_id of %s (missing in dbxref table)',
              $term,
              $cvterm_id,
              $prob_deets['dbxref_id']
            );
          }
        }
        $this->io()->listing($list);
      }

      // dbxref-not-attached
      if (array_key_exists('dbxref-not-attached', $problems['error'])) {
        $this->io()->section('Term Accessions not attached to their term.');
        $num_detected = count($problems['error']['dbxref-not-attached']);
        $this->output()->writeln("We have detected $num_detected database references that are not attached to the term we expected them to be.");
        $list = [];
        foreach ($problems['error']['dbxref-not-attached'] as $dbxref_id => $specific_issues) {
          foreach ($specific_issues as $prob_deets) {
            $list[] = sprintf(
              "'%s:%s' (%s) is attached to term '%s' (%s) but Tripal expected it to be attached to term '%s'.",
              $prob_deets['dbxref-dbname'],
              $prob_deets['dbxref-accession'],
              $prob_deets['dbxref-id'],
              $prob_deets['cvterm-name'],
              $prob_deets['cvterm-id'],
              $prob_deets['term'],
            );
          }
        }
        $this->io()->listing($list);
      }
    }
  }
}
