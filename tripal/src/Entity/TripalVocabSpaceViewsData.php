<?php

namespace Drupal\tripal\Entity;

use Drupal\views\EntityViewsData;

/**
 * Provides Views data for Tripal Vocabulary IDSpace entities.
 */
class TripalVocabSpaceViewsData extends EntityViewsData {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['tripal_vocab_space']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Tripal Vocabulary IDSpace'),
      'help' => $this->t('The Tripal Vocabulary IDSpace ID.'),
    );

    return $data;
  }

}
