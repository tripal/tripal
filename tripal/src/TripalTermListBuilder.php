<?php

namespace Drupal\tripal;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Routing\LinkGeneratorTrait;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Controlled Vocabulary Term entities.
 *
 * @ingroup tripal
 */
class TripalTermListBuilder extends EntityListBuilder {

  use LinkGeneratorTrait;

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['cv'] = $this->t('Vocabulary');
    $header['name'] = $this->t('Term Name');
    $header['accession'] = $this->t('Accession');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\tripal\Entity\TripalTerm */
    $vocab = $entity->getVocab();
    $row['vocabulary'] = $this->l(
      $vocab->getLabel(),
      new Url(
        'entity.tripal_vocab.canonical', array(
          'tripal_vocab' => $entity->getVocabID(),
        )
      )
    );
    $row['name'] = $this->l(
      $entity->getName(),
      new Url(
        'entity.tripal_term.canonical', array(
          'tripal_term' => $entity->id(),
        )
      )
    );
    $row['accession'] = $this->l(
      $vocab->getLabel() . ':' . $entity->getAccession(),
      new Url(
        'entity.tripal_term.canonical', array(
          'tripal_term' => $entity->id(),
        )
      )
    );
    return $row + parent::buildRow($entity);
  }

}
