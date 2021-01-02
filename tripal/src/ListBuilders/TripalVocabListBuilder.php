<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Controlled Vocabulary entities.
 *
 * @ingroup tripal
 */
class TripalVocabListBuilder extends EntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['name'] = $this->t('Name');
    $header['namespace'] = $this->t('Namespace');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\tripal\Entity\TripalVocab */

    $row['name'] = Link::fromTextAndUrl(
      $entity->getName(),
      $entity->toUrl('canonical', ['tripal_entity_type' => $entity->id()])
    )->toString();

    $row['namespace'] = $entity->getNamespace();

    return $row + parent::buildRow($entity);
  }
}
