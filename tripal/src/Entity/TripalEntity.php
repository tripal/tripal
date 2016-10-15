<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\UserInterface;

/**
 * Defines the Tripal Content entity.
 *
 * @ingroup tripal
 *
 * @ContentEntityType(
 *   id = "tripal_entity",
 *   label = @Translation("Tripal Content"),
 *   bundle_label = @Translation("Tripal Content type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\TripalEntityListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalEntityViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalEntityForm",
 *       "add" = "Drupal\tripal\Form\TripalEntityForm",
 *       "edit" = "Drupal\tripal\Form\TripalEntityForm",
 *       "delete" = "Drupal\tripal\Form\TripalEntityDeleteForm",
 *     },
 *     "access" = "Drupal\tripal\TripalEntityAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\TripalEntityHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "tripal_entity",
 *   admin_permission = "administer tripal content entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "bundle" = "type",
 *     "uid" = "user_id",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_entity/{tripal_entity}",
 *     "add-page" = "/admin/structure/tripal_entity/add",
 *     "add-form" = "/admin/structure/tripal_entity/add/{tripal_entity_type}",
 *     "edit-form" = "/admin/structure/tripal_entity/{tripal_entity}/edit",
 *     "delete-form" = "/admin/structure/tripal_entity/{tripal_entity}/delete",
 *     "collection" = "/admin/structure/tripal_entity",
 *   },
 *   bundle_entity_type = "tripal_entity_type",
 *   field_ui_base_route = "entity.tripal_entity_type.edit_form"
 * )
 */
class TripalEntity extends ContentEntityBase implements TripalEntityInterface {

  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage_controller, array &$values) {
    parent::preCreate($storage_controller, $values);
    $values += array(
      'user_id' => \Drupal::currentUser()->id(),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwner() {
    return $this->get('uid')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwnerId() {
    return $this->get('uid')->target_id;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwnerId($uid) {
    $this->set('uid', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwner(UserInterface $account) {
    $this->set('uid', $account->id());
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function isPublished() {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setPublished($published) {
    $this->set('status', $published ? NODE_PUBLISHED : NODE_NOT_PUBLISHED);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Authored by'))
      ->setDescription(t('The user ID of author of the Tripal Content entity.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setTranslatable(TRUE)
      ->setDisplayOptions('view', array(
        'label' => 'hidden',
        'type' => 'author',
        'weight' => 0,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'entity_reference_autocomplete',
        'weight' => 5,
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'autocomplete_type' => 'tags',
          'placeholder' => '',
        ),
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['term_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('CV Term'))
      ->setDescription(t('The controlled vocabulary term ID.'))
      ->setRevisionable(FALSE)
      ->setSetting('target_type', 'tripal_term')
      ->setSetting('handler', 'default')
      ->setTranslatable(FALSE);

    $fields['title'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Title'))
      ->setDescription(t('The title of this entity.'))
      ->setSettings(array(
        'max_length' => 1024,
        'text_processing' => 0,
      ))
      ->setDefaultValue('')
      ->setDisplayOptions('view', array(
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ))
      ->setDisplayOptions('form', array(
        'type' => 'string_textfield',
        'weight' => -4,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Publishing status'))
      ->setDescription(t('A boolean indicating whether the Tripal Content is published.'))
      ->setDefaultValue(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the entity was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the entity was last edited.'));

    return $fields;
  }

}
