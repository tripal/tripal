<?php

namespace Drupal\tripal\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Controlled Vocabulary Term entities.
 */
class TripalTermViewsData extends EntityViewsData implements EntityViewsDataInterface {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['tripal_term']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Controlled Vocabulary Term'),
      'help' => $this->t('The Controlled Vocabulary Term ID.'),
    );

    return $data;
  }

}
