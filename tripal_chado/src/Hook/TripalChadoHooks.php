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
  public function onRebuild(): void {
    $rebuild_service = \Drupal::service('tripal_chado.rebuild_service');
    $rebuild_service->executeRebuild();
  }

  /**
   * Implements hook_form_alter().
   */
  #[Hook('form_alter')]
  public function alter(array &$form, FormStateInterface $form_state, $form_id) {

    // If this is the field_config_edit_form and if we are adding a Chado
    // field, then we need to add a submit hook to set the base table in the
    // content type's 3rd party settings. The field settings form doesn't allow
    // us to set a submit callback, so we have to add it using hook_form_alter.
    if ($form_id == 'field_config_edit_form') {
      // If we have 'storge_plugin_settings' specifying a 'base_table', then
      // this is a Chado field and we should add the callback on submit so
      // that we can set the 3rd party settings on the content type.
      // Retrieve base_table from the form state, if present.
      $base_table = $form_state->getValue(['field_storage', 'subform', 'settings', 'storage_plugin_settings', 'base_table']);
      if ($base_table) {
        // Add submit callback.
        $form['actions']['submit']['#submit'][] = [
          'Drupal\tripal_chado\TripalField\ChadoFieldItemBase',
          'storageSettingsFormSubmitBaseTable',
        ];
      }
    }
  }

}
