<?php

namespace Drupal\tripal\ListBuilders;

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
    $vocab_label = '';
    if ($vocab) {
      $vocab_label = $vocab->getLabel();
    }

    $row['vocabulary'] = $vocab_label;
    $row['name'] = $entity->getName();
    $row['accession'] = $vocab_label . ':' . $entity->getAccession();

    return $row + parent::buildRow($entity);
  }

}
