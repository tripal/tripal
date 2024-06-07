<?php
namespace Drupal\tripal_chado\Commands;

use Drush\Commands\DrushCommands;

/**
 * Drush commands
 */
class ChadoManageCommands extends DrushCommands {

  /**
   * Install the Chado schema.
   *
   * @command tripal-chado:install-chado
   * @aliases trp-install-chado
   * @options schema-name
   *   The name of the schema to install chado in.
   * @options chado-version
   *   The version of chado to install. Currently only 1.3 is supported.
   * @usage drush trp-install-chado --schema-name='teapot' --version=1.3
   *   Installs chado 1.3 in a schema named "teapot".
   */
  public function installChado($options = ['schema-name' => 'chado', 'chado-version' => 1.3]) {

    $this->output()->writeln('Installing chado version ' . $options['chado-version'] . ' in a schema named "' . $options['schema-name']. '"');

    $installer = \Drupal::service('tripal_chado.installer');
    $installer->setParameters([
      'output_schemas' => [  $options['schema-name']  ],
      'version' => $options['chado-version'],
    ]);
    if ($installer->performTask()) {
      $this->output()->writeln(dt('<info>[Success]</info> Chado was successfully installed.'));
    }
    else {
      throw new \Exception(dt(
        'Unable to install chado {version} in {schema}',
        [
          'schema' => $options['schema-name'],
          'version' => $options['chado-version'],
        ]
      ));
    }
  }

  /**
   * Drops the Chado schema.
   *
   * @command tripal-chado:drop-chado
   * @aliases trp-drop-chado
   * @options schema-name
   *   The name of the schema to drop.
   * @usage drush trp-drop-chado --schema-name='teapot'
   *   Removes the chado schema named "teapot".
   */
  public function dropChado($options = ['schema-name' => 'chado']) {

    $remover = \Drupal::service('tripal_chado.remover');
    $remover->setParameters([
      'output_schemas' => [$options['schema-name']],
    ]);
    if ($remover->performTask()) {
      $this->output()->writeln('<info>[Success]</info> Chado was successfully dropped.');
    }
    else {
      throw new \Exception(dt(
        'Unable to drop chado in {schema}',
        [
          'schema' => $options['schema-name'],
        ]
      ));
    }

  }

  /**
   * Prepare the Tripal Chado system.
   *
   * @command tripal-chado:prepare
   * @aliases trp-prep-chado
   * @options schema-name
   *   The name of the chado schema to prepare. Only a single chado schema
   *   should be prepared with Tripal and this will become the default chado schema.
   * @usage drush trp-prep-chado --schema-name="chado"
   *   Prepare the Tripal Chado system and set the schema named "chado" as the
   *   default Chado instance to use with Tripal.
   */
  public function prepareChado($options = ['schema-name' => 'chado']) {

    $this->output()->writeln('Preparing Drupal ("public") + Chado ("' . $options['schema-name'] . '")...');

    $preparer = \Drupal::service('tripal_chado.preparer');
    $preparer->setParameters([
      'output_schemas' => [$options['schema-name']],
    ]);
    if ($preparer->performTask()) {
      $this->output()->writeln('<info>[Success]</info> Preparation complete.');
    }
    else {
      throw new \Exception(dt(
        'Unable to prepare Drupal + Chado in @schema',
        [
          '@schema' => $options['schema-name'],
        ]
      ));
    }
  }

