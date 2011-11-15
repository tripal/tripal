<?php
//
// Copyright 2009 Clemson University
//

/* 

This script must be run at the base directory level of the drupal installation 
in order to pick up all necessary dependencies 

*/

require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

jlibrary_feature_set_taxonomy($argv[1]);

?>
