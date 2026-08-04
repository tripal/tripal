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

  }

  /**
   * Implements hook_field_formatter_info_alter().
   */
  #[Hook('field_formatter_info_alter')]
  function fieldFormatterInfoAlter(array &$info) {
    // Allows us the option to use Drupal's formatter for real numbers.
    if (isset($info['number_decimal'])) {
      $info['number_decimal']['field_types'][] = 'chado_real_type_default';
    }
    if (isset($info['number_float'])) {
      $info['number_float']['field_types'][] = 'chado_real_type_default';
    }
  }

}
