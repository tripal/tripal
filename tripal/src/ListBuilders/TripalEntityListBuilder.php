<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Render\FormattableMarkup;

/**
 * Defines a class to build a listing of Tripal Content entities.
 *
 * @ingroup tripal
 */
class TripalEntityListBuilder extends EntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['title'] = $this->t('Title');
    $header['type'] = $this->t('Type');
    $header['term'] = $this->t('Term');
    $header['author'] = $this->t('Author');
    $header['created'] = $this->t('Created');

    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {

    $type_name = $entity->getType();
    $bundle = \Drupal\tripal\Entity\TripalEntityType::load($type_name);

    // @todo this variable could be made global and configurable
    $tripal_allowed_tags = ['em','strong'];
    $sanitized_value = Drupal\Component\Utility\Xss::filter($entity->getTitle(), $tripal_allowed_tags);
    $row['title'] = Link::fromTextAndUrl(
      new FormattableMarkup($sanitized_value, []),
      $entity->toUrl('canonical', ['tripal_entity' => $entity->id()])
    )->toString();

    $row['type'] = $bundle->getLabel();
    $row['term'] = '';

    $row['author'] = '';
    $owner = $entity->getOwner();
    if ($owner) {
      $row['author'] = $owner->getDisplayName();
    }

    $row['created'] = \Drupal::service('date.formatter')->format($entity->getCreatedTime());

    return $row + parent::buildRow($entity);
  }

  /**
   * {@inheritdoc}
   */
  public function load() {
    // Using the session helps speed up this listing by telling TripalEntity not
    // to load ALL the fields.
    if (\Drupal::request()->hasSession()) {
      $session = \Drupal::request()->getSession();
      $session->set('tripal_load_listing', TRUE);
    }

    try {
      $entity_ids = $this->getEntityIds();
      $entities = $this->storage->loadMultiple($entity_ids);
      $session->set('tripal_load_listing', FALSE);
      return $entities;
    }
    catch (\Exception $e) {
      $session->set('tripal_load_listing', FALSE);
      throw new \Exception($e);
    }
  }



}
