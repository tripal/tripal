<?php

namespace Drupal\tripal\Hook;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Render\Markup;
use Drupal\tripal\Entity\TripalEntity;

/**
 * Implements hooks from the Drupal Entity API and focused on Tripal Content.
 */
class TripalEntityHooks {

  /**
   * Finish loading TripalEntities via the TripalStorage plugin API.
   *
   * This is called during SqlContentStorage::doLoadMultiple() and only for
   * entities queried from the database. After this point the entities will
   * be added to the persistent cache until they are invalidated.
   *
   * @param array $entities
   *   An array of TripalEntity objects to finish loading.
   */
  #[Hook('tripal_entity_storage_load')]
  public function tripalEntityStorageLoad(array &$entities) {

    // Iterate through the entities provided.
    foreach ($entities as &$entity) {

      // Ensure that the Tripal Fields are registered for this entity.
      $entity->registerAllTripalFields();

      // @todo it would be great to skip the entity entirely if it is
      // fully cached in Drupal.
      // Create a values array appropriate for `loadValues()`
      [$values, $tripal_storages] = TripalEntity::getValuesArray($entity, TRUE);

      // Only do the following if there are any values to load.
      if (!empty($values)) {
        // Call the loadValues() function for each storage type.
        $load_success = FALSE;
        foreach ($values as $tsid => $tsid_values) {
          try {
            // If this storage backend is cache-aware then only the values for
            // fields which have un-cached properties will be loaded here.
            $load_success = $tripal_storages[$tsid]->loadValues($tsid_values);
            if ($load_success) {
              $values[$tsid] = $tsid_values;
            }
          }
          catch (\Exception $e) {
            \Drupal::logger('tripal')->notice($e->getMessage());
            \Drupal::messenger()->addError('Cannot load the entity. See the recent ' .
                'logs for more details or contact the site administrator if you cannot ' .
                'view the logs.');
          }
        }

        TripalEntity::saveValuesArray($entity, $values, $tripal_storages);
      }
    }
  }

  /**
   * Implements hook_preprocess_field().
   *
   * This is to permit HTML markup in the page title field, for
   * example on an organism page, to display genus and species
   * in italics.
   */
  #[Hook('preprocess_field')]
  public function preprocessField(&$variables) {
    if ($variables['element']['#field_name'] == 'title') {
      // We can configure which tags are allowed at /admin/tripal/config.
      $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags');
      $tripal_allowed_tags = explode(' ', $tag_string ?? '');

      // Process each item (usually only a single one).
      foreach ($variables['items'] as $delta => $item) {
        // The title can be either a simple inline template or a link.
        if ($item['content']['#type'] == 'inline_template') {
          $value = $item['content']['#context']['value'];
          // Convert strings into markup, this will cause HTML tags to
          // be rendered.
          if (is_string($value)) {
            $sanitized_value = Xss::filter($value, $tripal_allowed_tags);
            $variables['items'][$delta]['content']['#context']['value'] = Markup::create($sanitized_value);
          }
        }
        elseif ($item['content']['#type'] == 'link') {
          $value = $item['content']['#title']['#context']['value'];
          // Convert strings into markup, this will cause HTML tags to
          // be rendered.
          if (is_string($value)) {
            $sanitized_value = Xss::filter($value, $tripal_allowed_tags);
            $variables['items'][$delta]['content']['#title']['#context']['value'] = Markup::create($sanitized_value);
          }
        }
      }
    }
  }

  /**
   * Implements hook_config_schema_info_alter().
   *
   * Adds Tripal-specific field, storage, and formatter settings to Drupal's
   * field schema definitions.
   *
   * Drupal's config schema system for fields is *type-based* and *polymorphic*.
   * Settings for fields, field storage, and field formatters are not defined
   * directly on the field config objects, but instead delegated to schema types
   * such as:
   *
   * - field.settings.[field_type]
   * - field.storage_settings.[field_type]
   * - field.formatter.settings.[formatter_plugin]
   *
   * These base schema definitions are resolved dynamically based on the field
   * or formatter type. Because of this, it is NOT safe to override the generic
   * wildcard definitions (e.g. `field.settings.*` or
   * `field.formatter.settings.*`) with a custom mapping, as doing so would
   * replace Drupal core's schema and remove validation for existing settings.
   *
   * Additionally, Drupal's schema system does not support composition or reuse
   * of mapping definitions within YAML (i.e., there is no way to "inject" a
   * shared schema definition into an existing mapping via configuration alone).
   *
   * To work around this limitation, Tripal defines reusable schema fragments
   * (`tripal.field_settings`, `tripal.field_storage_settings`, etc.) and then
   * programmatically merges those definitions into the appropriate Drupal
   * field-type-specific schema definitions here.
   *
   * This approach:
   * - Preserves Drupal core and contrib schema definitions
   * - Avoids overriding polymorphic schema resolution
   * - Allows Tripal to define a shared set of settings form many field types
   * - Ensures proper schema validation in strict contexts (e.g., tests)
   *
   * Note that this modification applies to all field types of a given plugin,
   * as Drupal's schema system does not allow scoping schema changes to a
   * specific entity type (such as tripal_entity).
   */
  #[Hook('config_schema_info_alter')]
  public function configSchemaInfoAlter(&$definitions) {

    // Get the core Tripal-specific schema definitions.
    $field_storage_settings = $definitions['tripal.core.field_storage_settings']['mapping'] ?? [];
    $field_settings = $definitions['tripal.core.field_settings']['mapping'] ?? [];
    $field_widget_settings = $definitions['tripal.core.field_widget_settings']['mapping'] ?? [];
    $field_formatter_settings = $definitions['tripal.core.field_formatter_settings']['mapping'] ?? [];

    // Now loop through all schema definitions and merge the Tripal-specific
    // settings into the appropriate field-related definitions.
    // NOTE: This will apply to all field types, not just those used by Tripal,
    // because Drupal's schema system does not allow scoping to specific
    // entity types.
    foreach ($definitions as $key => &$definition) {
      if (str_starts_with($key, 'field.storage_settings.') && isset($definition['mapping'])) {
        $definition['mapping'] += $field_storage_settings;
      }
      elseif (str_starts_with($key, 'field.field_settings.') && isset($definition['mapping'])) {
        $definition['mapping'] += $field_settings;
      }
      elseif (str_starts_with($key, 'field.widget.settings.') && isset($definition['mapping'])) {
        $definition['mapping'] += $field_widget_settings;
      }
      elseif (str_starts_with($key, 'field.formatter.settings.') && isset($definition['mapping'])) {
        $definition['mapping'] += $field_formatter_settings;
      }
    }

  }

}
