<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 *
 * @upgrade
 *
 */
use Drupal\Core\Database\Database;

/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_load_drush_submit($version) {
  // $args = [$version];
  // $includes = [mmodule_load_include('inc', 'tripal_chado', 'src/LegacyIncludes/tripal_chado.install')];
  // @upgrade tripal_add_job($version, 'tripal_chado',
  //  'tripal_chado_install_chado', $args, 1, 10, $includes);
  tripal_chado_install_chado($action);
}

/**
 * Install Chado Schema
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_chado($action, TripalJob $job = NULL) {

  $vsql = "
    INSERT INTO chado.chadoprop (type_id, value)
      VALUES (
       (SELECT cvterm_id
        FROM chado.cvterm CVT
          INNER JOIN chado.cv CV on CVT.cv_id = CV.cv_id
         WHERE CV.name = 'chado_properties' AND CVT.name = 'version'),
       :version)
  ";

  $vusql = "
    UPDATE chado.chadoprop
    SET value = :version
    WHERE type_id = (SELECT cvterm_id
        FROM chado.cvterm CVT
          INNER JOIN chado.cv CV on CVT.cv_id = CV.cv_id
         WHERE CV.name = 'chado_properties' AND CVT.name = 'version')
  ";

  try {
    if ($action == 'Install Chado v1.3') {
      tripal_chado_install_chado_1_3();
      \Drupal::database()->query($vsql, [':version' => '1.3']);
    }
  } catch (Exception $e) {
    tripal_chado_install_done();
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error($e->getMessage());
    return FALSE;
  }
  return TRUE;


}

/**
 * Installs Chado v1.3.
 */
function tripal_chado_install_chado_1_3(TripalJob $job = NULL) {
  // Get the path to the schema and init SQL files.
  $module_path = drupal_get_path('module', 'tripal_chado');
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/initialize-1.3.sql';

  // Erase the Chado schema if it exists and perform the install.
  if (tripal_chado_reset_chado_schema()) {

    // Since the schema SQL file is large we have split it into multiple chunks.
    // This loop will load each chunk...
    $failed = FALSE;
    for ($i=1; $i<=41; $i++) {
      $schema_file = $module_path . '/chado_schema/parts-v1.3/default_schema-1.3.part' . $i . '.sql';
      $success = tripal_chado_install_sql($schema_file);
      if ($success) {
        // @upgrade tripal_report_error().
        print "\tImport of part $i of 41 Successful!\n";
      }
      else {
        $failed = TRUE;
        // @upgrade tripal_report_error().
        \Drupal::logger('tripal_chado')->error(
          "Schema installation part $i of 41 Failed...");
          break;
      }
    }
    if ($failed) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.3 (Step 1 of 2) Successful.\nInstallation Complete\n");
    }


    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.3 (Step 2 of 2) Successful.\nInstallation Complete\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "ERROR: cannot install chado.  Please check database permissions");
  }
}

/**
 * Reset the Chado Schema
 * This drops the current chado and chado-related schema and re-creates it
 *
 * @ingroup tripal_chado
 */
function tripal_chado_reset_chado_schema() {
  $connection = Database::getConnection();

  // determine the schema name.
  // @upgrade $chado_schema = chado_get_schema_name('chado');
  $chado_schema = 'chado';
  $chado_dot = $chado_schema . '.';

  // drop current chado and chado-related schema
  if (chado_dbschema_exists('genetic_code')) {
    // @upgrade tripal_report_error().
    print "Dropping existing 'genetic_code' schema\n";
    $connection->query("drop schema genetic_code cascade");
  }
  if (chado_dbschema_exists('so')) {
    // @upgrade tripal_report_error().
    print "Dropping existing 'so' schema\n";
    $connection->query("drop schema so cascade");
  }
  if (chado_dbschema_exists('frange')) {
    // @upgrade tripal_report_error().
    print "Dropping existing 'frange' schema\n";
    $connection->query("drop schema frange cascade");
  }
  if (chado_dbschema_exists($chado_schema)) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->info(
      "Dropping existing Chado ('$chado_schema') schema\n");
    $connection->query("drop schema $chado_schema cascade");
  }

  // create the new chado schema
  // @upgrade tripal_report_error().
  \Drupal::logger('tripal_chado')->info(
    "Creating '$chado_schema' schema\n");
  $connection->query("CREATE SCHEMA $chado_schema");
  if (chado_dbschema_exists('chado')) {
    // before creating the plpgsql language let's check to make sure
    // it doesn't already exists
    $sql = "SELECT COUNT(*) FROM pg_language WHERE lanname = 'plpgsql'";
    $results = $connection->query($sql);
    $count = $results->fetchObject();
    if (!$count or $count->count == 0) {
      $connection->query("create language plpgsql");
    }
    return TRUE;
  }

  return FALSE;
}

/**
 * Execute the provided SQL
 *
 * @param $sql_file
 *   Contains SQL statements to be executed
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_sql($sql_file) {
  $connection = Database::getConnection();

  // @upgrade $chado_local = chado_dbschema_exists('chado');
  $chado_local = TRUE;

  // determine the schema name.
  // @upgrade $chado_schema = chado_get_schema_name('chado');
  $chado_schema = 'chado';

  // @upgrade tripal_report_error().
  print "Loading $sql_file...\n";

  // Get the default database and chado schema.
  $databases = $connection->getConnectionOptions();
  $dsn = sprintf( 'dbname=%s host=%s port=%s user=%s password=%s',
    $databases['database'],
    $databases['host'],
    $databases['port'],
    $databases['username'],
    $databases['password'] );

  // Open a PHP connection to the database
  // since Drupal restricts us to a single statement per exec.
  $pgconnection = pg_connect($dsn);
  if (!$pgconnection) {
    \Drupal::logger('tripal_chado')->error(
      "Unable to connect to database using '$dsn' connection string.\n");

    pg_close($pgconnection);
    return FALSE;
  }
  $sql = file_get_contents($sql_file);
  $result = pg_query($pgconnection, $sql);
  if (!$result) {
    \Drupal::logger('tripal_chado')->error(
      "Unable to execute query block.\n");

    pg_close($pgconnection);
    return FALSE;
  }

  tripal_chado_install_done();
  return TRUE;
}

/**
 * Finish the Chado Schema Installation
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_done() {
  $connection = Database::getConnection();

  // @upgrade $drupal_schema = chado_get_schema_name('drupal');
  $drupal_schema = 'public';
  $connection->query("set search_path to $drupal_schema");
}
