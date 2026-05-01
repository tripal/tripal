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
   * Specifically, we are altering config schema set in the tripal module.
   * We use this approach to ensure we are extending the existing schema
   * which makes these changes available to extension modules defining their
   * own yml files.
   */
  #[Hook('config_schema_info_alter')]
  public function configSchemaInfoAlter(&$definitions) {

    // print_r(array_keys($definitions));
    // Temporary fix for Issue #1999.
    // We will collect the storage settings for all non-tripal fields and
    // manually add them to the field storage definition for tripal_entity.
    // NOTE: there are some conflicts between drupal field settings, which is
    // why this is only a temporary fix. For now, we will manually skip any
    // settings which are known to be different.
    $skipped_settings = ['allowed_values'];
    // Now, for each field storage definition, we will check for settings...
    foreach ($definitions as $key => $field_settings) {
      if (str_starts_with($key, 'field.storage_settings.') && !str_starts_with($key, 'field.storage.tripal_entity.')) {
        // If this field doesn't have any settings, we can skip it.
        if (!array_key_exists('mapping', $field_settings)) {
          continue;
        }
        // For each setting this field defines, see if we have it in our
        // tripal_entity field storage definition. If not, add it.
        foreach ($field_settings['mapping'] as $setting_key => $setting) {
          if (!isset($definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping'][$setting_key])) {
            // -- on entity.
            $definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping'][$setting_key] = $setting;
            // -- on field collection yaml.
            $definitions['tripal.tripalfield_collection.*']['mapping']['fields']['sequence']['mapping']['storage_settings']['mapping'][$setting_key] = $setting;
          }
          elseif (!in_array($setting_key, $skipped_settings)) {
            // If the setting already exists, we should check to make sure it
            // is the same. If not, we should log a warning.
            if ($definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping'][$setting_key] != $setting) {
              $field_name = str_replace('field.storage_settings.', '', $setting_key);
              throw new \Exception(
                "The field storage setting $setting_key for field $field_name is different in the tripal_entity field storage definition than in the field storage definition for $field_name. This may cause issues with field storage and retrieval. Please check the field storage definitions for both tripal_entity and $field_name to ensure they are consistent. Tripal Entity definition: " . print_r($definitions['field.storage.tripal_entity.*']['mapping']['settings']['mapping'][$setting_key], TRUE) . " Field definition: " . print_r($setting, TRUE)
              );
            }
          }
        }
      }
    }
  }

}
