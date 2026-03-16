<?php

namespace Drupal\tripal_chado\Hook;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the Tripal Chado module.
 */
class TripalChadoHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal_chado module.
      case 'help.page.tripal_chado':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Chado integration for Tripal.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

  /**
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function rebuild(): string {
    \Drupal::service('tripal_chado.rebuild_service')->executeRebuild();
    // Return value of the module name is only used for phpunit tests.
    return 'tripal_chado';
  }

  /**
   * Implements hook_form_alter().
   */
  #[Hook('form_alter')]
  public function formAlter(array &$form, FormStateInterface $form_state, $form_id) {

    // If this is the field_config_edit_form and if we are adding a Chado
    // field, then we need to add a submit hook to set the base table in the
    // content type's 3rd party settings. The field settings form doesn't allow
    // us to set a submit callback, so we have to add it using hook_form_alter.
    if ($form_id == 'field_config_edit_form') {
      // If we have 'storge_plugin_settings' specifying a 'base_table', then
      // this is a Chado field and we should add the callback on submit so
      // that we can set the 3rd party settings on the content type.
      // Retrieve base_table from the form state, if present.
      $base_table = $form_state->getValue([
        'field_storage',
        'subform',
        'settings',
        'storage_plugin_settings',
        'base_table',
      ]);
      if ($base_table) {
        // Add submit callback.
        $form['actions']['submit']['#submit'][] = [
          'Drupal\tripal_chado\TripalField\ChadoFieldItemBase',
          'storageSettingsFormSubmitBaseTable',
        ];
      }
    }
  }

  /**
   * Implements hook_config_schema_info_alter().
   *
   * Specifically, we are altering config schema set in the tripal module.
   * We use this approach to ensure we are extending the existing schema
   * which makes these changes available to extension modules defining their
   * own yml files.
   */
  #[Hook('config_schema_info_alter')]
  public function configSchemaInfoAlter(&$definitions) {

    // Third party settings for entity types
    // (always add to both collection + content type entity).
    // -- Collection.
    $ttetc_mapping = $definitions['tripal.tripalentitytype_collection.*']['mapping']['content_types']['sequence']['mapping']['settings']['mapping']
      ?? [];
    $ttetc_mapping['chado_base_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Base Chado Table',
      'nullable' => TRUE,
    ];
    $ttetc_mapping['bundle_type_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Table Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $ttetc_mapping['bundle_type_column'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Column Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $definitions['tripal.tripalentitytype_collection.*']['mapping']['content_types']['sequence']['mapping']['settings']['mapping']
      = $ttetc_mapping;

    // -- Content Type Entity.
    $tct_mapping = $definitions['tripal.content_type.*']['mapping']['third_party_settings']['mapping']['tripal']['mapping']
      ?? [];
    $tct_mapping['chado_base_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Base Chado Table',
      'nullable' => TRUE,
    ];
    $tct_mapping['bundle_type_table'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Table Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $tct_mapping['bundle_type_column'] = [
      'type' => 'string',
      'label' => 'Entity Third Party Setting for Column Specifying Content CV Term',
      'nullable' => TRUE,
    ];
    $definitions['tripal.content_type.*']['mapping']['third_party_settings']['mapping']['tripal']['mapping']
      = $tct_mapping;

    // ADDITIONS TO TRIPAL FIELDS.
    $fste_mapping = $definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping']['storage_plugin_settings']['mapping']
      ?? [];
    $fste_mapping['base_table'] = [
      'type' => 'string',
      'label' => 'Field Base Chado Table',
      'nullable' => FALSE,
    ];
    $fste_mapping['base_column'] = [
      'type' => 'string',
      'label' => 'Field Base Chado Column',
      'nullable' => FALSE,
    ];
    // Specific to the schema__additional_type field.
    // Indicates the table where the type_id column is used to differentiate
    // the type.
    $fste_mapping['type_table'] = [
      'type' => 'string',
      'label' => 'Table containing Type ID',
      'nullable' => FALSE,
    ];
    // Indicates the column used as a foreign key to the cvterm table where
    // the type is linked.
    $fste_mapping['type_column'] = [
      'type' => 'string',
      'label' => 'Column Linking to CVTerm table',
      'nullable' => FALSE,
    ];
    // Specific to the chado_linker__prop field.
    // Indicates the table that contains the property columns.
    $fste_mapping['prop_table'] = [
      'type' => 'string',
      'label' => 'Table containing property columns',
      'nullable' => FALSE,
    ];
    // Specific to linker tables.
    $fste_mapping['linker_table'] = [
      'type' => 'string',
      'label' => 'The linking table used by the current field.',
      'nullable' => TRUE,
    ];
    $fste_mapping['linker_fkey_column'] = [
      'type' => 'string',
      'label' => 'The column in the linking table which is a foreign key pointing to the base table (e.g. for feature_synonym this would be feature_id).',
      'nullable' => TRUE,
    ];
    $fste_mapping['linking_method'] = [
      'type' => 'string',
      'label' => 'The path through which to link the base table to the fields linked table. For example to link "feature" to "contact", the linking method would be "feature_contact → contact_id".',
      'nullable' => TRUE,
    ];
    // Specific to relationship tables.
    $fste_mapping['subject_column'] = [
      'type' => 'string',
      'label' => 'The column in the relationship table which is a foreign key for the relationship subject pointing to the base table.',
      'nullable' => TRUE,
    ];
    $fste_mapping['object_column'] = [
      'type' => 'string',
      'label' => 'The column in the relationship table which is a foreign key for the relationship object pointing to the base table.',
      'nullable' => TRUE,
    ];
    $definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping']['storage_plugin_settings']['mapping']
      = $fste_mapping;

    // ADDITIONS TO TRIPAL FIELD COLLECTION -YML FIELD CREATION.
    // Adds the Base table and column information needed for all Chado fields.
    $ttfc_mapping = $definitions['tripal.tripalfield_collection.*']['mapping']['fields']['sequence']['mapping']['storage_settings']['mapping']['storage_plugin_settings']['mapping']
      ?? [];
    $ttfc_mapping['base_table'] = [
      'type' => 'string',
      'label' => 'Field Base Chado Table',
      'nullable' => FALSE,
    ];
    $ttfc_mapping['base_column'] = [
      'type' => 'string',
      'label' => 'Field Base Chado Column',
      'nullable' => FALSE,
    ];
    // Specific to the schema__additional_type field.
    // Indicates the table where the type_id column is used to differentiate
    // the type.
    $ttfc_mapping['type_table'] = [
      'type' => 'string',
      'label' => 'Table containing Type ID',
      'nullable' => FALSE,
    ];
    // Indicates the column used as a foreign key to the cvterm table where
    // the type is linked.
    $ttfc_mapping['type_column'] = [
      'type' => 'string',
      'label' => 'Column Linking to CVTerm table',
      'nullable' => FALSE,
    ];
    // Specific to the chado_linker__prop field.
    // Indicates the table that contains the property columns.
    $ttfc_mapping['prop_table'] = [
      'type' => 'string',
      'label' => 'Table containing property columns',
      'nullable' => FALSE,
    ];
    // Specific to linker tables.
    $ttfc_mapping['linker_table'] = [
      'type' => 'string',
      'label' => 'The linking table used by the current field.',
      'nullable' => TRUE,
    ];
    $ttfc_mapping['linker_fkey_column'] = [
      'type' => 'string',
      'label' => 'The column in the linking table which is a foreign key pointing to the base table (e.g. for feature_synonym this would be feature_id).',
      'nullable' => TRUE,
    ];
    $ttfc_mapping['linking_method'] = [
      'type' => 'string',
      'label' => 'The path through which to link the base table to the fields linked table. For example to link "feature" to "contact", the linking method would be "feature_contact → contact_id".',
      'nullable' => TRUE,
    ];
    // Specific to relationship tables.
    $ttfc_mapping['subject_column'] = [
      'type' => 'string',
      'label' => 'The column in the relationship table which is a foreign key for the relationship subject pointing to the base table.',
      'nullable' => TRUE,
    ];
    $ttfc_mapping['object_column'] = [
      'type' => 'string',
      'label' => 'The column in the relationship table which is a foreign key for the relationship object pointing to the base table.',
      'nullable' => TRUE,
    ];
    $definitions['tripal.tripalfield_collection.*']['mapping']['fields']['sequence']['mapping']['storage_settings']['mapping']['storage_plugin_settings']['mapping']
      = $ttfc_mapping;
  }

}
