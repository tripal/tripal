<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityStorageInterface;

/**
 * Defines the Controlled Vocabulary entity.
 *
 * @ingroup tripal
 *
 * @ContentEntityType(
 *   id = "tripal_vocab",
 *   label = @Translation("Tripal Controlled Vocabulary"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalVocabListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalVocabViewsData",
 *     "translation" = "Drupal\tripal\TripalVocabTranslationHandler",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalVocabForm",
 *       "add" = "Drupal\tripal\Form\TripalVocabForm",
 *       "edit" = "Drupal\tripal\Form\TripalVocabForm",
 *       "delete" = "Drupal\tripal\Form\TripalVocabDeleteForm",
 *     },
 *     "access" = "Drupal\tripal\Access\TripalVocabAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\TripalVocabHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "tripal_vocab",
 *   translatable = FALSE,
 *   admin_permission = "administer controlled vocabulary entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "vocabulary",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_vocab/{tripal_vocab}",
 *     "add-form" = "/admin/structure/tripal_vocab/add",
 *     "edit-form" = "/admin/structure/tripal_vocab/{tripal_vocab}/edit",
 *     "delete-form" = "/admin/structure/tripal_vocab/{tripal_vocab}/delete",
 *     "collection" = "/admin/structure/tripal_vocab",
 *   },
 * )
 */
class TripalVocab extends ContentEntityBase implements ContentEntityInterface {

  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage, array &$values) {
    parent::preCreate($storage, $values);
  }

  /**
   * @see \Drupal\core\Entity\ContentEntityInterface::getID()
   */
  public function getID() {
    return $this->get('id')->value;
  }

  /**
   * @see \Drupal\core\Entity\ContentEntityInterface::getLabel()
   */
  public function getLabel() {
    return $this->get('vocabulary')->value;
  }

  /**
   * @see \Drupal\core\Entity\ContentEntityInterface::setLabel()
   */
  public function setLabel($vocabulary) {
    $this->set('vocabulary', $vocabulary);
    return $this;
  }

  /**
   *
   */
  public function getName() {
    return $this->get('name')->value;
  }

  /**
   *
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   *
   */
  public function getDescription() {
    return $this->get('description')->value;
  }

  /**
   *
   */
  public function setDescription($description) {
    $this->set('description', $description);
    return $this;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalVocabInterface::getCreatedTime()
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalVocabInterface::setCreatedTime()
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['vocabulary'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Vocabulary Name'))
      ->setDescription(t('The short name for the vocabulary (e.g. SO, PATO, etc.).'))
      ->setSettings(array(
        'max_length' => 1024,
        'text_processing' => 0,
      ));

    $fields['name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Vocabulary Name'))
      ->setDescription(t('The full name for the vocabulary (e.g. sequence).'))
      ->setSettings(array(
        'max_length' => 1024,
        'text_processing' => 0,
      ));

    $fields['description'] = BaseFieldDefinition::create('string_long')
      ->setLabel(t('Vocabulary Description'))
      ->setDescription(t('A description for the vocabulary.'))
      ->setSettings(array(
        'text_processing' => 0,
      ));

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the entity was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the entity was last edited.'));

    return $fields;
  }

}
