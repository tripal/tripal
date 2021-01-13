<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;
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

    // Add om the Label with link.
    $row['label'] = Link::fromTextAndUrl(
      $entity->label(),
      $entity->toUrl('edit-form', ['tripal_entity_type' => $entity->id()])
    )->toString();

    // Add in the machine name.
    $row['id'] = $entity->id();

    // Add in the term with link.
    $row['term'] = 'Uknown';
    $term = $entity->getTerm();
    if ($term) {
      $idspace = $term->getIDSpace();
      if ($idspace) {
        $term_display = $term->getName() . ' (' . $idspace->getIDSpace() . ':' . $term->getAccession() . ')';
        $row['term'] = Link::fromTextAndUrl(
          $term_display,
          $term->toUrl('canonical', ['tripal_term' => $term->id()])
        )->toString();
      }
    }
    return $row + parent::buildRow($entity);
  }

}
