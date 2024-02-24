<?php

namespace Drupal\tripal_layout\ListBuilders;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;


/**
 * Provides a listing of Content Types configurations.
 */
class TripalLayoutDefaultViewListBuilder extends ConfigEntityListBuilder {

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

    // You probably want a few more properties here...

    return $row + parent::buildRow($entity);
  }

}
