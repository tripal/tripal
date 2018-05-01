<?php

/**
 * Implements hook_ctools_plugin_api().
 */
function tripal_ds_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "ds" && $api == "ds") {
    return ["version" => "1"];
  }
}
