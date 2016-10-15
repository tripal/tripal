<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\UserInterface;

/**
 * Defines the Controlled Vocabulary entity.
 *
 * @ingroup tripal
 *
 * @ContentEntityType(
 *   id = "tripal_vocab",
 *   label = @Translation("Controlled Vocabulary"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\TripalVocabListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalVocabViewsData",
 *     "translation" = "Drupal\tripal\TripalVocabTranslationHandler",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalVocabForm",
 *       "add" = "Drupal\tripal\Form\TripalVocabForm",
 *       "edit" = "Drupal\tripal\Form\TripalVocabForm",
 *       "delete" = "Drupal\tripal\Form\TripalVocabDeleteForm",
 *     },
 *     "access" = "Drupal\tripal\TripalVocabAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\TripalVocabHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "tripal_vocab",
 *   data_table = "tripal_vocab_field_data",
 *   translatable = FALSE,
 *   admin_permission = "administer controlled vocabulary entities",
 *   entity_keys = {
 *     "id" = "id",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_vocab/{tripal_vocab}",
 *     "add-form" = "/admin/structure/tripal_vocab/add",
 *     "edit-form" = "/admin/structure/tripal_vocab/{tripal_vocab}/edit",
 *     "delete-form" = "/admin/structure/tripal_vocab/{tripal_vocab}/delete",
 *     "collection" = "/admin/structure/tripal_vocab",
 *   },
 *   field_ui_base_route = "tripal_vocab.settings"
 * )
 */
class TripalVocab extends ContentEntityBase implements TripalVocabInterface {

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
   * @see \Drupal\tripal\Entity\TripalVocabInterface::getVocabulary()
   */
  public function getVocabulary() {

  }
  /**
   * @see \Drupal\tripal\Entity\TripalVocabInterface::setVocabulary()
   */
  public function setVocabulary($vocabulary) {

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

//     $fields['user_id'] = BaseFieldDefinition::create('entity_reference')
//       ->setLabel(t('Authored by'))
//       ->setDescription(t('The user ID of author of the Controlled Vocabulary entity.'))
//       ->setRevisionable(TRUE)
//       ->setSetting('target_type', 'user')
//       ->setSetting('handler', 'default')
//       ->setTranslatable(TRUE)
//       ->setDisplayOptions('view', array(
//         'label' => 'hidden',
//         'type' => 'author',
//         'weight' => 0,
//       ))
//       ->setDisplayOptions('form', array(
//         'type' => 'entity_reference_autocomplete',
//         'weight' => 5,
//         'settings' => array(
//           'match_operator' => 'CONTAINS',
//           'size' => '60',
//           'autocomplete_type' => 'tags',
//           'placeholder' => '',
//         ),
//       ))
//       ->setDisplayConfigurable('form', TRUE)
//       ->setDisplayConfigurable('view', TRUE);

//     $fields['name'] = BaseFieldDefinition::create('string')
//       ->setLabel(t('Name'))
//       ->setDescription(t('The name of the Controlled Vocabulary entity.'))
//       ->setSettings(array(
//         'max_length' => 50,
//         'text_processing' => 0,
//       ))
//       ->setDefaultValue('')
//       ->setDisplayOptions('view', array(
//         'label' => 'above',
//         'type' => 'string',
//         'weight' => -4,
//       ))
//       ->setDisplayOptions('form', array(
//         'type' => 'string_textfield',
//         'weight' => -4,
//       ))
//       ->setDisplayConfigurable('form', TRUE)
//       ->setDisplayConfigurable('view', TRUE);

//     $fields['status'] = BaseFieldDefinition::create('boolean')
//       ->setLabel(t('Publishing status'))
//       ->setDescription(t('A boolean indicating whether the Controlled Vocabulary is published.'))
//       ->setDefaultValue(TRUE);

    $fields['vocabulary'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Vocabulary Name'))
      ->setDescription(t('The short name for the vocabulary (e.g. SO, PATO, etc.).'))
      ->setSettings(array(
        'max_length' => 1024,
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