  /**
   * Set-up the Tripal Chado test environment.
   *
   * @command tripal-chado:setup-tests
   * @aliases trp-prep-tests
   * @usage drush trp-prep-tests
   *   Sets up the standard Tripal Chado test environment.
   */
  public function setupTests() {

    $this->output()->writeln('There is no longer any need to prepare the chado test environment.');
  }

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
  public function tripalImportContentTypes($options = ['chado_schema' => NULL]) {
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

    $this->output()->writeln('');

    $chado = \Drupal::service('tripal_chado.database');
    $chado->setSchemaName($options['chado_schema']);

    $this->output()->writeln('Using the Chado Content Terms YAML specification to determine what Tripal expects.');

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
          $summary_comments = [];

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
              // $this->io()->warning('The cv.definition for this vocabulary is doesn\'t match what is expected.');
              // $sql_fix = "UPDATE {1:cv} SET definition='" . $vocab_info['label'] . "' WHERE cv_id=$summary_cv;";
              // $this->io()->note($sql_fix);
            }
          }
          else {
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
              $summary_dbs[$idspace_info['name'] ] = $existing_db->db_id;
              $defined_ispaces[$idspace_info['name'] ] = $existing_db->db_id;

              // Now check the db description, url prefix and url match what we expect and warn if not.
              if ($existing_db->description != $idspace_info['description']) {

                $summary_dbs[ $idspace_info['name'] ] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // $summary_comments[] = 'db.description mismatched';
                //$sql_fix = "UPDATE {1:db} SET description='" . $idspace_info['description'] . "' WHERE db_id=" . $existing_db->db_id . ";";
                //$this->io()->note($sql_fix);
              }
              if ($existing_db->urlprefix != $idspace_info['urlPrefix']) {

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // $summary_comments[] = 'db.urlprefix mismatched';
                // $this->io()->warning('The db.urlprefix for this ID Space is expected to be "' . $idspace_info['urlPrefix'] . '" but was actually "' . $existing_db->urlprefix . '".');
              }
              if ($existing_db->url != $vocab_info['url']) {

                $summary_dbs[$idspace_info['name']] = sprintf($yellow, $existing_db->db_id);

                // WARNING:
                // $summary_comments[] = 'db.url mismatched';
                //$this->io()->warning('The db.url for this ID Space is expected to be "' . $vocab_info['url'] . '" but was actually "' . $existing_db->url . '".');
              }

            } else {
              $summary_dbs[ $idspace_info['name'] ] = ' - ';
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
              // $summary_comments[] = 'ID space not defined in YAML';
              //$this->io()->error('The term id includes an id space ("' . $term_db . '") that was not defined in the YAML for this vocabulary.');
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
              if (array_key_exists('description', $term_info) && ($existing_cvterm->definition != $term_info['description'])) {

                $summary_cvterm = sprintf($yellow, $existing_cvterm->cvterm_id);

                // WARNING:
                // $summary_comments[] = 'cvterm.definition mismatched';
                // $this->io()->warning('The cvterm.definition for this Term is expected to be "' . $term_info['description'] . '" but was actually "' . $existing_cvterm->definition . '".');
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
                  // $summary_comments[] = 'existing dbxref has different db';
                  // $this->io()->error('The dbxref.db_id>db.name for this term is expected to be "' . $term_db . '" but was actually "' . $existing_dbxref->db_name . '".');
                }
                // Check the term accession matches the dbxref.accession
                if ($existing_dbxref->accession != $term_accession) {

                  $summary_dbxref = sprintf($red, $existing_dbxref->dbxref_id);

                  // ERROR:
                  // $summary_comments[] = 'existing dbxref has different accession';
                  // $this->io()->error('The dbxref.accession for this term is expected to be "' . $term_accession . '" but was actually "' . $existing_dbxref->accession . '".');
                }
              }
              else {
                $summary_dbxref = sprintf($red, $existing_cvterm->dbxref_id);

                // ERROR:
                // $summary_comments[] = 'dbxref dependency broken';
                $this->io()->error('NO DBXREF Record associated with the cvterm "' . $existing_cvterm->name . '" (' . $existing_cvterm->cv_id . ')! This should not be possible due to database contraints so your Chado is Very Broken!');
              }
            }
            else {
              $summary_cvterm = ' - ';
              // Check if the accession exists as defined in case there is a
              // mismatch between term name and accession.
              $query = $chado->select('1:dbxref', 'dbx')
              ->fields('dbx', ['dbxref_id', 'accession'])
              ->condition('dbx.accession', $term_accession);
              $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
              $query->addField('db', 'name', 'db_name');
              $query->condition('db.name', $term_db);
              $query->join('1:cvterm', 'cvt', 'cvt.dbxref_id = dbx.dbxref_id');
              $query->addField('cvt', 'name', 'cvterm_name');
              $query->addField('cvt', 'cvterm_id', 'cvterm_id');
              $existing_dbxref = $query->execute()->fetchObject();

              if ($existing_dbxref) {

                $summary_dbxref = sprintf($red, ' X ('. $existing_dbxref->dbxref_id . ')');

                // ERROR:
                // $summary_comments[] = 'dbxref not attached to term';
                // $this->io()->error('The accession of the term defined as "' . $term_info['name'] . '" ("' . $term_info['id'] . '") exists in the dbxref table (' . $existing_dbxref->dbxref_id . ') but not in the cvterm table. Instead this dbxref record is connected to a cvterm with the name "' . $existing_dbxref->cvterm_name . '" (' . $existing_dbxref->cvterm_id . ').');

              }
              else {
                // Otherwise it's just missing which is not a concern really.
                $summary_dbxref = ' - ';
                $summary_dbs[ $term_db ] = ' - ';
              }
            }

            // Now add the details of what we found for this term to the summary table.
            $summary_rows[] = [
              'term' => $summary_term,
              'cv' => $summary_cv,
              'db' => $summary_dbs[ $term_db ],
              'cvterm' => $summary_cvterm,
              'dbxref' => $summary_dbxref,
            ];
          }
        }
      }
    }

    // Finally tell the user the summary state of things.
    $this->output()->writeln('The following table summarizes the terms.');
    $this->io()->table($summary_headers, $summary_rows);
    $this->output()->writeln('Legend:');
    $this->output()->writeln(sprintf($yellow, ' YELLOW ') . ' Indicates there are some mismatches between the existing version and what we expected but it\'s minor.');
    $this->output()->writeln(sprintf($red, '  RED   ') . ' Indicates there is a serious mismatch which will cause the prepare to fail on this chado instance.');
    $this->output()->writeln('    -      Indicates this one is missing but that is not a concern as it will be added when you run prepare.');
    $this->output()->writeln('');
  }
}
