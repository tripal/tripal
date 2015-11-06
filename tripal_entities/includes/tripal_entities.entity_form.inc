<?php

/**
 *
 */
function tripal_entities_add_page() {
  $item = menu_get_item();

  $content = system_admin_menu_block($item);

  // Bypass the node/add listing if only one content type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('tripal_entities_add_list', array('content' => $content));

}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_tripal_entities_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any biological types yet. Go to the <a href="@create-content">content type creation page</a> to add a new content type.', array('@create-content' => url('admin/structure/BioData/add'))) . '</p>';
  }
  return $output;
}

/**
 *
 */
function tripal_entities_entity_form($form, &$form_state, $dbxref_id = '', $entity = NULL) {

  $bundle_id = 'dbxref_' . $dbxref_id;

  // Add a vertical tabs element
  $form['ev_tabs'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 999,
  );

  // If the entity doesn't exist then create one.
  if (!$entity) {
    $entity = entity_get_controller('BioData')->create(array('bundle' => $bundle_id));
    field_attach_form('BioData', $entity, $form, $form_state);

    $form['add_button'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#name' => 'add_data',
      '#weight' => 1000
    );
  }
  else {
    field_attach_form('BioData', $entity, $form, $form_state);
    $form['update_button'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#name' => 'update_data',
      '#weight' => 1000
    );
    $form['delete_button'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#name' => 'delete_data',
      '#weight' => 1001
    );
  }

  // The entity object must be added to the $form_state in order for
  // the Entity API to work. It must have a key of the entity name.
  $form_state['BioData'] = $entity;

  $form['#prefix'] = "<div id='$bundle_id-entity-form'>";
  $form['#suffix'] = "</div>";

  return $form;

}
/**
 * An Ajax callback for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;
}

/**
 * Implements hook_validate() for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_validate($form, &$form_state) {

  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='add_data') {
    $entity = $form_state['BioData'];
    field_attach_form_validate('BioData', $entity, $form, $form_state);
  }
}


/**
 * Implements hook_submit() for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_submit($form, &$form_state) {
  $entity = $form_state['BioData'];

  if ($form_state['clicked_button']['#name'] =='cancel') {
    $form_state['redirect'] = "BioData/" . $entity->id;
  }

  if ($form_state['clicked_button']['#name'] =='update_data' or
      $form_state['clicked_button']['#name'] =='add_data') {

    $entityform = entity_ui_controller('BioData')->entityFormSubmitBuildEntity($form, $form_state);
    if ($entityform->save()) {
      $form_state['redirect'] = "BioData/" . $entity->id;
    }
    else {
      drupal_set_message('Cannot save entity', 'error');
    }
  }
  if ($form_state['clicked_button']['#name'] =='delete_data') {
    $form_state['redirect'] = 'BioData/' . $entity->id .'/delete';
  }
}

/**
 * Form callback: confirmation form for deleting a tripal_entity.
 *
 * @param $tripal_entity The
 *          tripal_entity to delete
 *
 * @see confirm_form()
 */
function tripal_entities_entity_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  $form['#submit'][] = 'tripal_entities_entity_delete_form_submit';

  $form = confirm_form($form,
      t('Click the delete button below to confirm deletion of the record titled: %title',
          array('%title' => $entity->title)), 'admin/content/tripal_entity',
      '<p>' .t('This action cannot be undone.') .'</p>', t('Delete'), t('Cancel'), 'confirm');

  return $form;
}

/**
 * Submit callback for tripal_entity_delete_form
 */
function tripal_entities_entity_delete_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];

  $entity_controller = new TripalEntityController($entity->type);
  if ($entity_controller->delete($entity)) {
    drupal_set_message(t('The record title "%name" has been deleted.', array('%name' => $entity->title)));
    $form_state['redirect'] = 'admin/content/tripal_entitys';
  }
  else {
    drupal_set_message(t('The tripal_entity %name was not deleted.', array('%name' => $entity->title)), "error");
  }
}


/**
 * Implements hook_submit() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_add_bundle($cvterm) {

  // Create the bundle name and entity type name.  The bundle name is the
  // dbxref ID.  This isn't very human readable, but the alternative is to
  // use the accession which may not always be alpha-numeric.
  $bundle_name = 'dbxref_' . $cvterm->dbxref_id->dbxref_id;

  // Check to see if this bundle exists. If not then create it
  $bundle = db_select('tripal_bundle', 't')
    ->fields('t')
    ->condition('type', 'BioData')
    ->condition('bundle', $bundle_name)
    ->execute()
    ->fetchObject();

  if (!$bundle) {
    // The TripalBundle Entity manages the bundles we have available.
    // Therefore, we need to add a new entity for each bundle "type".
    $vals = array(
      'label' => $cvterm->name,
      'type' => 'BioData',
      'bundle' => $bundle_name,
      'data' => serialize(array()),
      'module' => 'tripal_entities'
    );
    $tripal_bundle = new TripalBundle($vals, 'BioData_bundles');
    $tripal_bundle->save();
  }

  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'BioData', $bundle_name, $cvterm);

}
/**
 * Implements hook_add_bundle_fields().
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $cvterm
 */
