<?php

class TripalEntityStorageSchema extends SqlContentEntityStorageSchema {

  /**
   * {@inheritdoc}
   */
  public function onFieldableEntityTypeCreate(EntityTypeInterface $entity_type, array $field_storage_definitions) {
    parent::onFieldableEntityTypeCreate($entity_type,$field_storage_definitions);

    // build storage operations for creating types
    $storageOps = array()
    foreach ($field_storage_definitions as $storageDefinition) {
      $field = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());
      if ($field instanceof TripalFieldItemInterface) {
        $types = $field->tripalTypes();
        $tsid = $field->tripalStorageId();
        if (array_key_exists($tsid,$storageOps)) {
          $storageOps[$tsid] = array_merge($storageOps[$tsid],$types);
        }
        else {
          $storageOps[$tsid] = $types;
        }
      }
    }

    // iterate through all storage plugins and create types
    foreach ($storageOps as $tsid => $types) {
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->addTypes($types);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function onEntityTypeDelete(EntityTypeInterface $entity_type) {
    // How can one get field storage definitions from entity_type?
  }

  /**
   * {@inheritdoc}
   */
  public function onFieldableEntityTypeUpdate(EntityTypeInterface $entity_type, EntityTypeInterface $original, array $field_storage_definitions, array $original_field_storage_definitions, array &$sandbox = NULL) {
    parent::onFieldableEntityTypeUpdate($entity_type,$original,$field_storage_definitions,$original_field_storage_definitions,$sandbox);

    // build associate array of old fields
    $oldTypes = array()
    foreach ($original_field_storage_definitions as $storageDefinition) {
        $oldTypes[$storageDefinition->getMainPropertyName()] = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());;
    }

    // build associate array of new fields
    $newTypes = array()
    foreach ($field_storage_definitions as $storageDefinition) {
        $newTypes[$storageDefinition->getMainPropertyName()] = \Drupal::service("plugin.manager.field.field_type").getInstance($storageDefinition->getType());;
    }

    // build storage add and update operations
    $storageAdd = array()
    $storageUpdate = array()
    foreach ($newTypes as $name => $field) {
      if ($field instanceof TripalFieldItemInterface) {
        $types = $field->tripalTypes();
        $tsid = $field->tripalStorageId();
        if ( array_key_exists($name,$oldTypes) && $oldTypes[$name]->tripalStorageId() == $tsid ) {
          $otypes = $oldTypes[$name]->tripalTypes();
          if (array_key_exists($tsid,$storageUpdate)) {
            $storageUpdate[$tsid] = array_push($storageUpdate[$tsid],array($types,$otypes);
          }
          else {
            $storageOps[$tsid] = array(array($types,$otypes));
          }
        }
        else {
          if (array_key_exists($tsid,$storageAdd)) {
            $storageAdd[$tsid] = array_merge($storageAdd[$tsid],$types);
          }
          else {
            $storageAdd[$tsid] = $types;
          }
        }
      }
    }

    // build storage remove operations
    $storageRemove = array()
    foreach ($oldTypes as $name => $field) {
      if ($field instanceof TripalFieldItemInterface) {
        if (!array_key_exists($name,$newTypes)) {
          $types = $field->tripalTypes();
          $tsid = $field->tripalStorageId();
          if array_key_exists($tsid,$storageRemove) {
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
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->RemoveTypes($types);
    }

    // iterate through all storage plugins and update types
    foreach ($storageUpdate as $tsid => $types) {
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->UpdateTypes($types[0],$types[1]);
    }

    // iterate through all storage plugins and add new types
    foreach ($storageAdd as $tsid => $types) {
      $tripalStorage = \Drupal::service("plugin.manager.tripal.storage")->getInstance($tsid);
      $tripalStorage->addTypes($types);
    }
  }

}
