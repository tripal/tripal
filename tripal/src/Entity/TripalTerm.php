<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\UserInterface;

/**
 * Defines the Controlled Vocabulary Term entity.
 *
 * @ingroup tripal
 *
 * @ContentEntityType(
 *   id = "tripal_term",
 *   label = @Translation("Controlled Vocabulary Term"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\TripalTermListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalTermViewsData",
 *     "translation" = "Drupal\tripal\TripalTermTranslationHandler",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalTermForm",
 *       "add" = "Drupal\tripal\Form\TripalTermForm",
 *       "edit" = "Drupal\tripal\Form\TripalTermForm",
 *       "delete" = "Drupal\tripal\Form\TripalTermDeleteForm",
 *     },
 *     "access" = "Drupal\tripal\TripalTermAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\TripalTermHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "tripal_term",
 *   data_table = "tripal_term_field_data",
 *   translatable = FALSE,
 *   admin_permission = "administer controlled vocabulary term entities",
 *   entity_keys = {
 *     "id" = "id",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_term/{tripal_term}",
 *     "add-form" = "/admin/structure/tripal_term/add",
 *     "edit-form" = "/admin/structure/tripal_term/{tripal_term}/edit",
 *     "delete-form" = "/admin/structure/tripal_term/{tripal_term}/delete",
 *     "collection" = "/admin/structure/tripal_term",
 *   },
 *   field_ui_base_route = "tripal_term.settings"
 * )
 */
class TripalTerm extends ContentEntityBase implements TripalTermInterface {

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
   * @see \Drupal\tripal\Entity\TripalTermInterface::getID()
   */
  public function getID() {
    return $this->get('id')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::getVocabID()
   */
  public function getVocabID(){
    return $this->get('vocab_id')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::setVocabID()
   */
  public function setVocabID($vocab_id) {
    $this->set('vocab_id', $vocab_id);
    return $this;
  }
  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::getAccession()
   */
  public function getAccession() {
    return $this->get('accession')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::setAccession()
   */
  public function setAccession($accession) {
    $this->set('accession', $accession);
    return $this;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::getName()
   */
  public function getName() {
    return $this->get('name')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::setName()
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::getCreatedTime()
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * @see \Drupal\tripal\Entity\TripalTermInterface::setCreatedTime()
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

//     $fields['id'] = BaseFieldDefinition::create('entity_reference')
//       ->setLabel(t('Authored by'))
//       ->setDescription(t('The user ID of author of the Controlled Vocabulary Term entity.'))
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
//       ->setDescription(t('The name of the Controlled Vocabulary Term entity.'))
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
//       ->setDescription(t('A boolean indicating whether the Controlled Vocabulary Term is published.'))
//       ->setDefaultValue(TRUE);

    $fields['vocab_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Vocabulary ID'))
      ->setDescription(t('The ID of the TripalVocab entity to which this term belongs.'))
      ->setSetting('target_type', 'tripal_vocab');

    $fields['accession'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Accession'))
      ->setDescription(t('The unique ID (or accession) of this term in the vocabulary.'))
      ->setSettings(array(
        'max_length' => 1024,
        'text_processing' => 0,
      ));

    $fields['name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Name'))
      ->setDescription(t('The human readable name for this term.'))
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
