<?php

namespace Drupal\tripal_chado\Plugin\TripalIdSpace;

use Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase;

/**
 * Chado Implementation of TripalIDSpaceBase
 * 
 *  @TripalIdSpace(
 *    id = "chado_id_space",
 *    label = @Translation("Vocabulary IDSpace in Chado"),
 *  )
 */
abstract class ChadoIdSpace extends TripalIdSpaceBase {

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * Removes this id space from its previous default vocabulary if one is set
   * and then adds this id space to its new default vocabulary if the given name
   * is not NULL. It is still the responsibility of an implementation to
   * actually save changes to its default vocabulary.
   *
   * {@inheritdoc}
   */
  public function setDefaultVocabulary($name) {
    // TODO
  }

}
