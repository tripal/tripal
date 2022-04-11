<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;

/**
 * Plugin implementation of Tripal RDFS content type.
 *
 * @FieldType(
 *   id = "tripal_rdfs_type",
 *   label = @Translation("Content Type"),
 *   description = @Translation("The resource content type."),
 *   default_widget = "default_tripal_rdfs_type_widget",
 *   default_formatter = "default_tripal_rdfs_type_formatter"
 * )
 */
class TripalRDFSTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_rdfs_type";

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes() {
    return [VarCharStoragePropertyType($this->getEntity()->getEntityTypeId(),$this->id,"type",255)];
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {
    $entity = $this->getEntity();
    return [StoragePropertyValue($entity->getEntityTypeId(),$this->id,"type",$entity->id())];
  }

  /**
   * {@inheritdoc}
   */
  public function tripalLoad($properties,$entity) {
    foreach ($properties as $property) {
      if ($property->getFieldKey() == "type") {
        $entity->tripalRDFSType = $property->value();
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalSave($properties,$entity) {
    foreach ($properties as $property) {
      if ($property->getFieldKey() == "type") {
        $property->setValue($entity->tripalRDFSType);
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalClear($entity) {
    $entity->tripalRDFSType = "";
  }
}
