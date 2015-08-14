<?php

function tripal_core_extensions_form($form, &$form_state = NULL) {
  // Get the RSS feed XML from the tripa.info website and save it to
  // a temp file so that we don't have to keep pulling the XML
  // everythime the page is loaded. If the temp file is older than 1 hours
  // then we'll pull it again. The number of seconds in an hour is 3600
  $tmp_file = sys_get_temp_dir() . '/tripal_rss_extensions.xml';
  if (!file_exists($tmp_file) or time() - filemtime($tmp_file) > 3600) {
    $content = file_get_contents("http://tripal.info/rss/extensions.xml");
    file_put_contents($tmp_file, $content);
  }
  else {
    $content = file_get_contents($tmp_file);
  }
  $xml = new SimpleXmlElement($content);
  $namespace = "http://tripal.info/rss/extensions/";


  $tab = array_key_exists('tab', $_GET) ? $_GET['tab'] : '';

  // Get any filters by categories.
  $filters = array();
  if (array_key_exists('values', $form_state)) {
    foreach ($form_state['values'] as $key => $value) {
      // Get the category to be filtered on.
      $matches = array();
      if (preg_match('/^categories-(.*?)$/', $key, $matches)) {
        if ($value == 'any') {
          continue;
        }
        $filters[$matches[1]] = $value;
      }
    }
  }

  // Parse the items into an array indexed by type and compatible versions.
  $extensions = array();
  $types = array();
  $type_ids = array();
  $categories = array();
  $cvs = array();
  $tvs = array();
  foreach ($xml->channel->item as $extension) {
    // Get the type of extension, convert that into a machine-readable name,
    // and build the array of types.
    $type = (string) $extension->category;
    $type_id = preg_replace('/[^\w]/','_', strtolower($type));
    $type_ids[$type] = $type_id;
    if (!in_array($type_id, $types)) {
      $types[$type] = 1;
    }

    // Get the categories list for this item.
    $cats = preg_split('/, /', (string) $extension->children($namespace)->categories);

    // Get the guid for this extension
    $guid = (string) $extension->guid;

    // In order to get fields in the 'tripal_extension' name space we must
    // pass in the $namespace to the children function.  We first get the
    // Tripal versions, then the chado versions and organize the elements
    // accordintly.
    $tvs_temp = preg_split('/, /', (string) $extension->children($namespace)->tripal_version);
    foreach($tvs_temp as $tv) {
      $tvs[$tv] = 1;
      $cvs_temp = preg_split('/, /', (string) $extension->children($namespace)->chado_version);
      foreach($cvs_temp as $cv) {
        $cvs[$cv] = 1;

        // Keep track of the categories this item has been assigned.
        foreach ($cats as $cat) {
          $categories[$tv][$cv][$type][$cat] = 1;
          $categories['any'][$cv][$type][$cat] = 1;
          $categories[$tv]['any'][$type][$cat] = 1;
          $categories['any']['any'][$type][$cat] = 1;
        }

        // If there are filters then only include extensions that match the filters.
        if (array_key_exists($type_id, $filters) and !in_array($filters[$type_id], $cats)) {
          continue;
        }

        // Index the items by type, tripal version and chado version.
        $item = array();
        foreach ($extension->children() as $child) {
          $item[$child->getName()] = (string) $child;
        }
        foreach ($extension->children($namespace) as $child) {
          $item[$namespace][$child->getName()] = (string) $child;
        }
        $extensions[$tv][$cv][$type][$guid] = $item;
        $extensions['any'][$cv][$type][$guid] = $item;
        $extensions[$tv]['any'][$type][$guid] = $item;
        $extensions['any']['any'][$type][$guid] = $item;
      }
    }
  }

  // Convert the arrays from an associative array into a normal array, and sort.
  $types = array_keys($types);
  sort($types);
  $cvs = array_keys($cvs);
  sort($cvs);
  $tvs = array_keys($tvs);
  sort($tvs);

  // Get the Chado version and convert to the expected format
  $chado_version = chado_get_version(TRUE);
  $chado_version = preg_replace('/^(\d\.\d).*$/', "v$1x", $chado_version);
  $my_chado_version = $chado_version;
  // The default value can come from the pager links (thus via $_GET) or
  // via ajax call (thus via $form_state).
  if (array_key_exists('cv', $_GET)) {
    $chado_version = $_GET['cv'];
  }
  if (array_key_exists('values', $form_state) and array_key_exists('cv', $form_state['values'])) {
    $chado_version = $form_state['values']['cv'];
  }

  // Get the Tripal version. This is the version set in the tripal_core.info
  $info = system_get_info('module', 'tripal_core');
  $tripal_version = $info['version'];
  $tripal_version = preg_replace('/^.*?-(\d\.\d+).*$/', "v$1", $tripal_version);
  $my_tripal_version = $tripal_version;
  if (array_key_exists('tv', $_GET)) {
    $tripal_version = $_GET['tv'];
  }
  if (array_key_exists('values', $form_state) and array_key_exists('tv', $form_state['values'])) {
    $tripal_version = $form_state['values']['tv'];
  }

  // Add the instructions.
  $form['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('This page will help you find extensions that are available
      for Tripal.  Select an extension type from the vertical tabs. The content
      of this page is constructed from an RSS feed provided by tripal.info.
      There may be no content if the tripal.info site is unavailable. The RSS
      feed will be cached for one hour.')
  );
  // Add the filters fieldset.
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => 'Filters',
    '#description' => t('You can filter which extensions are visible by
      changing the Tripal ahd Chado versions. By default only those
      extensions that are compatible with the currently installed Tripal
      and Chado verions are shown.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $summary_message = 'Currently showing extensions compatible with ';
  if ($tripal_version != 'any' and $chado_version != 'any') {
    $summary_message .= "<strong>Tripal $tripal_version</strong> and <strong>Chado $chado_version</strong>.";
  }
  elseif ($tripal_version == 'any' and $chado_version != 'any') {
    $summary_message .= "<strong>any Tripal</strong> version and <strong>Chado $chado_version</strong>.";
  }
  elseif ($tripal_version != 'any' and $chado_version == 'any') {
    $summary_message .= "<strong>Tripal $tripal_version</strong> and <strong>any Chado</strong> version.";
  }
  elseif ($tripal_version == 'any' and $chado_version == 'any') {
    $summary_message .= "<strong>any Tripal</strong> version and <strong>any Chado</strong> version.";
  }
  $form['filter_summary'] = array(
    '#type' => 'item',
    '#markup' => $summary_message,
  );

  // Add the Tripal version select box.
  $options = array();
  $options['any'] = '--Any--';
  foreach ($tvs as $tv) {
    $options[$tv] = $tv;
  }
  $form['filters']['tv'] = array(
    '#type' => 'select',
    '#title' => 'Tripal',
    '#options' => $options,
    '#default_value' => $tripal_version,
    '#ajax'  => array(
      'callback' => "tripal_core_extensions_form_ajax_callback",
      'wrapper'  => 'tripal_core_extensions',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
    '#prefix' => '<div style="float: left;">',
    '#suffix' => '</div>'
  );

  // Add the Chado version select box.
  $options = array();
  $options['any'] = '--Any--';
  foreach ($cvs as $cv) {
    $options[$cv] = $cv;
  }
  $form['filters']['cv'] = array(
    '#type' => 'select',
    '#title' => 'Chado',
    '#options' => $options,
    '#default_value' => $chado_version,
    '#ajax'  => array(
      'callback' => "tripal_core_extensions_form_ajax_callback",
      'wrapper'  => 'tripal_core_extensions',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
    '#prefix' => '<div style="float: left; padding-left: 10px">',
    '#suffix' => '</div>'
  );

  // Add the vertical tabs
  $form['extensions'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => $tab,
  );

  // Add the fieldsets for each type
  foreach ($types as $type) {
    $form[$type] = array(
      '#id'          => $type_ids[$type],
      '#type'        => 'fieldset',
      '#title'       => $type . 's',
      '#collapsed'   => TRUE,
      '#collapsible' => TRUE,
      '#group'       => 'extensions',
    );
  }

  // Iterate through all of the extensions and add them to the form.
  tripal_core_extension_form_add_extensions($form, $form_state,
    $extensions[$tripal_version][$chado_version], $categories, $tripal_version,
    $chado_version, $my_tripal_version, $my_chado_version, $type_ids,
    $namespace, $filters);

  foreach ($types as $type) {
    if (count(element_children($form[$type])) == 0) {
      $form[$type]['empty'] = array(
        '#type' => 'item',
        '#markup' => '<strong>There are no matching ' . strtolower($type) . '(s).</strong>',
      );
    }
  }

  $form['#prefix'] = '<div id="tripal_core_extensions">';
  $form['#suffix'] = '</div>';
  $form['#submit'][] = 'tripal_core_extensions_form_submit';
  return $form;
}

/**
 * Adds each extension to the form.
 *
 * This function exits to simplify the the tripal_core_extension_form()
 * function.
 */
function tripal_core_extension_form_add_extensions(&$form, $form_state, $extensions,
  $categories, $tripal_version, $chado_version, $my_tripal_version,
  $my_chado_version, $type_ids, $namespace, $filters) {

  // Iterate through the extensions. We will add a pager for
  // each type of extension, and display only those that should appear
  // on the page.
  $type_index = 0;
  foreach ($extensions as $type => $extensions) {
    $total_items = count($extensions);

    // Indicate how many matching extensions were found
    $form[$type]['total_found'] = array(
      '#type' => 'item',
      '#markup' => '<strong>Found ' . $total_items . ' matching ' . strtolower($type) . '(s)</strong>',
    );

    // Add the category select box;
    $cats = array_keys($categories[$tripal_version][$chado_version][$type]);
    sort($cats);
    $options = array();
    $options['any'] = '--Any--';
    foreach ($cats as $cat) {
      $options[$cat] = $cat;
    }
    // The default value can come from the pager links (thus via $_GET) or
    // via ajax call (thus via $form_state).
    $default_filter = '';
    if (array_key_exists('categories-' . $type_ids[$type], $_GET)) {
      $default_filter = $_GET['categories-' . $type_ids[$type]];
    }
    if (array_key_exists('values', $form_state) and array_key_exists('categories-' . $type_ids[$type], $form_state['values'])) {
      $default_filter = $form_state['values']['categories-' . $type_ids[$type]];
    }
    $form[$type]['filters']['categories-' . $type_ids[$type]] = array(
      '#type' => 'select',
      '#title' => 'Filter by Category',
      '#options' => $options,
      '#default_value' => $default_filter,
      '#ajax'  => array(
        'callback' => "tripal_core_extensions_form_ajax_callback",
        'wrapper'  => 'tripal_core_extensions',
        'effect'   => 'fade',
        'method'   => 'replace',
      ),
    );

    // Initialize pager and gets current page
    $num_per_page = 5;
    $page = pager_default_initialize($total_items, $num_per_page, $type_index);

    // Gets first record and last record to show
    $start = ($page) * $num_per_page;
    $end = ($start + $num_per_page < $total_items)? $start + $num_per_page : $total_items;
    // Iterate through each of the elements and add them to the form if
    // they are within the page
    $extension_index = 0;
    foreach ($extensions as $guid => $extension) {
      // Skip items that aren't on our current page.
      if ($extension_index < $start or $extension_index >= $end) {
        $extension_index++;
        continue;
      }

      $extension['title'] = trim($extension['title']);

      // If this is an extension module then there will be a home page for it
      $home_page = '';
      if (array_key_exists('home_page', $extension[$namespace])) {
        $home_page = "<strong>Project Home: </strong>" . $extension[$namespace]['home_page'] . "</br>";
      }

      // Determine if this extension is compatible with this site.
      $incompatible = '';
      $tvs_temp = preg_split('/, /', $extension[$namespace]['tripal_version']);
      $cvs_temp = preg_split('/, /', $extension[$namespace]['chado_version']);
      if (!in_array($my_tripal_version, $tvs_temp)) {
        $incompatible .= "<li>This extension is not compatible with this version of Tripal.</li>";
      }
      if (!in_array($my_chado_version, $cvs_temp)) {
        $incompatible .= "<li>This extension is not compatible with the installed Chado version.</li>";
      }
      $incompatible = t($incompatible);

      // Determine if this extension is already installed.
      $is_installed = '';
      switch ($type) {
        case 'Bulk Loader Template':
          $blk_id = db_select('tripal_bulk_loader_template' ,'tblt')
            ->fields('tblt', array('template_id'))
            ->condition('name', $extension['title'])
            ->execute()
            ->fetchField();
          if ($blk_id) {
            $is_installed = '<li>A bulk loader template with this name is already installed.</li>';
          }
          break;
        case 'Materialized View':
          $mview_id = tripal_get_mview_id($extension[$namespace]['mview_name']);
          if ($mview_id) {
            $is_installed = '<li>A materialized view with this name is already installed.</li>';
          }
          break;
        case 'Extension Module':
          if (array_key_exists('drupal_project', $extension[$namespace]) and
              module_exists($extension[$namespace]['drupal_project'])) {
            $is_installed = '<li>A module with this name is already installed.</li>';
          }
          break;
        default:
          break;
      }
      $is_installed = t($is_installed);

      // Does this module appear to be available on Drupal.org?
      $project = '';
      if ($type == 'Extension Module') {
        // Does it have a drupal project name?
        if (!array_key_exists('drupal_project', $extension[$namespace])) {
          // Since it doesn't have a drupal project name is it in a sandbox?
          if (!preg_match('/www.drupal.org\/sandbox/', $extension[$namespace]['home_page'])) {
            $project = t("<li>This module does not appear to be available as a " .
              "full Drupal project and thus cannot ".
              "be downloaded here. You may have to manually download it.</li>");
          }
          else {
            $project = ("<li>This module is in a sandbox on Drupal.org, and
              cannot be downloaded from here. You will have to manually download
              this module.</li>");
          }
        }
      }

      // Make sure the bulk loader module is installed, or we cannot provide
      // the bulk loader import button.
      $other = '';
      if ($type == 'Bulk Loader Template' and !module_exists('tripal_bulk_loader')) {
        $other = t('<li>The bulk loader
          module is not enabled. If you would like to import a loading template
          please enable it.</li>');
      }

      // If the user click's the button to import the extension then we
      // need the item in the submit function so we can process the import.
      $form[$type]['extension-' . $guid] = array(
        '#type' => 'value',
        '#value' => $extension,
      );

      $notices = '';
      if ($is_installed) {
        $notices = '<div class="messages status"><ul>' . $is_installed . '</ul></div>';
      }
      $warnings = '';
      if ($project or $other) {
        $warnings = '<div class="messages warning"><ul>' .
          $project .  ' ' .
          $other . '</ul></div>';
      }
      $errors = '';
      if ($incompatible) {
        $errors = '<div class="messages error"><ul>' . $incompatible .  '</ul></div>';
      }

      $state = '';
      if (array_key_exists('dev_stage', $extension[$namespace])) {
        $state = '<strong>Development State: </strong>' . $extension[$namespace]['dev_stage'] . "</br>";
      }

      // Create the form elements that we'll later theme into tables.
      $form[$type][$guid]['header'] = array(
        '#markup' => l($extension['title'], $extension['link']),
      );
      $form[$type][$guid]['details'] = array(
        '#markup' => "" .
        "<strong>Type:</strong> " . $type . "</br>" .
        "<strong>Categories: </strong>" . $extension[$namespace]['categories'] . "</br>" .
        "<strong>Authors: </strong>" . $extension[$namespace]['authors'] . "</br>" .
        $state .
        "<strong>Chado compatible versions: </strong>" . $extension[$namespace]['chado_version'] . "</br>" .
        "<strong>Tripal compatible versions: </strong>" . $extension[$namespace]['tripal_version'] . "</br>" .
        $home_page .
        "<strong>tripal.info Page: </strong>" . l($extension['link'], $extension['link']) . "</br>" .
        $notices .
        $warnings .
        $errors .
        "<p>" . $extension['description'] . "</p>",
      );
      // Add an import button to each of types that can support import.
      switch ($type) {
        case 'Bulk Loader Template':
          if (!$incompatible and !$is_installed and !$project) {
            $form[$type][$guid]['import'] = array(
              '#type' => 'submit',
              '#value' => "Import Loader",
              '#name' => "import-" . $guid,
            );
          }
          break;
        case 'Materialized View':
          if (!$incompatible and !$is_installed and !$project) {
            $form[$type][$guid]['import'] = array(
              '#type' => 'submit',
              '#value' => "Import MView",
              '#name' => "import-" . $guid,
            );
          }
          break;
        case 'Extension Module':
          if (!$incompatible and !$is_installed and !$project) {
            $form[$type][$guid]['import'] = array(
              '#type' => 'submit',
              '#value' => "Download Module",
              '#name' => "import-" . $guid,
            );
          }
          break;
        default:
          break;
      }
      $form[$type][$guid]['#theme'] = 'tripal_core_extensions_form_tables';
      $extension_index++;
    }

    // Now create and theme the pager.
    $pager = array(
      'tags' => array(),
      'element' => $type_index,
      'parameters' => array(
        'tab' => $type_ids[$type],
        'cv' => $chado_version,
        'tv' => $tripal_version,
      ),
      'quantity' => $num_per_page,
    );

    // now add the category filters to the params array
    foreach ($filters as $filter_id => $value) {
      $pager['parameters']['categories-' . $filter_id] = $value;
    }

    // because this may be an ajax callback, the theme_pager will set the URL to be
    // "system/ajax", so we need to reset that
    $pager = theme('pager', $pager);
    global $base_path;
    $pager = str_replace($base_path . "system/ajax", "", $pager) ;

    $form[$type]['pager'] = array(
      '#type' => 'item',
      '#markup' => $pager,
    );
    $type_index++;
  }
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
    $extension = $form_state['values']['extension-' . $guid];
    $type = $extension['category'];
    switch ($type) {
      case 'Bulk Loader Template':
        $options = array(
          'template_name' => $extension['title'],
          'template_array' => $extension[$namespace]['bulkldr_export'],
          'strict' => TRUE,
        );
        $errors = array();
        $warnings = array();
        $success = tripal_insert_bulk_loader_template($options, $errors, $warnings);
        if ($success) {
          drupal_set_message("Bulk loader succesfully added.");
        }
        else {
          drupal_set_message("Error importing this bulk loader.",  'error');
          if (count($errors) > 0) {
            foreach($errors as $field => $message) {
              drupal_set_message($message, 'error');
            }
          }
        }
        break;
      case 'Materialized View':
        $module_name = 'tripal_core';
        $mview_name = $extension[$namespace]['mview_name'];
        $mview_schema = $extension[$namespace]['mview_schema'];
        $mview_sql = $extension[$namespace]['mview_sql'];

        // Validate the contents of the schema array.
        // TODO: security issue!! Before using 'eval' on this string
        // we should make sure the array is valid and there is no hidden
        // PHP code.
        $schema_array = array();
        $success = eval("\$schema_array = $mview_schema;");
        $error = chado_validate_custom_table_schema($schema_array);
        if ($error) {
          drupal_set_message("Cannot import Materialized View: $error", "error");
        }
        tripal_add_mview($mview_name, $module_name, $schema_array, $mview_sql);
        break;
      case 'Extension Module':
        if (array_key_exists('drupal_project', $extension[$namespace])) {
          module_load_include('module', 'update', 'update');
          module_load_include('inc', 'update', 'update.manager');
          $project = $extension[$namespace]['drupal_project'];
          $tar = tripal_core_extensions_get_latest_module_version($project);
          if (!$tar) {
            drupal_set_message('Cannot find a suitable release of this module
              for download. You may need to manually install this module.', 'error');
          }
          else {

            // Download the file from the Drupal repository
            $local_cache = update_manager_file_get($tar);
            if (!$local_cache) {
              drupal_set_message('Cannot download the file. Check the
                "Recent log messages" for relavent errors.', 'error');
            }
            else {
              // The following code was borrowed from the update_manager_install_form_submit()
              // of drupal in the modules/update/update.manager.inc file.
              $directory = _update_manager_extract_directory();
              try {
                $archive = update_manager_archive_extract($local_cache, $directory);
              }
              catch (Exception $e) {
                drupal_set_message('Cannot extract the file. Please check
                  permissions in the modules directory', 'error');
                return;
              }
              $archive_errors = update_manager_archive_verify($project, $local_cache, $directory);
              if (!empty($archive_errors)) {
                foreach ($archive_errors as $error) {
                  drupal_set_message($error, 'error');
                }
              }
              $project_location = $directory . '/' . $project;
              try {
                $updater = Updater::factory($project_location);
              }
              catch (Exception $e) {
                drupal_set_message($e->getMessage(), 'error');
                return;
              }
              $project_real_location = drupal_realpath($project_location);
              $arguments = array(
                'project' => $project,
                'updater_name' => get_class($updater),
                'local_url' => $project_real_location,
              );
              module_load_include('inc', 'update', 'update.authorize');
              $filetransfer = new FileTransferLocal(DRUPAL_ROOT);
              call_user_func_array('update_authorize_run_install', array_merge(array($filetransfer), $arguments));

              drupal_set_message('It appears the module was downloaded and
                extracted. You can now ' . l('enable this module'. 'admin/modules') . '.');
            }
          }
        }
        else {
          drupal_set_message('Cannot download this module.  The Drpual project
            name is not set. You may have to manually download this module, and
            if possible encourage the developers to set the project name if it
            has been fully published on Drupal.org already.', 'error');
        }
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

/**
 * A callback function for the form
 *
 * @return unknown
 */
function tripal_core_extensions_form_ajax_callback($form, $form_state) {
  // This function need not do anything as the form will take care of
  // updates needed. We just need to return the form.
  return $form;
}

/**
 * Determines the most recent downloadable package for a module.
 *
 * This function will connect to Drupal.org's RSS feed for a project and
 * determine the most recent version of the module and return the URL
 * for it's package.
 *
 * @param $project
 */
function tripal_core_extensions_get_latest_module_version($project_name) {
  // First use the Drupal RESTful API to get the project
  $url = "https://www.drupal.org/api-d7/node.json?type=project_module&field_project_machine_name=$project_name";
  $result = json_decode(file_get_contents($url), TRUE);
  $project = $result['list'][0];
  $nid = $project['nid'];

  // Second get the releases for this node and find releases for this Drupal
  $url = "https://www.drupal.org/api-d7/node.json?type=project_release&field_release_project=$nid";
  $result = json_decode(file_get_contents($url), TRUE);
  $releases = $result['list'];
  $drupal_version = VERSION;
  $drupal_major = preg_replace('/^(\d+)\.\d+$/', "$1", $drupal_version);
  $best_release = NULL;
  foreach ($releases as $release) {
    // This release must match the Drupal major version or it won't
    // be compatiable. If it doesn't we'll skip it.
    $release_version = $release['field_release_version'];
    $release_drupal = preg_replace('/^(\d+)\.x-.*$/', "$1", $release_version);
    if ($release_drupal != $drupal_major) {
      continue;
    }

    // Set the current release to be the best one. On successive iterations
    // we'll check to see if that is still true.
    if ($best_release == NULL) {
      $best_release = $release;
      continue;
    }
    if ($release['field_release_version_major'] > $best_release['field_release_version_major']) {
      $best_release = $release;
      continue;
    }
    if ($release['field_release_version_patch'] > $best_release['field_release_version_patch']) {
      $best_release = $release;
      continue;
    }

    // If the best version has no extra part then let's keep it as this is the
    // most stable release.
    if (!$best_release['field_release_version_extra']) {
      continue;
    }

    // Convert the 'extra' part to a numeric value that will let us compare
    // the ranks of the release versions.
    $extra_rank = array(
      'unstable' => 1,
      'alpha' => 2,
      'beta' => 3,
      'rc' => 4,
    );
    $matches = array();
    $this_extra = 0;
    if (preg_match('/^(.*?)(\d+)$/', $release['field_release_version_extra'], $matches)) {
      $this_extra = $extra_rank[$matches[1]] . "." . $matches[2];
    }
    $best_extra = 0;
    if (preg_match('/^(.*?)(\d+)$/', $best_release['field_release_version_extra'], $matches)) {
      $best_extra = $extra_rank[$matches[1]] . "." . $matches[2];
    }
    if ($this_extra > $best_extra) {
      $best_release = $release;
      continue;
    }
  }
  // If we have a best result then build the download string.
  // TODO: we may need to make another web services call to get the actual
  // download path, but for now we'll hard code the construction of it.
  if ($best_release) {
    return 'http://ftp.drupal.org/files/projects/' . $project_name . '-' . $best_release['field_release_version'] . '.tar.gz';
  }
  return '';
}
