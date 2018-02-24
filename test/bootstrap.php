<?php
test_suite_read_and_set_environment_variables();
$drupal_root = getenv('DRUPAL_ROOT');
define('DRUPAL_ROOT', $drupal_root ?: __DIR__.'/../../../../..');
require_once DRUPAL_ROOT.'/includes/bootstrap.inc';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$current_dir = getcwd();
chdir(DRUPAL_ROOT);
// Bootstrap Drupal.
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
chdir($current_dir);
/**
 * @throws \Exception
 */
function test_suite_read_and_set_environment_variables() {
  $filename = __DIR__.'/.env';
  if(file_exists($filename)) {
    $file = fopen($filename, 'r');
    while ($line = str_replace("\n", '', fgets($file))) {
      // break line into key value
      $env = explode('=', $line);
      if(count($env) === 2) {
        putenv($line);
      } else {
        throw new Exception('Invalid environment line: ' . $line);
      }
    }
    fclose($file);
  }
}