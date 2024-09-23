<?php

namespace Drupal\tripal_layout\ListBuilders;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;


/**
 * Provides a listing of Form Layout collections.
 */
class TripalDefaultFormLayoutListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Configuration Name');
    $header['id'] = $this->t('ID');
    $header['description'] = $this->t('Description');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $entity->label();
    $row['id'] = $entity->id();
    $row['description'] = $entity->description();

    return $row + parent::buildRow($entity);
  }

}
