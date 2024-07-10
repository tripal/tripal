<?php

namespace Drupal\tripal\TripalVocabTerms;

use Drupal\Component\Plugin\PluginBase;

/**
 * Base class for tripal collection plugins.
 */
class TripalCollectionPluginBase extends PluginBase {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
    $this->name = $configuration["collection_name"];
  }

  /**
   * Returns the name of this collection.
   *
   * @return string
   *   The name.
   */
  public function getName() {
    return $this->name;
  }

  /**
   * The name of this collection.
   *
   * @var string
   */
  private $name;

  /**
   * An instance of the TripalLogger.
   *
   * @var object \Drupal\tripal\Services\TripalLogger
   */
  protected $messageLogger = NULL;

}
