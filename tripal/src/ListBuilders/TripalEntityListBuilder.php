<?php

namespace Drupal\tripal\ListBuilders;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Render\FormattableMarkup;
use Drupal\Component\Utility\Xss;

/**
 * Defines a class to build a listing of Tripal Content entities.
 *
 * @ingroup tripal
 */
class TripalEntityListBuilder extends EntityListBuilder {

  /**
   * Local copy of stored setting for better performance.
   */
  protected $tripal_allowed_tags = [];

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['title'] = $this->t('Title');
    $header['type'] = $this->t('Type');
    $header['term'] = $this->t('Term');
    $header['author'] = $this->t('Author');
    $header['created'] = $this->t('Created');

    // Retrieve allowed tags setting to use when building rows.
    $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags');
    $this->tripal_allowed_tags = explode(' ', $tag_string ?? '');

    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {

    $type_name = $entity->getType();
    $bundle = \Drupal\tripal\Entity\TripalEntityType::load($type_name);

    $sanitized_value = Xss::filter($entity->getTitle(), $this->tripal_allowed_tags);
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
