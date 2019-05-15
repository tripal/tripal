<?php

namespace Drupal\tripal\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Controlled Vocabulary entities.
 */
class TripalVocabViewsData extends EntityViewsData implements EntityViewsDataInterface {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['tripal_vocab']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Controlled Vocabulary'),
      'help' => $this->t('The Controlled Vocabulary ID.'),
    );

    return $data;
  }

}
