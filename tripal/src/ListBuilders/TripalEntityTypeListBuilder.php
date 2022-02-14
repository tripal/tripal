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
    $header['type-label'] = $this->t('Label');
    $header['type-id'] = $this->t('Machine name');
    $header['term'] = $this->t('Term');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row = [];

    // Add om the Label with link.
    $row['type-label'] = Link::fromTextAndUrl(
      $entity->label(),
      $entity->toUrl('edit-form', ['tripal_entity_type' => $entity->id()])
    )->toString();

    // Add in the machine name.
    $row['type-id'] = $entity->id();

    // Add in the term with link.
    $row['term'] = '';
    /*
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
    */

    // Add in classes for better themeing and testing.
    $final_row = [];
    foreach ($row as $key => $value) {
      $final_row[$key] = [
        'data' => $value,
        'class' => [$key],
      ];
    }
    return $final_row + parent::buildRow($entity);
  }

}
