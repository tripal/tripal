<?php
namespace Drupal\tripal_layout\Controller;

use Drupal\Core\Controller\ControllerBase;

class TripalEntityUILayoutController extends ControllerBase {

  /**
   *
   * @param unknown $tripal_entity_type
   * @return unknown
   */
  public function applyLayout($tripal_entity_type) {
    $bundle_name = $tripal_entity_type->id();
    $term = $tripal_entity_type->getTerm();

    // As a sanity check make sure we have a default view.
    /** @var Drupal\Core\Entity\EntityDisplayRepository $displays **/
    $displays = \Drupal::service('entity_display.repository');
    $view_modes = $displays->getViewModeOptionsByBundle('tripal_entity', $bundle_name);
    if (!in_array('default', array_keys($view_modes))) {
      \Drupal::messenger()->addWarning(t('Cannot apply the layout. The default view mode is not present.'));
      return $this->redirect('entity.entity_view_display.tripal_entity.default',
          ['tripal_entity_type' => $bundle_name]);
    }

    // Get the field manager, field definitions for the bundle type, and
    // the field type manager.
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle_name);
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    /** @var \Drupal\Core\Entity\Entity\EntityViewDisplay $entity_display **/
    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_entity_storage **/
    $entity_type_manager = \Drupal::service('entity_type.manager');
    $config_entity_storage = $entity_type_manager->getStorage('entity_view_display');
    $entity_display = $config_entity_storage->load('tripal_entity.' . $bundle_name . '.default');
    dpm($entity_display->getThirdPartySettings('field_group'));

    // Iterate over the field definitions for the bundle and collect the
    // information so we can use it later.
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    foreach ($field_defs as $field_name => $field_definition) {
      if (is_a($field_definition, 'Drupal\field\Entity\FieldConfig')) {

      }
    }

    return $this->redirect('entity.entity_view_display.tripal_entity.default',
        ['tripal_entity_type' => $bundle_name]);
  }

}