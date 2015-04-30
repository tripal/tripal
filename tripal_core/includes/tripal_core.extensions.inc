<?php

function tripal_core_extensions_form($form, &$form_state = NULL) {

  // Get the RSS feed XML from the tripa.info website.
  $content = file_get_contents("http://tripal.info/rss/extensions.xml");
  $xml = new SimpleXmlElement($content);
  $namespace = "http://tripal.info/rss/extensions/";

  // Parse the items into an array indexed by category and compatible versions.
  $items = array();
  foreach ($xml->channel->item as $item) {
    $category = (string) $item->category;
    // In order to get fields in the 'tripal_extension' name space we must
    // pass in the $namespace to the children function.  We first get the
    // Tripal versions, then the chado versions and organize the elements
    // accordintly.
    $tvs = preg_split('/, /', (string) $item->children($namespace)->tripal_version);
    foreach($tvs as $tv) {
      $cvs = preg_split('/, /', (string) $item->children($namespace)->chado_version);
      foreach($cvs as $cv) {
        // Index the items by category, tripal version and chado version
        $items[$tv][$cv][$category][] = $item;
      }
    }
  }

  // Get the Chado version and convert to the expected format
  $chado_version = chado_get_version(TRUE);
  $chado_version = preg_replace('/^(\d\.\d).*$/', "v$1x", $chado_version);

  // Get the Tripal version. This is the version set in the tripal_core.info
  $info = system_get_info('module', 'tripal_core');
  $tripal_version = $info['version'];
  $tripal_version = preg_replace('/^.*?-(\d\.\d+).*$/', "v$1", $tripal_version);

  $form['type_select'] = array(
    '#type' => 'select',
    '#options' => array(
      'Bulk Loader Templates' => 'Bulk Loader Templates',
      'Materialized View' => 'Materialized View',
      'Extension Module' => 'Extension Module',
    ),
    '#default_value' => 'Extension Module',
  );

  // Iterate through the compatible extensions
  foreach ($items[$tripal_version][$chado_version] as $category => $items) {
    foreach ($items as $item) {
      $guid = (string) $item->guid;
      $markup = '';
      $markup .= "<h3>" . (string) $item->title . "</h3>";
      $markup .= "<p>" . $category . "</p>";
      $markup .= "<p>" . (string) $item->description . "</p>";
      $form[$category][$guid] = array(
        '#type' => 'item',
        '#markup' => $markup,
      );
      switch ($category) {
        case 'Bulk Loader Template':
          $form[$category][$guid]['add'] = array(
            '#type' => 'submit',
            '#title' => 'Import'
          );
          break;
        case 'Materialized View':
          break;
        case 'Extension Module':
           break;
        default:
          break;
      }
    }
  }

  return $form;
}

function tripal_core_extensions_form_validate($form, &$form_state) {

}

function tripal_core_extensions_form_submit($form, &$form_state) {

}