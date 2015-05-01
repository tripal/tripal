<?php

function tripal_core_extensions_form($form, &$form_state = NULL) {
  $tab = $_GET['tab'];

  $form['extensions'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => $tab,
  );

  // Get the RSS feed XML from the tripa.info website.
  $content = file_get_contents("http://tripal.info/rss/extensions.xml");
  $xml = new SimpleXmlElement($content);
  $namespace = "http://tripal.info/rss/extensions/";

  // Parse the items into an array indexed by category and compatible versions.
  $items = array();
  $types = array();
  $categories = array();
  foreach ($xml->channel->item as $item) {
    $type = (string) $item->category;
    // In order to get fields in the 'tripal_extension' name space we must
    // pass in the $namespace to the children function.  We first get the
    // Tripal versions, then the chado versions and organize the elements
    // accordintly.
    $tvs = preg_split('/, /', (string) $item->children($namespace)->tripal_version);
    foreach($tvs as $tv) {
      $cvs = preg_split('/, /', (string) $item->children($namespace)->chado_version);
      foreach($cvs as $cv) {
        // Index the items by category, tripal version and chado version
        $items[$tv][$cv][$type][] = $item;
        if (!in_array($type, $types)) {
          $types[] = $type;
        }
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

  $form['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('This page will help you find extensions that are available
      for Tripal.  Select an extension type from the vertical tabs to see
      the each type of extension.')
  );

  // Build the fieldsets for each category
  sort($types);
  $type_ids = array();
  foreach ($types as $type) {
    $type_id = preg_replace('/[^\w]/','_', strtolower($type));
    $type_ids[$type] = $type_id;
    $form[$type] = array(
      '#id'          => $type_id,
      '#type'        => 'fieldset',
      '#title'       => $type . 's',
      '#collapsed'   => TRUE,
      '#collapsible' => TRUE,
      '#group'       => 'extensions',
    );
  }

  // Iterate through the compatible extensions. We will add a pager for
  // each type of extension, and display only those that should appear
  // on the page.
  $type_index = 0;
  foreach ($items[$tripal_version][$chado_version] as $type => $items) {
    // Initialize pager and gets current page
    $num_per_page = 5;
    $total_items = count($items);
    $page = pager_default_initialize($total_items, $num_per_page, $type_index);

    // Gets first record and last record to show
    $start = ($page) * $num_per_page;
    $end = ($start + $num_per_page < $total_items)? $start + $num_per_page : $total_items;
    // Iterate through each of the elements and add them to the form if
    // they are within the page
    $item_index = 0;
    foreach ($items as $item) {
      // Skip items that aren't on our current page.
      if ($item_index < $start or $item_index >= $end) {
        $item_index++;
        continue;
      }
      // The unique guid is used to keep track of each extension in the form.
      $guid = (string) $item->guid;

      // If this is an extension module then there will be a home page for it
      $home_page = '';
      if ((string) $item->children($namespace)->home_page) {
        $home_page = "<strong>Project Home: </strong>" . (string) $item->children($namespace)->home_page . "</br>";
      }

      // If the user click's the button to import the extension then we
      // need the item in the submit function so we can process the import.
      $form[$type]['item-' . $guid] = array(
        '#type' => 'value',
        '#value' => $item,
      );

      // Create the form elements that we'll later theme into tables.
      $form[$type][$guid]['header'] = array(
        '#markup' => l((string) $item->title, $item->link),
      );
      $form[$type][$guid]['details'] = array(
        '#markup' => "" .
          "<strong>Type:</strong> " . $type . "</br>" .
          "<strong>Categories: </strong>" . (string) $item->children($namespace)->categories . "</br>" .
          "<strong>Authors: </strong>" . (string) $item->children($namespace)->authors . "</br>" .
          "<strong>Chado compatible versions: </strong>" . (string) $item->children($namespace)->chado_version . "</br>" .
          "<strong>Chado compatible versions: </strong>" . (string) $item->children($namespace)->tripal_version . "</br>" .
          $home_page .
          "<p>" . (string) $item->description . "</p>",
      );
      // Add an import button to each of types that can support import.
      switch ($type) {
        case 'Bulk Loader Template':
          $form[$type][$guid]['import'] = array(
            '#type' => 'submit',
            '#value' => "Import Loader",
            '#name' => "import-" . $guid,
          );
          break;
        case 'Materialized View':
          $form[$type][$guid]['import'] = array(
            '#type' => 'submit',
            '#value' => "Import MView",
            '#name' => "import-" . $guid,
          );
          break;
        case 'Extension Module':
          $form[$type][$guid]['import'] = array(
            '#type' => 'submit',
            '#value' => "Download Module",
            '#name' => "import-" . $guid,
          );
          break;
        default:
          break;
      }
      $form[$type][$guid]['#theme'] = 'tripal_core_extensions_form_tables';
      $item_index++;
    }

    // Now create and theme the pager.
    $pager = array(
      'tags' => array(),
      'element' => $type_index,
      'parameters' => array(
        'tab' => $type_ids[$type],
      ),
      'quantity' => $num_per_page,
    );

    // because this may be an ajax callback, the theme_pager will set the URL to be
    // "system/ajax", so we need to reset that
    global $base_path;
    $pager = str_replace($base_path . "system/ajax", "", $pager) ;

    $form[$type]['pager'] = array(
      '#type' => 'item',
      '#markup' => theme('pager', $pager),
    );
    $type_index++;
  }
  return $form;
}


/**
 * Process the import buttons.
 *
 * @param $form
 * @param $form_state
 */
function tripal_core_extensions_form_submit($form, &$form_state) {
  // get the guid
  $clicked_button = $form_state['clicked_button']['#name'];
  $guid = preg_replace('/^import-(\d+)$/', "$1", $clicked_button);
  if ($guid) {
    $namespace = "http://tripal.info/rss/extensions/";
    $item = $form_state['values']['item-' . $guid];
    $type = $item->category;
    switch ($type) {
      case 'Bulk Loader Template':
        break;
      case 'Materialized View':
        $modulename = 'tripal_core';
        $mview_name = (string) $item->children($namespace)->mview_name;
        $mview_schema = (string) $item->children($namespace)->mview_schema;
        $mview_sql = (string) $item->children($namespace)->mview_sql;

        // Validate the contents of the schema array.
        // TODO: security issue!! Before using 'eval' on this string
        // we should make sure the array is valid and there is no hidden
        // PHP code.
        $schema_array = array();
        $success = eval("\$schema_array = $mview_schema;");
        $error = chado_validate_custom_table_schema($schema_array);
        if ($error) {
          drupal_set_message("Cannot import Materialized View.  $error");
        }
        tripal_add_mview($mview_name, $modulename, $schema_array, $mview_sql);
        break;
      case 'Extension Module':

        break;
      default:
        break;
    }
  }
}


/**
 * The theme function for rendering each element's table.
 *
 * @param $variables
 */
function theme_tripal_core_extensions_form_tables($variables) {
   $element = $variables['element'];
   $headers = array(
     array(
       'data' => drupal_render($element['header']),
       'colspan' => 2,
     )
   );
   $button = array_key_exists('import', $element) ? drupal_render($element['import']) : '&nbsp;';
   $rows = array(
     array(
       array(
         'data' => drupal_render($element['details']),
       ),
       array(
         'data' => $button,
         'width' => '5%',
         'align' => 'right',
       ),
     ),
   );

   $table = array(
     'header' => $headers,
     'rows' => $rows,
     'attributes' => array(),
     'sticky' => FALSE,
     'caption' => '',
     'colgroups' => array(),
     'empty' => '',
   );

  return theme_table($table);
}