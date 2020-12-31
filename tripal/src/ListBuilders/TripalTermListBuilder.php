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

    $header['namespace'] = $this->t('Namespace');
    $header['IDSpace'] = $this->t('IDSpace');
    $header['accession'] = $this->t('Term Accession');
    $header['name'] = $this->t('Term Name');

    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\tripal\Entity\TripalTerm */
    $idspace = $entity->getIDSpace();
    $vocab = ($idspace) ? $idspace->getVocab() : FALSE;

    // Namespace.
    if ($vocab) {
      $row['namespace'] = Link::fromTextAndUrl(
        $vocab->getNamespace(),
        $vocab->toUrl('canonical', ['tripal_vocab' => $vocab->id()])
      )->toString();
    }
    else {
      $row['namespace'] = '';
    }

    // IDSpace.
    if ($idspace) {
      $row['IDSpace'] = Link::fromTextAndUrl(
        $idspace->getIDSpace(),
        $idspace->toUrl('canonical', ['tripal_vocab_space' => $idspace->id()])
      )->toString();
    }
    else {
      $row['IDSpace'] = '';
    }

    // Term Accession.
    $row['accession'] = $entity->getAccession();

    // Term Name.
    $row['name'] = Link::fromTextAndUrl(
      $entity->getName(),
      $entity->toUrl('canonical', ['tripal_term' => $entity->id()])
    )->toString();


    return $row + parent::buildRow($entity);
  }

}
