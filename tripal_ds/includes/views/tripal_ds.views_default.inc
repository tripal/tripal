<?php

/**
 * Implements hook_views_default_views().
 */
function tripal_ds_views_default_views() {

  $view = new view();
  $view->name = 'tripal_content_type_toc';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'tripal_ds';
  $view->human_name = 'Tripal Content Type Table of Contents';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Table of Contents';
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
  $handler->display->display_options['fields']['tripal_ds_field_name']['id'] = 'tripal_ds_field_name';
  $handler->display->display_options['fields']['tripal_ds_field_name']['table'] = 'tripal_ds';
  $handler->display->display_options['fields']['tripal_ds_field_name']['field'] = 'tripal_ds_field_name';
  $handler->display->display_options['fields']['tripal_ds_field_name']['label'] = '';
  $handler->display->display_options['fields']['tripal_ds_field_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['tripal_ds_field_name']['element_label_colon'] = FALSE;
  /* Field: Tripal Panes: Bundle ID */
  $handler->display->display_options['fields']['bundle']['id'] = 'bundle';
  $handler->display->display_options['fields']['bundle']['table'] = 'tripal_ds';
  $handler->display->display_options['fields']['bundle']['field'] = 'bundle';
  $handler->display->display_options['fields']['bundle']['label'] = '';
  $handler->display->display_options['fields']['bundle']['exclude'] = TRUE;
  $handler->display->display_options['fields']['bundle']['element_label_colon'] = FALSE;
  /* Field: Tripal Panes: Field Label */
  $handler->display->display_options['fields']['tripal_ds_field_label']['id'] = 'tripal_ds_field_label';
  $handler->display->display_options['fields']['tripal_ds_field_label']['table'] = 'tripal_ds';
  $handler->display->display_options['fields']['tripal_ds_field_label']['field'] = 'tripal_ds_field_label';
  $handler->display->display_options['fields']['tripal_ds_field_label']['label'] = '';
  $handler->display->display_options['fields']['tripal_ds_field_label']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['tripal_ds_field_label']['alter']['text'] = '<a href="#" class="tripal_pane-toc-list-item-link" id ="[tripal_ds_field_name]">[tripal_ds_field_label] </a>';
  $handler->display->display_options['fields']['tripal_ds_field_label']['element_label_colon'] = FALSE;
  /* Sort criterion: Tripal Panes: Weight */
  $handler->display->display_options['sorts']['weight']['id'] = 'weight';
  $handler->display->display_options['sorts']['weight']['table'] = 'tripal_ds';
  $handler->display->display_options['sorts']['weight']['field'] = 'weight';
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

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'tripal_ct_toc';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';


  $views[$view->name] = $view;

  // Return views.
  return $views;
}