<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityStorageInterface;

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
    $storageOps = [];
    // Specifically, for each field...
    foreach ($this->bundleFieldDefinitions() as $fieldDefinition) {
      // Retrieve its Field Instance class.
      $field = \Drupal::service("plugin.manager.field.field_type").getInstance($fieldDefinition->getType());
      // If it is a TripalField then...
      if ($field instanceof TripalFieldItemInterface) {
        // Get empty template list of property values this field uses
        $props = $field->tripalValuesTemplate();
        // Retrieve the biological data to be saved...
        $field->tripalSave($props,$this);
        // Now we clear the biological data from the Drupal field values to ensure
        // this data is not duplicated.
        $field->tripalClear($this);
        // Finally based on the Tripal storage, we add this data to an array
        // for bulk save of the biological data to the appropriate database (e.g. Chado).
        $tsid = $field->tripalStorageId();
        if (array_key_exists($tsid,$storageOps)) {
          $storageOps[$tsid] = array_merge($storageOps[$tsid],$props);
        }
        else {
          $storageOps[$tsid] = $props;
        }
      }
    }

    // Save all properties to their respective storage plugins.
    // This is where the biological data is actually saved to the database
    // using the appropriate TripalStorage plugin.
    foreach ($storageOps as $tsid => $properties) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->saveValues($properties);
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function postLoad(EntityStorageInterface $storage, array &$entities) {
    parent::postLoad($storage, $entities);

    // Build the storage operations that will be done and entity references
    $storageOps = [];
    $entityRefs = [];
    // For each entity to be loaded, check each field so we can...
    foreach ($entities as $entity) {
      $hasTripalFields = FALSE;
      foreach ($entity->bundleFieldDefinitions() as $fieldDefinition) {
        $field = \Drupal::service("plugin.manager.field.field_type").getInstance($fieldDefinition->getType());
        // compile a list of TripalField values grouped by TripalStorage implementations.
        if ($field instanceof TripalFieldItemInterface) {
          $hasTripalFields = TRUE;
          $props = $field->tripalValuesTemplate();
          $tsid = $field->tripalStorageId();
          if (array_key_exists($tsid,$storageOps)) {
            $storageOps[$tsid] = array_merge($storageOps[$tsid],$props);
          }
          else {
            $storageOps[$tsid] = $props;
          }
          // Additionally, we compile a list of entities and fields
          // implementing the TripalField interface.
          // This is used below to re-add the loaded field values back into
          // the appropriate entities.
          $entityRefs[$entity->id][$field->getName()]["field"] = $field;
        }
      }
      if ($hasTripalFields) {
        $entityRefs[$entity->id]["entity"] = $entity;
      }
    }

    // Load all properties from their respective storage plugins
    $loaded = [];
    foreach ($storageOps as $tsid => $properties) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->loadValues($properties);
      $loaded = array_merge($loaded,$properties);
    }

    // Add loaded properties to their correct entity and field references
    // Note: Each $property is an instance of StoragePropertyValue
    // and thus contains information for it's associated entity ID/Type and
    // field ID/Key.
    foreach ($loaded as $property) {
      $tid = $property->getEntityId();
      // Note: The StoragePropertyValue field_key is equal to the field's name.
      $field_key = $property->getFieldKey();
      if (array_key_exists("props",$entityRefs[$tid][$field_key])) {
        $entityRefs[$tid][$field_key]["props"] = array_push($entityRefs[$tid],$property);
      }
      else {
        $entityRefs[$tid][$field_key]["props"] = array($property);
      }
    }

    // Attach all loaded properties to their respective entities and fields.
    foreach ($entityRefs as $entityRef) {
      $entity = $entityRef["entity"];
      foreach ($entityRef as $fieldRef) {
        $field = $fieldRef["field"];
        // Finally we let the TripalField attach the it's loaded properties
        // which allows another opprotunity for re-organization if needed.
        $field->tripalLoad($fieldRef["props"],$entity);
      }
    }
  }

}
