<?php

namespace Drupal\tripal;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provides a listing of Tripal Content type entities.
 */
class TripalEntityTypeListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Label');
    $header['id'] = $this->t('Machine name');
    $header['term'] = $this->t('Term');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $entity->label();
    $row['id'] = $entity->id();

    //dpm($entity, 'entity');
    $row['term'] = 'Uknown';
    $term = $entity->getTerm();
    if ($term) {
      $vocab = $term->getVocab();
      if ($vocab) {
        $row['term'] = $term->getName() . ' (' . $vocab->getLabel() . ':' . $term->getAccession() . ')';
      }
    }
    return $row + parent::buildRow($entity);
  }

}
