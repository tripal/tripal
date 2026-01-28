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

      // Ensure that the TripalStorage backends are setup.
      // @debug print "\n\nIN tripalEntityStorageLoad()!!!\n\n";
      // Now that we have the entity created, lets initialize TripalStorage
      // for all of the fields so it can be cached locally.
      $entity->registerAllTripalFields();

      // @debug print "\nWe now have " . count($entity->tripal_storages) . " TripalStorage backends setup in LOAD.\n";
      $tripal_storages = $entity->tripal_storages;

      // Create a values array appropriate for `loadValues()`
      $values = TripalEntity::getValuesArray($entity);

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

}
