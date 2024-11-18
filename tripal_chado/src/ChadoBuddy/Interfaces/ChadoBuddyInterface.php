<?php

namespace Drupal\tripal_chado\ChadoBuddy\Interfaces;

/**
 * Interface for chado_buddy plugins.
 */
interface ChadoBuddyInterface {

  /**
   * Returns the translated plugin label.
   */
  public function label(): string;

}
