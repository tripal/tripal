<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Routing\LinkGeneratorTrait;
use Drupal\Core\Link;
use Drupal\Core\Url;

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
    $row['label'] = Link::fromTextAndUrl(
      $entity->label(),
      $entity->toUrl('edit-form', ['tripal_entity_type' => $entity->id()])
    )->toString();
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
