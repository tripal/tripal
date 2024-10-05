<?php
namespace Drupal\tripal_layout\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityDisplayBase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\Core\Entity\Entity\EntityFormDisplay;

class TripalEntityUILayoutController extends ControllerBase {

  /**
   * FORM: Applies the default Tripal layout to a tripal entity form.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *  The TripalEntityType whose form display layout we want to apply the default
   *  tripal layout to. Note: The tripal layout specific to this TripalEntityType
   *  will be applied.
   * @return \Symfony\Component\HttpFoundation\Response
   *  Returns the action to take on the page; specifically, returns a redirect
   *  action to return to the "Manage Form Display" page for this TripalEntityType.
   */
  public function applyFormLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->getLabel();

    $successful = $this->applyLayout($tripal_entity_type, 'form');

    // And let the admin know we have.
    if ($successful === TRUE) {
      \Drupal::messenger()->addMessage(t(
        '%bundle @context Default Tripal Layout has been applied.',
        [
          '%bundle' => $bundle_label,
          '@context' => 'Page'
        ]
      ));
    } else {
      \Drupal::messenger()->addError(t(
        'Errors were encountered when attempting to apply the %bundle @context Default Tripal Layout!',
        [
          '%bundle' => $bundle_label,
          '@context' => 'Page'
        ]
      ));
    }

