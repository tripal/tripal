<?php

namespace Drupal\tripal_chado\ChadoBuddy;

use Drupal\Component\Plugin\PluginBase;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * Base class for chado_buddy plugins.
 */
abstract class ChadoBuddyPluginBase extends PluginBase implements ChadoBuddyInterface {

  /**
   * Provides the TripalDBX connection to chado that this ChadoBuddy should act upon.
   * @var Drupal\tripal_chado\Database\ChadoConnection
   *
   */
  public ChadoConnection $connection;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ChadoConnection $connection) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->connection = $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function label(): string {
    // Cast the label to a string since it is a TranslatableMarkup object.
    return (string) $this->pluginDefinition['label'];
  }

  /**
   * {@inheritdoc}
   */
  public function description(): string {
    // Cast the description to a string since it is a TranslatableMarkup object.
    return (string) $this->pluginDefinition['description'];
  }

  /**
   * Used to validate input arrays to various buddy functions,
   * or to generate a valid array subset.
   *
   * @param array $uservalues
   *   An associative array to be validated
   * @param array $validvalues
   *   An associative array listing all valid keys, and the
   *   values are un-aliased database table alias and table
   *   column. For example 'db_name' => 'db.name'
   * @param bool $filter
   *   Set to TRUE if we want to return a subset of the passed
   *   $uservalues containing only keys from $validvalues.
   *   If FALSE, then an exception is thrown for invalid keys.
   *
   * @return array
   *   The filtered set of $uservalues
   */
  protected function validateInput(array $uservalues, array $validvalues, bool $filter = FALSE) {
    $subset = [];
    foreach ($uservalues as $key => $value) {
      if (!array_key_exists($key, $validvalues)) {
        if (!$filter) {
          $calling_function = debug_backtrace()[1]['function'];
          throw new ChadoBuddyException("ChadoBuddy $calling_function error, value \"$key\" is not valid for for this function\n");
        }
      }
      $mapping = $validvalues[$key];  // e.g. 'db_name' => 'db.name'
      $parts = explode('.', $mapping);
      $subset[$parts[1]] = $value;
    }
    if (!$subset) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, no valid values were specified\n");
    }
    return $subset;
  }

}
