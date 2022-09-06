<?php

use Drupal\tripal\TripalStorage\TripalStorageUpdateException;
use Drupal\Core\Entity\Sql\SqlContentEntityStorageSchema;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;


class TripalEntityStorageSchema extends SqlContentEntityStorageSchema {

  /**
   * {@inheritdoc}
   */
  public function onFieldableEntityTypeCreate(EntityTypeInterface $entity_type, array $field_storage_definitions) {
    parent::onFieldableEntityTypeCreate($entity_type,$field_storage_definitions);

    // This is where we create the types each field describes in the
    // associated TripalStorage instance.
    $storageOps = [];
    foreach ($field_storage_definitions as $storageDefinition) {
      $field = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());
      if ($field instanceof TripalFieldItemInterface) {
        // Each field can define it's value as a single-depth array of
        // key => value pairs where the key is a name for that data (e.g. accession )
        // and the value is an implementation of StoragePropertyTypeBase setting the
        // type of the value (e.g. string).
        // Here we are retrieving that key-value array.
        $types = $field->tripalTypes();
        // Additionally, each field will have a TripalStorage type associated
        // with it that we are retrieving here.
        // Note: this can be changed in the admin UI since all TripalStorage
        // types are expected to be able to read this key-value structure.
        $tsid = $field->tripalStorageId();
        // Since each StoragePropertyTypeBase implementation knows it's
        // key (saved as id) and associated field, we can merge them all here
        // without saving the key mapping.
        // We're basically just compiling all field property types with the
        // same storage for the entire entity here.
        if (array_key_exists($tsid,$storageOps)) {
          $storageOps[$tsid] = array_merge($storageOps[$tsid], $types);
        }
        else {
          $storageOps[$tsid] = $types;
        }
      }
    }

    // Now we iterate through all storage plugins and add the compiled types
    // for each one. This will also save this information at the TripalStorage
    // layer.
    foreach ($storageOps as $tsid => $types) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->addTypes($types);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function onEntityTypeDelete(EntityTypeInterface $entity_type) {

    // build remove storage operations
    $storageOps = [];
    foreach ($this->fieldStorageDefinitions() as $storageDefinition) {
      $field = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());
      if ($field instanceof TripalFieldItemInterface) {
        $types = $field->tripalTypes();
        $tsid = $field->tripalStorageId();
        if (array_key_exists($tsid, $storageOps)) {
          $storageOps[$tsid] = array_merge($storageOps[$tsid], $types);
        }
        else {
          $storageOps[$tsid] = $types;
        }
      }
    }

    // iterate through all storage plugins and remove property types
    foreach ($storageOps as $tsid => $types) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->removeTypes($types);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function onFieldableEntityTypeUpdate(EntityTypeInterface $entity_type, EntityTypeInterface $original, array $field_storage_definitions, array $original_field_storage_definitions, array &$sandbox = NULL) {
    parent::onFieldableEntityTypeUpdate($entity_type,$original,$field_storage_definitions,$original_field_storage_definitions,$sandbox);

    // build associate array of old fields
    $oldTypes = [];
    foreach ($original_field_storage_definitions as $storageDefinition) {
        $oldTypes[$storageDefinition->getMainPropertyName()] = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());;
    }

    // build associate array of new fields
    $newTypes = [];
    foreach ($field_storage_definitions as $storageDefinition) {
        $newTypes[$storageDefinition->getMainPropertyName()] = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());;
    }

    // build storage add and update operations
    $storageAdd = [];
    $storageUpdate = [];
    // For each of the new field types...
    foreach ($newTypes as $name => $field) {
      if ($field instanceof TripalFieldItemInterface) {
        $types = $field->tripalTypes();
        $tsid = $field->tripalStorageId();
        if (array_key_exists($name,$oldTypes)) {
          // Case 1a: the new field already existed but the storage has changed.
          //   - this involves migrating of data from one storage to another
          //     which can be an error prone process leading to data loss.
          //   - As such, we are going to throw an exception and require admin
          //     to create a new field, migrate the data themselves, and then
          //     delete the old field and associated data.
          if ($oldTypes[$name]->tripalStorageId() != $tsid) {
            throw TripalStorageUpdateException(
              $entity_type->id
              ,"Cannot change tripal storage plugin type when updating fields."
            );
          }
          // Case 1b: the new field already existed and has the same storage.
          //   - we will want to update the key-value information.
          $otypes = $oldTypes[$name]->tripalTypes();
          if (array_key_exists($tsid,$storageUpdate)) {
            $storageUpdate[$tsid] = array_push($storageUpdate[$tsid], [$types,$otypes]);
          }
          else {
            $storageUpdate[$tsid] = [[$types, $otypes]];
          }
        }
        // Case 2: the new field did not exist before.
        //   - we will need to add the new field to it's new storage.
        else {
          if (array_key_exists($tsid, $storageAdd)) {
            $storageAdd[$tsid] = array_merge($storageAdd[$tsid], $types);
          }
          else {
            $storageAdd[$tsid] = $types;
          }
        }
      }
    }

    // Build storage remove operations.
    $storageRemove = [];
    foreach ($oldTypes as $name => $field) {
      if ($field instanceof TripalFieldItemInterface) {
        // Case 3: The old field no longer exists for this entity.
        //   - (i.e. the old field is not in the new field list)
        //   - we will want to remove it from the storage.
        if (!array_key_exists($name,$newTypes)) {
          $types = $field->tripalTypes();
          $tsid = $field->tripalStorageId();
          if (array_key_exists($tsid, $storageRemove)) {
            $storageRemove[$tsid] = array_merge($storageRemove[$tsid],$types);
          }
          else {
            $storageRemove[$tsid] = $types;
          }
        }
      }
    }

    // iterate through all storage plugins and remove old types
    foreach ($storageRemove as $tsid => $types) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->RemoveTypes($types);
    }

    // iterate through all storage plugins and update types
    foreach ($storageUpdate as $tsid => $types) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->UpdateTypes($types[0], $types[1]);
    }

    // iterate through all storage plugins and add new types
    foreach ($storageAdd as $tsid => $types) {
      $tripalStorage = \Drupal::service("tripal.storage")->getInstance($tsid);
      $tripalStorage->addTypes($types);
    }
  }

}
