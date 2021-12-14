<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\Interface\TripalFieldItemInterface
use Drupal\Core\Entity\ContentEntityBase
use Drupal\Core\Entity\ContentEntityInterface
use Drupal\Core\Entity\EntityStorageInterface

/**
 * TODO
 */
class TripalEntityBase extends ContentEntityBase {

  /**
   * {@inheritdoc}
   */
  public function preSave(EntityStorageInterface $storage) {
    parent::preSave($storage);

    // Build all storage operations that will be done, saving the tripal
    // fields that will be saved and clearing them from each entity.
    $storageOps = array()
    foreach($this->bundleFieldDefinitions() as $fieldDefinition) {
      $field = \Drupal::service("plugin.manager.field.field_type").getInstance($fieldDefinition->getType());
      if ($field instanceof TripalFieldItemInterface) {
        $props = $field->tripalValues();
        $field->tripalSave($props,$this);
        $field->tripalClear($this);
        $tsid = $field->tripalStorageId();
        if (array_key_exists($tsid,$storageOps)) {
          $storageOps[$tsid] = array_merge($storageOps[$tsid],$props);
        }
        else {
          $storageOps[$tsid] = $props;
        }
      }
    }

    // Save all properties to their respective storage plugins
    foreach ($storageOps as $tsid => $properties) {
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->saveValues($properties);
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function postLoad(EntityStorageInterface $storage, array &$entities) {
    parent::postLoad($storage, $entities);

    // Build the storage operations that will be done and entity references
    $storageOps = array()
    $entityRefs = array()
    foreach ($entities as $entity) {
      $hasTripalFields = False
      foreach ($entity->bundleFieldDefinitions() as $fieldDefinition) {
        $field = \Drupal::service("plugin.manager.field.field_type").getInstance($fieldDefinition->getType());
        if ($field instanceof TripalFieldItemInterface) {
          $hasTripalFields = True
          $props = $field->tripalValues();
          $tsid = $field->tripalStorageId();
          if (array_key_exists($tsid,$storageOps)) {
            $storageOps[$tsid] = array_merge($storageOps[$tsid],$props);
          }
          else {
            $storageOps[$tsid] = $props;
          }
          $entityRefs[$entity->id][$field->getName()]["field"] = $field
        }
      }
      if ($hasTripalFields) {
        $entityRefs[$entity->id]["entity"] = $entity
      }
    }

    // Load all properties from their respective storage plugins
    $loaded = array()
    foreach ($storageOps as $tsid => $properties) {
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->loadValues($properties);
      $loaded = array_merge($loaded,$properties);
    }

    // Add loaded properties to their correct entity and field references
    foreach ($loaded as $property) {
      $tid = $property->getEntityId();
      if (array_key_exists("props",$entityRefs[$tid])) {
        $entityRefs[$tid] = array_push($entityRefs[$tid],$property);
      }
      else {
        $entityRefs[$tid] = array($property);
      }
    }

    // Load all properties with their respective entities and fields
    foreach ($entityRefs as $entityRef) {
      $entity = $entityRef["entity"];
      foreach ($entityRef as $fieldRef) {
        $field = $fieldRef["field"];
        $field->tripalLoad($fieldRef["props"],$entity);
      }
    }
  }

}
