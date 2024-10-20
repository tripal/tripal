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
    $header['type-category'] = $this->t('Category');
    $header['type-label'] = $this->t('Label');
    $header['type-id'] = $this->t('Machine name');
    $header['term'] = $this->t('Term');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $data = [];

    // Category.
    $data['type-category'] = $entity->getCategory();

    // Add on the Label with link.
    $data['type-label'] = Link::fromTextAndUrl(
      $entity->label(),
      $entity->toUrl('edit-form', ['tripal_entity_type' => $entity->id()])
    )->toString();

    // Add in the machine name.
    $data['type-id'] = $entity->id();

    // Add in the term with link.
    $data['term'] = '';
    $term = $entity->getTerm();
    if ($term) {
      $data['term'] = $term->getName() . ' (' . $term->getTermId() . ')';
    }

    // Add in classes for better themeing and testing.
    $rowData = [];
    foreach ($data as $key => $value) {
      $rowData[$key] = [
        'data' => $value,
        'class' => [$key],
      ];
    }
    $row = $rowData + parent::buildRow($entity);
    return ['class' => [$data['type-category']], 'data' => $row ];
  }

  /**
   * {@inheritdoc}
   */
  public function load() {
    $entity_ids = $this
      ->getEntityIds();
    $entities = $this->storage
      ->loadMultipleOverrideFree($entity_ids);

    // Sort the entities using the entity class's sort() method.
    // See \Drupal\Core\Config\Entity\ConfigEntityBase::sort().
    uasort($entities, array(
      $this->entityType
        ->getClass(),
      'sortByCategory',
    ));
    return $entities;
  }

  /**
   * {@inheritdoc}
   */
  public function render() {
    $build = parent::render();

    $build['table']['#attributes']['class'][] = 'tripal-entity-type-list';
    $build['table']['#attached']['library'][] = 'tripal/tripal-entity-type-listbuilder';

    return $build;
  }

}
