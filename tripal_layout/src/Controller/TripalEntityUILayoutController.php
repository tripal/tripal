<?php
namespace Drupal\tripal_layout\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\Entity\EntityViewDisplay;

class TripalEntityUILayoutController extends ControllerBase {

  /**
   * FORM: Applies the default Tripal layout to a tripal entity form.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   */
  public function applyFormLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->getLabel();

    \Drupal::messenger()->addMessage(t('Not Yet Implemented.'));

    return $this->redirect(
      'entity.entity_form_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * FORM: Removes all layout applied by this module to the tripal entity form.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   */
  public function resetFormLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->getLabel();

    \Drupal::messenger()->addMessage(t('Not Yet Implemented.'));

    return $this->redirect(
      'entity.entity_form_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * DISPLAY: Applies the default Tripal layout to a tripal entity view.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   */
  public function applyLayout($tripal_entity_type) {

    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_entity_storage **/
    /** @var \Drupal\Core\Entity\Entity\EntityViewDisplay $display **/
    /** @var \Drupal\Core\Entity\EntityFieldManager $entity_field_manager **/
    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->getLabel();
    $entity_type_manager = \Drupal::service('entity_type.manager');
    $config_entity_storage = $entity_type_manager->getStorage('entity_view_display');
    $display = $config_entity_storage->load('tripal_entity.' . $bundle . '.default');
    $entity_field_manager = \Drupal::service('entity_field.manager');

    // First reset the display.
    $this->clearFieldGroups($display);
    $this->unhideAll($display);

    // Get the layout for this bundle.
    $bundle_layouts = $this->getLayout($bundle);
    if (count($bundle_layouts) == 0) {
      \Drupal::messenger()->addWarning(t('No default layouts could be found for this content type.'));
      return $this->redirect(
        'entity.entity_view_display.tripal_entity.default',
        ['tripal_entity_type' => $bundle]
      );
    }
    if (count($bundle_layouts) > 1) {
      \Drupal::messenger()->addWarning(t('There are multiple layouts for the same content type. '
      . 'Selecting the first. @layouts'), ['@layouts' => print_r($bundle_layouts, TRUE)]);
    }
    $layout =  array_values($bundle_layouts)[0];

    // If there are field group definitinos then create those.
    if (array_key_exists('field_groups', $layout)) {

      // First, create the field groups
      foreach ($layout['field_groups'] as $group_type => $field_groups) {
        if ($group_type == 'details') {
          foreach ($field_groups as $group_name => $settings) {
            $this->addDetailsFieldGroup($group_name, $display, $settings);
          }
        }
        if ($group_type == 'field_group_table') {
          foreach ($field_groups as $group_name => $settings) {
            $this->addTableFieldGroup($group_name, $display, $settings);
          }
        }
      }

      // Now set the children.
      foreach ($layout['field_groups'] as $group_type => $field_groups) {
        foreach ($field_groups as $group_name => $settings) {
          $children = $settings['children'];
          foreach ($children as $child) {
            // Prevent the case where the setup accidently sets the parent
            // as a child of itself.
            if ($child == $group_name) {
              \Drupal::messenger()->addWarning(t(
                'Please check the layout configuration.'
                . 'It is trying to set a element to be a child of itself: @group_name == @child',
                ['@group_name' => $group_name, '@child' => $child]
              ));
              continue;
            }

            // Before adding the child we need to distinguish between a
            // field instance name and a field type name.  The latter begins
            // with 'type:'.  If the former, we can simply add the child. If
            // the latter then we have to find all of the fields of the given
            // field type and then add each one at a time.
            $matches = [];
            if (preg_match('/^type:(.+)$/', $child, $matches)) {
              $child_type = $matches[1];

              // Get the fields of this bundle and if any match the type
              // then set the child.
              $entity_field_defs = $entity_field_manager->getFieldDefinitions('tripal_entity', $bundle);
              /** @var \Drupal\field\Entity\FieldConfig $entity_field_def **/
              foreach ($entity_field_defs as $entity_field_def) {
                if ($entity_field_def->getType() == $child_type) {
                  $this->setChild($entity_field_def->getName(), $group_name, $display);
                }
              }
            }
            // We don't have a field type, so simply set the field instance.
            else {
              $this->setChild($child, $group_name, $display);
            }
          }
        }
      }
    }

    // Now hide any fields that should be hidden.
    foreach ($layout['hidden'] as $field_name) {
      $this->hideComponent($field_name, $display);
    }

    // Hide the labels of all fields.
    $components = $display->getComponents();
    foreach ($components as $component_name => $options) {
      $options['label'] = 'hidden';
      $display->setComponent($component_name, $options);
    }

    // Save all of the changes to the display.
    $display->save();

    \Drupal::messenger()->addMessage(t('The default Tripal layout has been set for this content type and saved.'));

    return $this->redirect(
      'entity.entity_view_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * DISPLAY: Removes all layout applied by this module to the tripal entity view.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   */
  public function resetLayout($tripal_entity_type) {
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_entity_storage **/
    /** @var \Drupal\Core\Entity\Entity\EntityViewDisplay $display **/
    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->getLabel();
    $entity_type_manager = \Drupal::service('entity_type.manager');
    $config_entity_storage = $entity_type_manager->getStorage('entity_view_display');
    $display = $config_entity_storage->load('tripal_entity.' . $bundle . '.default');

    // First reset the display.
    $this->clearFieldGroups($display);
    $this->unhideAll($display);

    // Set the component label's back to above.
    $components = $display->getComponents();
    foreach ($components as $component_name => $options) {
      $options['label'] = 'above';
      $display->setComponent($component_name, $options);
    }

    // Save all of the changes to the display.
    $display->save();

    \Drupal::messenger()->addMessage(t('Layout has been reset and saved.'));

    return $this->redirect(
      'entity.entity_view_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * A generic funtion for getting a settings for addDetailsFieldGroup().
   *
   * @param string $name
   *   The setting name
   * @param unknown $default
   *   The default value if no setting is set.
   * @param array $settings
   *   The list of settings.
   *
   * @return unknown
   */
  protected function getSetting($name, $default, $settings = []) {
    $value = array_key_exists($name, $settings) ? $settings[$name] : $default;

    return $value;
  }

  /**
   *
   * @param unknown $name
   * @param unknown $parent
   * @param EntityViewDisplay $display
   */
  protected function setChild($child, $parent, EntityViewDisplay $display) {

    $field_groups = $display->getThirdPartySettings('field_group');

    // We need to know if this a field or a field group
    $is_field = FALSE;
    $is_field_group = FALSE;
    $components = $display->getComponents();
    $hidden = $display->get('hidden');

    if (in_array($child, array_keys($components))) {
      $is_field = TRUE;
    }
    else if (in_array($child, $hidden)) {
      $is_field = TRUE;
    }
    else if (in_array($child, array_keys($field_groups))) {
      $is_field_group = TRUE;
    }

    // If this isn't a field or a field group it is something we
    // don't know how to handle.
    if (!$is_field and !$is_field_group) {
      return;
    }

    // First remove the component from where it may already be a child.
    $field_groups = $display->getThirdPartySettings('field_group');
    foreach ($field_groups as $group_name => $group_details) {
      if (is_array($group_details['children'])) {
        if (in_array($child, $group_details['children'])) {
          unset($group_details['children'][$child]);
          $display->setThirdPartySetting('field_group', $group_name, $group_details);
        }
      }
    }

    // If this is a field group then be sure to set it's parent.
    if ($is_field_group) {
      $child_group_details = $field_groups[$child];
      $child_group_details['parent_name'] = $parent;
      $display->setThirdPartySetting('field_group', $child, $child_group_details);
    }

    // Now add it the child to the parent.
    $parent_group_details = $field_groups[$parent];
    if (!in_array($child, $parent_group_details['children'])) {
      $parent_group_details['children'][] = $child;
      $display->setThirdPartySetting('field_group', $parent, $parent_group_details);
    }
  }


  /**
   *
   * @param unknown $name
   * @param EntityViewDisplay $dispaly
   * @param array $settings
   */
  protected function addTableFieldGroup($name, EntityViewDisplay $display, $settings = []) {
    $field_groups = $display->getThirdPartySettings('field_group');

    // If the field group doesn't exist then add it.
    if (!array_key_exists($name, array_keys($field_groups))) {

      $classes = $this->getSetting('classes', '', $settings);
      $classes .= 'tripal-layout-table';

      $parent_name = $this->getSetting('parent_name', '', $settings);
      $table_group = [
        "children" => [],
        "label" => $this->getSetting('label', 'Missing Label', $settings),
        "parent_name" => $parent_name,
        "region" => "content",
        "weight" => $this->getSetting('weight', 0, $settings),
        "format_type" => "field_group_table",
        "format_settings" => [
          "show_empty_fields" => $this->getSetting('show_empty', FALSE, $settings),
          "label_visibility" => "1",
          "desc" => "",
          "desc_visibility" => "1",
          "first_column" => "",
          "second_column" => "",
          "empty_label_behavior" => "1",
          "table_row_striping" => "1",
          "always_show_field_label" => "1",
          "empty_field_placeholder" => "",
          "id" => "",
          "classes" => $classes,
          "always_show_field_value" => 0,
          "hide_table_if_empty" => 0
        ]
      ];
      $display->setThirdPartySetting('field_group', $name, $table_group);
    }
  }

  /**
   * Sets all fields for display.
   *
   * @param EntityViewDisplay $display
   */
  protected function unhideAll(EntityViewDisplay $display) {
     $display->set('hidden', []);
  }

  /**
   * Removes all field groups from the display
   *
   * @param EntityViewDisplay $display
   */
  protected function clearFieldGroups(EntityViewDisplay $display) {
    $field_groups = $display->getThirdPartySettings('field_group');
    foreach ($field_groups as $group_name => $group_details) {
      $display->unsetThirdPartySetting('field_group', $group_name);
    }
  }

  /**
   * Adds a field group.
   *
   * @param string $name
   *   The name of the field component
   * @param EntityViewDisplay $display
   *   The display configuration.
   */
  protected function addDetailsFieldGroup($name, EntityViewDisplay $display, $settings = []) {
    $field_groups = $display->getThirdPartySettings('field_group');

    // If the field group doesn't exist then add it.
    if (!array_key_exists($name, array_keys($field_groups))) {

      $classes = $this->getSetting('classes', '', $settings);
      $classes .= 'tripal-layout-details';

      $parent_name = $this->getSetting('parent_name', '', $settings);
      $details_group = [
        "children" => [],
        "label" => $this->getSetting('label', 'Missing Label', $settings),
        "parent_name" => $parent_name,
        "region" => "content",
        "weight" => $this->getSetting('weight', 0, $settings),
        "format_type" => "details",
        "format_settings" => [
          "classes" => $classes,
          "show_empty_fields" => $this->getSetting('show_empty', FALSE, $settings),
          "id" => "",
          "open" => $this->getSetting('open', FALSE, $settings),
          "description" => "",
        ],
      ];
      $display->setThirdPartySetting('field_group', $name, $details_group);
    }
  }

  protected function addFieldGroupChild($name, EntityViewDisplay $display) {

  }
  /**
   * Hides a field from display
   *
   * @param string $name
   *   The name of the field component
   * @param EntityViewDisplay $display
   *   The display configuration.
   */
  protected function hideComponent($name, EntityViewDisplay $display) {
    $components = $display->getComponents();
    $hidden = $display->get('hidden');
    if (in_array($name, array_keys($components))) {
      $display->removeComponent($name);
    }
    $hidden[$name] = TRUE;
    $display->set('hidden', $hidden);
  }

  /**
   * Retreives the layout configuration for the given bundle.
   *
   * @param string $bundle
   * @return array
   *   The layout configuration.
   */
  protected function getLayout($bundle) {
    $bundle_layouts = [];
    $config_factory = \Drupal::service('config.factory');
    $config_list = $config_factory->listAll('tripal_layout.tripal_layout_default_view');

    // Iterate throught the configuration files and find those that have
    // a layout for this bundle.
    foreach($config_list as $config_item) {
      $config = $config_factory->get($config_item);
      $layouts = $config->get('layouts');
      foreach ($layouts as $layout) {
        if ($layout['tripal_entity_type'] == $bundle) {
          $bundle_layouts[$config_item] = $layout;
        }
      }
    }

    return $bundle_layouts;
  }
}