    return $this->redirect(
      'entity.entity_form_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * FORM: Removes all layout applied by this module to the tripal entity form.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *  The TripalEntityType whose form display layout we want to reset to
   *  it's defaults.
   * @return \Symfony\Component\HttpFoundation\Response
   *  Returns the action to take on the page; specifically, returns a redirect
   *  action to return to the "Manage Form Display" page for this TripalEntityType.
   */
  public function resetFormLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->label();

    $this->resetLayout($tripal_entity_type, 'form');

    // And let the admin know we have.
    \Drupal::messenger()->addMessage(t(
      '%bundle @context Display Layout has been reset to show all fields using their display defaults.',
      [
        '%bundle' => $bundle_label,
        '@context' => 'Form'
      ]
    ));

    return $this->redirect(
      'entity.entity_form_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * VIEW: Applies the default Tripal layout to a tripal entity view.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *  The TripalEntityType whose view display layout we want to apply the default
   *  tripal layout to. Note: The tripal layout specific to this TripalEntityType
   *  will be applied.
   * @return \Symfony\Component\HttpFoundation\Response
   *  Returns the action to take on the page; specifically, returns a redirect
   *  action to return to the "Manage Display" page for this TripalEntityType.
   */
  public function applyViewLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->label();

    $successful = $this->applyLayout($tripal_entity_type, 'view');

    // And let the admin know we have.
    if ($successful === TRUE) {
      \Drupal::messenger()->addMessage(t(
        '%bundle @context Default Tripal Layout has been applied.',
        [
          '%bundle' => $bundle_label,
          '@context' => 'Page'
        ]
      ));
    }
    else {
      \Drupal::messenger()->addError(t(
        'Errors were encountered when attempting to apply the %bundle @context Default Tripal Layout!',
        [
          '%bundle' => $bundle_label,
          '@context' => 'Page'
        ]
      ));
    }

    return $this->redirect(
      'entity.entity_view_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * VIEW: Removes all layout applied by this module to the tripal entity view.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *  The TripalEntityType whose view display layout we want to reset to
   *  it's defaults.
   * @return \Symfony\Component\HttpFoundation\Response
   *  Returns the action to take on the page; specifically, returns a redirect
   *  action to return to the "Manage Display" page for this TripalEntityType.
   */
  public function resetViewLayout($tripal_entity_type) {

    $bundle = $tripal_entity_type->id();
    $bundle_label = $tripal_entity_type->label();

    // Now reset the view layout.
    $this->resetLayout($tripal_entity_type, 'view');

    // And let the admin know we have.
    \Drupal::messenger()->addMessage(t(
      '%bundle @context Display Layout has been reset to show all fields using their display defaults.',
      [
        '%bundle' => $bundle_label,
        '@context' => 'Page'
      ]
    ));

    return $this->redirect(
      'entity.entity_view_display.tripal_entity.default',
      ['tripal_entity_type' => $bundle]
    );
  }

  /**
   * A generic funtion for getting a setting and providing the default if its not set.
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
   * Sets the display option for a component without affecting other options.
   *
   * Note: this will override an existing version of that option.
   *
   * @param string $component_name
   *   The name of the component you want to set a display option for.
   * @param string $option_key
   *   The key for the display option (e.g. 'weight')
   * @param mixed $option_value
   *   The value for the display option
   * @param EntityDisplayBase $display
   *   The display the component is in and that you want to set it for.
   * @return void
   */
  protected function setDisplayOption(string $component_name, string $option_key, mixed $option_value, EntityDisplayBase $display) {
    $options = $display->getComponent($component_name);
    $options[$option_key] = $option_value;
    $display->setComponent($component_name, $options);
  }

  /**
   * Adds a number of children components to a specific field group.
   *
   * @param array $children
   *   An array of the children of a field group as defined in the layout array.
   * @param string $group_name
   *   The name of the field group that you want to add the children to.
   * @param string $group_type
   *   The type of field group you are adding the children to.
   * @param EntityDisplayBase $display
   *   The display to are adding the children to a field group in.
   * @param string $bundle
   *   The TripalEntityType the display is for.
   * @return void
   */
  protected function setFieldGroupChildren(array $children, string $group_name, string $group_type, EntityDisplayBase $display, string $bundle) {

    /** @var \Drupal\Core\Entity\EntityFieldManager $entity_field_manager **/
    $entity_field_manager = \Drupal::service('entity_field.manager');

    // Note: we also want to set weight to keep things in the order they were
    // listed in the yaml. This will override weights set in another way.
    $current_weight = 0;

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
        /** @var \Drupal\field\Entity\FieldConfig $entity_field_def **/
        $entity_field_defs = $entity_field_manager->getFieldDefinitions('tripal_entity', $bundle);
        foreach ($entity_field_defs as $entity_field_def) {
          if ($entity_field_def->getType() == $child_type) {
            $field_name = $entity_field_def->getName();
            $this->setChild($field_name, $group_name, $display);
            // Set weight.
            $current_weight += 5;
            $this->setDisplayOption($field_name, 'weight', $current_weight, $display);
          }
        }
      }
      // We don't have a field type, so simply set the field instance.
      else {
        $field_name = $child;
        $this->setChild($field_name, $group_name, $display);
        // Set weight.
        $current_weight += 5;
        $this->setDisplayOption($field_name, 'weight', $current_weight, $display);
      }
    }
  }

  /**
   * Sets the child as a child of the provided parent in the supplied display.
   * For example, this may mean setting a field as a child of a field group.
   *
   * @param string $child
   *   The name of the child component.
   *   The child must be one of the following in order to be supported:
   *     - an enabled field component
   *     - a hidden field component
   *     - a field group
   * @param string $parent
   *   The name of the parent component.
   * @param EntityDisplayBase $display
   *   The display to set the child as a child of the parent in.
   */
  protected function setChild($child, $parent, EntityDisplayBase $display) {

    $field_groups = $display->getThirdPartySettings('field_group');

    // We need to know if this a field or a field group
    $is_field = FALSE;
    $is_field_group = FALSE;
    $components = $display->getComponents();
    $hidden = $display->get('hidden');

    if (in_array($child, array_keys($field_groups))) {
      $is_field_group = TRUE;
    }
    elseif (in_array($child, array_keys($components))) {
      $is_field = TRUE;
    }
    else if (in_array($child, $hidden)) {
      $is_field = TRUE;
    }

    // If this isn't a field or a field group it is something we
    // don't know how to handle.
    if (!$is_field and !$is_field_group) {
      return;
    }

    // First remove the component from where it may already be a child.
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
   * Adds all the field groups defined in a layout.
   *
   * Currently supports:
   *  - details
   *  - field_group_table
   *
   * @param array $field_groups
   *   An array of field groups defined in a layout.
   * @param EntityDisplayBase $display
   *   The display to add the field groups to.
   * @return void
   */
  protected function addFieldGroups($field_groups, $display) {

    // If there is not a weight set then we want to set one based on
    // order in the YAML file.
    $default_weight = 0;

    foreach ($field_groups as $field_group) {
      $default_weight++;

      $group_type = $field_group['type'];
      $group_name = $field_group['id'];
      $settings = $field_group;
      unset($settings['type'], $settings['id']);

      $settings['weight'] = $settings['weight'] ?? $default_weight;

      if ($group_type == 'details') {
        $this->addDetailsFieldGroup($group_name, $display, $settings);
      }
      if ($group_type == 'field_group_table') {
        $this->addTableFieldGroup($group_name, $display, $settings);
      }
    }
  }

  /**
   * Adds a 'details' field group.
   *
   * @param string $name
   *   The name of the field group component.
   * @param EntityDisplayBase $display
   *   The display configuration to add the field group component to.
   * @param array $settings
   *   An array of settings to apply to the field group.
   */
  protected function addDetailsFieldGroup($name, EntityDisplayBase $display, $settings = []) {
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
        "region" => $this->getSetting('region', 'content', $settings),
        "weight" => $this->getSetting('weight', 0, $settings),
        "format_type" => "details",
        "format_settings" => [
          "classes" => $classes,
          "show_empty_fields" => $this->getSetting('show_empty', FALSE, $settings),
          "id" => $name,
          "open" => $this->getSetting('open', FALSE, $settings),
          "description" => $this->getSetting('description', '', $settings),
        ],
      ];
      $display->setThirdPartySetting('field_group', $name, $details_group);
    }
  }

