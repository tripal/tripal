<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Routing\LinkGeneratorTrait;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Controlled Vocabulary entities.
 *
 * @ingroup tripal
 */
class TripalVocabListBuilder extends EntityListBuilder {

  use LinkGeneratorTrait;

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['name'] = $this->t('Controlled Vocabulary Name');
    $header['short_name'] = $this->t('Short Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\tripal\Entity\TripalVocab */

    $row['name'] = \Drupal::l(
      $entity->getName(),
      $entity->urlInfo('canonical', ['tripal_entity_type' => $entity->id()])
    );

    $row['short_name'] = $entity->getLabel();

    return $row + parent::buildRow($entity);
  }

}
