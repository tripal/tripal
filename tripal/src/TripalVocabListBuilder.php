<?php

namespace Drupal\tripal;

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
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\tripal\Entity\TripalVocab */
    $row['name'] = $this->l(
      $entity->getLabel(),
      new Url(
        'entity.tripal_vocab.canonical', array(
          'tripal_vocab' => $entity->id(),
        )
      )
    );
    return $row + parent::buildRow($entity);
  }

}
