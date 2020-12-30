<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;

/**
 * Defines a class to build a listing of Tripal Vocabulary IDSpace entities.
 *
 * @ingroup tripal
 */
class TripalVocabSpaceListBuilder extends EntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['IDSpace'] = $this->t('IDSpace');
    $header['namespace'] = $this->t('Default Namespace');
    $header['vocab'] = $this->t('Default Tripal Vocabulary');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var \Drupal\tripal\Entity\TripalVocabSpace $entity */

    $row['IDSpace'] = Link::createFromRoute(
      $entity->getIDSpace(),
      'entity.tripal_vocab_space.canonical',
      ['tripal_vocab_space' => $entity->id()]
    );

    $vocab = $entity->getVocab();
    if ($vocab) {
      $row['namespace'] = $vocab->getNamespace();
      $row['vocab'] = Link::createFromRoute(
        $vocab->getName(),
        'entity.tripal_vocab.canonical',
        ['tripal_vocab' => $vocab->id()]
      );
    }
    else {
      $row['namespace'] = '';
      $row['vocab'] = '';
    }
    return $row + parent::buildRow($entity);
  }

}
