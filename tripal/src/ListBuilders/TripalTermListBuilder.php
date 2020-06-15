<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Controlled Vocabulary Term entities.
 *
 * @ingroup tripal
 */
class TripalTermListBuilder extends EntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {

    $header['name'] = $this->t('Term Name');
    $header['cv'] = $this->t('Vocabulary');
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
    $vocab_name = '';
    if ($vocab) {
      $vocab_name = Link::fromTextAndUrl(
        $vocab->getName(),
        $vocab->toUrl('canonical', ['tripal_vocab' => $vocab->id()])
      )->toString();
      $vocab_label = $vocab->getLabel();
    }

    $row['name'] = Link::fromTextAndUrl(
      $entity->getName(),
      $entity->toUrl('canonical', ['tripal_term' => $entity->id()])
    )->toString();
    $row['cv'] = $vocab_name;
    $row['accession'] = $vocab_label . ':' . $entity->getAccession();

    return $row + parent::buildRow($entity);
  }

}
