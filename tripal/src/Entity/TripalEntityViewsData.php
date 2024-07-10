<?php

namespace Drupal\tripal\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Tripal Content entities.
 */
class TripalEntityViewsData extends EntityViewsData implements EntityViewsDataInterface {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['tripal_entity']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Tripal Content'),
      'help' => $this->t('The Tripal Content ID.'),
    );

    return $data;
  }

}