  /**
   * Adds a 'field_group_table' field group.
   *
   * @param string $name
   *   The name of the field group component.
   * @param EntityDisplayBase $display
   *   The display configuration to add the field group component to.
   * @param array $settings
   *   An array of settings to apply to the field group.
   */
  protected function addTableFieldGroup($name, EntityDisplayBase $display, $settings = []) {
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
        "region" => $this->getSetting('region', 'content', $settings),
        "weight" => $this->getSetting('weight', 0, $settings),
        "format_type" => "field_group_table",
        "format_settings" => [
          "show_empty_fields" => $this->getSetting('show_empty', FALSE, $settings),
          "label_visibility" => $this->getSetting('label_visibility', '1', $settings),
          "desc" => $this->getSetting('description', '', $settings),
          "desc_visibility" => $this->getSetting('desc_visibility', '1', $settings),
          "first_column" => $this->getSetting('first_column', '', $settings),
          "second_column" => $this->getSetting('second_column', '', $settings),
          "empty_label_behavior" => $this->getSetting('empty_label_behavior', '1', $settings),
          "table_row_striping" => $this->getSetting('table_row_striping', '1', $settings),
          "always_show_field_label" => $this->getSetting('always_show_field_label', '1', $settings),
          "empty_field_placeholder" => $this->getSetting('empty_field_placeholder', '', $settings),
          "id" => $name,
          "classes" => $classes,
          "always_show_field_value" => $this->getSetting('always_show_field_value', FALSE, $settings),
          "hide_table_if_empty" => $this->getSetting('hide_table_if_empty', FALSE, $settings)
        ]
      ];
      $display->setThirdPartySetting('field_group', $name, $table_group);
    }
  }

  /**
   * Sets all fields to be enabled in the form display.
   *
   * @param EntityDisplayBase $display
   */
  protected function enableAllComponents(EntityDisplayBase &$display) {
    $disabled_fields = $display->get('hidden');
    foreach ($disabled_fields as $field_name => $disabled) {
      $display->setComponent($field_name);
    }
  }

  /**
   * Resets all enabled components to their default display settings.
   *
   * @param EntityDisplayBase $display
   * @return void
   */
  protected function resetComponents(EntityDisplayBase &$display) {
    $display_context = $display->get('displayContext');

    // Get the field definitions so we can find defaults.
    $bundle = $display->getEntityType()->id();
    $entity_field_manager = \Drupal::service('entity_field.manager');
    $entity_field_defs = $entity_field_manager->getFieldDefinitions('tripal_entity', $bundle);

    $components = $display->getComponents();
    foreach ($components as $name => $current_display_options) {
      $default_display_options = [];
      // If the component is a field we can get its default display options.
      if (array_key_exists($name, $entity_field_defs)) {
        $entity_field_def = $entity_field_defs[$name];
        $default_display_options = $entity_field_def->getDisplayOptions($display_context);
      }
      // Lets also reset the weights all back to 0 unless there is a default.
      // This is needed because otherwise Drupal just keeps increasing them.
      $default_display_options['weight'] = $default_display_options['weight'] ?? 0;
      $display->setComponent($name, $default_display_options);
    }
  }

  /**
   * Hides the label of all the components listed.
   *
   * @param array $component_names
   *  A list of component names whose label we want to hide.
   * @param EntityDisplayBase $display
   *  Thne display the components belong to.
   * @return void
   */
  protected function hideComponentLabels(array $component_names, EntityDisplayBase $display) {
    $components = $display->getComponents();
    foreach ($component_names as $name) {
      if (array_key_exists($name, $components)) {
        $options = $components[$name];
        if (array_key_exists('label', $options)) {
          $options['label'] = 'hidden';
        }
        $display->setComponent($name, $options);
      }
    }
  }

  /**
   * Removes all field groups from the display
   *
   * @param EntityDisplayBase $display
   */
  protected function clearFieldGroups(EntityDisplayBase $display) {
    $field_groups = $display->getThirdPartySettings('field_group');
    foreach ($field_groups as $group_name => $group_details) {
      $display->unsetThirdPartySetting('field_group', $group_name);
    }
  }

  /**
   * Removes any field groups without children from the current display.
   *
   * @param EntityDisplayBase $display
   *   The display to act on (i.e. the current display)
   * @return void
   */
  protected function removeEmptyFieldGroups(EntityDisplayBase $display) {
    $field_groups = $display->getThirdPartySettings('field_group');
    foreach ($field_groups as $group_name => $group_details) {
      if (empty($group_details['children'])) {
        $display->unsetThirdPartySetting('field_group', $group_name);
      }
    }
  }

  /**
   * Hides a field from display
   *
   * @param string $name
   *   The name of the field component
   * @param EntityDisplayBase $display
   *   The display configuration.
   */
  protected function hideComponents(array $names, EntityDisplayBase $display) {
    foreach ($names as $name) {
      $display->removeComponent($name);
    }
  }

  /**
   * Retreives the layout configuration for the given bundle.
   *
   * @param string $bundle
   *  The TripalEntityType to return the display for.
   * @param string $display_context
   *   One of 'view' or 'form' depending on whether you want the display for
   *   the page view display or the form display.
   * @return array
   *   The layout configuration if there is one, the first layout if there are
   *   multiple for the TripalEntityType and FALSE if there are none.
   */
  protected function getLayout(string $bundle, string $display_context) {
    $bundle_layouts = [];

    if ($display_context === 'view') {
      $config_entity_id = 'tripal_layout_default_view';
    } elseif ($display_context === 'form') {
      $config_entity_id = 'tripal_layout_default_form';
    } else {
      throw new \Exception("Unable to load the layout for $bundle [$display_context] as only 'view' and 'form' are supported.");
    }


    // Get all the layout entities of this type.
    $entities = \Drupal::entityTypeManager()
      ->getStorage($config_entity_id)
      ->loadByProperties([]);

    // Iterate through them and find those that have a layout for this bundle.
    foreach($entities as $entity) {
      if ($entity->hasLayout($bundle)) {
        $config_id = $entity->id();
        $bundle_layouts[$config_id] = $entity->getLayout($bundle);
      }
    }

    if (count($bundle_layouts) == 0) {
      \Drupal::messenger()->addError(t('No default layouts could be found for this content type.'));
      return FALSE;
    }
    if (count($bundle_layouts) > 1) {
      \Drupal::messenger()->addWarning(t('There are multiple layouts for the same content type. Selecting the first. @layouts'), ['@layouts' => print_r($bundle_layouts, TRUE)]);
    }

    return array_values($bundle_layouts)[0];
  }

  /**
   * Loads the EntityDisplay for a specific tripal content type and display context.
   *
   * @param TripalEntityType $tripal_entity_type
   *   The bundle whose display is being managed (e.g. organism)
   * @param string $display_context
   *   One of 'view' or 'form' depending on whether the display to
   *   be loaded is for the page view display or the form display.
   *
   * @return EntityDisplayBase $display
   *   The display requested to be loaded as defined by the parameters. This will
   *   be of type EntityDisplayBase for 'view' display context or
   *   EntityFormDisplay for 'form' display context.
   */
  protected function loadDisplay(TripalEntityType $tripal_entity_type, string $display_context) {

    $bundle = $tripal_entity_type->id();
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    $entity_type_manager = \Drupal::service('entity_type.manager');

    if ($display_context === 'view') {
      /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_entity_storage **/
      $config_entity_storage = $entity_type_manager->getStorage('entity_view_display');
    } elseif ($display_context === 'form') {
      /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_entity_storage **/
      $config_entity_storage = $entity_type_manager->getStorage('entity_form_display');
    } else {
      throw new \Exception("Unable to load the default display for $bundle [$display_context] as only 'view' and 'form' are supported.");
    }

    /** @var \Drupal\Core\Entity\Entity\EntityDisplayBase $display **/
    $display = $config_entity_storage->load('tripal_entity.' . $bundle . '.default');

    return $display;
  }

  /**
   * Clears any field groups, enables all fields and resets them
   * to their default display options.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *   The bundle whose display is being managed (e.g. organism)
   * @param string $display_context
   *   One of 'view' or 'form' depending on whether the display to
   *   be reset is for the page view display or the form display.
   * @param EntityDisplayBase $display
   *   The display to be reset. This is optional and will be loaded
   *   based on the first two parameters if not supplied.
   */
  public function resetLayout(TripalEntityType $tripal_entity_type, string $display_context, EntityDisplayBase $display = NULL) {

    // Load the display if it was not provided.
    if ($display === NULL) {
      $display = $this->loadDisplay($tripal_entity_type, $display_context);
    }

    // Now reset the display.
    $this->clearFieldGroups($display);
    $this->enableAllComponents($display);
    $this->resetComponents($display);

    // And save it.
    $display->save();
  }

  /**
   * Applies the default Tripal layout to a tripal entity.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *   The bundle whose display is being managed (e.g. organism)
   * @param string $display_context
   *   One of 'view' or 'form' depending on whether the display to
   *   apply the tripal layout to is for the page view display or the form
   *   display.
   * @param EntityDisplayBase $display
   *   The display to modified. This is optional and will be loaded
   *   based on the first two parameters if not supplied.
   * @return bool
   *   TRUE if the layout was applied successfully and FALSE otherwise.
   */
  protected function applyLayout(TripalEntityType $tripal_entity_type, string $display_context, EntityDisplayBase $display = NULL) {

    $bundle = $tripal_entity_type->id();

    // Load the display if it was not provided.
    if ($display === NULL) {
      $display = $this->loadDisplay($tripal_entity_type, $display_context);
    }

    // First reset the display.
    $this->resetLayout($tripal_entity_type, $display_context, $display);

    // Get the layout for this bundle.
    $layout = $this->getLayout($bundle, $display_context);
    if (!$layout) {
      return FALSE;
    }

    // If there are field group definitinos then create those.
    if (array_key_exists('field_groups', $layout)) {

      $this->addFieldGroups($layout['field_groups'], $display);

      // Now set the children for each field group.
      foreach ($layout['field_groups'] as $field_group) {
        $group_type = $field_group['type'];
        $group_name = $field_group['id'];
        $settings = $field_group;
        unset($settings['type'], $settings['id']);
        $this->setFieldGroupChildren($settings['children'], $group_name, $group_type, $display, $bundle);

        // We want to hide the label for all fields in a field_group_table.
        if ($group_type == 'field_group_table') {
          $this->hideComponentLabels($settings['children'], $display);
        }
      }
    }

    // Now hide any fields that should be hidden.
    if (array_key_exists('hidden', $layout) && is_array($layout['hidden']) && !empty($layout['hidden'])) {
      $this->hideComponents($layout['hidden'], $display);
    }

    // Now as the last step we should remove any field groups that never
    // did have children added to them. This happens when the YAML indicates a
    // field type as the child and there are no implementations of that type
    // for this display.
    $this->removeEmptyFieldGroups($display);
    // Do this two times for the case where there are empty field group tables
    // that result in empty field group details only after being removed.
    $this->removeEmptyFieldGroups($display);

    // Save all of the changes to the display.
    $display->save();

    return TRUE;
  }
}
