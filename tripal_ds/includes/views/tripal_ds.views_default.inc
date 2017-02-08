<?php

/**
 * Implements hook_views_default_views().
 */
function tripal_ds_views_default_views() {
 
$view = new view();
$view->name = 'page_info_menu';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'tripal_ds';
$view->human_name = 'Tripal Content Type Page Info';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Tripal Panes: Field Name */
$handler->display->display_options['fields']['field_name']['id'] = 'field_name';
$handler->display->display_options['fields']['field_name']['table'] = 'tripal_ds';
$handler->display->display_options['fields']['field_name']['field'] = 'field_name';
$handler->display->display_options['fields']['field_name']['label'] = '';
$handler->display->display_options['fields']['field_name']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_name']['element_label_colon'] = FALSE;
/* Field: Tripal Panes: Field Label */
$handler->display->display_options['fields']['field_label']['id'] = 'field_label';
$handler->display->display_options['fields']['field_label']['table'] = 'tripal_ds';
$handler->display->display_options['fields']['field_label']['field'] = 'field_label';
$handler->display->display_options['fields']['field_label']['label'] = '';
$handler->display->display_options['fields']['field_label']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_label']['alter']['text'] = '<a href="#" class="tripal_pane-toc-list-item-link" id ="[field_name]">[field_label] </a>';
$handler->display->display_options['fields']['field_label']['alter']['link_class'] = '[field_label]';
$handler->display->display_options['fields']['field_label']['element_label_colon'] = FALSE;
/* Contextual filter: Tripal Panes: Bundle ID */
$handler->display->display_options['arguments']['bundle']['id'] = 'bundle';
$handler->display->display_options['arguments']['bundle']['table'] = 'tripal_ds';
$handler->display->display_options['arguments']['bundle']['field'] = 'bundle';
$handler->display->display_options['arguments']['bundle']['default_action'] = 'default';
$handler->display->display_options['arguments']['bundle']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['bundle']['default_argument_options']['code'] = '$url = current_path();
$url_exploded = explode("/", $url);
$tripal_entity_id = (int)$url_exploded[1];

    $result = db_select(\'tripal_entity\', \'te\')
      ->fields(\'te\', array(\'bundle\'))
      ->condition(\'id\', $tripal_entity_id, \'=\')
      ->execute()
      ->fetchField();

return $result;';
$handler->display->display_options['arguments']['bundle']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['bundle']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['bundle']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['bundle']['limit'] = '0';
/* Filter criterion: Global: Fields comparison */
$handler->display->display_options['filters']['fields_compare']['id'] = 'fields_compare';
$handler->display->display_options['filters']['fields_compare']['table'] = 'views';
$handler->display->display_options['filters']['fields_compare']['field'] = 'fields_compare';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'page_info_menu';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';

 
$views[$view->name] = $view;
 
// return views
  return $views;
}