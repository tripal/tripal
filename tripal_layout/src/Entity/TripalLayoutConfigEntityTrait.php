<?php

namespace Drupal\tripal_layout\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Provides common functionality for the TripalLayoutDefaultView and
 * TripalLayoutDefaultForm config entities.
 */
trait TripalLayoutConfigEntityTrait {

  /**
   * The collection of layouts keyed by bundle.
   * This is calculated from $layouts.
   *
   * @var array
   */
  protected ?array $bundle_layouts = NULL;


  /**
   * Retrieve all the layouts defined by this entity.
   *
   * @return array
   *   An list of all the layouts defined by this entity.
   */
  public function getLayouts() {
    return $this->layouts;
  }

  /**
   * Check if there is a layout for this TripalEntityType.
   *
   * @param string $tripal_entity_type
   *   The id of the TripalEntityType you want to check if there is a layout for.
   * @return bool
   *   TRUE if there is a layout for the bundle and FALSE otherwise.
   */
  public function hasLayout(string $tripal_entity_type) {

    // Check to see if we have processed the layouts yet.
    if ($this->bundle_layouts === NULL) {
      // and if not, then do so...
      $this->processLayouts();
    }

    // Now check to see if we have a layout for the entity type.
    if (array_key_exists($tripal_entity_type, $this->bundle_layouts)) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  /**
   * Get the layout for a specific TripalEntityType.
   *
   * @param string $tripal_entity_type
   *   The id of the TripalEntityType you want the layout for.
   * @return array
   *   The layout array for this TripalEntityType or NULL if this collection
   *   does not define a layout for this TripalEntityType.
   */
  public function getLayout(string $tripal_entity_type) {

    // Check to see if we have processed the layouts yet.
    if ($this->bundle_layouts === NULL) {
      // and if not, then do so...
      $this->processLayouts();
    }

    // Now check to see if we have a layout for the entity type.
    if (array_key_exists($tripal_entity_type, $this->bundle_layouts)) {
      return $this->bundle_layouts[$tripal_entity_type];
    } else {
      return NULL;
    }
  }

  /**
   * Keys the layouts for this entity by the TripalEntityTYpe (i.e. bundle)
   * they are for.
   *
   * @return void
   */
  protected function processLayouts() {
    // For each layout, index it in the bundle_layouts array.
    foreach ($this->layouts as $key => $layout) {
      if (array_key_exists('tripal_entity_type', $layout)) {
        $bundle = $layout['tripal_entity_type'];
        $this->bundle_layouts[$bundle] = &$this->layouts[$key];
      }
    }
  }

  /**
   * Clears the bundle layout cache.
   *
   * @return void
   */
  public function clearLayoutCache() {
    $this->bundle_layouts = NULL;
  }
}
