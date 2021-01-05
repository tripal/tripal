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
 *   label = @Translation("Tripal Controlled Vocabulary Term"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalTermListBuilder",
 *     "views_data" = "Drupal\tripal\Entity\TripalTermViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\tripal\Form\TripalTermForm",
 *       "add" = "Drupal\tripal\Form\TripalTermForm",
 *       "edit" = "Drupal\tripal\Form\TripalTermForm",
 *       "delete" = "Drupal\tripal\Form\TripalTermDeleteForm",
 *     },
 *     "access" = "Drupal\tripal\Access\TripalTermAccessControlHandler",
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
 *     "label" = "name",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/tripal_term/{tripal_term}",
 *     "add-form" = "/admin/structure/tripal_term/add",
 *     "edit-form" = "/admin/structure/tripal_term/{tripal_term}/edit",
 *     "delete-form" = "/admin/structure/tripal_term/{tripal_term}/delete",
 *     "collection" = "/admin/structure/tripal_term",
 *   },
 * )
 */
class TripalTerm extends ContentEntityBase implements TripalTermInterface {

  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public function getID() {
    return $this->get('id')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    return $this->getName();
  }

  /**
   * {@inheritdoc}
   */
  public function getIDSpaceID(){
    return $this->get('idspace_id')->getString();
  }

  /**
   * {@inheritdoc}
   */
  public function setIDSpaceID($idspace_id) {
    $this->set('idspace_id', $idspace_id);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getIDSpace() {
    $idspace_id = $this->getIDSpaceID();
    $idspace = \Drupal\tripal\Entity\TripalVocabSpace::load($idspace_id);
    return $idspace;
  }

  /**
   * {@inheritdoc}
   */
  public function getVocabID(){
    $raw_values = $this->get('vocab_id')->getValue();
    $ids = [];
    foreach ($raw_values as $raw) {
      $ids[] = $raw['target_id'];
    }
    return $ids;
  }

  /**
   * {@inheritdoc}
   */
  public function setVocabID($vocab_id) {
    // @todo Check at least one is the default vocabulary for the IDSpace?
    if (!is_array($vocab_id)) {
      $vocab_id = [ $vocab_id ];
    }
    $this->set('vocab_id', $vocab_id);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getVocab() {
    $result = [];

    $vocab_ids = $this->getVocabID();
    foreach ($vocab_ids as $vocab_id) {
      $result[] = \Drupal\tripal\Entity\TripalVocab::load($vocab_id);
    }

    return $result;
  }

  /**
   * {@inheritdoc}
   */
  public function getAccession() {
    return $this->get('accession')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setAccession($accession) {
    $this->set('accession', $accession);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    return $this->get('name')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getDefinition() {
    return $this->get('definition')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setDefinition($definition) {
    $this->set('definition', $definition);
    return $this;
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
  public function getDetails() {
    $details = [];

    $details['TripalTerm'] = $this;
    $idspace = $this->getIDSpace();
    $vocab = $idspace->getVocab();
    $details['vocabulary'] = $vocab->getDetails();
    $details['vocabulary']['short_name'] = $idspace->getIDSpace();
    $details['vocabulary']['idspace'] = $details['vocabulary']['short_name'];
    $details['accession'] = $this->getAccession();
    $details['name'] = $this->getName();
    $details['definition'] = $this->getDefinition();

    return $details;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

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

    $fields['definition'] = BaseFieldDefinition::create('string_long')
      ->setLabel(t('Definition'))
      ->setDescription(t('The definition of this term.'))
      ->setSettings(array(
        'text_processing' => 0,
      ));

    $fields['vocab_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Vocabularies'))
      ->setDescription(t('The vocabularies (e.g. sequence) this term is included in.'))
      ->setCardinality(-1)
      ->setSetting('target_type', 'tripal_vocab');

    $fields['idspace_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('IDSpace'))
      ->setDescription(t('The IDSpace (e.g. SO) this term belongs to. The IDSpace also indicates the default vocabulary.'))
      ->setSetting('target_type', 'tripal_vocab_space');

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the entity was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the entity was last edited.'));

    return $fields;
  }

}