function tripal_entities_add_bundle_fields($entity_type_name, $bundle_name, $cvterm) {

  // Adds the fields for the base table to the entity.
  tripal_entities_add_bundle_base_fields($entity_type_name, $bundle_name, $cvterm);

  // Check to see if there are any kv-property tables associated to this
  // base table. If so, add the fields for that type of table.
  tripal_entities_add_bundle_kvproperty_adder_field($entity_type_name, $bundle_name, 'featureprop');
}

/**
 * Adds the fields for a kv-property table fields
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_entities_add_bundle_kvproperty_adder_field($entity_type_name, $bundle_name, $kv_table) {
  // First add a generic property field so that users can add new proeprty types.
  $field_name = $kv_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'kvproperty_adder',
    'widget_type' => 'tripal_fields_kvproperty_adder_widget',
    'field_settings' => array(),
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Properties',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}

/**
 * Adds the fields for the base table to the entity.
 */
function tripal_entities_add_bundle_base_fields($entity_type_name, $bundle_name, $cvterm) {
  // Get the list of tables where this cvterm is used.
  $match = array('cvterm_id' => $cvterm->cvterm_id);
  $term = chado_select_record('tripal_term', array('*'), $match);
  $values = array('term_id' => $term[0]->term_id);
  $tables = chado_select_record('tripal_term_usage', array('*'), $values);

  // Iterate through the tables.
  foreach ($tables as $table) {
    $table_name = $table->data_table;
    $type_table = $table->type_table;
    $type_field = $table->field;

    // We only want to look at base tables.
    if ($table_name == 'cvterm_dbxref' || $table_name == 'cvterm_relationship' ||
        $table_name == 'cvtermpath' || $table_name == 'cvtermprop' || $table_name == 'chadoprop' ||
        $table_name == 'cvtermsynonym' || preg_match('/_relationship$/', $table_name) ||
      preg_match('/_cvterm$/', $table_name)) {
      continue;
    }

    // Iterate through the columns of the table and see if fields have been
    // created for each one. If not, then create them.
    $schema = chado_get_schema($table_name);
    $columns = $schema['fields'];
    foreach ($columns as $column_name => $details) {
      $field_name = $table_name . '__' . $column_name;

      // Skip the primary key field.
      if ($column_name == $schema['primary key'][0]) {
        continue;
      }

      // Skip the type field.
      if ($table_name == $type_table and $column_name == $type_field) {
        continue;
      }

      // Get the field defaults for this column.
      $field_info = tripal_entities_get_table_column_field_default($table_name, $schema, $column_name);


      // Determine if the field is required.
      if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
        $field_info['is_required'] = array_key_exists('default', $details) ? 0 : 1;
      }

      // If we don't have a field type then we don't need to create a field.
      if (!$field_info['field_type']) {
        // If we don't have a field type but it is required and doesn't have
        // a default value then we are in trouble.
        if ($field_info['is_required'] and !array_key_exists('default', $details)) {
          throw new Exception(t('The %table.%field type, %type, is not yet supported for Entity fields, but it is required,',
              array('%table' => $table_name, '%field' => $column_name, '%type' => $details['type'])));
        }
        continue;
      }

      // If this field is a foreign key field then we will have a special custom
      // field provided by Tripal.
      $is_fk = FALSE;
      if (array_key_exists('foreign keys', $schema)) {
        foreach ($schema['foreign keys'] as $remote_table => $fk_details) {
          if (array_key_exists($column_name, $fk_details['columns'])) {
            $is_fk = TRUE;
          }
        }
      }

      // Add the field to the bundle.
      tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
    }
  }
}

/**
 * Returns a $field_info array for a field based on a databaes column.
 *
 */
function tripal_entities_get_table_column_field_default($table_name, $schema, $column_name) {
  $details = $schema['fields'][$column_name];

  // Create an array with information about this field.
  $field_info = array(
    'field_type' => '',
    'widget_type' => '',
    'field_settings' => array(
      'chado_table' => $table_name,
      'chado_column' => $column_name,
    ),
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
    'is_required' => 0,
  );

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textfield';
      $field_info['field_settings']['max_length'] = $details['length'];
      break;
    case 'varchar':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textfield';
      $field_info['field_settings']['max_length'] = $details['length'];
      break;
    case 'text':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textarea';
      $field_info['field_settings']['max_length'] = 17179869184;
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field_info['field_type'] = 'number_integer';
      $field_info['widget_type'] = 'number';
      break;
    case 'float':
      $field_info['field_type'] = 'number_float';
      $field_info['widget_type'] = 'number';
      $field_info['field_settings']['precision'] = 10;
      $field_info['field_settings']['scale'] = 2;
      $field_info['field_settings']['decimal_separator'] = '.';
      break;
    case 'numeric':
      $field_info['field_type'] = 'number_decimal';
      $field_info['widget_type'] = 'number';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field_info['field_type'] = 'list_boolean';
      $field_info['widget_type'] = 'options_onoff';
      $field_info['field_settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
      break;
    case 'datetime':
      // Use the Drupal Date and Date API to create the field/widget
      $field_info['field_type'] = 'datetime';
      $field_info['widget_type'] = 'date_select';
      $field_info['widget_settings']['increment'] = 1;
      $field_info['widget_settings']['tz_handling'] = 'none';
      $field_info['widget_settings']['collapsible'] = TRUE;

      // TODO: Add settings so that the minutes increment by 1.
      // And turn off the timezone, as the Chado field doesn't support it.
      break;
  }

  return $field_info;
}
