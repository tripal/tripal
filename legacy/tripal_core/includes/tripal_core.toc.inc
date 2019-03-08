<?php

/**
 *
 */
function tripal_core_node_toc_form($form, &$form_state, $node) {

  // Get info about this content type
  $all_types = node_type_get_types();
  $type_info = $all_types[$node->type];

  $form["#tree"] = TRUE;

  $form["instructions"] = [
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#title' => 'Instructions',
  ];
  $admin_link = l(
    $type_info->name . " TOC administrative page",
    "admin/tripal/legacy/" . $type_info->module . "/" . $node->type . "toc",
    ['attributes' => ['target' => '_blank']]
  );
  $form["instructions"]["main"] = [
    '#markup' => '<p>' . t("Below is a list of the titles of
      content panes that can appear on this page.  These titles appear in the
      the following order in the Table of Contents (TOC). You may rename
      the titles or drag and drop them to change the order.  <b>Any changes will
      only apply to this page</b>. If you would like to make changes apply to multiple
      pages of the same type, please visit the $admin_link. ") . '</p>' .
      '<p>' . t('The list below shows all possible content panes that can appear.
      However, those without content are automatically hidden and do not
      appear in the TOC.' . '</p>'),
  ];

  $form['node'] = [
    '#type' => 'value',
    '#value' => $node,
  ];

  // Get the content array for this node, then pass it through the
  // tripal_core_node_view_alter which generates the TOC.  After that
  // we can use the $build array to build the form. We have to add
  // a 'tripal_toc_mode' to the $node because we need to give the mode
  // to the tripal_core_node_view_build_toc function.
  $node->tripal_toc_mode = 'manage_node';
  node_build_content($node);
  $build = $node->content;
  $build["#node"] = $node;
  tripal_core_node_view_alter($build);

  // Iterate through the built items and add form elemetns for each one.
  foreach (element_children($build) as $key) {
    $element = $build[$key];

    if (array_key_exists('#tripal_toc_id', $element)) {
      $toc_id = $element['#tripal_toc_id'];
      $toc_title = $element['#tripal_toc_title'];
      $toc_weight = $element['#weight'];
      $toc_hide = $element['#hide'];

      // If this element is a link then we don't want to allow the user
      // to change the title as the link title is changed by using the
      // interface that created the link.
      $is_link = array_key_exists('#is_link', $element) ? $element['#is_link'] : FALSE;
      if (!$is_link) {
        $form['toc_items'][$toc_id]['title'] = [
          '#type' => 'textfield',
          '#default_value' => $toc_title,
        ];
      }
      else {
        $form['toc_items'][$toc_id]['title'] = [
          '#markup' => '<i>link title:</i> ' . $toc_title,
          '#value' => $toc_title,
        ];
      }
      $form['toc_items'][$toc_id]['hide'] = [
        '#type' => 'checkbox',
        '#default_value' => $toc_hide,
      ];
      $form['toc_items'][$toc_id]['weight'] = [
        '#type' => 'textfield',
        '#default_value' => $toc_weight,
        '#attributes' => [
          'class' => ['tripal-node-toc-items-weights'],
        ],
        '#size' => 5,
      ];
    }
  }
  $form['toc_items']['#theme'] = 'tripal_node_toc_items_table';

  $form['submit'] = [
    '#type' => 'submit',
    '#name' => 'toc_submit',
    '#value' => t('Submit'),
  ];
  $form['unset'] = [
    '#type' => 'submit',
    '#name' => 'toc_unset',
    '#value' => t('Unset Node Customizations'),
  ];

  // Check to see if this node's TOC is specifically being managed.
  $sql = "SELECT count(*) FROM {tripal_toc} where nid = :nid";
  $managed_items = db_query($sql, [':nid' => $node->nid])->fetchField();

  if ($managed_items > 0) {
    $form['is_managed'] = [
      '#markup' => '<p><font color="red">' .
        t('This page currently has customiations to the TOC.</font> This means
        that any customzations for the content type are overriden. Click the
        "Unset Node Customizations" button above to remove page-level
        customizations and default to the content type settings.') . '</p>',
    ];
  }


  return $form;
}

/**
 *
 * @param $variables
 */
function theme_tripal_node_toc_items_table($variables) {
  $elements = $variables['element'];
  $toc_items = [];

  // Sort the toc_items using a custom sort function. But we need to include
  // only the items we want in the table (exclude renderable stuff).
  foreach (element_children($elements) as $key) {
    $toc_items[] = $elements[$key];
  }
  usort($toc_items, 'theme_tripal_node_sort_toc_items');

  // Build the table header.
  $headers = ['Content Pane Name', 'Hide', 'Weight'];

  // Format the form elements as rows in the table.
  $rows = [];
  foreach ($toc_items as $key => $item) {
    $rows[] = [
      'data' => [
        drupal_render($item['title']),
        drupal_render($item['hide']),
        drupal_render($item['weight']),
      ],
      'class' => ['draggable'],
    ];
  }

  // Theme and return the table.
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => ["id" => 'tripal-node-toc-items-table'],
    'sticky' => TRUE,
    'caption' => t('Content Panes Available in the TOC'),
    'colgroups' => [],
    'empty' => t('There are no content panes for this page'),
  ];
  drupal_add_tabledrag('tripal-node-toc-items-table', 'order', 'sibling', 'tripal-node-toc-items-weights');
  return theme_table($table);
}

/**
 *
 * @param $a
 * @param $b
 */
function theme_tripal_node_sort_toc_items($a, $b) {

  if ($a['weight']['#value'] < $b['weight']['#value']) {
    return -1;
  }
  if ($a['weight']['#value'] > $b['weight']['#value']) {
    return 1;
  }
  if ($a['weight']['#value'] == $b['weight']['#value']) {
    return strcmp($a['title']['#value'], $b['title']['#value']);
  }
}

/**
 * Implements hook_validate for the tripal_core_node_toc_form.
 */
function tripal_core_node_toc_form_validate($form, &$form_state) {
  $toc_items = $form_state['values']['toc_items'];

  // Iterate through the TOC items and validate.
  foreach ($toc_items as $toc_id => $item) {
    if (array_key_exists('title', $item) and !$item['title']) {
      form_set_error('toc_items][' . $toc_id, "Please provide a valid title.");
    }
  }
}

/**
 * Implements hook_submit for the tripal_core_node_toc_form.
 */
function tripal_core_node_toc_form_submit($form, &$form_state) {
  $toc_items = $form_state['values']['toc_items'];
  $node = $form_state['values']['node'];

  if ($form_state['clicked_button']['#name'] == "toc_submit") {
    $transaction = db_transaction();
    try {
      // First delete any settings for this node
      db_delete('tripal_toc')
        ->condition('nid', $node->nid)
        ->execute();

      // Second add in any new settings for this node
      foreach ($toc_items as $toc_id => $item) {
        db_insert('tripal_toc')
          ->fields([
            'node_type' => $node->type,
            'key' => $toc_id,
            'title' => array_key_exists('title', $item) ? $item['title'] : '',
            'weight' => $item['weight'],
            'nid' => $node->nid,
            'hide' => $item['hide'],
          ])
          ->execute();
      }
      drupal_set_message("TOC changes successfully applied to this node only.");
    } catch (Exception $e) {
      $transaction->rollback();
      drupal_set_message("Failed to apply TOC changes.", "error");
    }
  }
  if ($form_state['clicked_button']['#name'] == "toc_unset") {
    $transaction = db_transaction();
    try {
      // First delete any settings for this node
      db_delete('tripal_toc')
        ->condition('nid', $node->nid)
        ->execute();

      drupal_set_message("TOC is no longer customized specifically for this page. Now using the content type settings.");
    } catch (Exception $e) {
      $transaction->rollback();
      drupal_set_message("Failed to apply TOC changes.", "error");
    }
  }
}

/**
 * To be called by tripal_core_node_view_alter() to generate the TOC.
 *
 * @param $build
 *   The build array passed to hook_node_view_alter()
 *
 */
function tripal_core_node_view_build_toc(&$build) {
  global $theme;

  // if this is not a full node view, we do not want to alter
  if ($build['#view_mode'] != 'full' OR !array_key_exists('#tripal_generic_node_template', $build)) {
    return;
  }

  $node_type = $build["#node"]->type;
  $nid = $build["#node"]->nid;

  // The mode alters the format of the build array. There are three types of
  // modes: "display", "manage_node", "manage_type".  If "display" is provided
  // then the build array is formatted for the display of the content.
  // If "manage_node" is provided then the build array will contain all
  // content panes regardless if the pane should be hidden. This allows
  // the management tool to find all content panes and their settings. If
  // "manage_type" is provided then node-specific content panes are
  // excluded.  Node-specific content panes are those that appear only on
  // specific nodes and therefore should not be used when managing the
  // TOC for a content type.
  $mode = isset($build["#node"]->tripal_toc_mode) ? $build["#node"]->tripal_toc_mode : "display";


  $cache = cache_get("theme_registry:$theme", 'cache');
  $node = $build['#node'];
  $toc = [];
  $toc_html = '';

  // If we are looking at a Tripal node template then we want to
  // make some changes to each pane of content so that we can associate
  // a table of contents and add administrator and curator messages.
  if ($build['#tripal_generic_node_template'] == TRUE) {

    // Iterate through all the elements of the $build array and for those
    // that are wanting to provide content for this node.
    $markup = [];
    foreach ($build as $key => $value) {
      $value = $build[$key];

      // Skip the body element as the Tripal node types do not use it.
      if ($key == 'body') {
        continue;
      }

      // Skip the table of contents and links as those will be placed elsewhere.
      if (preg_match('/^#/', $key) or $key == 'tripal_toc' or $key == 'links') {
        continue;
      }


      // For backwards compatibility we will handle the content type fields
      // named 'field_resource_blocks', 'field_resource_titles', and 'field_resource_links'
      // these fields can be added on the Drupal content types page and were
      // specifically recoginzed by Tripal v1.1.  If the mode type is "manage_type"
      // then remove these content panes because they are node specific.
      if ($mode == "manage_type" and (
          $key == "field_resource_links" or
          $key == "field_resource_titles" or
          $key == "field_resource_blocks")) {
        unset($build[$key]);
        continue;
      }
      if ($key == "field_resource_links") {
        // links should just appear on the sidebar as is and not open up a panel
        foreach (element_children($build[$key]) as $index) {
          $element = $build[$key][$index];
          $weight = 0;
          $hide = 0;
          $toc_item_id = "resource-link-$index";

          // Get any overrides for this key.
          $overrides = tripal_core_get_toc_overrides($nid, $toc_item_id, $node_type, $mode);
          $weight = $overrides['weight'] ? $overrides['weight'] : $weight;
          $hide = $overrides['hide'] ? $overrides['hide'] : $hide;

          // If the element should be hidden then unset this key the build
          // array continue to the next one
          if ($mode == "display" and $overrides['hide'] == 1) {
            continue;
          }

          // This field supports tokens, so we need to perform the substitutions
          // if one is needed. Get the tokens and format
          $base_table = preg_replace('/^chado_(.*)$/', '\1', $node_type);
          $tokens = chado_node_generate_tokens($base_table);
          $markup = $element['#markup'];
          // Determine which tokens were used in the format string
          if (preg_match_all('/\[[^]]+\]/', $markup, $used_tokens)) {
            // Get the value for each token used
            foreach ($used_tokens[0] as $token) {
              $token_info = $tokens[$token];
              if (!empty($token_info)) {
                $value = chado_get_token_value($token_info, $node);
                $markup = str_replace($token, $value, $markup);
              }
            }
            $element['#markup'] = $markup;
          }

          // Add the link to the TOC
          $parts = explode("|", $element['#markup']);
          if (count($parts) == 2) {
            $toc[$weight][$parts[0]] = "<div id=\"$toc_item_id\" class=\"tripal_toc_list_item\">" . l($parts[0], $parts[1], ['attributes' => ['target' => '_blank']]) . "</div>";
          }
          else {
            $toc[$weight][$parts[0]] = "<div id=\"$toc_item_id\" class=\"tripal_toc_list_item\">" . $element['#markup'] . "</div>";
          }

          // Add to the build array but do not add markup. This way
          // when the TOC is managed by the node 'TOC' menu these links can
          // be ordered as well.
          $build[$toc_item_id]['#toc_handled'] = TRUE;
          $build[$toc_item_id]['#tripal_toc_id'] = $toc_item_id;
          $build[$toc_item_id]['#tripal_toc_title'] = $parts[0];
          $build[$toc_item_id]['#weight'] = $weight;
          $build[$toc_item_id]['#hide'] = $hide;
          $build[$toc_item_id]['#is_link'] = TRUE;

        }
        // Remove the orilink from the build array as we've moved it to
        // appear in the TOC
        unset($build[$key]);
        continue;
      }
      if ($key == "field_resource_titles") {
        // ignore these, we will use them in the field_resource_blocks if
        // statement below
        continue;
      }
      if ($key == "field_resource_blocks") {
        foreach (element_children($build[$key]) as $index) {
          // get the details and the title
          $weight = 0;
          $hide = 0;
          $markup = $build[$key][$index]["#markup"];
          $toc_item_id = "resource-$index";

          // Get any overrides for this key.
          $overrides = tripal_core_get_toc_overrides($nid, $toc_item_id, $node_type, $mode);

          // If the element should be hidden then unset this key the build
          // array continue to the next one
          if ($mode == "display" and $overrides['hide'] == 1) {
            continue;
          }

          $toc_item_title = $build["field_resource_titles"][$index]["#markup"];
          $toc_item_title = $overrides['title'] ? $overrides['title'] : $toc_item_title;
          $weight = $overrides['weight'] ? $overrides['weight'] : $weight;
          $hide = $overrides['hide'] ? $overrides['hide'] : $hide;
          $updated_markup = "
            <div id=\"$toc_item_id-tripal-data-pane\" class=\"tripal-data-pane\">
              <div class=\"$toc_item_id-tripal-data-pane-title tripal-data-pane-title\">$toc_item_title</div>
                $markup
              </div>
            </div>
          ";
          $build[$toc_item_id]['#markup'] = $updated_markup;
          $build[$toc_item_id]['#toc_handled'] = TRUE;
          $build[$toc_item_id]['#tripal_toc_id'] = $toc_item_id;
          $build[$toc_item_id]['#tripal_toc_title'] = $toc_item_title;
          $build[$toc_item_id]['#weight'] = $weight;
          $build[$toc_item_id]['#hide'] = $hide;
          // add the entry to the TOC
          $toc_item_link = "
            <div class=\"tripal_toc_list_item\">
              <a id=\"$toc_item_id\" class=\"tripal_toc_list_item_link\" href=\"?pane=$toc_item_id\">$toc_item_title</a>
            </div>
          ";
          $toc[$weight][$toc_item_title] = $toc_item_link;
        }
        // Remove the key from the build array. We have have replaced it
        unset($build[$key]);
        unset($build["field_resource_titles"]);
        continue;
      } // end if ($mode != "manage_type" and $key == "field_resource_blocks") {

      // Skip any keys we may have already handled. This is the case for
      // the field_resource_blocks where we removed the old CCK fields
      // and added new ones.  We don't want these new ones to be processed
      // again by the code below.
      if (array_key_exists('#toc_handled', $build[$key]) and $build[$key]['#toc_handled'] == TRUE) {
        continue;
      }

      // For all other fields we will handle in the following way.
      //-----------------------
      // INITIALIZE THE CONTENT VARIABLES
      //-----------------------
      $toc_item_title = $key;
      $toc_item_id = $key;
      $toc_item_link = '';
      $weight = 0;
      $hide = 0;

      // get the title for the table of contents.  Tripal templates should
      // have a '#tripal_toc_title' element in the build array
      if (array_key_exists('#tripal_toc_title', $build[$key])) {
        $toc_item_title = $build[$key]['#tripal_toc_title'];
      }
      // other elements in the $build array may just have a '#title' element,
      if (array_key_exists('#title', $build[$key])) {
        $toc_item_title = $build[$key]['#title'];
      }
      $toc_item_title = ucwords($toc_item_title);
      if (array_key_exists('#weight', $build[$key])) {
        $weight = $build[$key]['#weight'];
      }
      if (array_key_exists('#tripal_toc_id', $build[$key])) {
        $toc_item_id = $build[$key]['#tripal_toc_id'];
      }

      // Get any overrides for this key.
      $overrides = tripal_core_get_toc_overrides($nid, $toc_item_id, $node_type, $mode);


      // If the element should be hidden then unset this key the build
      // array continue to the next one
      if ($mode == "display" and $overrides['hide'] == 1) {
        unset($build[$key]);
        continue;
      }

      // now override the title, weight, hidden values if a value is set in the tripal_toc table
      $toc_item_title = $overrides['title'] ? $overrides['title'] : $toc_item_title;
      $weight = $overrides['weight'] ? $overrides['weight'] : $weight;
      $hide = $overrides['hide'] ? $overrides['hide'] : $hide;
      $toc_item_link = "<div class=\"tripal_toc_list_item\"><a id=\"$toc_item_id\" class=\"tripal_toc_list_item_link\" href=\"?pane=$toc_item_id\">$toc_item_title</a></div>";


      //-----------------------
      // GET THE MARKUP FOR EACH ELEMENT
      //-----------------------
      $markup = '';

      // find the markup. Some fields will have a '#markup' and others, such
      // as CCK elements may have a set of '#markup' elements organized by
      // numerical keys.
      if (array_key_exists('#markup', $build[$key]) and trim($build[$key]['#markup'])) {
        $markup = $build[$key]['#markup'];
      }
      // For backwards copmatibility we should support the '#value' element as well.
      elseif (array_key_exists('#value', $build[$key]) and trim($build[$key]['#value'])) {
        $markup = $build[$key]['#markup'];
      }
      // if we have no '#markup' field then this element has not yet
      // been rendered.  Let's render it and substitute that for markup
      if (!$markup) {
        $markup = trim(render($build[$key]));
      }

      // Setup the content array for this element
      $build[$key] = [
        '#markup' => $markup,
        '#tripal_toc_id' => $toc_item_id,
        '#tripal_toc_title' => $toc_item_title,
        '#weight' => $weight,
        '#hide' => $hide,
      ];


      // if we still don't have markup then skip this one
      if (!$markup) {
        continue;
      }

      //-----------------------
      // FIND THE TEMPLATE PATH
      //-----------------------
      // get the template path so we can put it in an admin message box
      $path = '';
      if (!array_key_exists('#tripal_template_show', $build[$key]) or
        $build[$key]['#tripal_template_show'] == TRUE) {
        if ($cache and array_key_exists($key, $cache->data) and array_key_exists('path', $cache->data[$key])) {

          $path = $cache->data[$key]['path'] . '/' . $key . '.tpl.php';

          $path = tripal_set_message("Administrators, you can
            customize the way the content above is presented.  Tripal provides a template
            file for each pane of content.  To customize, copy the template file to your
            site's default theme, edit then " .
            l('clear the Drupal cache', 'admin/config/development/performance', ['attributes' => ['target' => '_blank']]) . ".
            Currently, the content above is provided by this template: <br><br>$path",
            TRIPAL_INFO,
            ['return_html' => 1]
          );
        }
      }

      //-----------------------
      // ADD THIS PANE TO THE TOC BY ORDER OF WEIGHT
      //-----------------------
      // set the weight of the TOC item and add it to our $toc array
      // for building of the TOC below
      $weight = 0;
      if (array_key_exists('#weight', $build[$key])) {
        $weight = $build[$key]['#weight'];
      }
      $toc[$weight][$toc_item_title] = $toc_item_link;

      //-----------------------
      // CREATE THE CONTENT PANE MARKUP
      //-----------------------
      // add a surrounding <div> box around the content
      $updated_markup = "
        <div id=\"$toc_item_id-tripal-data-pane\" class=\"tripal-data-pane\">
          <div class=\"$toc_item_id-tripal-data-pane-title tripal-data-pane-title\">$toc_item_title</div>
            $markup
            $path
          </div>
        </div>
      ";

      $build[$key]['#markup'] = $updated_markup;
    } // end foreach ($build as $key => $value) {
  } // end if ($build['#tripal_generic_node_template'] == TRUE) {

  //-----------------------
  // BUILD THE TABLE OF CONTENTS LINKS
  //-----------------------
  // first sort the links numerically by their weight
  ksort($toc, SORT_NUMERIC);
  $toc_html = '';
  foreach ($toc as $weight => $links) {
    // for links in the same weight, sort them alphabetically
    ksort($links);
    foreach ($links as $toc_item_title => $toc_item_link) {
      $toc_html .= $toc_item_link;
    }
  }
  $build['tripal_toc']['#markup'] = "<div id=\"$node->type-tripal-toc-pane\" class=\"tripal-toc-pane\">$toc_html</div>";
}

/**
 *
 * @param $build
 */
function tripal_core_get_toc_overrides($nid, $key, $node_type, $mode) {
  // Set override defaults
  $override_title = '';
  $override_weight = '';
  $override_hide = 0;

  if ($mode != "manage_type") {
    // First look to see if the node has customizations for this item.
    $toc_item_overrides = db_select('tripal_toc', 'tc')
      ->fields('tc', ['title', 'weight', 'hide'])
      ->condition('key', $key)
      ->condition('nid', $nid)
      ->execute()
      ->fetchObject();
    if ($toc_item_overrides) {
      $override_title = $toc_item_overrides->title;
      $override_weight = $toc_item_overrides->weight;
      $override_hide = $toc_item_overrides->hide;
      return [
        'title' => $override_title,
        'weight' => $override_weight,
        'hide' => $override_hide,
      ];
    }
  }

  // If there are no specific node customizations then look to see if there
  // are customizations for this content type.
  $toc_item_overrides = db_select('tripal_toc', 'tc')
    ->fields('tc', ['title', 'weight', 'hide'])
    ->condition('node_type', $node_type)
    ->condition('key', $key)
    ->isNull('nid')
    ->execute()
    ->fetchObject();
  if ($toc_item_overrides) {
    $override_title = $toc_item_overrides->title;
    $override_weight = $toc_item_overrides->weight;
    $override_hide = $toc_item_overrides->hide;
  }

  return [
    'title' => $override_title,
    'weight' => $override_weight,
    'hide' => $override_hide,
  ];

}

/**
 *
 */
function tripal_core_content_type_toc_form($form, &$form_state, $content_type) {

  // Get the type details
  $all_types = node_type_get_types();
  $type_info = $all_types[$content_type];

  $form["#tree"] = TRUE;

  // Get a single node of this type so we can get all the possible content
  // for it.
  $sql = "SELECT nid FROM {node} WHERE type = :type LIMIT 1 OFFSET 0";
  $nid = db_query($sql, [':type' => $content_type])->fetchField();
  if (!$nid) {
    $form["not_available"] = [
      '#markup' => t('Please sync at least one %type_name record. A node
          must exist before customizations to the Table of Contents (TOC) can
          be performed.', ['%type_name' => $type_info->name]),
    ];
    return $form;
  }

  // Load the node
  $node = node_load($nid);

  // Get the content array for this node, then pass it through the
  // tripal_core_node_view_alter which generates the TOC.  After that
  // we can use the $build array to build the form. We have to add
  // a 'tripal_toc_mode' to the $node because we need to give the mode
  // to the tripal_core_node_view_build_toc function.
  $node->tripal_toc_mode = 'manage_type';

  node_build_content($node);
  $build = $node->content;
  $build["#node"] = $node;
  tripal_core_node_view_alter($build);

  $form["instructions"] = [
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#title' => 'Instructions',
  ];
  $form["instructions"]["main"] = [
    '#markup' => '</p>' . t('Below is a list of the titles of
      content panes that can appear on all %type_name pages.  You may rename
      the titles or drag and drop them to change the order.  Content that appears
      only on a single page can not be ordered here, but must be ordered using
      the TOC tab on the page itself.  If a page has customized TOC settings
      then those settings will take precedent over these.',
        ['%type_name' => $type_info->name]) . '</p>' .
      '<p>' . t('The list below shows all possible content
      panes that can appear. However, those without content are automatically
      hidden and do not appear in the TOC.' . '</p>'),
  ];

  $form['content_type'] = [
    '#type' => 'value',
    '#value' => $content_type,
  ];

  // Iterate through the built items and add form elemetns for each one.
  foreach (element_children($build) as $key) {
    $element = $build[$key];

    if (array_key_exists('#tripal_toc_id', $element)) {
      $toc_id = $element['#tripal_toc_id'];
      $toc_title = $element['#tripal_toc_title'];
      $toc_weight = $element['#weight'];
      $toc_hide = $element['#hide'];
      $form['toc_items'][$toc_id]['title'] = [
        '#type' => 'textfield',
        '#default_value' => $toc_title,
      ];
      $form['toc_items'][$toc_id]['hide'] = [
        '#type' => 'checkbox',
        '#default_value' => $toc_hide,
      ];
      $form['toc_items'][$toc_id]['weight'] = [
        '#type' => 'textfield',
        '#default_value' => $toc_weight,
        '#attributes' => [
          'class' => ['tripal-node-toc-items-weights'],
        ],
        '#size' => 5,
      ];
    }
  }
  $form['toc_items']['#theme'] = 'tripal_node_toc_items_table';

  $form['submit'] = [
    '#type' => 'submit',
    '#name' => 'toc_submit',
    '#value' => t('Submit'),
  ];
  $form['unset'] = [
    '#type' => 'submit',
    '#name' => 'toc_unset',
    '#value' => t('Reset to Defaults'),
  ];

  return $form;
}


/**
 * Implements hook_validate for the tripal_core_node_toc_form.
 */
function tripal_core_content_type_toc_form_validate($form, &$form_state) {
  $toc_items = $form_state['values']['toc_items'];

  // Iterate through the TOC items and validate.
  foreach ($toc_items as $toc_id => $item) {
    if (!$item['title']) {
      form_set_error('toc_items][' . $toc_id, "Please provide a valid title.");
    }
  }
}

/**
 * Implements hook_submit for the tripal_core_node_toc_form.
 */
function tripal_core_content_type_toc_form_submit($form, &$form_state) {
  $toc_items = $form_state['values']['toc_items'];
  $content_type = $form_state['values']['content_type'];

  if ($form_state['clicked_button']['#name'] == "toc_submit") {
    $transaction = db_transaction();
    try {
      // First delete any settings for this content type
      db_delete('tripal_toc')
        ->condition('node_type', $content_type)
        ->isNull('nid')
        ->execute();

      // Second add in any new settings for this node
      foreach ($toc_items as $toc_id => $item) {
        db_insert('tripal_toc')
          ->fields([
            'node_type' => $content_type,
            'key' => $toc_id,
            'title' => $item['title'],
            'weight' => $item['weight'],
            'hide' => $item['hide'],
          ])
          ->execute();
      }
      drupal_set_message("TOC changes successfully applied to this content type.");
    } catch (Exception $e) {
      $transaction->rollback();
      drupal_set_message("Failed to apply TOC changes.", "error");
    }
  }
  if ($form_state['clicked_button']['#name'] == "toc_unset") {
    $transaction = db_transaction();
    try {
      // First delete any settings for this node
      db_delete('tripal_toc')
        ->condition('node_type', $content_type)
        ->isNull('nid')
        ->execute();

      drupal_set_message("The TOC is reset to defaults for this content type.");
    } catch (Exception $e) {
      $transaction->rollback();
      drupal_set_message("Failed to apply TOC changes.", "error");
    }
  }
}